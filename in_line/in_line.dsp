# Microsoft Developer Studio Project File - Name="in_line" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=in_line - Win32 Debug Logged
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "in_line.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "in_line.mak" CFG="in_line - Win32 Debug Logged"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "in_line - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "in_line - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "in_line - Win32 Debug Logged" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "in_line - Win32 Release Old Expand" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "in_line - Win32 Release Old Expand Logged" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/in_line", PAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "in_line - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "." /I "..\Expander" /D "NDEBUG" /D "NEWEXPAND" /D "WIN32" /D "_WINDOWS" /D "WINAMP" /D "NOMINMAX" /D "STRICT" /D "_STLP_NEW_PLATFORM_SDK" /FR /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 gdi32.lib dxguid.lib advapi32.lib winmm.lib kernel32.lib user32.lib comctl32.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"libcmtd.lib" /opt:ref /release
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ELSEIF  "$(CFG)" == "in_line - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "." /I "..\Expander" /D "_DEBUG" /D "NEWEXPAND" /D "WIN32" /D "_WINDOWS" /D "WINAMP" /D "NOMINMAX" /D "STRICT" /D "_STLP_NEW_PLATFORM_SDK" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 gdi32.lib dxguid.lib advapi32.lib winmm.lib kernel32.lib user32.lib comctl32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libcmt.lib" /out:"c:\program files\winamp5\plugins\in_line.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "in_line - Win32 Debug Logged"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "in_line___Win32_Debug_Logged"
# PROP BASE Intermediate_Dir "in_line___Win32_Debug_Logged"
# PROP BASE Ignore_Export_Lib 1
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "in_line___Win32_Debug_Logged"
# PROP Intermediate_Dir "in_line___Win32_Debug_Logged"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "i:/src/fltk" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WINAMP" /FR /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\Expander" /I "." /D "_DEBUG" /D "LOGGING" /D "WIN32" /D "_WINDOWS" /D "WINAMP" /D "NOMINMAX" /D "STRICT" /D "_STLP_NEW_PLATFORM_SDK" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 gdi32.lib dxguid.lib advapi32.lib winmm.lib kernel32.lib user32.lib comctl32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"msvcrt" /out:"c:\program files\winamp\plugins\in_line.dll" /pdbtype:sept /libpath:"i:/src/fltk/lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 gdi32.lib dxguid.lib advapi32.lib winmm.lib kernel32.lib user32.lib comctl32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"c:\program files\winamp5\plugins\in_line.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "in_line - Win32 Release Old Expand"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "in_line___Win32_Release_Old_Expand"
# PROP BASE Intermediate_Dir "in_line___Win32_Release_Old_Expand"
# PROP BASE Ignore_Export_Lib 1
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "in_line___Win32_Release_Old_Expand"
# PROP Intermediate_Dir "in_line___Win32_Release_Old_Expand"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "D:\Progs\Expander" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WINAMP" /FR /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\Expander" /I "." /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WINAMP" /D "NOMINMAX" /D "STRICT" /D "_STLP_NEW_PLATFORM_SDK" /FR /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 gdi32.lib dxguid.lib advapi32.lib winmm.lib kernel32.lib user32.lib comctl32.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"libcmtd.lib" /opt:ref /release
# SUBTRACT BASE LINK32 /pdb:none /nodefaultlib
# ADD LINK32 gdi32.lib dxguid.lib advapi32.lib winmm.lib kernel32.lib user32.lib comctl32.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"libcmtd.lib" /opt:ref /release
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ELSEIF  "$(CFG)" == "in_line - Win32 Release Old Expand Logged"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "in_line___Win32_Release_Old_Expand_Logged"
# PROP BASE Intermediate_Dir "in_line___Win32_Release_Old_Expand_Logged"
# PROP BASE Ignore_Export_Lib 1
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "in_line___Win32_Release_Old_Expand_Logged"
# PROP Intermediate_Dir "in_line___Win32_Release_Old_Expand_Logged"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "D:\Progs\Expander" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WINAMP" /FR /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\Expander" /I "." /D "NDEBUG" /D "LOGGING" /D "WIN32" /D "_WINDOWS" /D "WINAMP" /D "NOMINMAX" /D "STRICT" /D "_STLP_NEW_PLATFORM_SDK" /FR /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 gdi32.lib dxguid.lib advapi32.lib winmm.lib kernel32.lib user32.lib comctl32.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"libcmtd.lib" /opt:ref /release
# SUBTRACT BASE LINK32 /pdb:none /nodefaultlib
# ADD LINK32 gdi32.lib dxguid.lib advapi32.lib winmm.lib kernel32.lib user32.lib comctl32.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib:"libcmtd.lib" /opt:ref /release
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ENDIF 

# Begin Target

# Name "in_line - Win32 Release"
# Name "in_line - Win32 Debug"
# Name "in_line - Win32 Debug Logged"
# Name "in_line - Win32 Release Old Expand"
# Name "in_line - Win32 Release Old Expand Logged"
# Begin Group "Headers"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\aliases.h
# End Source File
# Begin Source File

SOURCE=..\alg\audio_conversions.h
# End Source File
# Begin Source File

SOURCE=.\audiotools.h
# End Source File
# Begin Source File

SOURCE=.\common.h
# End Source File
# Begin Source File

SOURCE=.\configdlg.h
# End Source File
# Begin Source File

SOURCE=..\alg\gen.h
# End Source File
# Begin Source File

SOURCE=.\in.h
# End Source File
# Begin Source File

SOURCE=..\alg\msgbox.h
# End Source File
# Begin Source File

SOURCE=.\out.h
# End Source File
# Begin Source File

SOURCE=.\preset.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\tool.h
# End Source File
# Begin Source File

SOURCE=.\wa_ipc.h
# End Source File
# End Group
# Begin Group "Source"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\aliases.cpp
# End Source File
# Begin Source File

SOURCE=.\configdlg.cpp
# End Source File
# Begin Source File

SOURCE=..\alg\gen.cpp
# End Source File
# Begin Source File

SOURCE=.\in_line.cpp
# End Source File
# Begin Source File

SOURCE=..\alg\msgbox.cpp
# End Source File
# Begin Source File

SOURCE=.\preset.cpp
# End Source File
# Begin Source File

SOURCE=.\tool.cpp
# End Source File
# End Group
# Begin Group "Resources"

# PROP Default_Filter "rc"
# Begin Source File

SOURCE=.\Script1.rc
# End Source File
# End Group
# Begin Source File

SOURCE=.\changes.txt
# End Source File
# Begin Source File

SOURCE=.\in_line.def
# End Source File
# End Target
# End Project
