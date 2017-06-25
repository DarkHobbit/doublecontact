# DoubleContact [![Travis CI](https://travis-ci.org/DarkHobbit/doublecontact.svg?branch=master)](https://travis-ci.org/DarkHobbit/doublecontact) [![Coverity Scan Build Status](https://scan.coverity.com/projects/10139/badge.svg)](https://scan.coverity.com/projects/darkhobbit-doublecontact)
Offline DE-independent contact manager primarily for phonebooks editing/merging.

License: GPLv2 or above.

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
