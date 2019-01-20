set SRC_DIR=%1
set EXE_DIR=%2
set MGW_DIR=%3
set QT_DIR=%4
set PKG_DIR=%5

set BIN_PATH=%PKG_DIR%
set TRANS_PATH=%PKG_DIR%
set DOC_PATH=%PKG_DIR%\doc

md %BIN_PATH%
md %DOC_PATH%

copy %EXE_DIR%\app\release\doublecontact.exe %BIN_PATH%\
copy %EXE_DIR%\contconv\release\contconv.exe %BIN_PATH%\
copy %SRC_DIR%\img\32x32\doublecontact_32x32.png %BIN_PATH%\
copy %SRC_DIR%\translations\*.qm %TRANS_PATH%\
copy %QT_DIR%\translations\qt_??.qm %TRANS_PATH%\
copy %QT_DIR%\translations\qt_??_*.qm %TRANS_PATH%\
copy %SRC_DIR%\translations\iso639-1.utf8 %TRANS_PATH%\
copy %SRC_DIR%\doc\* %DOC_PATH%\
copy %SRC_DIR%\COPYING %DOC_PATH%\
copy %SRC_DIR%\README.md %DOC_PATH%\
copy %MGW_DIR%\bin\libgcc_s_dw2-1.dll %BIN_PATH%\
rem copy %MGW_DIR%\bin\mingwm10.dll %BIN_PATH%\