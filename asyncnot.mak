# asyncnot.mak
# Created by IBM WorkFrame/2 MakeMake at 16:46:26 on 19 Feb 1996
#
# The actions included in this make file are:
#  Compile::C++ Compiler
#  Link::Linker
#  Lib::Import Lib

.SUFFIXES: .LIB .cpp .dll .obj 

.all: \
    .\asyncnot.LIB

.cpp.obj:
    @echo " Compile::C++ Compiler "
    icc.exe /Gm /Gd /Ge- /C %s

.dll.LIB:
    @echo " Lib::Import Lib "
    implib.exe %|dpfF.LIB %s

.\asyncnot.dll: \
    .\iasynthr.obj \
    .\ievntsem.obj \
    .\iasynbkg.obj \
    .\iasyngui.obj \
    .\iasyntfy.obj \
    {$(LIB)}asyncnot.def
    @echo " Link::Linker "
    icc.exe @<<
    /Tdp 
     /Gm /Gd /Ge- 
     /B" /noe"
     /Feasyncnot.dll 
     asyncnot.def
     .\iasynthr.obj
     .\ievntsem.obj
     .\iasynbkg.obj
     .\iasyngui.obj
     .\iasyntfy.obj
<<

.\iasynthr.obj: \
    F:\threads\iasynthr.cpp

.\iasyntfy.obj: \
    F:\threads\iasyntfy.cpp

.\iasyngui.obj: \
    F:\threads\iasyngui.cpp

.\iasynbkg.obj: \
    F:\threads\iasynbkg.cpp

.\ievntsem.obj: \
    F:\threads\ievntsem.cpp

.\asyncnot.LIB: \
    .\asyncnot.dll
