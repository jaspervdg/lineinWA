#LineIn plugin for Winamp

This directory contains the source code for the [LineIn plugin for Winamp](http://home.hccnet.nl/th.v.d.gronde/dev/lineinWA2/), as well as some (custom) dependencies.
Please note that this plugin was last actively worked on years ago, and may take some adjustments to get it to open/compile using modern IDEs/compilers.
The code and its dependencies (which were mostly written to be used with my plugins) are hereby released under the MIT license (see the LICENSE file), except for Expandlib, which (for historical reasons) is covered under the zlib license (very similar terms, see the source code).

The project contains numerous builds, but the most interesting are simply the normal Debug and
Release builds (and possibly the Logged builds, although those are mostly interesting when
having to debug a problem on a different computer). I'd recommend using the Debug build when
developing (WARNING: the debug build is set to automatically put the dll in the Winamp\plugins
directory, so be sure to adjust this path BEFORE you build it).

* alg\
** This directory is used by both the LineIn plugin and the File Writer plugin.

* alg\audio_conversions.h
** At the moment this just defines a struct that can be used as a 24bit integer (used by the
  channel remapper).

* alg\gen.*
** Some useful helper functions, if you want to display an error or warning, use the functions
  defined here. Other functions include date/time handling functions, multibyte/wide string
  conversion routines (wrappers for the relevant Win32 API functions) and array size macro and
  a logical XOR (sometimes useful).

* alg\msgbox.*
** A custom message box class. This basically mimics the normal Windows message boxes but lets
  you choose your own buttons (with virtually no restrictions). I've found this useful to
  provide users with buttons that say a little more than just Ok or Cancel and it is used by
  the Error and Warning functions in gen.cpp.

* Expander\
** A stripped-down version of the expander project used by the LineIn plugin (contains just the
  ExpandLib part, but not the project files, test program, etc.).

* file_t\
** File class which I find useful, it supports pretty much anything I've ever needed. The primary
  reason for creating it was to ease support of 64bit file sizes btw.

* in_line\
** The LineIn plugin source, including MSVC6 project files. I use STLPort to compile it, but any
  other STL should also work with only slight modifications needed.

* in_line\aliases.*
** The files that are responsible for resolving device aliases.

* in_line\audiotools.h
** A reasonably efficient channel remapper. Not assembly optimized or anything, but about as
  efficient as I could get it without resorting to assembly (let alone MMX, SSE, etc.).

* in_line\common.h
** Contains a few declarations needed throughout the plugin.

* in_line\configdlg.*
** The configuration dialog, as well as a wizard for adding line:// URLs to the playlist.
  I wouldn't even look at this unless you have to (over a thousand lines of very boring code).

* in_line\in.h,out.h
** Headers describing the Winamp plugin structures for input and output plugins (part of the
  Winamp SDK).

* in_line\in_line.cpp
** This is the main part of the plugin, this implements the Winamp plugin interface, along with
  a host of helper functions.

* in_line\preset.*
** Takes care of loading/storing presets.

* in_line\tool.*
** Title expanding, parameter parsing and some other miscellaneous things. One thing that might
  be useful if you're going to use a textbox or something somewhere is the Set/GetWindowText
  wrapper which works with std::string instead of char*. The stopme function can also be useful.

* in_line\wa_ipc.h
** Part of the Winamp SDK, contains some defines for messages that can be sent to Winamp.
