# $Id: Makefile.am,v 1.17 2009/07/25 00:37:30 tyreld Exp $
# ==================================================================
# (C) Copyright IBM Corp. 2005, 2009
#
# THIS FILE IS PROVIDED UNDER THE TERMS OF THE ECLIPSE PUBLIC LICENSE
# ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE
# CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
#
# You can obtain a current copy of the Eclipse Public License from
# http://www.opensource.org/licenses/ecplipse-1.0.php
#
# Author:       Viktor Mihajlovski <mihajlov@de.ibm.com>
# Contributors: Dr. Gareth S. Bestor <bestorga@us.ibm.com>
# Description:  Automake input file for OS Base Providers
# ==================================================================


# SBLIM Include Directory
sblimincdir = $(includedir)/sblim

# Start from an empty extra distribution file list
EXTRA_DIST=

# docdir defines where the documentation goes
docdir=$(datadir)/doc/$(PACKAGE)-$(VERSION)

#
# Automake instructions for documentation
#
doc_DATA=README AUTHORS COPYING 

# ADD EXTRA DOC FILES IF PRESENT
doc_DATA+=README.INDICATION README.tog-pegasus DEBUG
EXTRA_DIST+=README.INDICATION README.tog-pegasus DEBUG

# providerdir defines where provider libraries will be installed (@PROVIDERDIR@ is set by the configure script). 
providerdir = @PROVIDERDIR@

#
# Automake instructions for main dir
#

#  OSBase CMPI provider libraries
provider_LTLIBRARIES = 	libcmpiOSBase_ComputerSystemProvider.la \
		libcmpiOSBase_OperatingSystemProvider.la \
		libcmpiOSBase_OperatingSystemStatisticalDataProvider.la \
		libcmpiOSBase_UnixProcessProvider.la \
		libcmpiOSBase_ProcessorProvider.la \
		libcmpiOSBase_BaseBoardProvider.la \
		libcmpiOSBase_RunningOSProvider.la \
		libcmpiOSBase_OSProcessProvider.la \
		libcmpiOSBase_OperatingSystemStatisticsProvider.la \
		libcmpiOSBase_CSProcessorProvider.la \
		libcmpiOSBase_CSBaseBoardProvider.la

# Computer System
libcmpiOSBase_ComputerSystemProvider_la_SOURCES = \
		cmpiOSBase_ComputerSystemProvider.c \
		cmpiOSBase_ComputerSystem.c \
		OSBase_ComputerSystem.c
libcmpiOSBase_ComputerSystemProvider_la_LIBADD = -lcmpiOSBase_Common
libcmpiOSBase_ComputerSystemProvider_la_LDFLAGS = -avoid-version

# Operating System
libcmpiOSBase_OperatingSystemProvider_la_SOURCES = \
		cmpiOSBase_OperatingSystemProvider.c \
		cmpiOSBase_OperatingSystem.c \
		OSBase_OperatingSystem.c

libcmpiOSBase_OperatingSystemProvider_la_LIBADD = -lcmpiOSBase_Common \
						-ldmiinfo @LINDHELP@
# This gcc/ld specific flag is ugly - need to check in configure
libcmpiOSBase_OperatingSystemProvider_la_LDFLAGS = -avoid-version

# Operating System Statistical Data
libcmpiOSBase_OperatingSystemStatisticalDataProvider_la_SOURCES = \
		cmpiOSBase_OperatingSystemStatisticalDataProvider.c \
		cmpiOSBase_OperatingSystemStatisticalData.c \
		OSBase_OperatingSystemStatisticalData.c
libcmpiOSBase_OperatingSystemStatisticalDataProvider_la_LIBADD =  -lcmpiOSBase_Common
libcmpiOSBase_OperatingSystemStatisticalDataProvider_la_LDFLAGS = -avoid-version

# Unix Process
libcmpiOSBase_UnixProcessProvider_la_SOURCES = \
		cmpiOSBase_UnixProcessProvider.c \
		cmpiOSBase_UnixProcess.c \
		OSBase_UnixProcess.c
libcmpiOSBase_UnixProcessProvider_la_LIBADD =  -lcmpiOSBase_Common
libcmpiOSBase_UnixProcessProvider_la_LDFLAGS =  -avoid-version

# Processor 
libcmpiOSBase_ProcessorProvider_la_SOURCES = \
		cmpiOSBase_ProcessorProvider.c \
		cmpiOSBase_Processor.c \
		OSBase_Processor.c
libcmpiOSBase_ProcessorProvider_la_LIBADD =  -lcmpiOSBase_Common
libcmpiOSBase_ProcessorProvider_la_LDFLAGS =  -avoid-version

# Base Board
libcmpiOSBase_BaseBoardProvider_la_SOURCES = \
		cmpiOSBase_BaseBoardProvider.c \
		cmpiOSBase_BaseBoard.c \
		OSBase_BaseBoard.c
libcmpiOSBase_BaseBoardProvider_la_LIBADD = -lcmpiOSBase_Common -ldmiinfo
libcmpiOSBase_BaseBoardProvider_la_LDFLAGS = -avoid-version

libcmpiOSBase_RunningOSProvider_la_SOURCES = cmpiOSBase_RunningOSProvider.c
libcmpiOSBase_RunningOSProvider_la_LIBADD = -lcmpiOSBase_Common
libcmpiOSBase_RunningOSProvider_la_LDFLAGS = -avoid-version

libcmpiOSBase_OSProcessProvider_la_SOURCES = cmpiOSBase_OSProcessProvider.c
libcmpiOSBase_OSProcessProvider_la_LIBADD = -lcmpiOSBase_Common
libcmpiOSBase_OSProcessProvider_la_LDFLAGS = -avoid-version

libcmpiOSBase_OperatingSystemStatisticsProvider_la_SOURCES = cmpiOSBase_OperatingSystemStatisticsProvider.c
libcmpiOSBase_OperatingSystemStatisticsProvider_la_LIBADD = -lcmpiOSBase_Common
libcmpiOSBase_OperatingSystemStatisticsProvider_la_LDFLAGS = -avoid-version

libcmpiOSBase_CSProcessorProvider_la_SOURCES = cmpiOSBase_CSProcessorProvider.c
libcmpiOSBase_CSProcessorProvider_la_LIBADD = -lcmpiOSBase_Common
libcmpiOSBase_CSProcessorProvider_la_LDFLAGS = -avoid-version

libcmpiOSBase_CSBaseBoardProvider_la_SOURCES = cmpiOSBase_CSBaseBoardProvider.c
libcmpiOSBase_CSBaseBoardProvider_la_LIBADD = -lcmpiOSBase_Common
libcmpiOSBase_CSBaseBoardProvider_la_LDFLAGS = -avoid-version


# OSBase support utility libraries
lib_LTLIBRARIES = libcmpiOSBase_Common.la libdmiinfo.la

libcmpiOSBase_Common_la_SOURCES=OSBase_Common.c cmpiOSBase_Common.c
# This gcc/ld specific flag is ugly - need to check in configure
libcmpiOSBase_Common_la_LDFLAGS = -Wc

libdmiinfo_la_SOURCES=dmiinfo.c dmiinfo.h dmifind.h

# DMI layer test program (not installed)
noinst_PROGRAMS = dmitest
dmitest_SOURCES = dmitest.c
dmitest_LDADD = -ldmiinfo

# Installable Header Files
sbliminc_HEADERS = OSBase_Common.h cmpiOSBase_Common.h \
	OSBase_UnixProcess.h cmpiOSBase_UnixProcess.h

# Non-Installable Header Files
noinst_HEADERS = OSBase_ComputerSystem.h \
	OSBase_OperatingSystem.h \
	OSBase_OperatingSystemStatisticalData.h \
	OSBase_Processor.h \
	OSBase_BaseBoard.h \
	cmpiOSBase_Util.h \
	cmpiOSBase_ComputerSystem.h \
	cmpiOSBase_OperatingSystem.h \
	cmpiOSBase_OperatingSystemStatisticalData.h \
	cmpiOSBase_Processor.h \
	cmpiOSBase_BaseBoard.h

# We must explicity add the RPM spec file to the distribution package
EXTRA_DIST+=$(PACKAGE).spec $(PACKAGE).rh.spec


#
# Automake instructions for ./mof subdir
#
if NOEVENTS
SCHEMAS=$(srcdir)/mof/Linux_Base.mof 
REGISTRATIONS=$(srcdir)/mof/Linux_Base.registration
else
SCHEMAS=$(srcdir)/mof/Linux_Base.mof $(srcdir)/mof/Linux_BaseIndication.mof
REGISTRATIONS=$(srcdir)/mof/Linux_BaseIndication.registration
endif

# We must explicity add all the schema files to the distribution package
pkgdata_DATA=$(SCHEMAS) $(REGISTRATIONS)
pkgdata_SCRIPTS=provider-register.sh
EXTRA_DIST+=mof $(pkgdata_SCRIPTS)

# Register the provider(s) and class definition(s) to the current CIM server/CIMOM
postinstall:
	sh $(srcdir)/provider-register.sh -t @CIMSERVER@ -r $(REGISTRATIONS) -m $(SCHEMAS)

preuninstall:
	sh $(srcdir)/provider-register.sh -d -t @CIMSERVER@ -r $(REGISTRATIONS) -m $(SCHEMAS)

dist-hook:
	test -d "$(distdir)" &&	rm -rf `find $(distdir) -type d -name CVS`

#
# Automake instructions for ./test subdir
#
EXTRA_DIST+=test README.TEST
if TESTSUITE
testsuitedir=@TESTSUITEDIR@
testsuitesystemdir=@TESTSUITEDIR@/system/linux
testsuitecimdir=@TESTSUITEDIR@/cim

testsuite_SCRIPTS=test/test-cmpi-base.sh
testsuitesystem_DATA=test/system/linux/Linux_CSProcessor.system \
	test/system/linux/Linux_ComputerSystem.system \
	test/system/linux/Linux_OSProcess.system \
	test/system/linux/Linux_OperatingSystem.system \
	test/system/linux/Linux_Processor.system \
	test/system/linux/Linux_OperatingSystemStatisticalData.system \
	test/system/linux/Linux_OperatingSystemStatistics.system \
	test/system/linux/Linux_RunningOS.system \
	test/system/linux/Linux_UnixProcess.system \
	test/system/linux/Linux_BaseBoard.system \
	test/system/linux/Linux_CSBaseBoard.system
testsuitesystem_SCRIPTS=test/system/linux/Linux_OperatingSystem.version.sh \
	test/system/linux/Linux_Processor.pl \
	test/system/linux/Linux_UnixProcess.pl \
	test/system/linux/createKeyFiles.sh 
testsuitecim_DATA=test/cim/Linux_CSProcessor.cim \
	test/cim/Linux_ComputerSystem.cim \
	test/cim/Linux_OSProcess.cim \
	test/cim/Linux_OperatingSystem.cim \
	test/cim/Linux_OperatingSystemStatisticalData.cim \
	test/cim/Linux_OperatingSystemStatistics.cim \
	test/cim/Linux_Processor.cim \
	test/cim/Linux_RunningOS.cim \
	test/cim/Linux_UnixProcess.cim \
	test/cim/Linux_BaseBoard.cim \
	test/cim/Linux_CSBaseBoard.cim

runtest: install
	cd $(DESTDIR)$(TESTSUITEDIR) &&	./test-cmpi-base.sh

doc_DATA+=README.TEST
endif
