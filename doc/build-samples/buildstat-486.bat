SET QTDIR=c:\Qt\4.8.6.st
SET MINGWDIR=c:\Qt\2009.05\mingw
SET QMAKESPEC=win32-g++

SET PATH=%PATH%;%QTDIR%\bin;%MINGWDIR%\bin

configure -opensource -confirm-license -platform win32-g++ -static -release -no-exceptions -qt-zlib -qt-libpng -qt-libmng -qt-libjpeg -no-qt3support -no-phonon -no-webkit -nomake examples -nomake demos -prefix %QTDIR%
mingw32-make