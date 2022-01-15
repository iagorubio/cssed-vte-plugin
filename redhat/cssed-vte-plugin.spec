%define prefix	/usr
%define name	cssed-vte-plugin
%define version	0.2
%define release 0%{?dist}

Summary:         Vte terminal plugin for cssed
Name:            %{name}
Version:         %{version}
Release:         %{release}
Group:           Development/Tools
License:         GPL
Source:          http://prdownloads.sourceforge.net/cssed/%{name}-%{version}.tar.gz
Url:             http://cssed.sourceforge.net
BuildRoot:       %{_tmppath}/cssed-%{version}-%{release}-root-%(%{__id_u} -n)
BuildArch:       i386
Requires:        gtk2 >= 2.0.6, glib2 >= 2.0, cssed >= 0.4.0, vte >= 0.11
Buildrequires:   cssed-devel >= 0.4.0, gtk2-devel >= 2.0.6, glib2-devel >= 2.0, vte-devel >= 0.11

%description 
cssed is a tiny GTK+ CSS editor and validator
for web developers.

This plugin provides an vte terminal into cssed.

%prep
%setup -q -n %{name}-%{version}

%build
[ ! -f Makefile ] || make distclean
%configure
make

%install
[ -n "$RPM_BUILD_ROOT" -a "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT
DESTDIR=$RPM_BUILD_ROOT make install

%makeinstall
%clean
[ -n "$RPM_BUILD_ROOT" -a "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT

%files 
%defattr(-,root,root)
%{_libdir}/cssed/plugins/vte_terminal.so
%{_datadir}/locale/es/LC_MESSAGES/%{name}.mo
%{_datadir}/locale/fr/LC_MESSAGES/%{name}.mo

%doc AUTHORS COPYING ChangeLog README INSTALL NEWS

%changelog
* Sun Sep 18 2005 Iago Rubio <iago.rubio@hispalinux.es> 0.2-0
- Updated to 0.2

* Wed Jun 02 2004 Iago Rubio <iago.rubio@hispalinux.es> 0.1-0
- Initiall RPM.



