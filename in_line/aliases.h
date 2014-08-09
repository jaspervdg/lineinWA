
#ifndef ALIASESH_INC
#define ALIASESH_INC

#define SIL_MAXALIASSIZE 50

#include <string>
using namespace std;

#include <windows.h>

bool	GetDevAlias(const char * id, char * alias, const int devtype);
bool	DeleteAliasById(const char * id, const int devtype);
bool	DeleteAliasByAlias(const char * alias, const int devtype);
bool	GetDevStr(const char * alias, char * id, const int devtype);
bool	GetDevStr(const std::string &alias, std::string &id, const int devtype);
bool	SetDevAlias(const char * id, const char * newalias, const int devtype);

BOOL	ChangeDevAlias(HINSTANCE hInstance, HWND hwndParent, int lvindex, int devtype);

#define WAVE_DEVICE 0
#define DIRECTX_DEVICE 1

#endif //#ifndef ALIASESH_INC
