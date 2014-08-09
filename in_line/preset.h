
#ifndef PRESETH
#define PRESETH

#define PRESETKEY "Software\\Jasper van de Gronde\\LineIn plugin\\Presets"

#include <string>
using namespace std;

#include <windows.h>

extern const char* LINEINPLUGINKEY;

//BOOL GetPreset(const char * presetname, char * presetval);
bool GetPreset(const string& presetname, string& presetval);

bool AddPreset(const string& presetname, const string& presetval);

bool RenamePreset(const string& oldpresetname, const string& newpresetname);

bool SetPreset(const string& presetname, const string& presetval);

bool RemovePreset(const string& presetname);

bool GetNewPreset(HINSTANCE hInstance, HWND hwndParent);

bool ChangePreset(HINSTANCE hInstance, HWND hwndParent, int lvindex);

#endif // PRESETH
