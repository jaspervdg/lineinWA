
#ifndef WAVEWRITE_MSGBOX_H
#define WAVEWRITE_MSGBOX_H

#include <windows.h>
#include <string>
#include <vector>

typedef HWND window_t;

struct option_t
{
    enum
    {
        id_error=-1, // this is the same value as DialogBoxIndirect returns when an error occurs
        id_closed=-2,
        id_caption=16738, // just some strange number
        id_icon=16739, // just some strange number
        id_ok=0,
        id_cancel=1,
        id_yes=2,
        id_no=3,
        id_custom=100,
    };
    int id;
    std::wstring caption;
    //option_t(const int id_p, const std::string& caption_p);
    option_t(const int id_p, const std::wstring& caption_p);
    //option_t(const int id_p, const char* const caption_p);
    option_t(const int id_p, const wchar_t* const caption_p);
};

#include "gen.h"

class msgbox_t
{
public:
  enum style_t {
    style_default,
    style_error,
    style_warning,
    style_information,
    style_question,
  };

private:
  const window_t parent_m;
  const std::wstring title_m;
  const std::wstring message_m;
  const std::vector<option_t> options_m;
  const int default_id_m;
  const style_t style_m;

  //const std::wstring font_m;
  HGLOBAL hgbl;
  HICON hicon;

	void init();

public:
/*
  msgbox_t(const window_t& parent_p, const std::string& title_p, const std::string& message_p, const std::vector<option_t> options_p, const int default_id_p=option_t::id_error, const style_t style_p=style_default);
  msgbox_t(const window_t& parent_p, const char* const title_p, const char* const message_p, const std::vector<option_t> options_p, const int default_id_p=option_t::id_error, const style_t style_p=style_default);
  msgbox_t(const window_t& parent_p, const std::string& title_p, const char* const message_p, const std::vector<option_t> options_p, const int default_id_p=option_t::id_error, const style_t style_p=style_default);
  msgbox_t(const window_t& parent_p, const char* const title_p, const std::string& message_p, const std::vector<option_t> options_p, const int default_id_p=option_t::id_error, const style_t style_p=style_default);
*/
  template<typename T1, typename T2>
  msgbox_t(const window_t& parent_p, const T1& title_p, const T2& message_p, const std::vector<option_t>& options_p, const int default_id_p, const msgbox_t::style_t style_p) :
      parent_m(parent_p), title_m(title_p), message_m(message_p), options_m(options_p), default_id_m(default_id_p), style_m(style_p)/*, font_m(L"MS Sans Serif")*/, hicon(0)
  {
	  init();
  }
  ~msgbox_t();

  int show();
  template<typename T1, typename T2>
  static int show(const window_t& parent_p, const T1& title_p, const T2& message_p, const std::vector<option_t>& options_p, const int default_id_p, const msgbox_t::style_t style_p)
  {
    msgbox_t msgbox(parent_p, title_p, message_p, options_p, default_id_p, style_p);
    return msgbox.show();
  }
/*
  static int show(const window_t& parent_p, const std::string& title_p, const std::string& message_p, const std::vector<option_t> options_p, const int default_id_p=option_t::id_error, const style_t style_p=style_default);
  static int show(const window_t& parent_p, const char* const title_p, const char* const message_p, const std::vector<option_t> options_p, const int default_id_p=option_t::id_error, const style_t style_p=style_default);
  static int show(const window_t& parent_p, const std::string& title_p, const char* const message_p, const std::vector<option_t> options_p, const int default_id_p=option_t::id_error, const style_t style_p=style_default);
  static int show(const window_t& parent_p, const char* const title_p, const std::string& message_p, const std::vector<option_t> options_p, const int default_id_p=option_t::id_error, const style_t style_p=style_default);
*/
};

int OkMsgBox(const window_t& parent_p, const std::string& title_p, const std::string& message_p, const msgbox_t::style_t style_p=msgbox_t::style_default);

#endif //WAVEWRITE_MSGBOX_H
