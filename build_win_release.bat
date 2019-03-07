rem DoubleContact shadow build for Windows

set DIR_SRC=%1
set DIR_BUILD=%2
set DIR_QT=%3

if exist "%DIR_BUILD%" goto skipmkdir
mkdir "%DIR_BUILD%"
:skipmkdir

cd "%DIR_BUILD%"
mkdir app
mkdir contconv

%DIR_QT%\bin\qmake.exe "%DIR_SRC%\all.pro"
if errorlevel 1 goto fatal
mingw32-make release
if errorlevel 1 goto fatal
cd "%DIR_SRC%"
exit /b

:fatal
echo "Build failed"
exit 1
