# DoubleContact Build Manual #

## General ##

Use qmake for build doublecontact. Project all.pro in repo root builds both GUI (doublecontact) and console (contconv) application.

Repo also contains config files for cmake, but currently it isn't usable. Use qmake.

C++ compiler with C++03 aka "C with classes and some templates" also required. Author built program with gcc C++ (Linux), clang (macOS) and MinGW (Windows).

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

After Qt install:

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

