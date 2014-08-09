
#include "tool.h"

#include "in.h"

#include <boost/lexical_cast.hpp>
using namespace boost;

#include <dsound.h>

// Own inc's
#include "../alg/gen.h"
#include "aliases.h"
#include "preset.h"
struct In_Module;

#include "wa_ipc.h"

// Extern vars
extern int				paused;
extern int				Playing;
extern int				Stopping;
extern In_Module mod;
extern char lastfn[MAX_PATH];
extern HINSTANCE hInstance;
extern int Loaded;
typedef HRESULT (WINAPI * DIRECTSOUNDCAPTURECREATE)(LPGUID lpGUID, LPDIRECTSOUNDCAPTURE *lplpDSC, LPUNKNOWN pUnkOuter);
typedef HRESULT (WINAPI * DIRECTSOUNDCAPTUREENUMERATE)(LPDSENUMCALLBACK lpDSEnumCallback, LPVOID lpContext);
extern DIRECTSOUNDCAPTURECREATE			DxSndCapCreate;
extern DIRECTSOUNDCAPTUREENUMERATE		DxSndCapEnumerate;
extern bool						dxsoundsupport;

//void Error(const char * msg) { MessageBox(NULL, msg, "Error - Line-In plugin", MB_SYSTEMMODAL | MB_ICONERROR); }
//void Warning(const char * msg) { MessageBox(NULL, msg, "Warning - Line-In plugin", MB_SYSTEMMODAL | MB_ICONWARNING); }

// ReportError reports a media error

void ReportError(MMRESULT mmresult, const char * state)
{
	string temp;

	temp="Line-In plugin produced the following error:\n  ";

	switch(mmresult) {
	case MMSYSERR_NOERROR:
		temp+="No error";
		break;
	case MMSYSERR_ERROR:
        temp+="Unspecified error";
		break;
	case MMSYSERR_BADDEVICEID:
		temp+="Device ID out of range";
		break;
	case MMSYSERR_NOTENABLED:
		temp+="Driver failed enable";
		break;
	case MMSYSERR_ALLOCATED:
		temp+="Device already allocated";
		break;
	case MMSYSERR_INVALHANDLE:
		temp+="Device handle is invalid";
		break;
	case MMSYSERR_NODRIVER:
		temp+="No device driver present";
		break;
	case MMSYSERR_NOMEM:
		temp+="Memory allocation error";
		break;
	case MMSYSERR_NOTSUPPORTED:
		temp+="Function isn't supported";
		break;
	case MMSYSERR_BADERRNUM:
		temp+="Error value out of range";
		break;
	case MMSYSERR_INVALFLAG:
		temp+="Invalid flag passed";
		break;
	case MMSYSERR_INVALPARAM:
		temp+="Invalid parameter passed";
		break;
	case MMSYSERR_HANDLEBUSY:
		temp+="Handle being used simultaneously on another thread";
		break;
	case MMSYSERR_INVALIDALIAS:
		temp+="Specified alias not found";
		break;
	case MMSYSERR_BADDB:
		temp+="Bad registry database";
		break;
	case MMSYSERR_KEYNOTFOUND:
		temp+="Registry key not found";
		break;
	case MMSYSERR_READERROR:
		temp+="Registry read error";
		break;
	case MMSYSERR_WRITEERROR:
		temp+="Registry write error";
		break;
	case MMSYSERR_DELETEERROR:
		temp+="Registry delete error";
		break;
	case MMSYSERR_VALNOTFOUND:
		temp+="Registry value not found";
		break;
	case MMSYSERR_NODRIVERCB:
		temp+="Driver does not call DriverCallback";
		break;

	case WAVERR_BADFORMAT:
		temp+="Unsupported wave format";
		break;
	case WAVERR_STILLPLAYING:
		temp+="Still something playing";
		break;
	case WAVERR_UNPREPARED:
		temp+="Header not prepared";
		break;
	case WAVERR_SYNC:
		temp+="Device is synchronous";
		break;

	case DS_NO_VIRTUALIZATION:
		temp+="The call succeeded, but the 3D algorithm had to be substituted";
		break;

//	case DS_INCOMPLETE:
//		temp+="The call succeeded, but not all of the optional effects were obtained";
//		break;

	case DSERR_ALLOCATED:
		temp+="Resources already in use";
		break;

	case DSERR_CONTROLUNAVAIL:
		temp+="The control (vol, pan, etc.) requested by the caller is not available";
		break;

	case DSERR_INVALIDPARAM:
		temp+="An invalid parameter was passed to the returning function";
		break;

	case DSERR_INVALIDCALL:
		temp+="Call is not valid for the current state of the object";
		break;

	case DSERR_GENERIC:
		temp+="An undetermined error occurred inside the DirectSound subsystem";
		break;

	case DSERR_PRIOLEVELNEEDED:
		temp+="The caller does not have the priority level required for the function to succeed";
		break;

	case DSERR_OUTOFMEMORY:
		temp+="Not enough free memory is available to complete the operation";
		break;

	case DSERR_BADFORMAT:
		temp+="The specified WAVE format is not supported";
		break;

	case DSERR_UNSUPPORTED:
		temp+="The called function is not supported at this time";
		break;

	case DSERR_NODRIVER:
		temp+="No sound driver is available";
		break;

	case DSERR_ALREADYINITIALIZED:
		temp+="This object is already initialized";
		break;

	case DSERR_NOAGGREGATION:
		temp+="This object does not support aggregation";
		break;

	case DSERR_BUFFERLOST:
		temp+="The buffer memory has been lost";
		break;

	case DSERR_OTHERAPPHASPRIO:
		temp+="Another application has a higher priority level";
		break;

	case DSERR_UNINITIALIZED:
		temp+="This object has not been initialized";
		break;

	case DSERR_NOINTERFACE:
		temp+="The requested COM interface is not available";
		break;

	case DSERR_ACCESSDENIED:
		temp+="Access is denied";
		break;

	case DSERR_BUFFERTOOSMALL:
		temp+="Tried to create a DSBCAPS_CTRLFX buffer shorter than DSBSIZE_FX_MIN milliseconds";
		break;

	case DSERR_DS8_REQUIRED:
		temp+="Attempt to use DirectSound 8 functionality on an older DirectSound object";
		break;

	case DSERR_SENDLOOP:
		temp+="A circular loop of send effects was detected";
		break;

	case DSERR_BADSENDBUFFERGUID:
		temp+="The GUID specified in an audiopath file does not match a valid MIXIN buffer";
		break;

	case DSERR_OBJECTNOTFOUND:
		temp+="The object requested was not found";
		break;


	default:
		temp+="Some kind of unknown error";
	}

	temp+="\n  Error number: ";
	try {
		temp+=lexical_cast<string>(mmresult);
//		mmresult, temp+strlen(temp), 10);
	} catch(bad_lexical_cast) {
		temp+="Error converting number!";
	}

	if ( state ) {
		temp+="\n";
		temp+=state;
	}

	Error(temp.c_str());
}

void stopme() {
	long t;

	if (Stopping) return;
	Stopping=TRUE;

	for(t=0 ;mod.outMod->IsPlaying() && t<50; t++, Sleep(10));

	PostMessage(mod.hMainWindow,WM_WA_MPEG_EOF,0,0);

	return;
}

LPGUID StrToGuid(const char* const src, LPGUID lpGuid)
{
	return StrToGuid(std::string(src), lpGuid);
}

LPGUID StrToGuid(const std::string& src, LPGUID lpGuid)
{
/*typedef struct  _GUID
    {
    DWORD Data1;
    WORD Data2;
    WORD Data3;
    BYTE Data4[ 8 ];
    }	GUID;*/
	int t;

	if ( src.size() != 38 ) {
		memset(lpGuid, 0, sizeof(GUID));
		return lpGuid;
	}

	lpGuid->Data1=strtoul(src.c_str()+1, NULL, 16);

	lpGuid->Data2=(WORD)strtoul(src.c_str()+10, NULL, 16);

	lpGuid->Data3=(WORD)strtoul(src.c_str()+15, NULL, 16);

	lpGuid->Data4[0]=(BYTE)strtoul(src.substr(20,2).c_str(), NULL, 16);

	lpGuid->Data4[1]=(BYTE)strtoul(src.substr(22,2).c_str(), NULL, 16);

	for(t=2; t<8; t++)
		lpGuid->Data4[t]=(BYTE)strtoul(src.substr(25+((t-2)*2),2).c_str(), NULL, 16);

	return lpGuid;
}

string GuidToStr(LPGUID lpGuid)
{
/*typedef struct  _GUID
    {
    DWORD Data1;
    WORD Data2;
    WORD Data3;
    BYTE Data4[ 8 ];
    }	GUID;*/
	string dest;

	char temp[20];
	unsigned int t, tlen, t2;

	if ( lpGuid == NULL ) {
		return dest;
	}
	
	dest+="{";

	_ultoa(lpGuid->Data1, temp, 16);
	tlen=dest.size();
	for(t=tlen;t<tlen+8-strlen(temp);t++) dest+="0";
	dest+=convtoupr(temp);

	dest+="-";

	_ultoa(lpGuid->Data2, temp, 16);
	tlen=dest.size();
	for(t=tlen;t<tlen+4-strlen(temp);t++) dest+="0";
	dest+=convtoupr(temp);

	dest+="-";

	_ultoa(lpGuid->Data3, temp, 16);
	tlen=dest.size();
	for(t=tlen;t<tlen+4-strlen(temp);t++) dest+="0";
	dest+=convtoupr(temp);

	dest+="-";

	_ultoa(lpGuid->Data4[0], temp, 16);
	tlen=dest.size();
	for(t=tlen;t<tlen+2-strlen(temp);t++) dest+="0";
	dest+=convtoupr(temp);

	_ultoa(lpGuid->Data4[1], temp, 16);
	tlen=dest.size();
	for(t=tlen;t<tlen+2-strlen(temp);t++) dest+="0";
	dest+=convtoupr(temp);

	dest+="-";

	for(t2=2;t2<8;t2++) {
		_ultoa(lpGuid->Data4[t2], temp, 16);
		tlen=dest.size();
		for(t=tlen;t<tlen+2-strlen(temp);t++) dest+="0";
		dest+=convtoupr(temp);
	}

	dest+="}";

	return dest;
}

/*struct DEVNAMEANDGUID {
	DEVNAMEANDGUID(string &n, LPGUID lpg, bool f) : name(n) { lpguid=lpg; found=f; };
	string &name;
	LPGUID lpguid;
	bool found;
};*/

BOOL WINAPI GetDxDevNameDSEnumCallback(LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext)
{
	DEVNAMEANDGUID * inf=(DEVNAMEANDGUID*)lpContext;

	if ( lpGuid == NULL ) {
		if ( inf->lpguid == NULL ) {
			inf->name=lpcstrDescription;
			return FALSE;
		}
	} else if ( inf->lpguid != NULL ) {
		if ( inf->lpguid->Data1 == lpGuid->Data1 && inf->lpguid->Data2 == lpGuid->Data2 && inf->lpguid->Data3 == lpGuid->Data3 && *((DWORD*)(inf->lpguid->Data4)) == *((DWORD*)(lpGuid->Data4)) && *(((DWORD*)(inf->lpguid->Data4))+1) == *(((DWORD*)(lpGuid->Data4))+1) ) {
			inf->name=lpcstrDescription;
			return FALSE;
		}
	}

	return TRUE;
}

string GetDxDevName(LPGUID lpguid)
{
	string dest;
	DEVNAMEANDGUID inf(dest,lpguid,false);
	DxSndCapEnumerate(GetDxDevNameDSEnumCallback, &inf);
	return dest;
}

bool GetWaveDevByAlias(const char * alias, long &devId)
{
	char devname[1024];
	if ( !GetDevStr(alias, devname, WAVE_DEVICE) ) strcpy(devname, alias);

	WAVEINCAPS waveInCaps;

	waveInGetDevCaps(WAVE_MAPPER, &waveInCaps, sizeof(waveInCaps));
	if ( lstrcmpi(waveInCaps.szPname, devname) == 0 ) {
		devId=WAVE_MAPPER;
		return true;
	}

	for(int t=waveInGetNumDevs()-1; t>=0; t--) {
		waveInGetDevCaps(t, &waveInCaps, sizeof(waveInCaps));
		if ( lstrcmpi(waveInCaps.szPname, devname) == 0 ) {
			devId=t;
			return true;
		}
	}

	return false;
}

BOOL WINAPI GetDxDevGuidDSEnumCallback(LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext)
{
	DEVNAMEANDGUID * inf=(DEVNAMEANDGUID*)lpContext;

	if ( lpcstrDescription == inf->name ) {
		if ( lpGuid ) {
			inf->lpguid=(LPGUID)malloc(sizeof(GUID));
			if ( inf->lpguid == NULL ) {
				Error("Unable to allocate memory for GUID.");
				return FALSE;
			}
			memcpy(inf->lpguid, lpGuid, sizeof(GUID));
		} else {
			inf->lpguid=NULL;
		}
		inf->found=true;
		return FALSE;
	}

	return TRUE;
}

bool GetDxDevByAlias(const string &alias, LPGUID &devGuid)
{
	string devname;
	if ( !GetDevStr(alias, devname, DIRECTX_DEVICE) ) devname=alias;

	DEVNAMEANDGUID inf(devname,NULL,false);

	DxSndCapEnumerate(GetDxDevGuidDSEnumCallback, &inf);

	devGuid=inf.lpguid;

	return inf.found;
}

#ifndef NEWEXPAND

// expandtitle function to expand the song title
// it removes any single backslashes and substitutes codes for values
BOOL expandtitle(string &title1, const string &title2, unsigned long srate, unsigned short bps, unsigned short nch, long leninms, SYSTEMTIME starttime, BOOL isstarttime, BOOL isstartdate, long numbufs, long bufsize, int bufferlen, int prebufferlen, BOOL curplaying, unsigned long device, BOOL mute, BOOL checks, int devtype, BOOL avoidstopproblem) {
	string::const_iterator t2=title2.begin();

	BOOL backslash=FALSE;
	long percent=0;
	BOOL hex=FALSE;
	
	string tempbuf;
	char pcbuf;
	SYSTEMTIME systime;
	WAVEINCAPS waveInCaps;
	
	title1="";

	for(; t2!=title2.end(); t2++) {
		if ( backslash ) {
			title1+=*t2;
			backslash=FALSE;
		} else if ( percent ) {
			switch(percent) {
			case 1:
				pcbuf=*t2;
				percent++;
				break;
			case 2:
				if(pcbuf=='x') {
					pcbuf=*t2;
					hex=TRUE;
				} else {
					percent=0;
					
					if(hex) {
						tempbuf[0]=hextoc(pcbuf,*t2);
						tempbuf[1]=0;
						hex=FALSE;
					} else {
						
						tempbuf="";
						switch(pcbuf) {
						case 's':
							switch(*t2) {
							case 'r':
								try {
									tempbuf=lexical_cast<string>(srate);
								} catch(bad_lexical_cast) {
									tempbuf="0";
								}
								break;
							case 't':
								if ( isstarttime ) {
									char tempbuffer[50];
									GetTimeFormat(LOCALE_USER_DEFAULT, 0, &starttime, NULL, tempbuffer, 45);
									tempbuf=tempbuffer;
								} else {
									tempbuf="(no starting time)";
								}
								break;
							case 'd':
								if ( isstartdate ) {
									char tempbuffer[50];
									GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &starttime, NULL, tempbuffer, 45);
									tempbuf=tempbuffer;
								} else {
									tempbuf="(no starting date)";
								}
								break;
							}
							break;
						case 'c':
							switch(*t2) {
							case 't':
								if ( nch < 3 ) {
									tempbuf=( (nch==1) ? "Mono" : "Stereo" );
								} else {
									try {
										tempbuf=lexical_cast<string>(nch);
									} catch(bad_lexical_cast) {
										tempbuf="0";
									}
									tempbuf+=" channels";
								}
								break;
							case 'n':
								try {
									tempbuf=lexical_cast<string>(nch);
								} catch(bad_lexical_cast) {
									tempbuf="0";
								}
								break;
							case 'k':
								if ( checks ) {
									tempbuf="checks";
								} else {
									tempbuf="no checks";
								}
								break;
							}
							break;
						case 'b':
							switch(*t2) {
							case 'i':
								try {
									tempbuf=lexical_cast<string>(bps);
								} catch(bad_lexical_cast) {
									tempbuf="0";
								}
								break;
							case 'r':
								try {
									tempbuf=lexical_cast<string>((srate*(((bps/8) + (bps % 8?1:0))*nch))/1000);
								} catch(bad_lexical_cast) {
									tempbuf="0";
								}
								break;
							case 'n':
								try {
									tempbuf=lexical_cast<string>(numbufs);
								} catch(bad_lexical_cast) {
									tempbuf="0";
								}
								break;
							case 's':
								try {
									tempbuf=lexical_cast<string>(bufsize);
								} catch(bad_lexical_cast) {
									tempbuf="0";
								}
								break;
							}
							break;
						case 'l':
							switch(*t2) {
							case 't':
								if ( leninms>-1 ) {
									tempbuf=mstotime(leninms, TRUE).c_str();
								} else {
									tempbuf="forever";
								}
								break;
							case 'n':
								if ( leninms>-1 ) {
									try {
										tempbuf=lexical_cast<string>(leninms);
									} catch(bad_lexical_cast) {
										tempbuf="0";
									}
								} else {
									tempbuf="";
								}
								break;
							}
							break;
						case 't':
							if(!curplaying) {
								tempbuf="(time)";
							} else {
								char tempbuffer[50];
								GetLocalTime(&systime);
								switch(*t2) {
								case 'h':
									GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOMINUTESORSECONDS, &systime, NULL, tempbuffer, 254);
									tempbuf=tempbuffer;
									break;
								case 'm':
									GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &systime, NULL, tempbuffer, 254);
									tempbuf=tempbuffer;
									break;
								case 's':
									GetTimeFormat(LOCALE_USER_DEFAULT, 0, &systime, NULL, tempbuffer, 254);
									tempbuf=tempbuffer;
									break;
								case 'l':
									tempbuf=mstotime((systime.wHour*3600000)+(systime.wMinute*60000)+(systime.wSecond*1000)+systime.wMilliseconds, TRUE);
									break;
								}
							}
							break;
						case 'd':
							GetLocalTime(&systime);
							switch(*t2) {
							case 'l':
								if(!curplaying) {
									tempbuf="(date)";
								} else {
									char tempbuffer[50];
									GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &systime, NULL, tempbuffer, 45);
									tempbuf=tempbuffer;
								}
								break;
							case 's':
								if(!curplaying) {
									tempbuf="(date)";
								} else {
									char tempbuffer[50];
									GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &systime, NULL, tempbuffer, 45);
									tempbuf=tempbuffer;
								}
								break;
							case 'n':
								switch(devtype) {
								case WAVE_DEVICE:
									waveInGetDevCaps(device, &waveInCaps, sizeof(WAVEINCAPS));
									tempbuf=waveInCaps.szPname;
									break;
								case DIRECTX_DEVICE:
									tempbuf=GetDxDevName((LPGUID)device);
									break;
								}
								break;
							case 'i':
								switch(devtype) {
								case WAVE_DEVICE:
									try {
										tempbuf=lexical_cast<string>(device);
									} catch(bad_lexical_cast) {
										tempbuf="0";
									}
									break;
								case DIRECTX_DEVICE:
									tempbuf=GuidToStr((LPGUID)device);
									break;
								}
								break;
							case 't':
								switch(devtype) {
								case WAVE_DEVICE:
									tempbuf="Standard";
									break;
								case DIRECTX_DEVICE:
									tempbuf="DirectSound";
									break;
								default:
									tempbuf="Illegal device type";
								}
								break;
							}
							break;
						case 'm':
							switch(*t2) {
							case 'u':
								if ( mute ) tempbuf="muted";
								break;
							case 't':
								if ( mute ) {
									tempbuf="muted";
								} else {
									tempbuf="not muted";
								}
								break;
							}
							break;
						case 'o':
							switch(*t2) {
							case 'b':
								try {
									tempbuf=lexical_cast<string>(bufferlen);
								} catch(bad_lexical_cast) {
									tempbuf="0";
								}
								break;
							case 'p':
								try {
									tempbuf=lexical_cast<string>(prebufferlen);
								} catch(bad_lexical_cast) {
									tempbuf="0";
								}
								break;
							case 't':
								if ( bufferlen>-1 ) {
									tempbuf=mstotime(bufferlen, TRUE);
								} else {
									tempbuf="none";
								}
								break;
							case 'u':
								if ( prebufferlen>-1 ) {
									tempbuf=mstotime(prebufferlen, TRUE);
								} else {
									tempbuf="none";
								}
								break;
							}
							break;
						case 'a':
							switch(*t2) {
							case 's':
								tempbuf=avoidstopproblem ? "avoiding stop problem" : "";
								break;
							}
							break;

						default:
							tempbuf="(error)";
							break;
						}
					}
					title1+=tempbuf;
				}
				break;
			default:
				percent=0;
			}
		} else if ( *t2 == '\\' ) {
			backslash=TRUE;
		} else if ( *t2 == '%' ) {
			percent=1;
		} else {
			title1+=*t2;
		}
	}
	
	return TRUE;
}
#endif // #ifndef NEWEXPAND

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

//#include "option.h"

// typedefs for ParseOptionString (& co.)
//typedef pair<string,OptFlags> OptionAttrs;
typedef expandvartype<>* OptionAttrs;
typedef map<string,OptionAttrs> OptionType;
//typedef expandable<>::FVarMapType OptionType;
typedef pair<string,string> ToggleAttrs;
typedef map<string,ToggleAttrs> ToggleType;

// ParseOptionString
template <char ischar, char sepchar, class StringType> bool ParseOptionString(const StringType::const_iterator &ostrbeg, const StringType::const_iterator &ostrend, OptionType &options/*, unsigned int &optcount*/, const ToggleType &toggles, const unsigned int depth)
{
	int mode=0;
	bool escaped=false;

	string presetval;

	StringType curname;
	StringType curval;

	for(StringType::const_iterator t=ostrbeg; t!=ostrend; t++) {
		switch(mode) {
		case 0:	// Searching for option name
			if ( *t == '\\' ) {
				escaped=true;
				mode=1; // Go search for ischar
				curname="";
			} else if ( *t != ' ' ) {
				mode=1; // Go search for ischar
				curname="";
				t--;	// If this char was an ischar we want to detect it
			}
			break;

		case 1:	// Searching for ischar
			if ( *t == '\\' && !escaped ) {
				escaped=true;
			} else if ( *t == ischar && !escaped ) {
				mode=2; // Go search for sepchar
				curval="";
			} else if ( *t == sepchar && !escaped ) { // Flag
				ToggleType::const_iterator it=toggles.find(curname);
//				optcount++;
				if ( it != toggles.end() ) {
					OptionType::iterator optit=options.find((*it).second.first);
					if ( optit != options.end() ) {
						(*optit).second->SetValue((*it).second.second);
					}
					// TODO: Handle a var that was NOT found
				} else {
					OptionType::iterator optit=options.find(curname);
					if ( optit != options.end() ) {
						(*optit).second->SetValue("");
					}
					// TODO: Handle a var that was NOT found
				}
				mode=0;	// Go search for option name
			} else {
				curname+=*t;
				if ( escaped ) escaped=false;
			}
			break;

		case 2:	// Searching for sepchar
			if ( *t == '\\' && !escaped ) {
				escaped=true;
			} else if ( *t == sepchar && !escaped ) {
				if ( curname == "preset" ) { // Handle presets
					if ( depth >= 100 ) {
						Error("I detected a possible preset loop, check your presets and try again.");
						return false;
					}
					if ( !GetPreset(curval, presetval) ) {
						Error("Error getting preset! Unable to parse parameters.");
						return false;
					}
					if ( !ParseOptionString<ischar,sepchar, StringType>(presetval.begin(), presetval.end(), options/*, optcount*/, toggles, depth+1) ) {
						return false;
					}
				} else if ( curname.size() > 0 ) { // No NULL options
//					optcount++;
					OptionType::iterator optit=options.find(curname);
					if ( optit != options.end() ) {
						(*optit).second->SetValue(curval);
					}
					// TODO: Handle a var that was NOT found
				}
				mode=0;	// Go search for option name
			} else {
				curval+=*t;
				if ( escaped ) escaped=false;
			}
			break;
		}
	}
	switch(mode) {
	case 1:	// Searching for ischar
		{
			ToggleType::const_iterator it=toggles.find(curname);
//			optcount++;
			if ( it != toggles.end() ) {
				OptionType::iterator optit=options.find((*it).second.first);
				if ( optit != options.end() ) {
					(*optit).second->SetValue((*it).second.second);
				}
				// TODO: Handle a var that was NOT found
			} else {
				OptionType::iterator optit=options.find(curname);
				if ( optit != options.end() ) {
					(*optit).second->SetValue("");
				}
				// TODO: Handle a var that was NOT found
			}
		}
		break;

	case 2:	// Searching for sepchar
		if ( curname == "preset" ) { // Handle presets
			if ( depth >= 100 ) {
				Error("I detected a possible preset loop, check your presets and try again.");
				return false;
			}
			if ( !GetPreset(curval.c_str(), presetval) ) {
				Error("Error getting preset! Unable to parse parameters.");
				return false;
			}
//			if ( !ParseOptionString<ischar,sepchar>(string(presetval), options/*, optcount*/, toggles, depth+1) ) return false;
			if ( !ParseOptionString<ischar,sepchar, StringType>(presetval.begin(), presetval.end(), options/*, optcount*/, toggles, depth+1) ) {
				return false;
			}
		} else if ( curname.size() > 0 ) { // No NULL options
//			optcount++;
			OptionType::iterator optit=options.find(curname);
			if ( optit != options.end() ) {
				(*optit).second->SetValue(curval);
			}
			// TODO: Handle a var that was NOT found
		}
		break;
	}

	return true;
}

// parseparameters function
bool parseparameters(OptionType &options, const string &filename/*, int preset*/, unsigned long *srate, unsigned short *bps, unsigned short *nch, long *leninms, SYSTEMTIME *starttime, bool *isstarttime, bool *isstartdate, long *numbufs, long *bufsize, int *bufferlen, int *prebufferlen, string &title, string &titlefile, bool *showformat, int curplaying, unsigned long *device, bool *mute, bool *checks, int *devtype, size_t *chnmap, unsigned short *nch_out, long* triggertime, float* startthreshold, float* stopthreshold, float* pausethreshold)
{
	// Parse the string
//	OptionType options;
//	unsigned int optcount=0;
//	unsigned int numprocessedoptions=0;
//	string ostr(filename.substr(7, filename.npos));
	ToggleType toggles;

//	ostr=filename+7;

	options.clear();

//	options["srate"]=new LimitedNumericValue<unsigned long,1,768000>(44100);
//	options["bps"]=new LimitedNumericValue<unsigned short,1,64>(16);
//	options["nch"]=new LimitedNumericValue<unsigned short,1,32>(2);

	options["srate"]=new expvartype_numeric<string, unsigned long,
		expvartype_numeric_traits_limited_with_default<unsigned long, 1, 768000, 44100> >();
	options["bps"]=new expvartype_numeric<string, unsigned short,
		expvartype_numeric_traits_limited_with_default<unsigned short, 1, 64, 16> >();
	options["nch"]=new expvartype_numeric<string, unsigned short,
		expvartype_numeric_traits_limited_with_default<unsigned short, 1, 32, 2> >();
	options["time"]=new EVTime("-");
	options["stime"]=new EVTime("-");
	options["sdate"]=new EVDate("-");
	options["numbuffers"]=new expvartype_numeric<string, long,
		expvartype_numeric_traits_limited_with_default<long, 1, 1000, 10> >;
	options["bufsize"]=new EVText("s1152");
	options["title"]=new EVText("Line-In");
	options["titlefile"]=new EVText("");
	options["showformat"]=new EVBool("true"); // not documented
	options["dev"]=new EVText("-1");
	options["ismuted"]=new EVBool("false"); // not documented
	options["outbuflen"]=new EVTime("-");
	options["outprebuflen"]=new EVTime("-");
	options["performchecks"]=new EVBool("true"); // not documented
//	options["preset"]=new EVText("");
	options["chnmap"]=new EVText("");

	// new options
	options["triggertime"]=new EVTime("-");
	options["startthreshold"]=new expvartype_numeric<string, float, expvartype_numeric_traits_limited_with_default<float, 0, 1, -1> >();
	options["pausethreshold"]=new expvartype_numeric<string, float, expvartype_numeric_traits_limited_with_default<float, 0, 1, -1> >();
	options["stopthreshold"]=new expvartype_numeric<string, float, expvartype_numeric_traits_limited_with_default<float, 0, 1, -1> >();

/*
	options["srate"]=OptionAttrs("44100");
	options["bps"]=OptionAttrs("16",OptFlags(0,false));
	options["nch"]=OptionAttrs("2",OptFlags(0,false));
	options["time"]=OptionAttrs("-",OptFlags(0,false));
	options["stime"]=OptionAttrs("-",OptFlags(0,false));
	options["sdate"]=OptionAttrs("-",OptFlags(0,false));
	options["numbuffers"]=OptionAttrs("10",OptFlags(0,false));
	options["bufsize"]=OptionAttrs("s1152",OptFlags(0,false));
	options["title"]=OptionAttrs("Line-In",OptFlags(0,false));
	options["showformat"]=OptionAttrs("true",OptFlags(0,false)); // not documented
	options["dev"]=OptionAttrs("-1",OptFlags(0,false));
	options["ismuted"]=OptionAttrs("false",OptFlags(0,false)); // not documented
	options["outbuflen"]=OptionAttrs("-",OptFlags(0,false));
	options["outprebuflen"]=OptionAttrs("-",OptFlags(0,false));
	options["performchecks"]=OptionAttrs("true",OptFlags(0,false)); // not documented
//	options["preset"]=OptionAttrs("",OptFlags(0,false));
	options["chnmap"]=OptionAttrs("",OptFlags(0,false));
*/
	// NOTE: chnmap="" is now the same as nochnmap

	toggles["showformat"]=ToggleAttrs("showformat","true");
	toggles["hideformat"]=ToggleAttrs("showformat","false");
	toggles["mono"]=ToggleAttrs("nch","1");
	toggles["stereo"]=ToggleAttrs("nch","2");
	toggles["notime"]=ToggleAttrs("time","-");
	toggles["nostime"]=ToggleAttrs("stime","-");
	toggles["nosdate"]=ToggleAttrs("sdate","-");
	toggles["mute"]=ToggleAttrs("ismuted","true");
	toggles["unmute"]=ToggleAttrs("ismuted","false");
	toggles["checks"]=ToggleAttrs("performchecks","yes");
	toggles["nochecks"]=ToggleAttrs("performchecks","no");
	toggles["nochnmap"]=ToggleAttrs("chnmap","");

	if ( !ParseOptionString<'=',',',string>(filename.begin()+7, filename.end(), options/*, optcount*/, toggles, 0) ) return FALSE;

	// Fill all the parameters
	OptionType::iterator optit, optit2, optit3;

	// Set some defaults (makes the if's smaller)
//	*srate=44100;
//	*bps=16;
//	*nch=2;
	*leninms=-1000;
	*numbufs=10;
	*bufsize=4608; // This is overridden by the else of the bufsize option below
	*bufferlen=-1;
	*prebufferlen=-1;
	*mute=FALSE;
	*isstarttime=FALSE;
	*isstartdate=FALSE;
	*checks=TRUE;
	*showformat=TRUE;
	*device=WAVE_MAPPER;
	*devtype=WAVE_DEVICE;
	GetLocalTime(starttime);

	// Integer/numeric values

	*srate=reinterpret_cast< expvartype_numeric<string, unsigned long,
		expvartype_numeric_traits_limited_with_default<unsigned long, 1, 768000, 44100> >* >
		(options["srate"])->get_value();

	*bps=reinterpret_cast< expvartype_numeric<string, unsigned short,
		expvartype_numeric_traits_limited_with_default<unsigned short, 1, 64, 16> >* >
		(options["bps"])->get_value();

	*nch=reinterpret_cast< expvartype_numeric<string, unsigned short,
		expvartype_numeric_traits_limited_with_default<unsigned short, 1, 32, 2> >* >
		(options["nch"])->get_value();

	*numbufs=reinterpret_cast< expvartype_numeric<string, long,
		expvartype_numeric_traits_limited_with_default<long, 1, 1000, 10> >* >
		(options["numbuffers"])->get_value();

	*startthreshold=reinterpret_cast< expvartype_numeric<string, float,
		expvartype_numeric_traits_limited_with_default<float, 0, 1, .01> >* >
		(options["startthreshold"])->get_value();

	*pausethreshold=reinterpret_cast< expvartype_numeric<string, float,
		expvartype_numeric_traits_limited_with_default<float, 0, 1, -1> >* >
		(options["pausethreshold"])->get_value();

	*stopthreshold=reinterpret_cast< expvartype_numeric<string, float,
		expvartype_numeric_traits_limited_with_default<float, 0, 1, .001> >* >
		(options["stopthreshold"])->get_value();

/*
	{
		unsigned long val=((EVULong*)options["srate"])->get_value();
		if ( val >= 1 && val <= 768000 ) *srate=val;
	}
	{
		unsigned short val=((EVUShort*)options["bps"])->get_value();
		if ( val >= 1 && val <= 64 ) *bps=val;
	}
	{
		unsigned short val=((EVUShort*)options["nch"])->get_value();
		if ( val >= 1 && val <= 32 ) *nch=val;
	}
	{
		unsigned long val=((EVLong*)options["numbuffers"])->get_value();
		if ( val >= 1 && val <= 1000 ) *numbufs=val;
	}
*/

	// Buffersize

	{
		if ( (((EVText*)options["bufsize"])->get_string())[0] == 't' ) {
			long val=timetoms((((EVText*)options["bufsize"])->get_string()).c_str()+1);
			if ( val >= 1 && val <= 10000 ) {
				__int64 temp;
				temp=__int64(val) * __int64(*srate);
				temp*=*nch * ((*bps / 8) + (*bps % 8?1:0) );
				*bufsize=(long)(temp/1000);
			}
		} else if ( (((EVText*)options["bufsize"])->get_string())[0] == 's' ) {
			long val;
			try {
				val=lexical_cast<long>((((EVText*)options["bufsize"])->get_string()).c_str()+1);
			} catch(bad_lexical_cast) {
				val=1152;
			}
			if ( val >= 1 && val <= 1048576 ) *bufsize=val * ( *nch * ( (*bps / 8) + (*bps % 8?1:0) ) );
		} else {
			long val;
			try {
				val=lexical_cast<long>((((EVText*)options["bufsize"])->get_string()));
			} catch(bad_lexical_cast) {
				val=1152 * ( *nch * ( (*bps / 8) + (*bps % 8?1:0) ) );
			}
			if ( val >= 1 && val <= 1048576 ) *bufsize=val;
		}
	}


	// Time/date values

	{
		long lvalue=((EVTime*)options["outbuflen"])->get_milliseconds();
		if ( lvalue >= -1 ) *bufferlen=lvalue;
	}

	{
		long lvalue=((EVTime*)options["outprebuflen"])->get_milliseconds();
		if ( lvalue <= *bufferlen && lvalue >= -1 ) *prebufferlen=lvalue;
	}

	{
		long lvalue=((EVTime*)options["time"])->get_milliseconds();
		if ( lvalue >= 0 ) *leninms=lvalue; else *leninms=-1000;
	}

	{
		long lvalue=((EVTime*)options["triggertime"])->get_milliseconds();
		if ( lvalue >= -1 ) *triggertime=lvalue;
	}

	GetLocalTime(starttime); // This is needed for the stime and sdate options (you can have an stime without an sdate and vice versa)

	if ( ((EVTime*)options["stime"])->get_string() == "-" ) {
		*isstarttime=FALSE;
	} else {
		timetosystime(((EVTime*)options["stime"])->get_string(), starttime);
		*isstarttime=TRUE;
	}

	if ( ((EVDate*)options["sdate"])->get_string() == "-" ) {
		*isstartdate=FALSE;
	} else {
		datetosystime(((EVDate*)options["sdate"])->get_string(), starttime);
		*isstartdate=TRUE;
	}

	
	// The title

	{
		title=((EVText*)options["title"])->get_string();
	}

	{
		titlefile=((EVText*)options["titlefile"])->get_string();
	}

	
	// Now for some booleans

	*showformat=((EVBool*)options["showformat"])->get_value();
	*mute=((EVBool*)options["ismuted"])->get_value();
	*checks=((EVBool*)options["performchecks"])->get_value();


	// Device option

	{
		string dstr(((EVText*)options["dev"])->get_string());
		if ( dstr[0] == '-' ) {
			if ( dstr == "-1" ) {
				*device=WAVE_MAPPER;
				*devtype=WAVE_DEVICE;
			} else {
				Error("Invalid device id.");
				return FALSE;
			}
		} else {
			switch(dstr[0]) {
			case 'w':	// WAVE_DEVICE
				{
					long lvalue;

					*devtype=WAVE_DEVICE;

                    if ( dstr.size() == 1 ) {
                        *device=(unsigned long)-1;
                    } else if ( GetWaveDevByAlias(dstr.c_str()+1, lvalue) ) {
						if ( ( lvalue < (long)waveInGetNumDevs() && lvalue >= 0 ) || lvalue==(long)WAVE_MAPPER ) {
							*device=(unsigned long)lvalue;
						} else {
							Error("Invalid device id.");
							return FALSE;
						}
					} else {
						Error("Invalid device alias.");
						return FALSE;
					}
				}
				break;

			case 'd':	// DIRECTX_DEVICE
				{
					if ( !dxsoundsupport ) {
						Error("No DirectSound support!");
						return FALSE;
					}

					LPGUID lpguid;

					*devtype=DIRECTX_DEVICE;

					if ( dstr.size() == 1 ) {
                        *device=(unsigned long)NULL;
                    } else if ( GetDxDevByAlias(dstr.c_str()+1, lpguid) ) {
						*device=(unsigned long)lpguid;
					} else {
						Error("Invalid device alias.");
						return FALSE;
					}
				}
				break;

			default:	// WAVE_DEVICE
				{
					long lvalue;
					try {
						lvalue=lexical_cast<long>(dstr);
					} catch(bad_lexical_cast) {
						lvalue=-1;
					}
					if ( lvalue < (long)waveInGetNumDevs() && lvalue >= -1 ) {
						*device=(unsigned long)(lvalue);
					} else {
						Error("Invalid device id.");
						return FALSE;
					}
				}
			}
		}
	}


	// Channel map option

	{
		string cmstr(((EVText*)options["chnmap"])->get_string());
		if ( cmstr == "" ) {
      *nch_out = *nch;
			chnmap[0]=-2;
    }
		/* This is now allowed, the number channels from the map can differ from the number of channels from the input stream
    else if ( cmstr.size() != *nch ) {
      *nch_out = *nch;
			chnmap[0]=-2;
			Error("Invalid channel map.");
    } */
		else {
      *nch_out = cmstr.size();
			unsigned int t;
			for(t=0; t<cmstr.size(); t++) {
        if ( cmstr[t] == '-' ) {
          chnmap[t] = -1;
        }
        else if ( cmstr[t] < '0' || cmstr[t] > '9' || static_cast<int>(cmstr[t]-48) >= *nch ) {
          *nch_out = *nch;
			    chnmap[0]=-2;
			    Error("Invalid channel map.");
          break;
        }
        else {
          chnmap[t] = static_cast<size_t>(cmstr[t]-48);
        }
			}
		}
	  options["nch-out"] = new expvartype_numeric<string, unsigned short>(*nch_out);
	}
/*
	// TODO: Should only show the ones that weren't recognized
	if ( options.size() > numprocessedoptions ) {
		string temp;
		temp+="Some of the following options (that you specified) were not recognized:\n";

		for(optit=options.begin(); optit!=options.end(); optit++) {
			temp+="\t";
			temp+=(*optit).first;
			if ( (*optit).second.second.isflag == false ) {
				temp+="=";
				temp+=(*optit).second.first;
			}
			temp+="\n";
		}
		temp+="these options were ignored.";

		Warning(temp.c_str());
	}
*/
#ifdef NEWEXPAND
	// Insert some extra vars in options
	try {
		options["bitrate"]=new EVText(lexical_cast<string>((*srate * (*nch * ( (*bps/8) + (*bps % 8?1:0) )))/1000));
	} catch(bad_lexical_cast) {
		throw "(*srate * (*nch * ( (*bps/8) + (*bps % 8?1:0) )))/1024) cannot be lexical_cast-ed";
	}
	try {
		options["bufsize-bytes"]=new EVText(lexical_cast<string>(*bufsize));
	} catch(bad_lexical_cast) {
		throw "*bufsize cannot be lexical_cast-ed";
	}

/*	options["nch-text"]=OptionAttrs(*nch == 1 ? "Mono" : ( *nch == 2 ? "Stereo" : lexical_cast<string>(*nch) + " channels" ),OptFlags(0,false));
	options["time-time"]=OptionAttrs(mstotime(*leninms,true),OptFlags(0,false));
	options["outbuflen-time"]=OptionAttrs(mstotime(*bufferlen,true),OptFlags(0,false));
	options["outprebuflen-time"]=OptionAttrs(mstotime(*prebufferlen,true),OptFlags(0,false));
	options["ismuted-text"]=OptionAttrs(*mute ? "muted" : "",OptFlags(0,false));
	options["ismuted-exptext"]=OptionAttrs(*mute ? "muted" : "not muted",OptFlags(0,false));
	options["performchecks-text"]=OptionAttrs(*checks ? "performing checks" : "",OptFlags(0,false));
	options["performchecks-exptext"]=OptionAttrs(*checks ? "performing checks" : "omitting checks",OptFlags(0,false));
*/
	{
		switch(*devtype) {
		case WAVE_DEVICE:
			{
				WAVEINCAPS waveInCaps;
				waveInGetDevCaps(*device, &waveInCaps, sizeof(WAVEINCAPS));
				options["dev-name"]=new EVText(string(waveInCaps.szPname));
				try {
					options["dev-id"]=new EVText(lexical_cast<string>(*device));
				} catch(bad_lexical_cast) {
					throw "*device can't be lexical_cast-ed";
				}
				options["dev-type"]=new EVText("Standard");
			}
			break;
		case DIRECTX_DEVICE:
			{
				options["dev-name"]=new EVText(GetDxDevName((LPGUID)*device));
				options["dev-id"]=new EVText(GuidToStr((LPGUID)*device));
				options["dev-type"]=new EVText("DirectSound");
			}
			break;
		default:
			{
				options["dev-name"]=new EVText("Illegal device type");
				options["dev-id"]=new EVText("Illegal device type");
				options["dev-type"]=new EVText("Illegal device type");
			}
		}
	}

	{
		if(!curplaying) {
			options["curtime-h"]=new EVText("(time)");
			options["curtime-hm"]=new EVText("(time)");
			options["curtime-hms"]=new EVText("(time)");
			options["curtime-long"]=new EVText("(time)");
		} else {
			SYSTEMTIME st;
			char tempbuf[128];
			GetLocalTime(&st);

			GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOMINUTESORSECONDS, &st, NULL, tempbuf, 254);
			options["curtime-h"]=new EVText(string(tempbuf));

			GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &st, NULL, tempbuf, 254);
			options["curtime-hm"]=new EVText(string(tempbuf));

			GetTimeFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, tempbuf, 254);
			options["curtime-hms"]=new EVText(string(tempbuf));

			options["curtime-long"]=new EVText(mstotime((st.wHour*3600000)+(st.wMinute*60000)+(st.wSecond*1000)+st.wMilliseconds, TRUE));
		}
	}
/*
	{
		char tempbuf[128];
		if ( *isstarttime ) {
			GetTimeFormat(LOCALE_USER_DEFAULT, 0, starttime, NULL, tempbuf, 254);
		} else {
			strcpy(tempbuf, "(no starting time)");
		}
		options["stime-hms"]=OptionAttrs(string(tempbuf),OptFlags(0,false));
	}
*/
/*
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
	options["srate"]=OptionAttrs("44100",OptFlags(0,false));
*/
#endif // NEWEXPAND
	return TRUE;
}

std::string& GetWindowText(HWND hwnd, std::string& str)
{
    int size=GetWindowTextLength(hwnd)+1;
    if ( size > 0 )
    {
        char* const tempbuffer=new char[size];
        if ( tempbuffer != 0 )
        {
            if ( GetWindowText(hwnd, tempbuffer, size) != 0 )
            {
                str=tempbuffer;
                return str;
            }
        }
    }
    str.resize(0);
    return str;
}

std::string GetWindowText(HWND hwnd)
{
    std::string ret;
    return GetWindowText(hwnd, ret);
}

BOOL SetWindowText(HWND hwnd, const std::string& str)
{
    return SetWindowText(hwnd, str.c_str());
}
