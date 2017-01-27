Summary: The Contact manager
Name: doublecontact
Version: 0.1
Release: 20170127.f23
Group: Applications/PIM
License: GPL v2 or above
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

%Pre

%Post

%Preun

%Postun
