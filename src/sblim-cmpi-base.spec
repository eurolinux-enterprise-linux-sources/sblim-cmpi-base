#
# $Id: sblim-cmpi-base.spec.in,v 1.13 2009/07/25 00:37:31 tyreld Exp $
#
# Package spec for sblim-cmpi-base
#

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

Summary: SBLIM Base Providers
Name: sblim-cmpi-base
Version: 1.6.2
Release: 1
Group: Systems Management/Base
URL: http://www.sblim.org
License: EPL

Source0: http://prdownloads.sourceforge.net/sblim/%{name}-%{version}.tar.bz2

BuildRequires: cmpi-devel
Requires: cimserver

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

%configure TESTSUITEDIR=%{_datadir}/sblim-testsuite 

make %{?_smp_mflags}

%install

[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT

make DESTDIR=$RPM_BUILD_ROOT install

# remove unused libtool files
rm -f $RPM_BUILD_ROOT/%{_libdir}/*a
rm -f $RPM_BUILD_ROOT/%{_libdir}/cmpi/*a

%pre
# Conditional definition of schema and registration files
%if "-lind_helper" != ""
%define SCHEMA %{_datadir}/%{name}/Linux_Base.mof %{_datadir}/%{name}/Linux_BaseIndication.mof
%define REGISTRATION %{_datadir}/%{name}/Linux_BaseIndication.registration
%else
%define SCHEMA %{_datadir}/%{name}/Linux_Base.mof
%define REGISTRATION %{_datadir}/%{name}/Linux_Base.registration
%endif

# If upgrading, deregister old version
if [ $1 -gt 1 ]
then
  %{_datadir}/%{name}/provider-register.sh -d \
	-r %{REGISTRATION} -m %{SCHEMA} > /dev/null
fi

%post
# Register Schema and Provider - this is higly provider specific

%{_datadir}/%{name}/provider-register.sh \
	-r %{REGISTRATION} -m %{SCHEMA} > /dev/null

/sbin/ldconfig

%preun
# Deregister only if not upgrading 
if [ $1 -eq 0 ]
then
  %{_datadir}/%{name}/provider-register.sh -d \
	-r %{REGISTRATION} -m %{SCHEMA} > /dev/null
fi

%postun -p /sbin/ldconfig

%clean

[ "$RPM_BUILD_ROOT" != "/" ] && rm -rf $RPM_BUILD_ROOT

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
* Wed Oct 28 2005 Viktor Mihajlovski <mihajlov@de.ibm.com>	1.5.4-1
  - enhanced commonality with RH spec file

* Wed Jul 20 2005 Mark Hamzy <hamzy@us.ibm.com>	1.5.3-1
  - initial support
