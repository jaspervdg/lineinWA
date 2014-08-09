/*********************************\
*                                 *
*        General functions        *
*                                 *
\*********************************/

#ifndef GENFUNCSFORWINAMPPLUGS
#define GENFUNCSFORWINAMPPLUGS

// STL (and boost) includes
#include <string>
#include <vector>
#include <stdexcept>

// Windows includes
#include <windows.h>

// Own includes
#include "msgbox.h"

extern const std::vector<option_t> default_error_options;
extern const std::vector<option_t> default_warning_options;
int Error(const wchar_t* msg, HWND parent=0, const std::vector<option_t>& options=default_error_options, const int default_id=option_t::id_ok);
int Warning(const wchar_t* msg, HWND parent=0, const std::vector<option_t>& options=default_warning_options, const int default_id=option_t::id_ok);
int Error(const std::wstring& msg, HWND parent=0, const std::vector<option_t>& options=default_error_options, const int default_id=option_t::id_ok);
int Warning(const std::wstring& msg, HWND parent=0, const std::vector<option_t>& options=default_warning_options, const int default_id=option_t::id_ok);
int Error(const char* msg, HWND parent=0, const std::vector<option_t>& options=default_error_options, const int default_id=option_t::id_ok);
int Warning(const char* msg, HWND parent=0, const std::vector<option_t>& options=default_warning_options, const int default_id=option_t::id_ok);
int Error(const std::string& msg, HWND parent=0, const std::vector<option_t>& options=default_error_options, const int default_id=option_t::id_ok);
int Warning(const std::string& msg, HWND parent=0, const std::vector<option_t>& options=default_warning_options, const int default_id=option_t::id_ok);

// hexctoc, converts one hexadecimal digit into a char (for hextoc)
inline char hexctoc(char cur)
{
	if( cur < '0' || ( cur > '9' && cur < 'A' ) || cur > 'F' ) return 0;
	if((cur-48)>9) {
		return ( cur - 'A' + 10 );
	} else {
		return ( cur - '0' );
	}
}

// hextoc, converts a hexadecimal string to a char (for expandtitle)
inline char hextoc(char one, char two) { return ( ( hexctoc( one ) * 16 ) + hexctoc( two ) ); }


template<typename T> T calc_sample_align(const T bps)
{
    return (bps/8) + (bps % 8?1:0);
}

// timetoms function to convert given time to number of ms
long timetoms(const std::string& time);

// mstotime converts an integer containing a time in ms to a string in the form [[h:]m:]ss[.ms]
std::string mstotime(const long orgms, const bool zero, const bool full=false, const bool force_ms=false);

// timetosystemtime function to convert given time to system time
bool timetosystime(const std::string &time, SYSTEMTIME * systime);

bool datetosystime(const std::string &date, SYSTEMTIME * systime);

// Converts the time portion of the SYSTIME structure to milliseconds
long systime_to_ms(const SYSTEMTIME& systime);

// Compares two dates (only uses the date portion, does nothing with time).
// Returns 0 if equal, -1 if date1 is smaller than date2 and 1 if date1 is bigger than date2.
int compare_sysdate(const SYSTEMTIME& date1, const SYSTEMTIME& date2);

std::string& convtoupr(std::string& str);
std::string convtoupr(const std::string& str);

std::wstring& make_wide(const char* const s, std::wstring& t);
std::wstring& make_wide(const std::string& s, std::wstring& t);
//static std::wstring make_wide(const std::string& s) { std::wstring t; return make_wide(s, t); }
//static std::wstring make_wide(const char* const s) { std::wstring t; return make_wide(s, t); }

std::string& make_multibyte(const wchar_t* const s, std::string& t);
std::string& make_multibyte(const std::wstring& s, std::string& t);
//static std::string make_multibyte(const std::wstring& s) { std::string t; return make_multibyte(s, t); }
//static std::string make_multibyte(const wchar_t* const s) { std::string t; return make_multibyte(s, t); }

#define arraysize(X) (sizeof(X)/sizeof(X[0]))

bool logical_xor(const bool a, const bool b);

#endif //GENFUNCSFORWINAMPPLUGS
