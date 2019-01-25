# Microsoft Developer Studio Generated NMAKE File, Based on SpatialIndex.dsp
!IF "$(CFG)" == ""
CFG=SpatialIndex - Win32 Debug
!MESSAGE No configuration specified. Defaulting to SpatialIndex - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "SpatialIndex - Win32 Release" && "$(CFG)" != "SpatialIndex - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "SpatialIndex - Win32 Release"

OUTDIR=.\lib
INTDIR=.\tmp
# Begin Custom Macros
OutDir=.\lib
# End Custom Macros

ALL : "$(OUTDIR)\SpatialIndex.dll"


CLEAN :
	-@erase "$(INTDIR)\BitList.obj"
	-@erase "$(INTDIR)\SpatialConstraint.obj"
	-@erase "$(INTDIR)\SpatialConvex.obj"
	-@erase "$(INTDIR)\SpatialDomain.obj"
	-@erase "$(INTDIR)\SpatialEdge.obj"
	-@erase "$(INTDIR)\SpatialException.obj"
	-@erase "$(INTDIR)\SpatialIndex.obj"
	-@erase "$(INTDIR)\SpatialInterface.obj"
	-@erase "$(INTDIR)\SpatialVector.obj"
	-@erase "$(INTDIR)\sqlInterface.obj"
	-@erase "$(INTDIR)\VarCmp.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\XP_HTM.obj"
	-@erase "$(INTDIR)\XP_SQT.obj"
	-@erase "$(OUTDIR)\SpatialIndex.dll"
	-@erase "$(OUTDIR)\SpatialIndex.exp"
	-@erase "$(OUTDIR)\SpatialIndex.ilk"
	-@erase "$(OUTDIR)\SpatialIndex.lib"
	-@erase "$(OUTDIR)\SpatialIndex.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /I "../include" /I "include" /I "src" /I "." /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_EXPORTING" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\SpatialIndex.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=opends60.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\SpatialIndex.pdb" /debug /machine:I386 /out:"$(OUTDIR)\SpatialIndex.dll" /implib:"$(OUTDIR)\SpatialIndex.lib" 
LINK32_OBJS= \
	"$(INTDIR)\BitList.obj" \
	"$(INTDIR)\SpatialConstraint.obj" \
	"$(INTDIR)\SpatialConvex.obj" \
	"$(INTDIR)\SpatialDomain.obj" \
	"$(INTDIR)\SpatialEdge.obj" \
	"$(INTDIR)\SpatialException.obj" \
	"$(INTDIR)\SpatialIndex.obj" \
	"$(INTDIR)\SpatialInterface.obj" \
	"$(INTDIR)\SpatialVector.obj" \
	"$(INTDIR)\sqlInterface.obj" \
	"$(INTDIR)\VarCmp.obj" \
	"$(INTDIR)\XP_HTM.obj" \
	"$(INTDIR)\XP_SQT.obj"

"$(OUTDIR)\SpatialIndex.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\lib
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\SpatialIndex.dll"
   copy C:\Projects\HTM\SpatialIndex\lib\SpatialIndex.dll   C:\Projects\HTM\SpatialIndex\bin
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "SpatialIndex - Win32 Debug"

OUTDIR=.\lib
INTDIR=.\tmp
# Begin Custom Macros
OutDir=.\lib
# End Custom Macros

ALL : "$(OUTDIR)\SpatialIndex.dll" "$(OUTDIR)\SpatialIndex.bsc"


CLEAN :
	-@erase "$(INTDIR)\BitList.obj"
	-@erase "$(INTDIR)\BitList.sbr"
	-@erase "$(INTDIR)\SpatialConstraint.obj"
	-@erase "$(INTDIR)\SpatialConstraint.sbr"
	-@erase "$(INTDIR)\SpatialConvex.obj"
	-@erase "$(INTDIR)\SpatialConvex.sbr"
	-@erase "$(INTDIR)\SpatialDomain.obj"
	-@erase "$(INTDIR)\SpatialDomain.sbr"
	-@erase "$(INTDIR)\SpatialEdge.obj"
	-@erase "$(INTDIR)\SpatialEdge.sbr"
	-@erase "$(INTDIR)\SpatialException.obj"
	-@erase "$(INTDIR)\SpatialException.sbr"
	-@erase "$(INTDIR)\SpatialIndex.obj"
	-@erase "$(INTDIR)\SpatialIndex.sbr"
	-@erase "$(INTDIR)\SpatialInterface.obj"
	-@erase "$(INTDIR)\SpatialInterface.sbr"
	-@erase "$(INTDIR)\SpatialVector.obj"
	-@erase "$(INTDIR)\SpatialVector.sbr"
	-@erase "$(INTDIR)\sqlInterface.obj"
	-@erase "$(INTDIR)\sqlInterface.sbr"
	-@erase "$(INTDIR)\VarCmp.obj"
	-@erase "$(INTDIR)\VarCmp.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\XP_HTM.obj"
	-@erase "$(INTDIR)\XP_HTM.sbr"
	-@erase "$(INTDIR)\XP_SQT.obj"
	-@erase "$(INTDIR)\XP_SQT.sbr"
	-@erase "$(OUTDIR)\SpatialIndex.bsc"
	-@erase "$(OUTDIR)\SpatialIndex.dll"
	-@erase "$(OUTDIR)\SpatialIndex.exp"
	-@erase "$(OUTDIR)\SpatialIndex.ilk"
	-@erase "$(OUTDIR)\SpatialIndex.lib"
	-@erase "$(OUTDIR)\SpatialIndex.pdb"
	-@erase ".\tmp\SpatialIndex.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../include" /I "include" /I "src" /I "." /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_EXPORTING" /D "_WINDLL" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\SpatialIndex.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\BitList.sbr" \
	"$(INTDIR)\SpatialConstraint.sbr" \
	"$(INTDIR)\SpatialConvex.sbr" \
	"$(INTDIR)\SpatialDomain.sbr" \
	"$(INTDIR)\SpatialEdge.sbr" \
	"$(INTDIR)\SpatialException.sbr" \
	"$(INTDIR)\SpatialIndex.sbr" \
	"$(INTDIR)\SpatialInterface.sbr" \
	"$(INTDIR)\SpatialVector.sbr" \
	"$(INTDIR)\sqlInterface.sbr" \
	"$(INTDIR)\VarCmp.sbr" \
	"$(INTDIR)\XP_HTM.sbr" \
	"$(INTDIR)\XP_SQT.sbr"

"$(OUTDIR)\SpatialIndex.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=opends60.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\SpatialIndex.pdb" /map:"$(INTDIR)\SpatialIndex.map" /debug /machine:I386 /out:"$(OUTDIR)\SpatialIndex.dll" /implib:"$(OUTDIR)\SpatialIndex.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\BitList.obj" \
	"$(INTDIR)\SpatialConstraint.obj" \
	"$(INTDIR)\SpatialConvex.obj" \
	"$(INTDIR)\SpatialDomain.obj" \
	"$(INTDIR)\SpatialEdge.obj" \
	"$(INTDIR)\SpatialException.obj" \
	"$(INTDIR)\SpatialIndex.obj" \
	"$(INTDIR)\SpatialInterface.obj" \
	"$(INTDIR)\SpatialVector.obj" \
	"$(INTDIR)\sqlInterface.obj" \
	"$(INTDIR)\VarCmp.obj" \
	"$(INTDIR)\XP_HTM.obj" \
	"$(INTDIR)\XP_SQT.obj"

"$(OUTDIR)\SpatialIndex.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\lib
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\SpatialIndex.dll" "$(OUTDIR)\SpatialIndex.bsc"
   copy lib\SpatialIndex.dll   bin
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("SpatialIndex.dep")
!INCLUDE "SpatialIndex.dep"
!ELSE 
!MESSAGE Warning: cannot find "SpatialIndex.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "SpatialIndex - Win32 Release" || "$(CFG)" == "SpatialIndex - Win32 Debug"
SOURCE=.\src\BitList.cpp

!IF  "$(CFG)" == "SpatialIndex - Win32 Release"


"$(INTDIR)\BitList.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SpatialIndex - Win32 Debug"


"$(INTDIR)\BitList.obj"	"$(INTDIR)\BitList.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\SpatialConstraint.cpp

!IF  "$(CFG)" == "SpatialIndex - Win32 Release"


"$(INTDIR)\SpatialConstraint.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SpatialIndex - Win32 Debug"


"$(INTDIR)\SpatialConstraint.obj"	"$(INTDIR)\SpatialConstraint.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\SpatialConvex.cpp

!IF  "$(CFG)" == "SpatialIndex - Win32 Release"


"$(INTDIR)\SpatialConvex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SpatialIndex - Win32 Debug"


"$(INTDIR)\SpatialConvex.obj"	"$(INTDIR)\SpatialConvex.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\SpatialDomain.cpp

!IF  "$(CFG)" == "SpatialIndex - Win32 Release"


"$(INTDIR)\SpatialDomain.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SpatialIndex - Win32 Debug"


"$(INTDIR)\SpatialDomain.obj"	"$(INTDIR)\SpatialDomain.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\SpatialEdge.cpp

!IF  "$(CFG)" == "SpatialIndex - Win32 Release"


"$(INTDIR)\SpatialEdge.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SpatialIndex - Win32 Debug"


"$(INTDIR)\SpatialEdge.obj"	"$(INTDIR)\SpatialEdge.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\SpatialException.cpp

!IF  "$(CFG)" == "SpatialIndex - Win32 Release"


"$(INTDIR)\SpatialException.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SpatialIndex - Win32 Debug"


"$(INTDIR)\SpatialException.obj"	"$(INTDIR)\SpatialException.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\SpatialIndex.cpp

!IF  "$(CFG)" == "SpatialIndex - Win32 Release"


"$(INTDIR)\SpatialIndex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SpatialIndex - Win32 Debug"


"$(INTDIR)\SpatialIndex.obj"	"$(INTDIR)\SpatialIndex.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\SpatialInterface.cpp

!IF  "$(CFG)" == "SpatialIndex - Win32 Release"


"$(INTDIR)\SpatialInterface.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SpatialIndex - Win32 Debug"


"$(INTDIR)\SpatialInterface.obj"	"$(INTDIR)\SpatialInterface.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\SpatialVector.cpp

!IF  "$(CFG)" == "SpatialIndex - Win32 Release"


"$(INTDIR)\SpatialVector.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SpatialIndex - Win32 Debug"


"$(INTDIR)\SpatialVector.obj"	"$(INTDIR)\SpatialVector.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\sqlInterface.cpp

!IF  "$(CFG)" == "SpatialIndex - Win32 Release"


"$(INTDIR)\sqlInterface.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SpatialIndex - Win32 Debug"


"$(INTDIR)\sqlInterface.obj"	"$(INTDIR)\sqlInterface.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\VarCmp.cpp

!IF  "$(CFG)" == "SpatialIndex - Win32 Release"


"$(INTDIR)\VarCmp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SpatialIndex - Win32 Debug"


"$(INTDIR)\VarCmp.obj"	"$(INTDIR)\VarCmp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\XP_HTM.Cpp

!IF  "$(CFG)" == "SpatialIndex - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /I "../include" /I "include" /I "src" /I "." /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_EXPORTING" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\XP_HTM.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "SpatialIndex - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /I "../include" /I "include" /I "src" /I "." /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_EXPORTING" /D "_WINDLL" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\XP_HTM.obj"	"$(INTDIR)\XP_HTM.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\src\XP_SQT.Cpp

!IF  "$(CFG)" == "SpatialIndex - Win32 Release"


"$(INTDIR)\XP_SQT.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "SpatialIndex - Win32 Debug"


"$(INTDIR)\XP_SQT.obj"	"$(INTDIR)\XP_SQT.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

