SET QTDIR=c:\Qt\5.10.0.st
SET MINGWDIR=C:\Qt\5.10.0\Tools\mingw530_32
SET PYTHON2DIR=C:\QNX650\host\win32\x86\unsupported\Python25
SET PATH=%PATH%;%QTDIR%\bin;%MINGWDIR%\bin;%PYTHON2DIR%
call configure -I "c:\Qt\3dparty\openssl-1.0.2m\include" -L "c:\Qt\3dparty\openssl-1.0.2m" -opensource -confirm-license -platform win32-g++ -static -release -opengl desktop -qt-zlib -qt-libpng -qt-libjpeg -openssl -nomake examples -prefix %QTDIR%
mingw32-make
mingw32-make install
