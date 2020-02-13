# DoubleContact [![Travis CI](https://travis-ci.org/DarkHobbit/doublecontact.svg?branch=master)](https://travis-ci.org/DarkHobbit/doublecontact) [![Coverity Scan Build Status](https://scan.coverity.com/projects/10139/badge.svg)](https://scan.coverity.com/projects/darkhobbit-doublecontact)
Offline DE-independent contact manager primarily for phonebooks editing/merging.

License: GPLv3 or above. (Some parts of code, except NBU support can be distributed under GPLv2.)

Language: C++.

Libraries: Qt4/5.

## Build ##

Use qmake for build doublecontact. Project all.pro in repo root builds both GUI (doublecontact) and console (contconv) application.

Repo also contains config files for cmake, but currently it isn't usable. Use qmake.

### Build instructions ###

#### Ubuntu ####

sudo apt-get install qt4-qmake

git clone https://github.com/DarkHobbit/doublecontact.git

cd doublecontact

qmake

make

## Help ##

### User manual ###

DoubleContact user manual [is available](https://github.com/DarkHobbit/doublecontact/blob/master/doc/manual.en.md) on Github.

### Translations ###

Please, use [Weblate](https://hosted.weblate.org/projects/doublecontact/) if you want to translate program on a new language.

Please, don't place ellipsis and other non-latin character in source files directly. Place this into translation files (including
English).
