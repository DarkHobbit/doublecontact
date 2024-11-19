# DoubleContact Build Manual #

## General ##

Use qmake for build doublecontact. Project all.pro in repo root builds both GUI (doublecontact) and console (contconv) application.

Repo also contains config files for cmake, but currently it isn't usable. Use qmake.

C++ compiler with C++03 aka "C with classes and some templates" also required. Author built program with gcc C++ (Linux), clang (macOS) and MinGW (Windows).

Supported Qt versions:
* Qt4 (>=4.6)
* Qt5 (any but >=5.10 is recommended)

Qt6 is not supported yet.

## Ubuntu and Debian ##

At first, you need g++, GNU make and standard libraries. The best way to get it is to install `build-essential` package.

To get Qt4:

```
sudo apt-get install qt4-qmake
```

To get Qt5 (you may need install zlib before qmake):

```
sudo apt-get install zlib1g-dev qt5-qmake qtbase5-dev libqt5scripttools5 qttools5-dev-tools
```

TODO: in pack/deb/DEBIAN, control file adopted to Qt4 only, it will fixed later.

## Fedora ##

To get build tools:

```
sudo dnf install make gcc gcc-c++
```

To get Qt5:

```
sudo dnf install zlib-devel qt5-qtbase-devel
```

## After Qt install (any Linux) ##

```
git clone https://github.com/DarkHobbit/doublecontact.git
cd doublecontact
qmake
make
```

In some distributions, you may need use `qmake-qt5` (`qmake-qt4`) command instead `qmake` if some Qt versions are present and `qtchooser` package installed.

## Arch and Manjaro ##

In Arch-based distributions, `base-devel` package contains build tools.

DoubleContact [is now available](https://aur.archlinux.org/packages/doublecontact) in AUR. You can install ready package through `pacaur` or use [its PKGBUILD](https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=doublecontact) as a template for your own build.

## Static Linux build ##

To get static doublecontact build, at first, build Qt from git repo or opensource tarball.
You can exclude unused Qt components, such as QtWebEngine, QML, openGL.
You can see  a sample of build script yin build-samples/build-qt-stat-512 file.
Qt building may take several hours.

After it, you must call define manually QTDIR variable and call qmake by absolute path from Qt directory.

TODO sample

## macOS ##

TODO

## Windows ##

Preferable way for getting Windows build is to use any Qt SDK based 
on MinGW C++, Qt Creator and one of supported Qt versions (see General section).

You also can make static build. See build-samples/buildstat-486.bat and build-samples/buildstat-510.bat for examples
(second file requires path to Python and OpenSSL from MSYS installation, currently you can exclude OpenSSL from your build).

