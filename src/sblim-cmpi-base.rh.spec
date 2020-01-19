#
# $Id: sblim-cmpi-base.rh.spec,v 1.4 2009/07/25 00:37:31 tyreld Exp $
#
# Package spec for sblim-cmpi-base - RedHat/Fedora Flavor
#
# Use this SPEC if building for a RH/Fedora System for usage with
# OpenPegasus
#

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

Summary: SBLIM Base Providers
Name: sblim-cmpi-base
Version: 1.5.4
Release: 3
Group: Systems Management/Base
URL: http://www.sblim.org
License: EPL

Source0: http://download.sourceforge.net/pub/sourceforge/s/sb/sblim/%{name}-%{version}.tar.bz2

BuildRequires: tog-pegasus-devel >= 2.5
Requires: tog-pegasus >= 2.5.1

%Description
Standards Based Linux Instrumentation Base CMPI Providers for
System-related CIM classes

%Package devel
Summary: SBLIM Base Instrumentation Header Development Files
Group: Systems Management/Base
Requires: %{name} = %{version}-%{release}

%Description devel
SBLIM Base Provider Development Package contains header files and 
link libraries for dependent provider packages

%Package test
Summary: SBLIM Base Instrumentation Testcase Files
Group: Systems Management/Base
Requires: %{name} = %{version}-%{release}
Requires: sblim-testsuite

%Description test
SBLIM Base Provider Testcase Files for the SBLIM Testsuite

%prep

%setup -q

%build

%configure TESTSUITEDIR=%{_datadir}/sblim-testsuite \
	CIMSERVER=pegasus
make %{?_smp_mflags}

%install

rm -rf $RPM_BUILD_ROOT

make DESTDIR=$RPM_BUILD_ROOT install

# remove unused libtool files
rm -f $RPM_BUILD_ROOT/%{_libdir}/*a
rm -f $RPM_BUILD_ROOT/%{_libdir}/cmpi/*a

%pre

%define SCHEMA %{_datadir}/%{name}/Linux_Base.mof
%define REGISTRATION %{_datadir}/%{name}/Linux_Base.registration

# If upgrading, deregister old version
if [ $1 -gt 1 ]
then
  %{_datadir}/%{name}/provider-register.sh -d -t pegasus \
	-r %{REGISTRATION} -m %{SCHEMA} > /dev/null 2>&1
fi

%post
# Register Schema and Provider - this is higly provider specific

%{_datadir}/%{name}/provider-register.sh -t pegasus \
	-r %{REGISTRATION} -m %{SCHEMA} > /dev/null  2>&1

/sbin/ldconfig

%preun
# Deregister only if not upgrading 
if [ $1 -eq 0 ]
then
  %{_datadir}/%{name}/provider-register.sh -d -t pegasus \
	-r %{REGISTRATION} -m %{SCHEMA} > /dev/null 2>&1
fi

%postun -p /sbin/ldconfig

%clean

rm -rf $RPM_BUILD_ROOT

%files

%defattr(-,root,root) 
%docdir %{_datadir}/doc/%{name}-%{version}
%{_datadir}/%{name}
%{_datadir}/doc/%{name}-%{version}
%{_libdir}/*.so.*
%{_libdir}/cmpi/*.so

%files devel

%defattr(-,root,root)
%{_includedir}/*
%{_libdir}/*.so

%files test

%defattr(-,root,root)
%{_datadir}/sblim-testsuite

%changelog

* Thu Nov 10 2005  <mihajlov@de.ibm.com> - 1.5.4-3
- suppress error output in post scriptlets

* Wed Oct 27 2005  <mihajlov@de.ibm.com> - 1.5.4-2
- went back to original provider dir location as FC5 pegasus 2.5.1 support
  /usr/lib[64]/cmpi

* Wed Oct 12 2005  <mihajlov@de.ibm.com> - 1.5.4-1
- new spec file specifically for Fedora/RedHat

* Wed Jul 20 2005 Mark Hamzy <hamzy@us.ibm.com>	1.5.3-1
- initial support
