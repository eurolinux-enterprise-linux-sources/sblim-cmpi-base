Name:           sblim-cmpi-base
Version:        1.6.2
Release:        5%{?dist}
Summary:        SBLIM CMPI Base Providers

Group:          Applications/System
License:        EPL
URL:            http://sblim.wiki.sourceforge.net/
Source0:        http://downloads.sourceforge.net/sblim/%{name}-%{version}.tar.bz2
Patch0:         sblim-cmpi-base-1.6.0-missing-fclose.patch
Patch1:         sblim-cmpi-base-1.6.0-methods-enable.patch
Patch2:         sblim-cmpi-base-1.6.0-provider-register-sfcb-init.patch
Patch3:         sblim-cmpi-base-1.6.1-double-fclose.patch
# Patch4: already upstream, http://sourceforge.net/p/sblim/bugs/2634/
Patch4:         sblim-cmpi-base-1.6.2-max-cpu-frequency.patch
# Patch5: already upstream, http://sourceforge.net/p/sblim/bugs/2644/
Patch5:         sblim-cmpi-base-1.6.2-wrong-UserModeTime-and-KernelModeTime.patch
# Patch6: use Pegasus root/interop instead of root/PG_Interop
Patch6:         sblim-cmpi-base-1.6.2-pegasus-interop.patch
Requires:       cim-server sblim-indication_helper
BuildRequires:  sblim-cmpi-devel sblim-indication_helper-devel


%description
SBLIM (Standards Based Linux Instrumentation for Manageability)
CMPI (Common Manageability Programming Interface) Base Providers
for System-Related CIM (Common Information Model) classes.

%package devel
Summary:        SBLIM CMPI Base Providers Development Header Files
Group:          Development/Libraries
Requires:       %{name} = %{version}-%{release}

%description devel
SBLIM (Standards Based Linux Instrumentation for Manageability)
CMPI (Common Manageability Programming Interface) Base Provider
development header files and link libraries.

%package test
Summary:        SBLIM CMPI Base Providers Test Cases
Group:          Applications/System
Requires:       %{name} = %{version}-%{release}
Requires:       sblim-testsuite

%description test
SBLIM (Standards Based Linux Instrumentation for Manageability)
CMPI (Common Manageability Programming Interface) Base Provider
Testcase Files for the SBLIM Testsuite.

%prep
%setup -q
%patch0 -p0 -b .missing-fclose
%patch1 -p0 -b .methods-enable
%patch2 -p1 -b .provider-register-sfcb-init
%patch3 -p1 -b .double-fclose
%patch4 -p1 -b .cpu-freq
%patch5 -p1 -b .wrong-UserModeTime-and-KernelModeTime
%patch6 -p1 -b .pegasus-interop

%build
%configure TESTSUITEDIR=%{_datadir}/sblim-testsuite --disable-static
sed -i 's|^hardcode_libdir_flag_spec=.*|hardcode_libdir_flag_spec=""|g' libtool
sed -i 's|^runpath_var=LD_RUN_PATH|runpath_var=DIE_RPATH_DIE|g' libtool
make

%install
make install DESTDIR=$RPM_BUILD_ROOT
cp -fp *OSBase_UnixProcess.h $RPM_BUILD_ROOT/%{_includedir}/sblim
chmod 644 $RPM_BUILD_ROOT/%{_includedir}/sblim/*OSBase_UnixProcess.h
# remove unused libtool files
rm -f $RPM_BUILD_ROOT/%{_libdir}/*a
rm -f $RPM_BUILD_ROOT/%{_libdir}/cmpi/*a

%files
%docdir %{_datadir}/doc/%{name}-%{version}
%{_datadir}/doc/%{name}-%{version}
%{_datadir}/%{name}
%{_libdir}/*.so.*
%{_libdir}/cmpi/*.so*

%files devel
%{_includedir}/*
%{_libdir}/*.so

%files test
%dir %{_datadir}/sblim-testsuite/cim
%dir %{_datadir}/sblim-testsuite/system
%dir %{_datadir}/sblim-testsuite/system/linux
%{_datadir}/sblim-testsuite/test-cmpi-base.sh
%{_datadir}/sblim-testsuite/cim/*.cim
%{_datadir}/sblim-testsuite/system/linux/*.system
%{_datadir}/sblim-testsuite/system/linux/*.sh
%{_datadir}/sblim-testsuite/system/linux/*.pl

%global SCHEMA %{_datadir}/%{name}/Linux_Base.mof %{_datadir}/%{name}/Linux_BaseIndication.mof
%global REGISTRATION %{_datadir}/%{name}/Linux_BaseIndication.registration

%pre
function unregister()
{
  %{_datadir}/%{name}/provider-register.sh -d \
        $1 \
        -m %{SCHEMA} \
        -r %{REGISTRATION} > /dev/null 2>&1 || :;
  # don't let registration failure when server not running fail upgrade!
}

# If upgrading, deregister old version
if [ $1 -gt 1 ]
then
        unregistered=no
        if [ -e /usr/sbin/cimserver ]; then
           unregister "-t pegasus";
           unregistered=yes
        fi

        if [ -e /usr/sbin/sfcbd ]; then
           unregister "-t sfcb";
           unregistered=yes
        fi

        if [ "$unregistered" != yes ]; then
           unregister
        fi
fi

%post
function register()
{
  # The follwoing script will handle the registration for various CIMOMs.
  %{_datadir}/%{name}/provider-register.sh \
        $1 \
        -m %{SCHEMA} \
        -r %{REGISTRATION} > /dev/null 2>&1 || :;
  # don't let registration failure when server not running fail install!
}

/sbin/ldconfig
if [ $1 -ge 1 ]
then
        registered=no
        if [ -e /usr/sbin/cimserver ]; then
          register "-t pegasus";
          registered=yes
        fi

        if [ -e /usr/sbin/sfcbd ]; then
          register "-t sfcb";
          registered=yes
        fi

        if [ "$registered" != yes ]; then
          register
        fi
fi

%preun
function unregister()
{
  %{_datadir}/%{name}/provider-register.sh -d \
        $1 \
        -m %{SCHEMA} \
        -r %{REGISTRATION} > /dev/null 2>&1 || :;
  # don't let registration failure when server not running fail erase!
}

if [ $1 -eq 0 ]
then
        unregistered=no
        if [ -e /usr/sbin/cimserver ]; then
          unregister "-t pegasus";
          unregistered=yes
        fi

        if [ -e /usr/sbin/sfcbd ]; then
          unregister "-t sfcb";
          unregistered=yes
        fi

        if [ "$unregistered" != yes ]; then
          unregister
        fi
fi

%postun -p /sbin/ldconfig

%changelog
* Wed Aug 14 2013 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.6.2-5
- Use Pegasus root/interop instead of root/PG_Interop

* Mon Jun 17 2013 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.6.2-4
- Fix wrong UserModeTime and KernelModeTime
- Fix bogus date in %%changelog

* Thu Jun 06 2013 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.6.2-3
- Fix incorrect max cpu frequency

* Thu Feb 14 2013 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.6.2-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_19_Mass_Rebuild

* Wed Dec 05 2012 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.6.2-1
- Update to sblim-cmpi-base-1.6.2

* Tue Sep 04 2012 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.6.1-8
- Fix issues found by fedora-review utility in the spec file

* Sat Jul 21 2012 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.6.1-7
- Rebuilt for https://fedoraproject.org/wiki/Fedora_18_Mass_Rebuild

* Thu May 10 2012 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.6.1-6
- Fix double fclose() call (patch by Roman Rakus <rrakus@redhat.com>)
  Resolves: #820315

* Wed Feb 15 2012 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.6.1-5
- Enable indications and method providers

* Sat Jan 14 2012 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.6.1-4
- Rebuilt for https://fedoraproject.org/wiki/Fedora_17_Mass_Rebuild

* Wed Apr 27 2011 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.6.1-3
- Fix/enhance mofs registration for various CIMOMs (patch by Masatake Yamato)
  Resolves: #695626

* Wed Feb 09 2011 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.6.1-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_15_Mass_Rebuild

* Mon Feb  7 2011 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.6.1-1
- Update to sblim-cmpi-base-1.6.1

* Mon Dec 13 2010 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.6.0-2
- Fix license, mofs registration for various CIMOMs, sblim-sfcb init script
  path in provider-register.sh, rpmlint warnings

* Wed Oct  6 2010 Praveen K Paladugu <praveen_paladugu@dell.com> - 1.6.0-1
- Updated to 1.6.0
- removed the CIMOM dependencies
- following the upstream packaging obsolete, sblim-cmpi-base-test pkg.
- Added the patches from upstream packaging
- fix to restart tog-pegasus properly

* Thu Aug 27 2009 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.5.9-1
- Update to 1.5.9

* Sun Jul 26 2009 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.5.7-4
- Rebuilt for https://fedoraproject.org/wiki/Fedora_12_Mass_Rebuild

* Wed Feb 25 2009 Fedora Release Engineering <rel-eng@lists.fedoraproject.org> - 1.5.7-3
- Rebuilt for https://fedoraproject.org/wiki/Fedora_11_Mass_Rebuild

* Tue Nov  4 2008 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.5.7-2
- Fix %%files (to be able build -devel dependent packages)
- Remove rpath from libraries
- Spec file cleanup, rpmlint check

* Fri Oct 24 2008 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.5.7-1
- Update to 1.5.7
  Resolves: #468325

* Wed Jul  2 2008 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.5.5-2
- Fix testsuite dependency

* Tue Jul  1 2008 Vitezslav Crhonek <vcrhonek@redhat.com> - 1.5.5-1
- Update to 1.5.5
- Spec file revision

* Mon Feb 18 2008 Fedora Release Engineering <rel-eng@fedoraproject.org> - 1.5.4-8
- Autorebuild for GCC 4.3

* Tue Dec 05 2006 Mark Hamzy <hamzy@us.ibm.com> - 1.5.4-7
- Ignore failures when running provider-register.sh.  cimserver may be down

* Thu Oct 05 2006 Christian Iseli <Christian.Iseli@licr.org> 1.5.4-6
- rebuilt for unwind info generation, broken in gcc-4.1.1-21

* Thu Nov 10 2005  <mihajlov@de.ibm.com> - 1.5.4-3
- suppress error output in post scriptlets

* Wed Oct 26 2005  <mihajlov@de.ibm.com> - 1.5.4-2
- went back to original provider dir location as FC5 pegasus 2.5.1 support
  /usr/lib[64]/cmpi

* Wed Oct 12 2005  <mihajlov@de.ibm.com> - 1.5.4-1
- new spec file specifically for Fedora/RedHat

* Wed Jul 20 2005 Mark Hamzy <hamzy@us.ibm.com> - 1.5.3-1
- initial support
