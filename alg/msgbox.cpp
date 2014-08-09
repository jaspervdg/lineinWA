
#include "msgbox.h"
#include "tool.h"
#include <stdexcept>

#define for if(true)for

static unsigned long dword_align(unsigned long size)
{
  size += 3;
  size >>= 2;
  size <<= 2;
  return size;
}

template<typename T> static T* dword_align(T* const ptr)
{
  return reinterpret_cast<T*>(dword_align(reinterpret_cast<unsigned long>(ptr)));
}

static SIZE GetTextExtent(const HDC hdc, const std::string& caption)
{
    SIZE totalsize={0,0};
    std::string curstr;
    bool ampersand = false;
    for(std::string::const_iterator it = caption.begin();; ++it)
    {
        if ( *it == '\n' || *it == '\0' || it == caption.end() )
        {
            SIZE size={0,0};
            if ( curstr.empty() ) curstr=" ";
            if ( GetTextExtentPoint32(hdc, curstr.c_str(), curstr.size(), &size) != 0 )
            {
                totalsize.cx = __max(totalsize.cx, size.cx);
                totalsize.cy += size.cy;
            }
            if ( it == caption.end() ) break; else { curstr.erase(); continue; }
        }
        else if ( *it == '&' )
        {
            if ( ampersand ) ampersand = false; else { ampersand = true; continue; }
        }
        curstr += *it;
    }
    return totalsize;
}

static SIZE GetIconSize(const HICON hicon)
{
    SIZE size={32, 32};
    if ( hicon )
    {
        ICONINFO info;
        if ( GetIconInfo(hicon, &info) != 0 )
        {
            if ( info.hbmMask )
            {
                BITMAP bm;
                if ( GetObject(info.hbmMask, sizeof(BITMAP), (LPSTR) &bm) )
                {
                    size.cx=bm.bmWidth;
                    size.cy=bm.bmHeight;
                }
                DeleteObject(info.hbmMask);
            }
            if ( info.hbmColor ) DeleteObject(info.hbmColor);
        }
    }
    return size;
}

static BOOL CALLBACK MyMessageDialogEnumChildProc(
  HWND hwnd,      // handle to child window
  LPARAM lParam   // application-defined value
)
{
    std::vector<HWND>* hwnds=reinterpret_cast<std::vector<HWND>*>(lParam);
    if ( GetDlgCtrlID(hwnd) != option_t::id_caption && GetDlgCtrlID(hwnd) != option_t::id_icon ) hwnds->push_back(hwnd);
    return TRUE;
}

template<typename T> static inline T ceildiv(const T v1, const T v2)
{
    const T m=v1%v2;
    return ((v1-m)/v2)+(m!=0?1:0);
}

class dialog_unit_converter_t {
private:
    int dux, duy; // dialog base unit size
public:
    dialog_unit_converter_t(const HDC hdc)
    {
        TEXTMETRIC textmetric;
        GetTextMetrics(hdc, &textmetric);
        dux=textmetric.tmAveCharWidth;
        duy=textmetric.tmHeight;
    }
    int to_pixels_x(int du) const { return ceildiv(du*dux, 4); }
    int to_pixels_y(int dy) const { return ceildiv(dy*duy, 8); }
};

static struct messageglobals_t {
  HFONT messagefont;
  messageglobals_t() {
    NONCLIENTMETRICS nonclientmetrics;
    ZeroMemory(&nonclientmetrics, sizeof(nonclientmetrics));
    nonclientmetrics.cbSize = sizeof(nonclientmetrics);
    SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(nonclientmetrics), &nonclientmetrics, 0);
    messagefont = CreateFontIndirect(&nonclientmetrics.lfMessageFont);
  }
  ~messageglobals_t() {
    if ( messagefont ) DeleteObject(messagefont);
  }
} messageglobals;

static BOOL CALLBACK MyMessageDialogProc(
  HWND hwndDlg,  // handle to dialog box
  UINT uMsg,     // message
  WPARAM wParam, // first message parameter
  LPARAM lParam  // second message parameter
)
{
  if ( uMsg == WM_CLOSE ) {
    EndDialog(hwndDlg, option_t::id_closed);
    return -1;
  }
  else if ( uMsg == WM_COMMAND ) {
    switch(HIWORD(wParam)) {
    case BN_CLICKED:
      EndDialog(hwndDlg, LOWORD(wParam));
      return -1;
    }
  }
  else if ( uMsg == WM_INITDIALOG ) {
    const HDC hdc=GetDC(hwndDlg);
    HFONT oldfont=0;
    oldfont=(HFONT)SelectObject(hdc, messageglobals.messagefont);
    const dialog_unit_converter_t du(hdc);
    std::vector<HWND> button_hwnds;
    std::vector<int> button_widths;
    EnumChildWindows(hwndDlg, MyMessageDialogEnumChildProc, reinterpret_cast<LPARAM>(&button_hwnds));
    int buttonswidth=0, buttonsheight=0;
    {
      bool firstbut=true;
      for(std::vector<HWND>::const_iterator it=button_hwnds.begin(); it!=button_hwnds.end(); ++it) {
        std::string caption;
        GetWindowText(*it, caption);
        const SIZE textsize=GetTextExtent(hdc, caption);
        if ( firstbut ) firstbut=false; else buttonswidth+=du.to_pixels_x(4);
        button_widths.push_back(__max(du.to_pixels_x(50),du.to_pixels_y(5)+textsize.cx+du.to_pixels_y(5))); // This is a test (the Windows layout guide doesn't specify what to do for horizontal spacing in buttons)
        buttonswidth+=button_widths.back();
        buttonsheight=__max(buttonsheight,du.to_pixels_y(3)+textsize.cy+du.to_pixels_y(3)); // Actually the guide also doesn't say what the vertical spacing should be in a button, but as a button should normally be 14 dialog units high the spacing can be calculated
      }
      buttonsheight=__max(du.to_pixels_y(14),buttonsheight);
    }
    int iconwidth=0, iconheight=0;
    {
      if ( lParam != 0 ) {
        SendMessage(GetDlgItem(hwndDlg, option_t::id_icon), STM_SETIMAGE, IMAGE_ICON, lParam);
        const SIZE size=GetIconSize(reinterpret_cast<HICON>(lParam));
        iconwidth=size.cx;
        iconheight=size.cy;
        SetWindowPos(GetDlgItem(hwndDlg, option_t::id_icon), 0, du.to_pixels_x(7), du.to_pixels_y(7), 0, 0, SWP_NOZORDER|SWP_NOSIZE);
      }
    }
    const int iconspacingx = ( iconwidth==0 ? 0 : du.to_pixels_x(15) );
    const int buttonspacingy = du.to_pixels_y(10);
    {
      HWND captionhwnd=GetDlgItem(hwndDlg, option_t::id_caption);
      std::string caption;
      SendMessage(captionhwnd, WM_SETFONT, (WPARAM)messageglobals.messagefont, MAKELPARAM(FALSE,0));
      GetWindowText(captionhwnd, caption);
      const SIZE textsize=GetTextExtent(hdc, caption);
      if ( textsize.cx != 0 ) {
        const int maxwidth=__max(buttonswidth,static_cast<int>(textsize.cx+iconwidth+iconspacingx));
        const int maxheight=__max(iconheight,static_cast<int>(textsize.cy));
        {
          RECT r,c;
          GetWindowRect(hwndDlg, &r);
          GetClientRect(hwndDlg, &c);
          SetWindowPos(hwndDlg, 0, 0, 0, du.to_pixels_x(7)+maxwidth+du.to_pixels_x(7)+((r.right-r.left)-c.right), du.to_pixels_y(7)+maxheight+buttonspacingy+buttonsheight+du.to_pixels_y(7)+((r.bottom-r.top)-c.bottom), SWP_NOZORDER|SWP_NOMOVE);
        }
        SetWindowPos(captionhwnd, 0, du.to_pixels_x(7)+iconwidth+iconspacingx, du.to_pixels_y(7), textsize.cx, textsize.cy, SWP_NOZORDER);
        {
          int curxpos=du.to_pixels_x(7)+((maxwidth-buttonswidth)/2);
          std::vector<int>::const_iterator width_it=button_widths.begin();
          for(std::vector<HWND>::const_iterator it=button_hwnds.begin(); it!=button_hwnds.end(); ++it, ++width_it) {
            SendMessage(*it, WM_SETFONT, (WPARAM)messageglobals.messagefont, MAKELPARAM(FALSE,0));
            SetWindowPos(*it, 0, curxpos, du.to_pixels_y(7)+maxheight+buttonspacingy, *width_it, buttonsheight, SWP_NOZORDER);
            curxpos += *width_it + du.to_pixels_x(4);
          }
        }
      }
    }
    SelectObject(hdc, oldfont);
    ReleaseDC(hwndDlg, hdc);
    return FALSE; // This makes sure the default button isn't changed
  }
  return 0;
}
/*
int msgbox_t::show(const window_t& parent_p, const std::string& title_p, const std::string& message_p, const std::vector<option_t> options_p, const int default_id_p, const msgbox_t::style_t style_p)
{
    msgbox_t msgbox(parent_p, title_p, message_p, options_p, default_id_p, style_p);
    return msgbox.show();
}

int msgbox_t::show(const window_t& parent_p, const char* const title_p, const char* const message_p, const std::vector<option_t> options_p, const int default_id_p, const msgbox_t::style_t style_p)
{
    msgbox_t msgbox(parent_p, title_p, message_p, options_p, default_id_p, style_p);
    return msgbox.show();
}

int msgbox_t::show(const window_t& parent_p, const std::string& title_p, const char* const message_p, const std::vector<option_t> options_p, const int default_id_p, const msgbox_t::style_t style_p)
{
    msgbox_t msgbox(parent_p, title_p, message_p, options_p, default_id_p, style_p);
    return msgbox.show();
}

int msgbox_t::show(const window_t& parent_p, const char* const title_p, const std::string& message_p, const std::vector<option_t> options_p, const int default_id_p, const msgbox_t::style_t style_p)
{
    msgbox_t msgbox(parent_p, title_p, message_p, options_p, default_id_p, style_p);
    return msgbox.show();
}
*/
/*
msgbox_t::msgbox_t(const window_t& parent_p, const std::string& title_p, const std::string& message_p, const std::vector<option_t> options_p, const int default_id_p, const msgbox_t::style_t style_p) :
    parent_m(parent_p), title_m(make_wide(title_p)), message_m(make_wide(message_p)), options_m(options_p), default_id_m(default_id_p), style_m(style_p), font_m(L"MS Sans Serif"), hicon(0)
{
	init();
}

msgbox_t::msgbox_t(const window_t& parent_p, const char* const title_p, const char* const message_p, const std::vector<option_t> options_p, const int default_id_p, const style_t style_p) :
    parent_m(parent_p), title_m(make_wide(title_p)), message_m(make_wide(message_p)), options_m(options_p), default_id_m(default_id_p), style_m(style_p), font_m(L"MS Sans Serif"), hicon(0)
{
	init();
}

msgbox_t::msgbox_t(const window_t& parent_p, const std::string& title_p, const char* const message_p, const std::vector<option_t> options_p, const int default_id_p, const style_t style_p) :
    parent_m(parent_p), title_m(make_wide(title_p)), message_m(make_wide(message_p)), options_m(options_p), default_id_m(default_id_p), style_m(style_p), font_m(L"MS Sans Serif"), hicon(0)
{
	init();
}

msgbox_t::msgbox_t(const window_t& parent_p, const char* const title_p, const std::string& message_p, const std::vector<option_t> options_p, const int default_id_p, const style_t style_p) :
    parent_m(parent_p), title_m(make_wide(title_p)), message_m(make_wide(message_p)), options_m(options_p), default_id_m(default_id_p), style_m(style_p), font_m(L"MS Sans Serif"), hicon(0)
{
	init();
}
*/
void msgbox_t::init()
{
  LPDLGTEMPLATE lpdt;
  LPDLGITEMTEMPLATE lpdit;
  LPWORD lpw;
  LPWSTR lpwsz;
  int nchar;

  unsigned long control_sizes=0;
  for(std::vector<option_t>::const_iterator options_it=options_m.begin(); options_it!=options_m.end(); ++options_it)
  {
    control_sizes+=dword_align(sizeof(DLGITEMTEMPLATE)+sizeof(WORD)+sizeof(WORD)+(options_it->caption.size()*sizeof(wchar_t))+sizeof(wchar_t)+sizeof(WORD));
  }

  hgbl = GlobalAlloc(GMEM_ZEROINIT,
                     dword_align(sizeof(DLGTEMPLATE)+sizeof(WORD)+sizeof(WORD)+(title_m.size()*sizeof(wchar_t))+sizeof(wchar_t)/*+sizeof(short)+(font_m.size()*sizeof(wchar_t))+sizeof(wchar_t)*/)+
                     control_sizes+
                     dword_align(sizeof(DLGITEMTEMPLATE)+sizeof(WORD)+sizeof(WORD)+(message_m.size()*sizeof(wchar_t))+sizeof(wchar_t)+sizeof(WORD))+
                     ( style_m != style_default ? dword_align(sizeof(DLGITEMTEMPLATE)+sizeof(WORD)+sizeof(WORD)+sizeof(wchar_t)+sizeof(WORD)) : 0)
                    );
  if ( !hgbl ) throw std::runtime_error("Unable to allocate memory for messagebox!");

  lpdt = reinterpret_cast<LPDLGTEMPLATE>(GlobalLock(hgbl));

  // Define a dialog box.

  lpdt->style = DS_MODALFRAME | WS_POPUP | WS_CAPTION;// | DS_SETFONT;//DS_3DLOOK | WS_POPUP | WS_BORDER | WS_SYSMENU
                 //| DS_MODALFRAME | WS_CAPTION;
  lpdt->cdit = 1+options_m.size()+(style_m==style_default?0:1);  // number of controls
  lpdt->x  = 10;  lpdt->y  = 10;
  lpdt->cx = 100; lpdt->cy = 100;

  lpw = (LPWORD) (lpdt + 1);
  *lpw++ = 0;   // no menu
  *lpw++ = 0;   // predefined dialog box class (by default)

  // Set title
  lpwsz = (LPWSTR) lpw; wcscpy(lpwsz, title_m.c_str());
  nchar = title_m.size()+1;
  lpw   += nchar;

  // Set font
  /* Now done in INITDIALOG
  *lpw++ = 8;
  lpwsz = (LPWSTR) lpw; wcscpy(lpwsz, font_m.c_str());
  nchar = font_m.size()+1;
  lpw   += nchar;
  */

  //-----------------------
  // Define any buttons.
  //-----------------------
  short x=10;
  int max_id=0;
  for(std::vector<option_t>::const_iterator options_it=options_m.begin(); options_it!=options_m.end(); ++options_it)
  {
    lpw = dword_align(lpw); // align DLGITEMTEMPLATE on DWORD boundary
    lpdit = reinterpret_cast<LPDLGITEMTEMPLATE>(lpw);
    lpdit->x  = x; lpdit->y  = 70; x+=40;
    lpdit->cx = 40; lpdit->cy = 20;
    lpdit->id = options_it->id;  // OK button identifier
    lpdit->style = WS_CHILD | WS_VISIBLE | (options_it->id == default_id_m ? BS_DEFPUSHBUTTON : 0);

    lpw = (LPWORD) (lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080;    // button class

    lpwsz = (LPWSTR) lpw; wcscpy(lpwsz, options_it->caption.c_str());
    nchar = options_it->caption.size()+1;
    lpw   += nchar;

    *lpw++ = 0;           // no creation data
  }

  //-----------------------
  // Define a static text control.
  //-----------------------
  lpw = dword_align(lpw); // align DLGITEMTEMPLATE on DWORD boundary
  lpdit = (LPDLGITEMTEMPLATE) lpw;
  lpdit->x  = 10; lpdit->y  = 10;
  lpdit->cx = 40; lpdit->cy = 20;
  lpdit->id = option_t::id_caption;  // text identifier
  lpdit->style = WS_CHILD | WS_VISIBLE | SS_LEFT;

  lpw = (LPWORD) (lpdit + 1);
  *lpw++ = 0xFFFF;
  *lpw++ = 0x0082;                         // static class

  lpwsz = (LPWSTR)lpw; wcscpy(lpwsz, message_m.c_str());
  nchar = message_m.size()+1;
  lpw   += nchar;

  *lpw++ = 0;           // no creation data

  //-----------------------
  // Define a static icon control.
  //-----------------------
  if ( style_m != style_default )
  {
    lpw = dword_align(lpw); // align DLGITEMTEMPLATE on DWORD boundary
    lpdit = (LPDLGITEMTEMPLATE) lpw;
    lpdit->x  = 0; lpdit->y  = 0;
    lpdit->cx = 0; lpdit->cy = 0;
    lpdit->id = option_t::id_icon;  // text identifier
    lpdit->style = WS_CHILD | WS_VISIBLE | SS_ICON;

    lpw = (LPWORD) (lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0082;                         // static class

    lpwsz = (LPWSTR)lpw; *lpwsz=L'\0';
    nchar = 1;
    lpw   += nchar;

    *lpw++ = 0;           // no creation data
  }

  //-----------------------
  // Load icon.
  //-----------------------
  switch(style_m)  // hicon is inited to zero by the constructor (see above)
  {
  case style_error:
    hicon=LoadIcon(0, IDI_ERROR);
    break;
  case style_warning:
    hicon=LoadIcon(0, IDI_WARNING);
    break;
  case style_information:
    hicon=LoadIcon(0, IDI_INFORMATION);
    break;
  case style_question:
    hicon=LoadIcon(0, IDI_QUESTION);
    break;
  }

  GlobalUnlock(hgbl);
}

msgbox_t::~msgbox_t()
{
    GlobalFree(hgbl); 
    DestroyIcon(hicon);
}

int msgbox_t::show()
{
    int ret=DialogBoxIndirectParam(GetModuleHandle(0), (LPDLGTEMPLATE) hgbl, parent_m, (DLGPROC) MyMessageDialogProc, reinterpret_cast<LPARAM>(hicon));
    int err = GetLastError();
    return ret;
}

// option_t constructors
//option_t::option_t(const int id_p, const std::string& caption_p) : id(id_p), caption(make_wide(caption_p)) {}
option_t::option_t(const int id_p, const std::wstring& caption_p) : id(id_p), caption(caption_p) {}
//option_t::option_t(const int id_p, const char* const caption_p) : id(id_p), caption(make_wide(caption_p)) {}
option_t::option_t(const int id_p, const wchar_t* const caption_p) : id(id_p), caption(caption_p) {}

// misc. functions

int OkMsgBox(const window_t& parent_p, const std::string& title_p, const std::string& message_p, const msgbox_t::style_t style_p)
{
  static const option_t options_array[]={option_t(option_t::id_ok, L"Ok")};
  static const std::vector<option_t> options(options_array, options_array+1);
  std::wstring wtitle, wmessage;
  make_wide(title_p, wtitle);
  make_wide(message_p, wmessage);
  return msgbox_t::show(parent_p, wtitle.c_str(), wmessage.c_str(), options, option_t::id_ok, style_p);
}
