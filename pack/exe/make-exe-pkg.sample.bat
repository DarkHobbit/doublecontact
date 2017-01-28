Rem This script requires Inno Setup
Rem Change paths to build, MinGW and Inno Setup!

set PKG_DIR=..\..\doublecontact_0.1_win32

call ..\make-bin-image.bat ..\.. ..\..\..\build-all-Qt_4_8_6_st-Release C:\Qt\2009.05\mingw %PKG_DIR%
copy doublecontact.iss %PKG_DIR%\doublecontact.iss

cd %PKG_DIR%
"C:\Program Files (x86)\Inno Setup 5\Compil32.exe" /cc doublecontact.iss
cd Output
