#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(SBLIM BaseOS Providers Base, 1.6.1, sblim-devel@lists.sourceforge.net,sblim-cmpi-base)
AC_CONFIG_SRCDIR([OSBase_Common.c])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE

AC_CANONICAL_HOST
case $host_cpu in
     i*86) HW=INTEL;; 
     s390*) HW=S390;; 
     ppc*) HW=PPC;; 
     x86_64) HW=X86_64;; 
     ia64) HW=IA64;; 
     mips*) HW=MIPS;;
     **) HW=GENERIC;;
esac

# Configuration Arguments
AC_ARG_VAR([TESTSUITEDIR],[the directory where the SBLIM testsuite resides.])
AC_ARG_VAR([PROVIDERDIR],[the directory where the CMPI providers will be installed.])
AC_ARG_VAR([CIMSERVER],[the target CIM server (pegasus|sfcb|openwbem|sniacimom).])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

# Checks for libraries.
AC_CHECK_LIB([ind_helper], [ind_start], [LINDHELP=-lind_helper], [LINDHELP=])

# Check for required CMPI header files (defined in acinclude.m4)
CHECK_CMPI

# Check for required libraries (defined in acinclude.m4)
CHECK_PROVIDERDIR

# Check for CIM Server (defined in acinclude.m4)
CHECK_CIMSERVER

# Check for the Testsuite (defined in acinclude.m4)
CHECK_TESTSUITE

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h langinfo.h limits.h malloc.h stdlib.h string.h sys/time.h unistd.h])

# Test for indication support
EVENTFLAGS=-DNOEVENTS
if test x$LINDHELP != x; then
# Indication lib is there, what about headers
  CHECK_INDHELP_HEADER
  if test "$have_INDHELP" == "yes"; then
     EVENTFLAGS=
  else
     LINDHELP=		
  fi
fi
AC_SUBST(LINDHELP)

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_MKTIME
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_STRFTIME
AC_CHECK_FUNCS([gethostname getpagesize gettimeofday memset munmap nl_langinfo strcasecmp strchr strdup strerror strrchr strstr uname])

AC_CONFIG_FILES([Makefile sblim-cmpi-base.spec])

AM_CONDITIONAL(NOEVENTS,[test x"$EVENTFLAGS" != x])

# Extend CPPFLAGS
CPPFLAGS="$CPPFLAGS -D$HW $EVENTFLAGS"


if test x"$EVENTFLAGS" == x; then
   INDICATION_SUPPORT=Enabled
else	
   INDICATION_SUPPORT=Disabled
fi
if test x"$TESTSUITEDIR" == x; then
   TESTSUITE_SUPPORT=Disabled
else	
   TESTSUITE_SUPPORT=Enabled
fi
# Display configuration options
echo "-------------------------------------------------------"
echo "Configuration for $PACKAGE complete."
echo ""
echo "The following configuration options have been selected:"
echo "CIMSERVER: " $CIMSERVER
echo "PROVIDERDIR: " $PROVIDERDIR
echo "CPPFLAGS:" $CPPFLAGS
echo "Indication Support" $INDICATION_SUPPORT
echo "Test Suite Support" $TESTSUITE_SUPPORT
if test x"$TESTSUITEDIR" != x; then
echo "TESTSUITEDIR " $TESTSUITEDIR
fi
echo "-------------------------------------------------------"

AC_OUTPUT

echo "You may now run make"
