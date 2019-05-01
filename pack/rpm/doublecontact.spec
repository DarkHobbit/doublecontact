Summary: The Contact manager
Name: doublecontact
Version: 0.2.0
Release: 20190501.f29
Group: Applications/PIM
License: GPL v3 or above (some parts under GPL v2)
Packager: Mikhail Y. Zvyozdochkin

%define ubin /usr/bin
%define ushare /usr/share

%Description
Offline DE-independent Qt-based contact manager primarily for phonebooks editing/merging

%Files 

%defattr(-,root,root)

%{ubin}/doublecontact
%{ubin}/contconv
%{ushare}/doublecontact/translations/*
%{ushare}/doc/doublecontact/*
%{ushare}/applications/*
%{ushare}/pixmaps/*
%Pre

%Post

%Preun

%Postun
