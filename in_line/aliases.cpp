
#include <windows.h>
#include <dsound.h>

#include "../alg/gen.h"
#include "aliases.h"
//#include "tool.h"

using namespace std;

#define DXDEVALIASKEY "Software\\Jasper van de Gronde\\SoundInput Library\\DxDevAliases"
#define WVDEVALIASKEY "Software\\Jasper van de Gronde\\SoundInput Library\\WvDevAliases"

bool	GetDevAlias(const char * id, char * alias, const int devtype)
{
	HKEY hkey;
	DWORD size;
	char key[1024];

	switch(devtype) {
	case WAVE_DEVICE:
		strcpy(key, WVDEVALIASKEY);
		break;
	case DIRECTX_DEVICE:
		strcpy(key, DXDEVALIASKEY);
		break;
	}

	if ( RegOpenKey(HKEY_CURRENT_USER, key, &hkey) != ERROR_SUCCESS ) {
		if ( RegCreateKey(HKEY_CURRENT_USER, key, &hkey) != ERROR_SUCCESS ) {
			Error("Error opening/creating device alias key.");
			return false;
		}
	}

	size=SIL_MAXALIASSIZE;
	if ( RegQueryValueEx(hkey, id, NULL, NULL, (LPBYTE)alias, &size) != ERROR_SUCCESS ) {
		strcpy(alias, "");
		RegCloseKey(hkey);
		return true;
	}
	
	RegCloseKey(hkey);

	return true;
}

bool	DeleteAliasById(const char * id, const int devtype)
{
	HKEY hkey;
	DWORD size;
	char key[1024];

	switch(devtype) {
	case WAVE_DEVICE:
		strcpy(key, WVDEVALIASKEY);
		break;
	case DIRECTX_DEVICE:
		strcpy(key, DXDEVALIASKEY);
		break;
	}

	if ( RegOpenKey(HKEY_CURRENT_USER, key, &hkey) != ERROR_SUCCESS ) {
		if ( RegCreateKey(HKEY_CURRENT_USER, key, &hkey) != ERROR_SUCCESS ) {
			Error("Error opening/creating device alias key.");
			return false;
		}
	}

	size=SIL_MAXALIASSIZE;
	if ( RegDeleteValue(hkey, id) != ERROR_SUCCESS ) {
		RegCloseKey(hkey);
		return false;
	}
	
	RegCloseKey(hkey);

	return true;
}

bool	DeleteAliasByAlias(const char * alias, const int devtype)
{
	HKEY hkey;
	char key[1024];

	char name[1024];
	DWORD cbname;
	BYTE data[SIL_MAXALIASSIZE+1];
	DWORD cbdata;

	DWORD values, t;

	switch(devtype) {
	case WAVE_DEVICE:
		strcpy(key, WVDEVALIASKEY);
		break;
	case DIRECTX_DEVICE:
		strcpy(key, DXDEVALIASKEY);
		break;
	}

	if ( RegOpenKey(HKEY_CURRENT_USER, key, &hkey) != ERROR_SUCCESS ) {
		if ( RegCreateKey(HKEY_CURRENT_USER, key, &hkey) != ERROR_SUCCESS ) {
			Error("Error opening/creating device alias key.");
			return false;
		}
	}

	RegQueryInfoKey(hkey, NULL, NULL, NULL, NULL ,NULL, NULL, &values, NULL, NULL, NULL, NULL);

	for(t=0; t<values; t++) {
		cbname=1024;
		cbdata=SIL_MAXALIASSIZE+1;
		RegEnumValue(hkey, t, name, &cbname, NULL, NULL, data, &cbdata);
		data[SIL_MAXALIASSIZE]=0;
		if ( lstrcmpi(alias, (char*)data) == 0 ) {
			RegDeleteValue(hkey, name);
			RegCloseKey(hkey);
			return true;
		}
	}

	RegCloseKey(hkey);

	return true;
}

bool	GetDevStr(const string &alias, string &id, const int devtype)
{
	HKEY hkey;
	char key[1024];

	char name[1024];
	DWORD cbname;
	BYTE data[SIL_MAXALIASSIZE+1];
	DWORD cbdata;

	DWORD values, t;

	switch(devtype) {
	case WAVE_DEVICE:
		strcpy(key, WVDEVALIASKEY);
		break;
	case DIRECTX_DEVICE:
		strcpy(key, DXDEVALIASKEY);
		break;
	}

	if ( RegOpenKey(HKEY_CURRENT_USER, key, &hkey) != ERROR_SUCCESS ) {
		if ( RegCreateKey(HKEY_CURRENT_USER, key, &hkey) != ERROR_SUCCESS ) {
			Error("Error opening/creating device alias key.");
			return false;
		}
	}

	RegQueryInfoKey(hkey, NULL, NULL, NULL, NULL ,NULL, NULL, &values, NULL, NULL, NULL, NULL);

	for(t=0; t<values; t++) {
		cbname=1024;
		cbdata=SIL_MAXALIASSIZE+1;
		RegEnumValue(hkey, t, name, &cbname, NULL, NULL, data, &cbdata);
		data[SIL_MAXALIASSIZE]=0;
		if ( alias == (char*)data ) {
			id=name;
			RegCloseKey(hkey);
			return true;
		}
	}

	RegCloseKey(hkey);

	return false;
}

bool	GetDevStr(const char * alias, char * id, const int devtype)
{
	HKEY hkey;
	char key[1024];

	char name[1024];
	DWORD cbname;
	BYTE data[SIL_MAXALIASSIZE+1];
	DWORD cbdata;

	DWORD values, t;

	switch(devtype) {
	case WAVE_DEVICE:
		strcpy(key, WVDEVALIASKEY);
		break;
	case DIRECTX_DEVICE:
		strcpy(key, DXDEVALIASKEY);
		break;
	}

	if ( RegOpenKey(HKEY_CURRENT_USER, key, &hkey) != ERROR_SUCCESS ) {
		if ( RegCreateKey(HKEY_CURRENT_USER, key, &hkey) != ERROR_SUCCESS ) {
			Error("Error opening/creating device alias key.");
			return false;
		}
	}

	RegQueryInfoKey(hkey, NULL, NULL, NULL, NULL ,NULL, NULL, &values, NULL, NULL, NULL, NULL);

	for(t=0; t<values; t++) {
		cbname=1024;
		cbdata=SIL_MAXALIASSIZE+1;
		RegEnumValue(hkey, t, name, &cbname, NULL, NULL, data, &cbdata);
		data[SIL_MAXALIASSIZE]=0;
		if ( lstrcmpi(alias, (char*)data) == 0 ) {
			strcpy(id, name);
			RegCloseKey(hkey);
			return true;
		}
	}

	RegCloseKey(hkey);

	return false;
}

bool	SetDevAlias(const char * id, const char * newalias, const int devtype)
{
	HKEY hkey;
	char key[1024];

	char name[1024];
	DWORD cbname;
	BYTE data[SIL_MAXALIASSIZE+1];
	DWORD cbdata;
	
	DWORD values, t;

	switch(devtype) {
	case WAVE_DEVICE:
		strcpy(key, WVDEVALIASKEY);
		break;
	case DIRECTX_DEVICE:
		strcpy(key, DXDEVALIASKEY);
		break;
	}

	if ( RegOpenKey(HKEY_CURRENT_USER, key, &hkey) != ERROR_SUCCESS ) {
		if ( RegCreateKey(HKEY_CURRENT_USER, key, &hkey) != ERROR_SUCCESS ) {
			Error("Error opening/creating device alias key.");
			return false;
		}
	}

	RegQueryInfoKey(hkey, NULL, NULL, NULL, NULL ,NULL, NULL, &values, NULL, NULL, NULL, NULL);

	for(t=0; t<values; t++) {
		cbname=1024;
		cbdata=SIL_MAXALIASSIZE+1;
		RegEnumValue(hkey, t, name, &cbname, NULL, NULL, data, &cbdata);
		data[SIL_MAXALIASSIZE]=0;
		if ( lstrcmpi(newalias, (char*)data) == 0 ) {
			Error("Alias already exists.");
			RegCloseKey(hkey);
			return false;
		}
	}

	if ( RegSetValueEx(hkey, id, 0, REG_SZ, (LPBYTE)newalias, strlen(newalias)+1) != ERROR_SUCCESS ) {
		Error("Error setting the device alias.");
		RegCloseKey(hkey);
		return false;
	}

	RegCloseKey(hkey);

	return true;
}

#include <commctrl.h>
#include "resource.h"

extern HWND devList;

HWND dnEdit;
HWND dvEdit;
int  dLVINDEX;
HWND dParentWnd;
int  dDevType;

BOOL CALLBACK devaliasDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
//	LVITEM lvi;
//	int index;
	char guid[1024];
	char alias[SIL_MAXALIASSIZE+1];

	switch(uMsg) {
	case WM_CLOSE:
		EnableWindow(dParentWnd, TRUE);
		DestroyWindow(hwndDlg);
		break;

	case WM_COMMAND:
		switch(HIWORD(wParam)) {
		case BN_CLICKED:
			switch(LOWORD(wParam)) {
			case IDOK:
				memset(guid, 0, 1024);
				memset(alias, 0, SIL_MAXALIASSIZE+1);
				SendMessage(dnEdit, WM_GETTEXT, (WPARAM) 1024, (LPARAM) guid);
				SendMessage(dvEdit, WM_GETTEXT, (WPARAM) SIL_MAXALIASSIZE+1, (LPARAM) alias);

				if ( strcmp(alias, "") == 0 ) {
					//Error("You have to fill in an alias (or click Cancel)!");
					if ( MessageBox(hwndDlg, "Do you really want to delete this alias?", "Just checking", MB_YESNO) == IDYES ) {
						DeleteAliasById(guid, dDevType);

						ListView_SetItemText(devList, dLVINDEX, (dDevType==DIRECTX_DEVICE?1:1), alias);

						EnableWindow(dParentWnd, TRUE);

						DestroyWindow(hwndDlg);
						break;
					}
					return 0;
				}

				if ( !SetDevAlias(guid, alias, dDevType) ) {
					Error("Error trying to add a device alias.");
					return 0;
				}
				
				ListView_SetItemText(devList, dLVINDEX, (dDevType==DIRECTX_DEVICE?1:1), alias);

				EnableWindow(dParentWnd, TRUE);

				DestroyWindow(hwndDlg);
				break;

			case IDCANCEL:
				EnableWindow(dParentWnd, TRUE);
				DestroyWindow(hwndDlg);
				break;
			}
			break;
		}
		break;
	};
	return 0;
}

BOOL ChangeDevAlias(HINSTANCE hInstance, HWND hwndParent, int lvindex, int devtype)
{
	HWND dxdevaliasWnd;
	char guid[1024];
	char alias[SIL_MAXALIASSIZE+1];

//	ListView_GetItemText(devList, lvindex, (devtype==DIRECTX_DEVICE?3:2), guid, SIL_MAXALIASSIZE+1);
	ListView_GetItemText(devList, lvindex, (devtype==DIRECTX_DEVICE?0:0), guid, 1024);
	ListView_GetItemText(devList, lvindex, (devtype==DIRECTX_DEVICE?1:1), alias, SIL_MAXALIASSIZE+1);

	//if ( strcmp(presetname, "") == 0 ) return TRUE;

	// Create the dialog

	dxdevaliasWnd=CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DXDEVALIASDLG), hwndParent, devaliasDialogProc);
	if ( !dxdevaliasWnd ) {
		Error("Error creating dialog box!");
		return FALSE;
	}
	ShowWindow(dxdevaliasWnd, SW_SHOW);

	switch(devtype) {
	case WAVE_DEVICE:
		SetWindowText(dxdevaliasWnd, "Standard Device Alias");
		ShowWindow(GetDlgItem(dxdevaliasWnd, IDC_DEVICEID), SW_SHOW);
		ShowWindow(GetDlgItem(dxdevaliasWnd, IDC_DEVICEGUID), SW_HIDE);
		break;

	case DIRECTX_DEVICE:
		SetWindowText(dxdevaliasWnd, "DirectSound Device Alias");
		ShowWindow(GetDlgItem(dxdevaliasWnd, IDC_DEVICEID), SW_HIDE);
		ShowWindow(GetDlgItem(dxdevaliasWnd, IDC_DEVICEGUID), SW_SHOW);
		break;
	}

	dnEdit=GetDlgItem(dxdevaliasWnd, IDC_GUID);
	if ( !dnEdit ) {
		Error("Error trying to retrieve handle of edit control.");
		return FALSE;
	}
	SendMessage(dnEdit, EM_SETLIMITTEXT, (WPARAM) 1000, 0);
	SendMessage(dnEdit, WM_SETTEXT, 0, (LPARAM) guid);
	EnableWindow(dnEdit, FALSE);

	dvEdit=GetDlgItem(dxdevaliasWnd, IDC_ALIAS);
	if ( !dvEdit ) {
		Error("Error trying to retrieve handle of edit control.");
		return FALSE;
	}
	SendMessage(dvEdit, EM_SETLIMITTEXT, (WPARAM) SIL_MAXALIASSIZE, 0);
	SendMessage(dvEdit, WM_SETTEXT, 0, (LPARAM) alias);

	dLVINDEX=lvindex;

	EnableWindow(hwndParent, FALSE);

	dParentWnd=hwndParent;
	dDevType=devtype;

	SetFocus(dvEdit);

	return TRUE;
}
