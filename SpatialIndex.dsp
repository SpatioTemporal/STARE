# Microsoft Developer Studio Project File - Name="SpatialIndex" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=SpatialIndex - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SpatialIndex.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SpatialIndex.mak" CFG="SpatialIndex - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SpatialIndex - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SpatialIndex - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SpatialIndex - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "lib"
# PROP Intermediate_Dir "tmp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SPATIALINDEX_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /I "../include" /I "include" /I "src" /I "." /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_EXPORTING" /FD /c
# SUBTRACT CPP /Gf /Gy /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 opends60.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /debug /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy C:\Projects\HTM\SpatialIndex\lib\SpatialIndex.dll   C:\Projects\HTM\SpatialIndex\bin
# End Special Build Tool

!ELSEIF  "$(CFG)" == "SpatialIndex - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "lib"
# PROP Intermediate_Dir "tmp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SPATIALINDEX_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../include" /I "include" /I "src" /I "." /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_EXPORTING" /D "_WIN32" /D "_WINDLL" /D "_AFXDLL" /FR /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 opends60.lib /nologo /dll /map /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy lib\SpatialIndex.dll   ..\bin
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "SpatialIndex - Win32 Release"
# Name "SpatialIndex - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\cc_aux.c
# End Source File
# Begin Source File

SOURCE=.\src\Htmio.cpp
# End Source File
# Begin Source File

SOURCE=.\src\HtmRange.cpp
# End Source File
# Begin Source File

SOURCE=.\memleak.c
# End Source File
# Begin Source File

SOURCE=.\src\RangeConvex.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SkipList.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SkipListElement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SpatialConstraint.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SpatialDomain.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SpatialEdge.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SpatialException.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SpatialIndex.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SpatialInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SpatialVector.cpp
# End Source File
# Begin Source File

SOURCE=.\src\sqlInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\src\XP_Fast.Cpp
# End Source File
# Begin Source File

SOURCE=.\src\XP_HTM.Cpp

!IF  "$(CFG)" == "SpatialIndex - Win32 Release"

!ELSEIF  "$(CFG)" == "SpatialIndex - Win32 Debug"

# SUBTRACT CPP /O<none>

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\XP_SQT.Cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\include\BitList.h
# End Source File
# Begin Source File

SOURCE=.\include\Htmio.h
# End Source File
# Begin Source File

SOURCE=.\include\HtmRange.h
# End Source File
# Begin Source File

SOURCE=.\include\RangeConvex.h
# End Source File
# Begin Source File

SOURCE=.\include\SkipList.h
# End Source File
# Begin Source File

SOURCE=.\include\SkipListElement.h
# End Source File
# Begin Source File

SOURCE=.\include\SpatialConstraint.h
# End Source File
# Begin Source File

SOURCE=.\include\SpatialDoc.h
# End Source File
# Begin Source File

SOURCE=.\include\SpatialDomain.h
# End Source File
# Begin Source File

SOURCE=.\include\SpatialEdge.h
# End Source File
# Begin Source File

SOURCE=.\include\SpatialException.h
# End Source File
# Begin Source File

SOURCE=.\include\SpatialGeneral.h
# End Source File
# Begin Source File

SOURCE=.\include\SpatialIndex.h
# End Source File
# Begin Source File

SOURCE=.\include\SpatialInterface.h
# End Source File
# Begin Source File

SOURCE=.\include\SpatialVector.h
# End Source File
# Begin Source File

SOURCE=.\include\VarStr.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
