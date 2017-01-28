set PKG_DIR=..\..\doublecontact_0.1_win32

call ..\make-bin-image.bat ..\.. ..\..\..\build-all-Qt_4_8_6_st-Release C:\Qt\2009.05\mingw %PKG_DIR%
copy doublecontact.iss %PKG_DIR%\doublecontact.iss

cd %PKG_DIR%\Output
ren setup.exe doublecontact_0.1_win32_setup.exe
