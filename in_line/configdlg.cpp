
#include "configdlg.h"

#include <dsound.h>
#include <commctrl.h>

extern HWND presetList;
extern HWND devList;
extern const char* LINEINPLUGINKEY;

#include "in.h"
#include "aliases.h"
#include "tool.h"
#include "preset.h"
#include "../alg/gen.h"

// Extern vars
extern int				paused;
extern int				Playing;
extern int				Stopping;
extern In_Module mod;
extern char lastfn[MAX_PATH];
extern HINSTANCE hInstance;
extern int Loaded;
extern const unsigned long WM_WA_MPEG_EOF;
typedef HRESULT (WINAPI * DIRECTSOUNDCAPTURECREATE)(LPGUID lpGUID, LPDIRECTSOUNDCAPTURE *lplpDSC, LPUNKNOWN pUnkOuter);
typedef HRESULT (WINAPI * DIRECTSOUNDCAPTUREENUMERATE)(LPDSENUMCALLBACK lpDSEnumCallback, LPVOID lpContext);
extern DIRECTSOUNDCAPTURECREATE		DxSndCapCreate;
extern DIRECTSOUNDCAPTUREENUMERATE		DxSndCapEnumerate;
extern bool						dxsoundsupport;


//extern HINSTANCE hInstance;
void	CreateAddLocationWizard(HWND hwndOwner);

HWND	hwndWinAMPConfigWnd;

HWND presetList;
HWND devList;

#define IPC_PLAYFILE 100

#define FREE(pntr) (free((LPVOID)pntr),pntr=NULL)

BOOL WINAPI DevListDSEnumCallback(LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext)
{
	LVITEM lvi;
	int index;
	char temp[SIL_MAXALIASSIZE+1];
//	char temp2[50];

	lvi.mask=LVIF_TEXT;
	lvi.pszText=(char*)lpcstrDescription;
	lvi.iSubItem=0;
	lvi.iItem=ListView_GetItemCount(devList);
	if ( (index=ListView_InsertItem(devList, &lvi)) == -1 ) {
		Error("Error trying to insert a directsoundcapture device listitem.");
		return FALSE;
	}
//	GetDevAlias(GuidToStr(lpGuid, temp), temp2, DIRECTX_DEVICE);
	GetDevAlias(lpcstrDescription, temp, DIRECTX_DEVICE);
	ListView_SetItemText(devList, index, 1, temp);
	ListView_SetItemText(devList, index, 2, (char*)lpcstrModule);
	ListView_SetItemText(devList, index, 3, strcpy(temp, GuidToStr(lpGuid).c_str()));
	
	return TRUE;
}

void FillDevList(HWND configWnd, int devtype)
{
	LVCOLUMN lvc;
	LVITEM lvi;
	char temp[SIL_MAXALIASSIZE+1];
//	char temp[20];
//	char temp2[50];
	unsigned long t;
	unsigned long numDevs;
	MMRESULT mmresult;
	WAVEINCAPS waveInCaps;

	switch(devtype) {
	case WAVE_DEVICE:
		// Set the right statics
		ShowWindow(GetDlgItem(configWnd, IDC_DXDEVTEXT), SW_HIDE);
		ShowWindow(GetDlgItem(configWnd, IDC_WAVEDEVTEXT), SW_SHOW);

		// Fill the device list with devices
		ListView_DeleteAllItems(devList);
		for(;ListView_DeleteColumn(devList, 0););

		lvc.mask=LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
		lvc.pszText="Device Name";
		lvc.iSubItem=0;
		lvc.cx=230;
		if ( ListView_InsertColumn(devList, 0, &lvc) == -1 ) {
			Error("Error trying to add the device name column.");
			return;
		}

		lvc.mask=LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
		lvc.pszText="Device Alias";
		lvc.iSubItem=1;
		lvc.cx=100;
		if ( ListView_InsertColumn(devList, 1, &lvc) == -1 ) {
			Error("Error trying to add the device alias column.");
			return;
		}

		lvc.mask=LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
		lvc.pszText="Device ID";
		lvc.iSubItem=2;
		lvc.cx=100;
		if ( ListView_InsertColumn(devList, 2, &lvc) == -1 ) {
			Error("Error trying to add the device id column.");
			return;
		}

		if ( mmresult=waveInGetDevCaps(WAVE_MAPPER, &waveInCaps, sizeof(WAVEINCAPS)) != MMSYSERR_NOERROR ) {
			ReportError(mmresult, "while trying to get device capabilities of the WAVE_MAPPER device");
			return;
		}

		lvi.mask=LVIF_TEXT;
		lvi.pszText=waveInCaps.szPname;
		lvi.iSubItem=0;
		lvi.iItem=0;
		if ( ListView_InsertItem(devList, &lvi) == -1 ) {
			Error("Error trying to insert the WAVE_MAPPER item.");
			return;
		}
//		GetDevAlias(_ltoa(WAVE_MAPPER, temp, 10), temp2, WAVE_DEVICE);
		GetDevAlias(waveInCaps.szPname, temp, WAVE_DEVICE);
		ListView_SetItemText(devList, 0, 1, temp);
		ListView_SetItemText(devList, 0, 2, _ltoa(WAVE_MAPPER, temp, 10));

		numDevs=waveInGetNumDevs();
		for(t=0; t<numDevs; t++) {
			if ( mmresult=waveInGetDevCaps(t, &waveInCaps, sizeof(WAVEINCAPS)) != MMSYSERR_NOERROR ) {
				ReportError(mmresult, "while trying to get device capabilities");
				return;
			}
			
			lvi.mask=LVIF_TEXT;
			lvi.pszText=waveInCaps.szPname;
			lvi.iSubItem=0;
			lvi.iItem=t+1;
			if ( ListView_InsertItem(devList, &lvi) == -1 ) {
				Error("Error trying to insert a device item.");
				return;
			}
//			GetDevAlias(_ultoa(t, temp, 10), temp2, WAVE_DEVICE);
			GetDevAlias(waveInCaps.szPname, temp, WAVE_DEVICE);
			ListView_SetItemText(devList, t+1, 1, temp);
			ListView_SetItemText(devList, t+1, 2, _ultoa(t, temp, 10));
		}
		break;

	case DIRECTX_DEVICE:

		if ( !dxsoundsupport ) {
			Error("No DirectSound support!");
			return;
		}

		// Set the right statics
		ShowWindow(GetDlgItem(configWnd, IDC_DXDEVTEXT), SW_SHOW);
		ShowWindow(GetDlgItem(configWnd, IDC_WAVEDEVTEXT), SW_HIDE);

		// Fill the device list with devices
		ListView_DeleteAllItems(devList);
		for(;ListView_DeleteColumn(devList, 0););

		lvc.mask=LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
		lvc.pszText="Device Description";
		lvc.iSubItem=0;
		lvc.cx=230;
		if ( ListView_InsertColumn(devList, 0, &lvc) == -1 ) {
			Error("Error trying to add the device description column.");
			return;
		}

		lvc.mask=LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
		lvc.pszText="Device Alias";
		lvc.iSubItem=1;
		lvc.cx=100;
		if ( ListView_InsertColumn(devList, 1, &lvc) == -1 ) {
			Error("Error trying to add the device alias column.");
			return;
		}

		lvc.mask=LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
		lvc.pszText="Device Module";
		lvc.iSubItem=2;
		lvc.cx=100;
		if ( ListView_InsertColumn(devList, 2, &lvc) == -1 ) {
			Error("Error trying to add the device module column.");
			return;
		}

		lvc.mask=LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
		lvc.pszText="Device GUID";
		lvc.iSubItem=3;
		lvc.cx=300;
		if ( ListView_InsertColumn(devList, 3, &lvc) == -1 ) {
			Error("Error trying to add the device guid column.");
			return;
		}

		DxSndCapEnumerate(DevListDSEnumCallback, (LPVOID)devList);
		//DirectSoundCaptureEnumerate(DevListDSEnumCallback, (LPVOID)devList);
		break;

	}
}

BOOL CALLBACK configDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
//	LVITEM lvi;
	int index;
	char presetname[257];
	int idCtrl;
	LPNMHDR iteminfo;
	NMLISTVIEW * dciteminfo;
	NMLVDISPINFO * eleiteminfo;
	NMLVKEYDOWN * keyinfo;
//	char presetval[1024];

	switch(uMsg) {
	case WM_CLOSE:
		DestroyWindow(hwndDlg);
		break;

	case WM_NOTIFY:
		idCtrl=(int)wParam;
		iteminfo=(LPNMHDR)lParam;
		
		switch(iteminfo->idFrom) {
		
		case IDC_DEVLISTTAB:
			switch(iteminfo->code) {
			case TCN_SELCHANGE:
				FillDevList(hwndDlg, TabCtrl_GetCurSel(iteminfo->hwndFrom));
				break;
			}
			break;
		
		case IDC_DEVLIST:
			switch(iteminfo->code) {
			case NM_DBLCLK:
				dciteminfo=(LPNMLISTVIEW)lParam;
				index=TabCtrl_GetCurSel(GetDlgItem(hwndDlg, IDC_DEVLISTTAB));
				if ( dciteminfo->iItem >= 0 ) {
					if ( ChangeDevAlias(hInstance, hwndDlg, dciteminfo->iItem, index) == FALSE ) {
						return 0;
					}
				}
				break;
			case LVN_KEYDOWN:
				keyinfo=(NMLVKEYDOWN*)lParam;
				switch(keyinfo->wVKey) {
				case VK_DELETE:
					{
						index=ListView_GetSelectionMark(devList);
						if ( index == -1 ) {
							Error("Error retrieving selected device item index. (perhaps you didn't select one yet)");
							return 0;
						}
						int devtype=TabCtrl_GetCurSel(GetDlgItem(hwndDlg, IDC_DEVLISTTAB));

						char id[1024];
						char alias[SIL_MAXALIASSIZE+1];
						char temp[1024+SIL_MAXALIASSIZE+1+100];

						ListView_GetItemText(devList, index, (devtype==DIRECTX_DEVICE?0:0), id, 1024);
						ListView_GetItemText(devList, index, (devtype==DIRECTX_DEVICE?1:1), alias, SIL_MAXALIASSIZE+1);

						sprintf(temp, "Do you really want to delete the alias \'%s\', for device \'%s\'?", alias, id);
						if ( MessageBox(hwndDlg, temp, "Just checking", MB_YESNO) == IDYES ) {
							DeleteAliasById(id, devtype);

							ListView_SetItemText(devList, index, (devtype==DIRECTX_DEVICE?1:1), "");
							break;
						}
					}					
					break;
				}
				break;
			}
			break;

		case IDC_PRESETLIST:
			switch(iteminfo->code) {
			case NM_DBLCLK:
				dciteminfo=(LPNMLISTVIEW)lParam;
				if ( dciteminfo->iItem >= 0 ) {
					if ( ChangePreset(hInstance, hwndDlg, dciteminfo->iItem) == FALSE ) {
						return 0;
					}
				}
				break;
			case LVN_ENDLABELEDIT:
				eleiteminfo=(NMLVDISPINFO*)lParam;
				if ( eleiteminfo->item.pszText && eleiteminfo->item.iSubItem == 0 ) {
					ListView_GetItemText(presetList, eleiteminfo->item.iItem, 0, presetname, 256);
					if ( RenamePreset(presetname, eleiteminfo->item.pszText) == FALSE ) {
						Error("Error trying to rename a preset.");
						return 0;
					}
					ListView_SetItemText(presetList, eleiteminfo->item.iItem, 0, eleiteminfo->item.pszText);
					return TRUE;
				}
				break;
			case LVN_KEYDOWN:
				keyinfo=(NMLVKEYDOWN*)lParam;
				switch(keyinfo->wVKey) {
				case VK_DELETE:
					goto CDP_REMOVEPRESET;
					break;
				case VK_F2:
					index=ListView_GetSelectionMark(presetList);
					if ( index == -1 ) {
						Error("Error retrieving selected preset item index.");
						return 0;
					}
					ListView_EditLabel(presetList, index);
					break;
				}
				break;
			}
			break;
		}
		break;

	case WM_COMMAND:
		switch(HIWORD(wParam)) {
		case BN_CLICKED:
			switch(LOWORD(wParam)) {
			case IDOK:
				{
					HKEY hkey;
					BOOL asp=IsDlgButtonChecked(hwndDlg, IDC_HANGONSTOPCHECK);
					BOOL repwhdrs=IsDlgButtonChecked(hwndDlg, IDC_REPREPARECHECK);
					if ( RegCreateKey(HKEY_CURRENT_USER, LINEINPLUGINKEY, &hkey) == ERROR_SUCCESS ) {
						if ( RegSetValueEx(hkey, "AvoidStopProblem", 0, REG_DWORD, (LPBYTE)&asp, sizeof(BOOL)) != ERROR_SUCCESS ) {
							Error("Error setting registry value.");
						}
						if ( RegSetValueEx(hkey, "ReprepareWaveHeaders", 0, REG_DWORD, (LPBYTE)&repwhdrs, sizeof(BOOL)) != ERROR_SUCCESS ) {
							Error("Error setting registry value.");
						}
						RegCloseKey(hkey);
					} else {
						Error("Error creating registry key.");
					}
				}
				DestroyWindow(hwndDlg);
				break;

			case IDC_ADDLOCWIZARDBUT:
				CreateAddLocationWizard(hwndDlg);
				break;

			case IDC_ADDPRESETBUT:
				if ( GetNewPreset(hInstance, hwndDlg) == FALSE ) {
					return 0;
				}
				break;

			case IDC_REMOVEPRESETBUT:
CDP_REMOVEPRESET:
				{
					index=ListView_GetSelectionMark(presetList);
					if ( index == -1 ) {
						Error("Error retrieving selected preset item index. (perhaps you didn't select one yet)");
						return 0;
					}
					
					ListView_GetItemText(presetList, index, 0, presetname, 256);
					if ( !RemovePreset(presetname) ) {
						Error("Error trying to get the name of a preset.");
						return 0;
					}

					char temp[1024];
					sprintf(temp, "Are you sure you want to delete this preset: %s?", presetname);
					if ( MessageBox(hwndDlg, temp, "Just checking", MB_YESNO) != IDYES ) break;

					if ( ListView_DeleteItem(presetList, index) == FALSE ) {
						Error("Error deleting the selected preset item.");
						return 0;
					}
				}
				break;
			}
			break;
		}
		break;
	};
	return 0;
}

void config(HWND hwndParent)
{
	HWND configWnd;
	HWND devlistTab;
	LVCOLUMN lvc;
	LVITEM lvi;
//	char temp[20];
	unsigned long t;
	unsigned long numPresets;
//	MMRESULT mmresult;
//	WAVEINCAPS waveInCaps;
	char presetname[256], presetval[1024];
	int index;
	HKEY hkey;
	DWORD values, presetnamesize, presetvalsize;
	TCITEM tcitem;

	// Create the dialog

	configWnd=CreateDialog(hInstance, MAKEINTRESOURCE(IDD_CONFIGDLG), hwndParent, configDialogProc);
	if ( !configWnd ) {
		Error("Error creating dialog box!");
		return;
	}
	ShowWindow(configWnd, SW_SHOW);
	hwndWinAMPConfigWnd=configWnd;

	// Get the device list hwnd
	devList=GetDlgItem(configWnd, IDC_DEVLIST);
	if ( !devList ) {
		Error("Error trying to retrieve handle of device list.");
		return;
	}

	// Create the devlist tab
	devlistTab=GetDlgItem(configWnd, IDC_DEVLISTTAB);

	TabCtrl_DeleteAllItems(devlistTab);

	tcitem.mask=TCIF_TEXT;
	tcitem.pszText="Standard";
	TabCtrl_InsertItem(devlistTab, WAVE_DEVICE, &tcitem);

	if ( dxsoundsupport ) {
		tcitem.mask=TCIF_TEXT;
		tcitem.pszText="DirectSound";
		TabCtrl_InsertItem(devlistTab, DIRECTX_DEVICE, &tcitem);
	}

	TabCtrl_SetCurSel(devlistTab, WAVE_DEVICE);

	// Fill the devlist
	FillDevList(configWnd, WAVE_DEVICE);

	// Fill the preset list with presets
	presetList=GetDlgItem(configWnd, IDC_PRESETLIST);
	if ( !presetList ) {
		Error("Error trying to retrieve handle of preset list.");
		return;
	}

	lvc.mask=LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
	lvc.pszText="Preset name";
	lvc.iSubItem=0;
	lvc.cx=150;
	if ( ListView_InsertColumn(presetList, 0, &lvc) == -1 ) {
		Error("Error trying to add the preset name column.");
		return;
	}

	lvc.mask=LVCF_TEXT | LVCF_SUBITEM | LVCF_WIDTH;
	lvc.pszText="Option string";
	lvc.iSubItem=1;
	lvc.cx=250;
	if ( ListView_InsertColumn(presetList, 1, &lvc) == -1 ) {
		Error("Error trying to add the option string column.");
		return;
	}

	// Beging filling
	if ( RegOpenKey(HKEY_CURRENT_USER, PRESETKEY, &hkey) == ERROR_SUCCESS ) {
		RegQueryInfoKey(hkey, NULL, NULL, NULL, NULL ,NULL, NULL, &values, NULL, NULL, NULL, NULL);

		numPresets=(int)values;
		for(t=0; t<numPresets; t++) {
			presetnamesize=256;
			presetvalsize=1024;
			RegEnumValue(hkey, t, presetname, &presetnamesize, NULL, NULL, (LPBYTE)presetval, &presetvalsize);
			
			lvi.mask=LVIF_TEXT;
			lvi.pszText=presetname;
			lvi.iSubItem=0;
			lvi.iItem=0;
			if ( (index=ListView_InsertItem(presetList, &lvi)) == -1 ) {
				Error("Error trying to insert a preset item.");
				return;
			}
			ListView_SetItemText(presetList, index, 1, presetval);
		}

		RegCloseKey(hkey);
	}

	// Set the avoidstopproblem checkbox (IDC_HANGONSTOPCHECK)
	BOOL asp=FALSE;
	DWORD cbdata;

	if ( RegOpenKey(HKEY_CURRENT_USER, LINEINPLUGINKEY, &hkey) == ERROR_SUCCESS ) {
		cbdata=sizeof(BOOL);
		if ( RegQueryValueEx(hkey, "AvoidStopProblem2", NULL, NULL, (LPBYTE)&asp, &cbdata) != ERROR_SUCCESS ) {
			asp=FALSE;
		}
		RegCloseKey(hkey);
	} else {
		asp=FALSE;
	}
	CheckDlgButton(configWnd, IDC_HANGONSTOPCHECK, asp);


	// Set the reprepare waveheaders checkbox (IDC_REPREPARECHECK)
	BOOL repwhdrs=TRUE;
//	DWORD cbdata;

	if ( RegOpenKey(HKEY_CURRENT_USER, LINEINPLUGINKEY, &hkey) == ERROR_SUCCESS ) {
		cbdata=sizeof(BOOL);
		if ( RegQueryValueEx(hkey, "ReprepareWaveHeaders", NULL, NULL, (LPBYTE)&repwhdrs, &cbdata) != ERROR_SUCCESS ) {
			repwhdrs=FALSE;
		}
		RegCloseKey(hkey);
	} else {
		repwhdrs=FALSE;
	}
	CheckDlgButton(configWnd, IDC_REPREPARECHECK, repwhdrs);
}


// Add location wizard stuff
const int ALW_NUMPAGES=3;

HWND hwndPropsheet;
char * alwurl=NULL;
bool alwneedcomma;
bool alwneedcommabeforepage[ALW_NUMPAGES];
unsigned int alwlenbeforepage[ALW_NUMPAGES];

char alwtitle[260];
bool alwshowformat;

char alwsrate[30];
char alwbps[20];
char alwnch[20];
int alwdevtype;
int alwdev;

bool alwmakepreset;
bool alwaddtoplaylist;
char alwpresetname[257];

BOOL CALLBACK ALWStartDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg) {
	case WM_NOTIFY:
		{
			LPNMHDR lpnmh=(LPNMHDR)lParam;
			switch(lpnmh->code) {
			case PSN_SETACTIVE:
				{
					alwlenbeforepage[0]=0;
					alwneedcommabeforepage[0]=false;
					CheckDlgButton(hwndDlg, IDC_ALWSTDFMTCHECK, (alwshowformat?TRUE:FALSE));
					SendMessage(GetDlgItem(hwndDlg, IDC_ALWTITLE), EM_SETLIMITTEXT, 255, 0);
					SetWindowText(GetDlgItem(hwndDlg, IDC_ALWTITLE), alwtitle);
					PropSheet_SetWizButtons(hwndPropsheet, PSWIZB_NEXT);
				}
				break;

			case PSN_WIZNEXT:
				{
					if ( alwurl ) FREE(alwurl);

					alwneedcomma=false;

//					alwurl=NULL;
					alwurl=(char*)malloc(strlen("")+1);//(char*)malloc(strlen("line://")+1);
					strcpy(alwurl, "");//strcpy(alwurl, "line://");

					if ( !IsDlgButtonChecked(hwndDlg, IDC_ALWSTDFMTCHECK) ) {
						alwurl=(char*)realloc(alwurl, strlen(alwurl) + strlen("hideformat") + 1);
						strcat(alwurl, "hideformat");
						alwneedcomma=true;

						alwshowformat=false;
					} else {
						alwshowformat=true;
					}

					char			temp[520];
					char			temp2[520];
					int				offset=0;
					bool			quoted=false;
					unsigned int	t;

					GetWindowText(GetDlgItem(hwndDlg, IDC_ALWTITLE), temp, 255);
					temp[255]=0;
					strcpy(alwtitle, temp);

					for(t=0; t<strlen(temp); t++) {
						if ( !quoted ) {
							if ( temp[t] == '\\' ) {
								quoted=true;
							} else if ( temp[t] == ',' ) {
								temp2[t+offset]='\\';
								offset++;
							}
						} else {
							quoted=false;
						}
						temp2[t+offset]=temp[t];
					}
					temp2[t+offset]=0;

					if ( temp2[0] ) {
						alwurl=(char*)realloc(alwurl, strlen(alwurl) + (alwneedcomma?1:0) + strlen("title=") + strlen(temp2) + 1);
						if ( alwneedcomma ) strcat(alwurl, ",");
						strcat(alwurl, "title=");
						strcat(alwurl, temp2);
						alwneedcomma=true;
					}

					alwneedcommabeforepage[1]=alwneedcomma;
				}
				break;

			case PSN_QUERYCANCEL:
				{
					if ( alwurl!=NULL ) FREE(alwurl);
				}
				break;
			}
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK ALWDevsandfmtDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg) {
	case WM_NOTIFY:
		{
			LPNMHDR lpnmh=(LPNMHDR)lParam;
			switch(lpnmh->code) {
			case PSN_SETACTIVE:
				{
					if ( alwlenbeforepage[1] == 0 ) alwlenbeforepage[1]=strlen(alwurl);
					SendMessage(GetDlgItem(hwndDlg, IDC_ALWTITLE), EM_SETLIMITTEXT, 20, 0);
					SetWindowText(GetDlgItem(hwndDlg, IDC_ALWSRATE), alwsrate);
					SendMessage(GetDlgItem(hwndDlg, IDC_ALWTITLE), EM_SETLIMITTEXT, 10, 0);
					SetWindowText(GetDlgItem(hwndDlg, IDC_ALWNCH), alwnch);
					SendMessage(GetDlgItem(hwndDlg, IDC_ALWTITLE), EM_SETLIMITTEXT, 10, 0);
					SetWindowText(GetDlgItem(hwndDlg, IDC_ALWBPS), alwbps);

					//CheckRadioButton(hwndDlg, IDC_ALWDEVTYPESTD, IDC_ALWDEVTYPEDSOUND, IDC_ALWDEVTYPESTD);

					// Fill devcombo
					WAVEINCAPS waveInCaps;
					MMRESULT mmresult;

					SendMessage(GetDlgItem(hwndDlg, IDC_ALWDEVCOMBO), CB_RESETCONTENT, 0, 0);

					if ( mmresult=waveInGetDevCaps(WAVE_MAPPER, &waveInCaps, sizeof(WAVEINCAPS)) != MMSYSERR_NOERROR ) {
						ReportError(mmresult, "while trying to get device capabilities of the WAVE_MAPPER device");
						return FALSE;
					}

					SendMessage(GetDlgItem(hwndDlg, IDC_ALWDEVCOMBO), CB_ADDSTRING, 0, (LPARAM)waveInCaps.szPname);
					for(unsigned int t=0; t<waveInGetNumDevs(); t++) {
						if ( mmresult=waveInGetDevCaps(t, &waveInCaps, sizeof(WAVEINCAPS)) != MMSYSERR_NOERROR ) {
							ReportError(mmresult, "while trying to get device capabilities of a wave device");
							return FALSE;
						}
						SendMessage(GetDlgItem(hwndDlg, IDC_ALWDEVCOMBO), CB_ADDSTRING, 0, (LPARAM)waveInCaps.szPname);
					}

					SendMessage(GetDlgItem(hwndDlg, IDC_ALWDEVCOMBO), CB_SETCURSEL, alwdev+1, 0);

					PropSheet_SetWizButtons(hwndPropsheet, PSWIZB_NEXT | PSWIZB_BACK);
				}
				break;

			case PSN_WIZBACK:
				{
					char temp[30];

					GetWindowText(GetDlgItem(hwndDlg, IDC_ALWSRATE), temp, 20);
					temp[20]=0;
					strcpy(alwsrate, temp);

					GetWindowText(GetDlgItem(hwndDlg, IDC_ALWBPS), temp, 20);
					temp[20]=0;
					strcpy(alwbps, temp);
				
					GetWindowText(GetDlgItem(hwndDlg, IDC_ALWNCH), temp, 20);
					temp[20]=0;
					strcpy(alwnch, temp);

					int cursel;

					cursel=SendMessage(GetDlgItem(hwndDlg, IDC_ALWDEVCOMBO), CB_GETCURSEL, 0, 0);

					if ( cursel != CB_ERR ) {
						cursel--;
						alwdev=cursel;
					}


					alwurl[alwlenbeforepage[1]]=0;
					alwlenbeforepage[1]=0;
				}
				break;

			case PSN_WIZNEXT:
				{
					alwneedcomma=alwneedcommabeforepage[1];

					if ( strlen(alwurl) > alwlenbeforepage[1] ) alwurl[alwlenbeforepage[1]]=0;

					char temp[30];

					GetWindowText(GetDlgItem(hwndDlg, IDC_ALWSRATE), temp, 20);
					temp[20]=0;
					strcpy(alwsrate, temp);

					if ( temp[0] ) {
						alwurl=(char*)realloc(alwurl, strlen(alwurl) + (alwneedcomma?1:0) + strlen("srate=") + strlen(temp) + 1);
						if ( alwneedcomma ) strcat(alwurl, ",");
						strcat(alwurl, "srate=");
						strcat(alwurl, temp);
						alwneedcomma=true;
					}

					GetWindowText(GetDlgItem(hwndDlg, IDC_ALWBPS), temp, 20);
					temp[20]=0;
					strcpy(alwbps, temp);

					if ( temp[0] ) {
						alwurl=(char*)realloc(alwurl, strlen(alwurl) + (alwneedcomma?1:0) + strlen("bps=") + strlen(temp) + 1);
						if ( alwneedcomma ) strcat(alwurl, ",");
						strcat(alwurl, "bps=");
						strcat(alwurl, temp);
						alwneedcomma=true;
					}
				
					GetWindowText(GetDlgItem(hwndDlg, IDC_ALWNCH), temp, 20);
					temp[20]=0;
					strcpy(alwnch, temp);

					if ( temp[0] ) {
						alwurl=(char*)realloc(alwurl, strlen(alwurl) + (alwneedcomma?1:0) + strlen("nch=") + strlen(temp) + 1);
						if ( alwneedcomma ) strcat(alwurl, ",");
						strcat(alwurl, "nch=");
						strcat(alwurl, temp);
						alwneedcomma=true;
					}

					
					int cursel;

					cursel=SendMessage(GetDlgItem(hwndDlg, IDC_ALWDEVCOMBO), CB_GETCURSEL, 0, 0);

					if ( cursel != CB_ERR ) {
						cursel--;
						if ( cursel > -1 ) {
							_itoa(cursel, temp, 10);

							alwurl=(char*)realloc(alwurl, strlen(alwurl) + (alwneedcomma?1:0) + strlen("dev=") + strlen(temp) + 1);
							if ( alwneedcomma ) strcat(alwurl, ",");
							strcat(alwurl, "dev=");
							strcat(alwurl, temp);
							alwneedcomma=true;
						}

						alwdev=cursel;
					}


					SetWindowLong(hwndDlg, DWL_MSGRESULT, 0);
					return TRUE;
				}
				break;

			case PSN_QUERYCANCEL:
				{
					if ( alwurl ) FREE(alwurl);
				}
				break;
			}
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK ALWStopDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg) {
	case WM_NOTIFY:
		{
			LPNMHDR lpnmh=(LPNMHDR)lParam;
			switch(lpnmh->code) {
			case PSN_SETACTIVE:
				{
					if ( alwlenbeforepage[2] == 0 ) alwlenbeforepage[2]=strlen(alwurl);
					SendMessage(GetDlgItem(hwndDlg, IDC_ALWURL), EM_SETLIMITTEXT, 1024, 0);
					SetWindowText(GetDlgItem(hwndDlg, IDC_ALWURL), alwurl);
					SendMessage(GetDlgItem(hwndDlg, IDC_ALWPRESETNAME), EM_SETLIMITTEXT, 255, 0);
					SetWindowText(GetDlgItem(hwndDlg, IDC_ALWPRESETNAME), alwpresetname);

					CheckDlgButton(hwndDlg, IDC_ALWMAKEPRESET, (alwmakepreset?TRUE:FALSE));
					EnableWindow(GetDlgItem(hwndDlg, IDC_ALWPRESETNAMESTATIC), (alwmakepreset?TRUE:FALSE));
					EnableWindow(GetDlgItem(hwndDlg, IDC_ALWPRESETNAME), (alwmakepreset?TRUE:FALSE));
					CheckDlgButton(hwndDlg, IDC_ALWADDTOPLAYLIST, (alwaddtoplaylist?TRUE:FALSE));

					PropSheet_SetWizButtons(hwndPropsheet, PSWIZB_FINISH | PSWIZB_BACK);
				}
				break;

			case PSN_WIZBACK:
				{
					char temp[257];

					alwmakepreset=IsDlgButtonChecked(hwndDlg, IDC_ALWMAKEPRESET)?true:false;
					alwaddtoplaylist=IsDlgButtonChecked(hwndDlg, IDC_ALWADDTOPLAYLIST)?true:false;

					GetWindowText(GetDlgItem(hwndDlg, IDC_ALWPRESETNAME), temp, 256);
					temp[256]=0;
					strcpy(alwpresetname, temp);

					alwurl[alwlenbeforepage[1]]=0;
					alwlenbeforepage[1]=0;
				}
				break;

			case PSN_WIZFINISH:
				{
					char url[1035];

					strcpy(url, "line://");
					GetWindowText(GetDlgItem(hwndDlg, IDC_ALWURL), url+7, 1024);
					url[1034]=0;

					if ( IsDlgButtonChecked(hwndDlg, IDC_ALWMAKEPRESET) ) {
						char temp[257];

						GetWindowText(GetDlgItem(hwndDlg, IDC_ALWPRESETNAME), temp, 256);
						temp[256]=0;
						strcpy(alwpresetname, temp);

						if ( AddPreset(alwpresetname, url+7) == FALSE ) {
							SetWindowLong(hwndDlg, DWL_MSGRESULT, 1);
							return TRUE;
						}

						LVITEM lvi;
						int index;

						lvi.mask=LVIF_TEXT;
						lvi.pszText=alwpresetname;
						lvi.iSubItem=0;
						lvi.iItem=0;
						if ( (index=ListView_InsertItem(presetList, &lvi)) == -1 ) {
							Error("Error trying to insert the preset item.");
							return TRUE;
						}
						ListView_SetItemText(presetList, index, 1, url+7);
					}

					if ( IsDlgButtonChecked(hwndDlg, IDC_ALWADDTOPLAYLIST) ) {

						HWND hwnd_winamp=FindWindow("Winamp v1.x",NULL);

						/*
						** COPYDATASTRUCT cds;
						** cds.dwData = IPC_PLAYFILE;
						** cds.lpData = (void *) "file.mp3";
						** cds.cbData = strlen((char *) cds.lpData)+1; // include space for null char
						** SendMessage(hwnd_winamp,WM_COPYDATA,(WPARAM)NULL,(LPARAM)&cds);
						**
						** This will play the file "file.mp3".
						**
						*/

						COPYDATASTRUCT cds;
						cds.dwData=IPC_PLAYFILE;
						cds.lpData=(void*)url;
						cds.cbData=strlen(url)+1;
						SendMessage(hwnd_winamp, WM_COPYDATA, NULL, (LPARAM)&cds);
					}

					if ( alwurl ) FREE(alwurl);

					SetWindowLong(hwndDlg, DWL_MSGRESULT, 0);
					return TRUE;
				}
				break;

			case PSN_QUERYCANCEL:
				{
					if ( alwurl ) FREE(alwurl);
				}
				break;
			}
		}
		break;

	case WM_COMMAND:
		{
			WORD wNotifyCode = HIWORD(wParam); // notification code 
			WORD wID = LOWORD(wParam);         // item, control, or accelerator identifier 
			HWND hwndCtl = (HWND) lParam;      // handle of control 
			switch(wNotifyCode) {
			case BN_CLICKED:
				switch(wID) {
				case IDC_ALWMAKEPRESET:
					{
						if ( IsDlgButtonChecked(hwndDlg, IDC_ALWMAKEPRESET) ) {
							EnableWindow(GetDlgItem(hwndDlg, IDC_ALWPRESETNAMESTATIC), TRUE);
							EnableWindow(GetDlgItem(hwndDlg, IDC_ALWPRESETNAME), TRUE);
						} else {
							EnableWindow(GetDlgItem(hwndDlg, IDC_ALWPRESETNAMESTATIC), FALSE);
							EnableWindow(GetDlgItem(hwndDlg, IDC_ALWPRESETNAME), FALSE);
						}
					}
					break;
				}
				break;
			}
		}
		break;
	}
	return FALSE;
}

int CALLBACK PropSheetProc(HWND hwndDlg, UINT uMsg, LPARAM lParam)
{
	switch(uMsg) {
	case PSCB_INITIALIZED:
		hwndPropsheet=hwndDlg;
		break;
	}
	return 0;
}

void	CreateAddLocationWizard(HWND hwndOwner)
{
	int t;
	PROPSHEETPAGE		psp[ALW_NUMPAGES];
	PROPSHEETHEADER		psh;

	alwurl=NULL;
	alwneedcomma=false;
	for(t=0; t<ALW_NUMPAGES; t++) alwlenbeforepage[t]=0;
	for(t=0; t<ALW_NUMPAGES; t++) alwneedcommabeforepage[t]=false;

	strcpy(alwtitle, "");
	alwshowformat=true;

	strcpy(alwsrate, "");
	strcpy(alwbps, "");
	strcpy(alwnch, "");
	alwdevtype=WAVE_DEVICE;
	alwdev=-1;

	alwmakepreset=false;
	alwaddtoplaylist=true;
	strcpy(alwpresetname, "");

    psp[0].dwSize = sizeof(PROPSHEETPAGE);
    psp[0].dwFlags = PSP_USETITLE;
    psp[0].hInstance = hInstance;
    psp[0].pszTemplate = MAKEINTRESOURCE(DLG_START);
    psp[0].pszIcon = NULL;//MAKEINTRESOURCE(IDI_FONT);
    psp[0].pfnDlgProc = ALWStartDlgProc;
    psp[0].pszTitle = "Start";
    psp[0].lParam = 0;
    psp[0].pfnCallback = NULL;

    psp[1].dwSize = sizeof(PROPSHEETPAGE);
    psp[1].dwFlags = PSP_USETITLE;
    psp[1].hInstance = hInstance;
    psp[1].pszTemplate = MAKEINTRESOURCE(DLG_DEVSANDFMT);
    psp[1].pszIcon = NULL;//MAKEINTRESOURCE(IDI_FONT);
    psp[1].pfnDlgProc = ALWDevsandfmtDlgProc;
    psp[1].pszTitle = "Wave format and devices";
    psp[1].lParam = 0;
    psp[1].pfnCallback = NULL;

    psp[2].dwSize = sizeof(PROPSHEETPAGE);
    psp[2].dwFlags = PSP_USETITLE;
    psp[2].hInstance = hInstance;
    psp[2].pszTemplate = MAKEINTRESOURCE(DLG_STOP);
    psp[2].pszIcon = NULL;//MAKEINTRESOURCE(IDI_FONT);
    psp[2].pfnDlgProc = ALWStopDlgProc;
    psp[2].pszTitle = "The final touch";
    psp[2].lParam = 0;
    psp[2].pfnCallback = NULL;

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_WIZARD | PSH_PROPSHEETPAGE | PSH_USECALLBACK;
    psh.hwndParent = hwndOwner;
    psh.hInstance = hInstance;
    psh.pszIcon = NULL;
    psh.pszCaption = (LPSTR) "Cell Properties";
    psh.nPages = sizeof(psp) / sizeof(PROPSHEETPAGE);
    psh.nStartPage = 0;
    psh.ppsp = (LPCPROPSHEETPAGE) &psp;
    psh.pfnCallback = PropSheetProc;

    PropertySheet(&psh);
    return;

}
