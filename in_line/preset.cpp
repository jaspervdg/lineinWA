
#include "preset.h"

#include "../alg/gen.h"
#include "configdlg.h"
#include "tool.h"

#include <commctrl.h>

extern HWND presetList;

bool GetPreset(const string& presetname, string& presetval)
{
	HKEY hkey;
	DWORD size;

	presetval="";

	if ( RegOpenKey(HKEY_CURRENT_USER, PRESETKEY, &hkey) != ERROR_SUCCESS ) {
		if ( RegCreateKey(HKEY_CURRENT_USER, PRESETKEY, &hkey) != ERROR_SUCCESS ) {
			Error("Error opening/creating preset key.");
			return false;
		}
	}

	size=0;
	if ( RegQueryValueEx(hkey, presetname.c_str(), NULL, NULL, NULL, &size) != ERROR_SUCCESS ) {
		Error("The preset doesn't exist!");
		RegCloseKey(hkey);
		return false;
	}
	char* temp;
	temp=new char[size];
	if ( temp == NULL ) {
		Error("Error allocating memory!");
		RegCloseKey(hkey);
		return false;
	}
	if ( RegQueryValueEx(hkey, presetname.c_str(), NULL, NULL, (LPBYTE)temp, &size) != ERROR_SUCCESS ) {
		Error("Unable to retrieve preset!");
		RegCloseKey(hkey);
		return false;
	}
	presetval=temp;
	delete temp;
	
	RegCloseKey(hkey);

	return true;
}

bool AddPreset(const string& presetname, const string& presetval)
{
	HKEY hkey;

	if ( RegOpenKey(HKEY_CURRENT_USER, PRESETKEY, &hkey) != ERROR_SUCCESS ) {
		if ( RegCreateKey(HKEY_CURRENT_USER, PRESETKEY, &hkey) != ERROR_SUCCESS ) {
			Error("Error opening/creating preset key.");
			return false;
		}
	}

	if ( RegQueryValueEx(hkey, presetname.c_str(), NULL, NULL, NULL, NULL) == ERROR_SUCCESS ) {
		Error("The preset already exists!");
		RegCloseKey(hkey);
		return false;
	}
	
	if ( RegSetValueEx(hkey, presetname.c_str(), 0, REG_SZ, (LPBYTE)(presetval.c_str()), presetval.size()+1) != ERROR_SUCCESS ) {
		Error("Error adding the preset.");
		RegCloseKey(hkey);
		return false;
	}

	RegCloseKey(hkey);

	return true;
}

bool RenamePreset(const string& oldpresetname, const string& newpresetname)
{
	HKEY hkey;
	BYTE data[1025];
	DWORD cbval=1025;

	if ( RegOpenKey(HKEY_CURRENT_USER, PRESETKEY, &hkey) != ERROR_SUCCESS ) {
		if ( RegCreateKey(HKEY_CURRENT_USER, PRESETKEY, &hkey) != ERROR_SUCCESS ) {
			Error("Error opening/creating preset key.");
			return false;
		}
	}

	if ( RegQueryValueEx(hkey, oldpresetname.c_str(), NULL, NULL, data, &cbval) != ERROR_SUCCESS ) {
		Error("The preset doesn't exist!");
		RegCloseKey(hkey);
		return false;
	}
	
	if ( RegSetValueEx(hkey, newpresetname.c_str(), 0, REG_SZ, data, cbval) != ERROR_SUCCESS ) {
		Error("Error setting the preset.");
		RegCloseKey(hkey);
		return false;
	}

	if ( RegDeleteValue(hkey, oldpresetname.c_str()) != ERROR_SUCCESS ) {
		Error("Error deleting the preset.");
		RegCloseKey(hkey);
		return false;
	}

	RegCloseKey(hkey);

	return true;
}

bool SetPreset(const string& presetname, const string& presetval)
{
	HKEY hkey;

	if ( RegOpenKey(HKEY_CURRENT_USER, PRESETKEY, &hkey) != ERROR_SUCCESS ) {
		if ( RegCreateKey(HKEY_CURRENT_USER, PRESETKEY, &hkey) != ERROR_SUCCESS ) {
			Error("Error opening/creating preset key.");
			return false;
		}
	}

	if ( RegQueryValueEx(hkey, presetname.c_str(), NULL, NULL, NULL, NULL) != ERROR_SUCCESS ) {
		Error("The preset doesn't exist!");
		RegCloseKey(hkey);
		return false;
	}
	
	if ( RegSetValueEx(hkey, presetname.c_str(), 0, REG_SZ, (LPBYTE)(presetval.c_str()), presetval.size()+1) != ERROR_SUCCESS ) {
		Error("Error setting the preset.");
		RegCloseKey(hkey);
		return false;
	}

	RegCloseKey(hkey);

	return true;
}

bool RemovePreset(const string& presetname)
{
	HKEY hkey;

	if ( RegOpenKey(HKEY_CURRENT_USER, PRESETKEY, &hkey) != ERROR_SUCCESS ) {
		if ( RegCreateKey(HKEY_CURRENT_USER, PRESETKEY, &hkey) != ERROR_SUCCESS ) {
			Error("Error opening/creating preset key.");
			return false;
		}
	}

	if ( RegDeleteValue(hkey, presetname.c_str()) != ERROR_SUCCESS ) {
		Error("Error deleting the preset.");
		RegCloseKey(hkey);
		return false;
	}
	
	RegCloseKey(hkey);

	return true;
}

HWND pnEdit;
HWND pvEdit;
int  LVINDEX;
HWND ParentWnd;

BOOL CALLBACK presetDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LVITEM lvi;
	int index;
	char presetname[256];
	char presetval[1024];

	switch(uMsg) {
	case WM_CLOSE:
		EnableWindow(ParentWnd, TRUE);
		DestroyWindow(hwndDlg);
		break;

	case WM_COMMAND:
		switch(HIWORD(wParam)) {
		case BN_CLICKED:
			switch(LOWORD(wParam)) {
			case IDOK:
				memset(presetname, 0, 256);
				memset(presetval, 0, 1024);
				SendMessage(pnEdit, WM_GETTEXT, (WPARAM) 256, (LPARAM) presetname);
				SendMessage(pvEdit, WM_GETTEXT, (WPARAM) 1024, (LPARAM) presetval);

				if ( strcmp(presetname, "") == 0 ) {
					Error("You have to at least fill in a name!");
					return 0;
				}

				if ( IsWindowEnabled(pnEdit) ) {
					if ( !AddPreset(presetname, presetval) ) {
						Error("Error trying to add a preset.");
						return 0;
					}
					
					lvi.mask=LVIF_TEXT;
					lvi.pszText=presetname;
					lvi.iSubItem=0;
					lvi.iItem=0;
					if ( (index=ListView_InsertItem(presetList, &lvi)) == -1 ) {
						Error("Error trying to insert the preset item.");
						return 0;
					}
					ListView_SetItemText(presetList, index, 1, presetval);
				} else {
					if ( !SetPreset(presetname, presetval) ) {
						Error("Error trying to set a preset.");
						return 0;
					}

					ListView_SetItemText(presetList, LVINDEX, 1, presetval);
				}

				EnableWindow(ParentWnd, TRUE);

				DestroyWindow(hwndDlg);
				break;

			case IDCANCEL:
				EnableWindow(ParentWnd, TRUE);
				DestroyWindow(hwndDlg);
				break;
			}
			break;
		}
		break;
	};
	return 0;
}

bool GetNewPreset(HINSTANCE hInstance, HWND hwndParent)
{
	HWND presetWnd;

	// Create the dialog

	presetWnd=CreateDialog(hInstance, MAKEINTRESOURCE(IDD_PRESETDLG), hwndParent, presetDialogProc);
	if ( !presetWnd ) {
		Error("Error creating dialog box!");
		return false;
	}
	ShowWindow(presetWnd, SW_SHOW);

	pnEdit=GetDlgItem(presetWnd, IDC_PRESETNAME);
	if ( !pnEdit ) {
		Error("Error trying to retrieve handle of preset name edit control.");
		return false;
	}
	SendMessage(pnEdit, EM_SETLIMITTEXT, (WPARAM) 255, 0);

	pvEdit=GetDlgItem(presetWnd, IDC_PRESETVAL);
	if ( !pvEdit ) {
		Error("Error trying to retrieve handle of preset value edit control.");
		return false;
	}
	SendMessage(pvEdit, EM_SETLIMITTEXT, (WPARAM) 1023, 0);

	EnableWindow(hwndParent, FALSE);

	ParentWnd=hwndParent;

	SetFocus(pnEdit);

	return true;
}

bool ChangePreset(HINSTANCE hInstance, HWND hwndParent, int lvindex)
{
	HWND presetWnd;
	char presetname[257];
	char presetval[1025];

	ListView_GetItemText(presetList, lvindex, 0, presetname, 256);
	ListView_GetItemText(presetList, lvindex, 1, presetval, 1024);

	if ( strcmp(presetname, "") == 0 ) return true;

	// Create the dialog

	presetWnd=CreateDialog(hInstance, MAKEINTRESOURCE(IDD_PRESETDLG), hwndParent, presetDialogProc);
	if ( !presetWnd ) {
		Error("Error creating dialog box!");
		return false;
	}
	ShowWindow(presetWnd, SW_SHOW);

	pnEdit=GetDlgItem(presetWnd, IDC_PRESETNAME);
	if ( !pnEdit ) {
		Error("Error trying to retrieve handle of preset name edit control.");
		return false;
	}
	SendMessage(pnEdit, EM_SETLIMITTEXT, (WPARAM) 255, 0);
	SendMessage(pnEdit, WM_SETTEXT, 0, (LPARAM) presetname);
	EnableWindow(pnEdit, FALSE);

	pvEdit=GetDlgItem(presetWnd, IDC_PRESETVAL);
	if ( !pvEdit ) {
		Error("Error trying to retrieve handle of preset value edit control.");
		return false;
	}
	SendMessage(pvEdit, EM_SETLIMITTEXT, (WPARAM) 1023, 0);
	SendMessage(pvEdit, WM_SETTEXT, 0, (LPARAM) presetval);

	LVINDEX=lvindex;

	EnableWindow(hwndParent, FALSE);

	ParentWnd=hwndParent;

	SetFocus(pvEdit);

	return true;
}
