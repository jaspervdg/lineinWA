
#ifndef INLINE_TOOLH
#define INLINE_TOOLH

#ifdef _MSC_VER
#pragma warning (disable : 4786) // Prevent very long warnings
#endif

// STL (and boost) includes
#include <string>
#include <map>
using namespace std;

// Include for expander
#include "expandlib/expandlib-expandable.h"
using namespace expander;

// Windows includes
#include <windows.h>
//#include <commctrl.h>
//#include <dsound.h>
//#include <stdio.h>

// Error and warning messages
//void Error(const char * msg);
//void Warning(const char * msg);

// ReportError reports a media error

void ReportError(MMRESULT mmresult, const char * state);

void stopme();

LPGUID StrToGuid(const char * const src, LPGUID lpGuid);
LPGUID StrToGuid(const std::string& src, LPGUID lpGuid);

string GuidToStr(LPGUID lpGuid);

struct DEVNAMEANDGUID {
	DEVNAMEANDGUID(string &n, LPGUID lpg, bool f) : name(n) { lpguid=lpg; found=f; };
	string &name;
	LPGUID lpguid;
	bool found;
};
/*
BOOL WINAPI GetDxDevNameDSEnumCallback(LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext);

string GetDxDevName(LPGUID lpguid);

bool GetWaveDevByAlias(const char * alias, long &devId);

BOOL WINAPI GetDxDevGuidDSEnumCallback(LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext);

bool GetDxDevByAlias(const string &alias, LPGUID &devGuid);
*/
#ifndef NEWEXPAND

// expandtitle function to expand the song title
// it removes any single backslashes and substitutes codes for values
BOOL expandtitle(string &title1, const string &title2, unsigned long srate, unsigned short bps, unsigned short nch, long leninms, SYSTEMTIME starttime, BOOL isstarttime, BOOL isstartdate, long numbufs, long bufsize, int bufferlen, int prebufferlen, BOOL curplaying, unsigned long device, BOOL mute, BOOL checks, int devtype, BOOL avoidstopproblem);

#endif // #ifndef NEWEXPAND
/*
#include <expandlib/expandlib-vartype-bool.h>
#include <expandlib/expandlib-vartype-numeric.h>
#include <expandlib/expandlib-vartype-text.h>
#include <expandlib/expandlib-vartype-time.h>
#include <expandlib/expandlib-vartype-date.h>

typedef expvartype_text<std::string> EVText;
typedef expvartype_bool<std::string> EVBool;
typedef expvartype_numeric<std::string,int> EVInt;
typedef expvartype_numeric<std::string,unsigned int> EVUInt;
typedef expvartype_numeric<std::string,long> EVLong;
typedef expvartype_numeric<std::string,unsigned long> EVULong;
typedef expvartype_numeric<std::string,short> EVShort;
typedef expvartype_numeric<std::string,unsigned short> EVUShort;
typedef expvartype_time<std::string> EVTime;
typedef expvartype_date<std::string> EVDate;
*/
//#include "option.h"

// typedefs for ParseOptionString (& co.)
//typedef pair<string,OptFlags> OptionAttrs;
typedef expandvartype<>* OptionAttrs;
typedef map<string,OptionAttrs> OptionType;
//typedef expandable<>::FVarMapType OptionType;
//typedef pair<string,string> ToggleAttrs;
//typedef map<string,ToggleAttrs> ToggleType;

// ParseOptionString
//template <char ischar, char sepchar, class StringType> bool ParseOptionString(const StringType &ostr, OptionType &options/*, unsigned int &optcount*/, const ToggleType &toggles, const unsigned int depth);

// parseparameters function
bool parseparameters(OptionType &options, const string &filename/*, int preset*/, unsigned long *srate, unsigned short *bps, unsigned short *nch, long *leninms, SYSTEMTIME *starttime, bool *isstarttime, bool *isstartdate, long *numbufs, long *bufsize, int *bufferlen, int *prebufferlen, string &title, string& titlefile, bool *showformat, int curplaying, unsigned long *device, bool *mute, bool *checks, int *devtype, size_t *chnmap, unsigned short *nch_out, long* triggertime, float* startthreshold, float* stopthreshold, float* pausethreshold);
//BOOL parseparameters(OptionType &options, char * filename, int preset, unsigned long *srate, unsigned short *bps, unsigned short *nch, long *leninms, SYSTEMTIME *starttime, BOOL *isstarttime, BOOL *isstartdate, long *numbufs, long *bufsize, int *bufferlen, int *prebufferlen, char ** title, BOOL *showformat, int curplaying, unsigned long *device, BOOL *mute, BOOL *checks, int *devtype, int *chnmap);

std::string& GetWindowText(HWND hwnd, std::string& str);
std::string GetWindowText(HWND hwnd);
BOOL SetWindowText(HWND hwnd, const std::string& str);

#endif //INLINE_TOOLH
