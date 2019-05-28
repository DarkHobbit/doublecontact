rem Sample of build_win_release.bat using`

set DIR_SRC=..\doublecontact
rem set DIR_BUILD=..\build-manual-Release
set DIR_BUILD=..\build-all-Qt_4_8_6_st-Release
set DIR_QT=c:\qt\4.8.6.st
set DIR_MINGW=c:\Qt\2009.05\MinGW
set DIR_DIST=..\bin-Release

set path=%path%;%DIR_MINGW%\bin;%DIR_QT%\bin
call build_win_release.bat %DIR_SRC% %DIR_BUILD% %DIR_QT%
if errorlevel 1 goto fatal

call .\pack\make-bin-image.bat %DIR_SRC% %DIR_BUILD% %DIR_MINGW% %DIR_QT% %DIR_DIST%
exit

:fatal
exit 1
