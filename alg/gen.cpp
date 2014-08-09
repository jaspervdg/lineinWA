
/*********************************\
*                                 *
*        General functions        *
*                                 *
\*********************************/

#include "gen.h"
#include "common.h"
#include <locale>
#include <boost/lexical_cast.hpp>

static const option_t default_option_array[]={option_t(option_t::id_ok,L"Ok")};
const std::vector<option_t> default_error_options(default_option_array,default_option_array+1);
const std::vector<option_t> default_warning_options(default_option_array,default_option_array+1);

int Error(const wchar_t* msg, HWND parent, const std::vector<option_t>& options, const int default_id)
{
  static const std::wstring error_msgbox_title(L"Error - " + msgbox_program_title);
  if ( msg && msg[0] != 0 ) {
    return msgbox_t::show(parent ? parent : out.hMainWindow, error_msgbox_title, msg, options, default_id, msgbox_t::style_error);
  }
  else {
    return default_id;
  }
}

int Warning(const wchar_t* msg, HWND parent, const std::vector<option_t>& options, const int default_id)
{
  static const std::wstring warning_msgbox_title(L"Warning - " + msgbox_program_title);
  if ( msg && msg[0] != 0 ) {
    return msgbox_t::show(parent ? parent : out.hMainWindow, warning_msgbox_title, msg, options, default_id, msgbox_t::style_warning);
  }
  else {
    return default_id;
  }
}

int Error(const std::wstring& msg, HWND parent, const std::vector<option_t>& options, const int default_id)
{
  return Error(msg.c_str(), parent, options, default_id);
}

int Warning(const std::wstring& msg, HWND parent, const std::vector<option_t>& options, const int default_id)
{
  return Warning(msg.c_str(), parent, options, default_id);
}

int Error(const char* msg, HWND parent, const std::vector<option_t>& options, const int default_id)
{
  std::wstring wmsg;
  make_wide(msg, wmsg);
  return Error(wmsg.c_str(), parent, options, default_id);
}

int Warning(const char* msg, HWND parent, const std::vector<option_t>& options, const int default_id)
{
  std::wstring wmsg;
  make_wide(msg, wmsg);
  return Warning(wmsg.c_str(), parent, options, default_id);
}

int Error(const std::string& msg, HWND parent, const std::vector<option_t>& options, const int default_id)
{
  std::wstring wmsg;
  make_wide(msg, wmsg);
  return Error(wmsg.c_str(), parent, options, default_id);
}

int Warning(const std::string& msg, HWND parent, const std::vector<option_t>& options, const int default_id)
{
  std::wstring wmsg;
  make_wide(msg, wmsg);
  return Warning(wmsg.c_str(), parent, options, default_id);
}

// timetoms function to convert given time to number of ms
long timetoms(const std::string& time) {
	long ms=0;
	long value=0;
	unsigned char colon=0;
	bool dot=false;
	char numofdotnums=0;

	for(std::string::const_iterator t=time.begin(); t!=time.end() && *t!=','; ++t) {
		if ( *t == ':' && !dot ) {
			if (colon>1) return -1000;
			ms=(ms+value)*60;
			value=0;
			++colon;
		} else if ( *t == '.' ) {
			if (dot) return -1000;
			ms=(ms+value)*1000;
			value=0;
			dot=true;
		} else {
			if (dot) {
				++numofdotnums;
				if ( numofdotnums>3 ) {
					if ( (*t-'0')>4 ) ++value;
					break;
				}
			}
			value*=10;
			value+=*t-'0';
		}
	}

	if(value || dot || colon) {
		if(dot) {
			if (numofdotnums<3) {
				numofdotnums=3-numofdotnums;
				for(;numofdotnums;--numofdotnums) value*=10;
			}
			ms+=value;
		} else if (colon) {
			ms=(ms+value)*1000;
		} else {
			ms=value*1000;
		}
	}
	return ms;
}

// mstotime converts an integer containing a time in ms to a string in the form [[h:]m:]ss[.ms]
std::string mstotime(const long orgms, const bool zero, const bool full, const bool force_ms) {
	ldiv_t value;
	bool prev=false;
	long ms=orgms;
	std::string time;

    for(long msborder=3600000; msborder>=1000; msborder/=60)
    {
	    if ( ms>=msborder ) {	// hours
		    value=ldiv(ms, msborder);
		    ms=value.rem;
            if ( zero && value.quot < 10 ) time+='0';
		    try {
                time+=boost::lexical_cast<std::string>(value.quot);
            } catch(boost::bad_lexical_cast) {
			    throw "Something went wrong in mstotime (time+=lexical_cast<string>(value.quot))"; 
		    }
	    }
        else if ( full || prev )
        {
            time+=( zero ? "00" : "0" );
        }
        else if ( msborder == 1000 )
        {
    		time+=( zero && prev ? "00" : "0" );
        }
	    if ( !time.empty() && msborder > 1000 )
        {
            time+=':';
            prev=true;
        }
    }

	if ( ms>0 || force_ms ) {			//milliseconds
		time+='.';
        std::string ms_str;
		try {
            ms_str=boost::lexical_cast<std::string>(ms);
        } catch(boost::bad_lexical_cast) {
			throw "Something went wrong in mstotime (time+=lexical_cast<string>(ms), for milliseconds)"; 
		}
		for(;ms<100;ms*=10) time+='0';
        time+=ms_str;
	}

	return time;
}

// timetosystemtime function to convert given time to system time
bool timetosystime(const std::string &time, SYSTEMTIME * systime) {
	WORD value=0;
	unsigned char colon=0;
	bool dot=false;
	char numofdotnums=0;

	systime->wHour=0;
	systime->wMinute=0;
	systime->wSecond=0;
	systime->wMilliseconds=0;

	for(std::string::const_iterator t=time.begin(); t!=time.end() && *t!=','; ++t) {
		if ( *t == ':' ) {
			if (colon>1 || dot) return false;
			switch(colon) {
			case 0:
				systime->wHour=value;
				break;
			case 1:
				systime->wMinute=value;
				break;
			}
			value=0;
			++colon;
		} else if ( *t == '.' ) {
			if (dot) return false;
			systime->wSecond=value;
			value=0;
			dot=true;
		} else {
			if (dot) {
				++numofdotnums;
				if ( numofdotnums>3 ) {
					if ( (*t-'0')>4 ) ++value;
					break;
				}
			}
			value*=10;
			value+=*t-'0';
		}
	}

	if(value) {
		if(dot) {
			if (numofdotnums<3) {
				numofdotnums=3-numofdotnums;
				for(;numofdotnums;--numofdotnums) value*=10;
			}
			systime->wMilliseconds=value;
		} else if ( colon == 2 ) {
			systime->wSecond=value;
		} else if ( colon == 1 ) {
			systime->wMinute=value;
		} else {
			Error("Wrong time format!");
			return false;
		}
	}
	return true;
}

bool datetosystime(const std::string &date, SYSTEMTIME * systime)
{
	WORD year=0;
	WORD month=0;
	WORD day=0;
	std::string::const_iterator t;

	for(t=date.begin(); t!=date.end() && *t!='-' && *t!='/'; ++t) {
		if ( *t<'0' || *t>'9' ) return false;
		year*=10;
		year+=*t-'0';
	}

	if ( t == date.end() ) return false;

	for(++t; t!=date.end() && *t!='-' && *t!='/'; ++t) {
		if (*t<'0' || *t>'9' ) return false;
		month*=10;
		month+=*t-'0';
	}

	if ( t == date.end() ) return false;

	for(++t; t!=date.end() && *t!=',' && *t!='-' && *t!='/'; ++t) {
		if ( *t<'0' || *t>'9' ) return false;
		day*=10;
		day+=*t-'0';
	}

	systime->wYear=year;
	systime->wMonth=month;
	systime->wDay=day;

	return true;
}

long systime_to_ms(const SYSTEMTIME& systime)
{
  return ((systime.wHour*60+systime.wMinute)*60+systime.wSecond)*1000+systime.wMilliseconds;
}

int compare_sysdate(const SYSTEMTIME& date1, const SYSTEMTIME& date2)
{
  if ( date1.wYear < date2.wYear ) return -1;
  if ( date1.wYear > date2.wYear ) return 1;
  if ( date1.wMonth < date2.wMonth ) return -1;
  if ( date1.wMonth > date2.wMonth ) return 1;
  if ( date1.wDay < date2.wDay ) return -1;
  if ( date1.wDay > date2.wDay ) return 1;
  return 0;
}

std::string& convtoupr(std::string& str)
{
#if _STLP_DEBUG
  std::use_facet< std::ctype<std::string::value_type> >(std::locale("")).toupper(&(*(str.begin())), &(*(str.end())));
#else
//#ifdef _MSC_VER
//  std::_USE(std::locale(""), std::ctype<std::string::value_type>).toupper(str.begin(), str.end());
//#else
  std::use_facet< std::ctype<std::string::value_type> >(std::locale("")).toupper(str.begin(), str.end());
//#endif
#endif
	return str;
}

std::string convtoupr(const std::string& str)
{
	std::string retstr(str);
	return convtoupr(retstr);
}

/*std::wstring& make_wide(const std::string& s, std::wstring& t);
{
  const int bufsize = MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, 0, 0);
  if ( bufsize <= 0 ) return std::wstring(L"#Error running MultiByteToWideChar!#");
  std::wstring retval(bufsize+1, L'\0');
  MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, const_cast<wchar_t*>(retval.data()), retval.size());
  return retval.c_str(); // This is to make sure no double ending zeroes are left.
}*/

std::wstring& make_wide(const char* const s, std::wstring& retval)
{
  const int bufsize = MultiByteToWideChar(CP_ACP, 0, s, -1, 0, 0);
  if ( bufsize <= 0 ) throw std::runtime_error("Error querying for needed buffer size with MultiByteToWideChar.");
  retval.assign(bufsize+1, L'\0');
  const int charswritten = MultiByteToWideChar(CP_ACP, 0, s, -1, const_cast<wchar_t*>(retval.data()), retval.size());
  if ( charswritten <= 0 ) throw std::runtime_error("Error using MultiByteToWideChar.");
  if ( charswritten > bufsize ) throw std::runtime_error("MultiByteToWideChar wrote more characters than anticipated, this could have resulted in memory corruption.");
  // This is to make sure no double ending zeroes are left.
  const last_non_zero_pos = retval.find_last_not_of(L'\0');
  if ( last_non_zero_pos != retval.npos ) retval.resize(last_non_zero_pos+1);
  return retval;
}

std::wstring& make_wide(const std::string& s, std::wstring& t)
{
  return make_wide(s.c_str(), t);
}

std::string& make_multibyte(const wchar_t* const s, std::string& retval)
{
  const int bufsize = WideCharToMultiByte(CP_ACP, 0, s, -1, 0, 0, 0, 0);
  if ( bufsize <= 0 ) throw std::runtime_error("Error querying for needed buffer size with WideCharToMultiByte.");
  retval.assign(bufsize+1, '\0');
  const int charswritten = WideCharToMultiByte(CP_ACP, 0, s, -1, const_cast<char*>(retval.data()), retval.size(), 0, 0);
  if ( charswritten <= 0 ) throw std::runtime_error("Error using WideCharToMultiByte.");
  if ( charswritten > bufsize ) throw std::runtime_error("WideCharToMultiByte wrote more characters than anticipated, this could have resulted in memory corruption.");
  // This is to make sure no double ending zeroes are left.
  const last_non_zero_pos = retval.find_last_not_of('\0');
  if ( last_non_zero_pos != retval.npos ) retval.resize(last_non_zero_pos+1);
  return retval;
}

std::string& make_multibyte(const std::wstring& s, std::string& t)
{
  return make_multibyte(s.c_str(), t);
}

bool logical_xor(const bool a, const bool b)
{
  return (a || b) && !(a && b);
}
