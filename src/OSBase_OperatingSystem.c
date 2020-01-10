/*
 * OSBase_OperatingSystem.c
 *
 * (C) Copyright IBM Corp. 2002, 2009
 *
 * THIS FILE IS PROVIDED UNDER THE TERMS OF THE ECLIPSE PUBLIC LICENSE
 * ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE
 * CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
 *
 * You can obtain a current copy of the Eclipse Public License from
 * http://www.opensource.org/licenses/eclipse-1.0.php
 *
 * Author:       Heidi Neumann <heidineu@de.ibm.com>
 * Contributors: Viktor Mihajlovski <mihajlov@de.ibm.com>
 *               C. Eric Wu <cwu@us.ibm.com>               
 *               Ferenc Szalai <szferi@einstein.ki.iif.hu>
 *               Chris Buccella <buccella@linux.vnet.ibm.com>
 *               Tyrel Datwyler <tyreld@us.ibm.com>
 *
 * Description:
 * This shared library provides resource access functionality for the class
 * Linux_OperatingSystem. 
 * It is independent from any specific CIM technology.
*/

/* ---------------------------------------------------------------------------*/

#define _GNU_SOURCE

#include "OSBase_Common.h"
#include "OSBase_OperatingSystem.h"
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <langinfo.h>

/* ---------------------------------------------------------------------------*/
// private declarations

char * CIM_OS_DISTRO = NULL;

/* ---------------------------------------------------------------------------*/

/* initialization routine */
void __attribute__ ((constructor)) _osbase_os_init() {
   _init_os_distro();
}

/* deinitialization routine */
void __attribute__ ((destructor)) _osbase_os_fini() { 
  free ( CIM_OS_DISTRO );
}

/* ---------------------------------------------------------------------------*/

/* returns a structure, which contains necessary information about the currently
 * running operating system
 */
int get_operatingsystem_data( struct cim_operatingsystem ** sptr ){

  FILE   *fhd = NULL;
  char   *ptr = NULL;
  char    buf[30000];
  size_t  bytes_read = 0;

  _OSBASE_TRACE(3,("--- get_operatingsystem_data() called"));

  (*sptr) = calloc(1,sizeof(struct cim_operatingsystem));

  (*sptr)->version = CIM_OS_DISTRO;
  (*sptr)->osType = 36;
  (*sptr)->licensedUsers = 0;
  (*sptr)->healthState = 5;

  (*sptr)->numOfProcesses = get_os_numOfProcesses();
  (*sptr)->numOfUsers     = get_os_numOfUsers();
  (*sptr)->maxNumOfProc   = get_os_maxNumOfProc();
  (*sptr)->maxProcMemSize = get_os_maxProcMemSize();
  (*sptr)->totalSwapSize  = get_os_totalSwapSize();

  /* get values for memory properties :
   * TotalVisibleMemorySize, FreePhysicalMemory, SizeStoredInPagingFiles,
   * FreeSpaceInPagingFiles
   */
  if ( (fhd=fopen("/proc/meminfo","r")) != NULL ) {
    bytes_read = fread(buf, 1, sizeof(buf)-1, fhd);
    buf[bytes_read] = 0; /* safeguard end of buffer */
    ptr = strstr(buf,"MemTotal");
    sscanf(ptr, "%*s %lld", &((*sptr)->totalPhysMem));
    ptr = strstr(buf,"MemFree");
    sscanf(ptr, "%*s %lld", &((*sptr)->freePhysMem));
    ptr = strstr(buf,"SwapTotal");
    sscanf(ptr, "%*s %lld", &((*sptr)->totalSwapMem));
    ptr = strstr(buf,"SwapFree");
    sscanf(ptr, "%*s %lld", &((*sptr)->freeSwapMem));
    fclose(fhd);
  }
  /* TotalVirtualMemorySize */
  (*sptr)->totalVirtMem = (*sptr)->totalPhysMem + (*sptr)->totalSwapMem;
  /* FreeVirtualMemory */
  (*sptr)->freeVirtMem = (*sptr)->freePhysMem + (*sptr)->freeSwapMem;

  //  fprintf( stderr,"%lli : %lli: %lli: %lli\n",(*sptr)->totalPhysMem,(*sptr)->freePhysMem,(*sptr)->totalSwapMem,(*sptr)->freeSwapMem);

  /* CurrentTimeZone */
  (*sptr)->curTimeZone = get_os_timezone();

  /* LocalDateTime */
  (*sptr)->localDate = get_os_localdatetime();

  /* InstallDate */
  (*sptr)->installDate = get_os_installdate();

  /* LastBootUp */
  (*sptr)->lastBootUp = get_os_lastbootup();

  /* CodeSet */
  (*sptr)->codeSet = get_os_codeSet();

  /* LanguageEdition */
  (*sptr)->langEd = get_os_langEd();

  /* DefaultPageSize */
  (*sptr)->defPageSize = sysconf(_SC_PAGESIZE);

  _OSBASE_TRACE(3,("--- get_operatingsystem_data() exited"));
  return 0;
}


void _init_os_distro() {
  char ** hdout   = NULL;
  char *  ptr     = NULL;
  char *  cmd     = NULL;
  int     strl    = 0;
  int     i       = 0;
  int     j       = 0;
  int     rc      = 0;

  if( !CIM_OS_DISTRO ) {
    
    _OSBASE_TRACE(4,("--- _init_os_distro() called : init"));

    rc = runcommand( "find /etc/ -type f -maxdepth 1 -name *release* 2>/dev/null" , NULL , &hdout , NULL );
    if( rc == 0 && *hdout != NULL) {
      while (hdout[j] && hdout[j][0]) {
	if (strstr(hdout[j],"lsb-release") && hdout[j+1] && hdout[j+1][0]) {
	  /* found lsb-release but there are other (preferred) release files */
	  j++;
	  continue;
	}
	strl = strlen ("cat  2>/dev/null") + strlen (hdout[j]) + 1;
	ptr = strchr(hdout[j], '\n');
	if (ptr) {
	  *ptr = '\0';
	}
	cmd = calloc (strl, sizeof(char));
	snprintf(cmd, strl, "cat %s 2>/dev/null", hdout[j]);
	freeresultbuf(hdout);
	hdout = NULL;
	strl = 0;
	rc = runcommand(cmd, NULL , &hdout , NULL );
	if( rc == 0 ) {
	  while ( hdout[i]) {
	    strl = strl+strlen(hdout[i])+1;
	    ptr = strchr(hdout[i], '\n');
	    if (ptr) {
	      *ptr = '\0';
	    }
	    i++;
	  }	
	  i=0;
	  CIM_OS_DISTRO = calloc(1,strl);
	  strcpy( CIM_OS_DISTRO , hdout[0] );
	  i++;
	  while ( hdout[i]) {
	    strcat( CIM_OS_DISTRO , " " );
	    strcat( CIM_OS_DISTRO , hdout[i] );
	    i++;
	  }
	}
	break;
      }
      free (cmd);
    } else {
      CIM_OS_DISTRO = calloc(1,6);
      strcpy( CIM_OS_DISTRO , "Linux" );
    }
    freeresultbuf(hdout);
    _OSBASE_TRACE(4,("--- _init_os_distro() : CIM_OS_DISTRO initialized with %s",CIM_OS_DISTRO));
  }

  _OSBASE_TRACE(4,("--- _init_os_distro() exited : %s",CIM_OS_DISTRO));
}


char * get_kernel_version() {
  char ** hdout = NULL ;
  char *  str   = NULL;
  int     rc    = 0;

  _OSBASE_TRACE(4,("--- get_kernel_version() called"));

  rc = runcommand( "uname -r" , NULL , &hdout , NULL );
  if( rc == 0 ) {
    str = (char *) malloc((strlen(hdout[0])+1));
    strcpy( str, hdout[0]);
  }
  else {
    str = (char *) malloc(10);
    strcpy( str , "not found");
  }
  freeresultbuf(hdout);

  _OSBASE_TRACE(4,("--- get_kernel_version() exited : %s",str));
  return str;
}


unsigned long long get_os_totalSwapSize() {
   char          ** hdout = NULL;
   unsigned long long num = 0;
   int                  i = 0;
   int                 rc = 0;
   
   _OSBASE_TRACE(4,("--- get_os_totalSwapSize() called"));

   rc = runcommand("cat /proc/swaps | awk '{print $3}' | sed '1d' 2>/dev/null", NULL, &hdout, NULL);
   if ( rc == 0 && *hdout ) {
      while (hdout[i] && hdout[i][0]) {
         num += strtoull(hdout[i], NULL, 10);
         i++;
      }
   }
   freeresultbuf(hdout);
   
   _OSBASE_TRACE(4,("--- get_os_totalSwapSize() exited : %ld",num));
   return num;
}


char * get_os_installdate() {
  struct tm    date;
  char      ** hdout = NULL;
  char      *  dstr  = NULL;
  char      *  str   = NULL;
  char      *  ptr   = NULL;
  int          rc    = 0;

  _OSBASE_TRACE(4,("--- get_os_installdate() called"));

  if( CIM_OS_DISTRO && strstr( CIM_OS_DISTRO, "Red Hat" ) ) {
    /* we guess it is Red Hat */
    rc = runcommand( "rpm -qi redhat-release | grep Install" , NULL , &hdout , NULL );
    if(rc!=0) { 
    /* we guess it is Fedora */
      rc = runcommand( "rpm -qi fedora-release | grep Install" , NULL , &hdout , NULL );
    }
    if( rc == 0 ) {
      str = strstr( hdout[0], ": ");
      str+=2;
      for( ptr = str ; (*ptr)!=' ' ; ptr++ ) {
	if( *(ptr+1)==' ' ) { ptr ++; }
      }
      dstr = (char *) malloc( (strlen(str)-strlen(ptr)+1));
      strncpy( dstr , str, strlen(str)-strlen(ptr)-1);
      strptime(dstr, "%A %d %B %Y %H:%M:%S %p %Z", &date);
      str = (char*)malloc(26);
      strftime(str,26,"%Y%m%d%H%M%S.000000",&date);
      _cat_timezone(str, get_os_timezone());
      if(dstr) free(dstr);
    }
    freeresultbuf(hdout);
  }

  _OSBASE_TRACE(4,("--- get_os_installdate() exited : %s",str));
  return str;
}


char * get_os_lastbootup() {
  char          * uptime = NULL;
  struct tm       uptm;
  unsigned long   up     = 0;

  _OSBASE_TRACE(4,("--- get_os_lastbootup() called"));

  up = _get_os_boottime();
  if( up == 0 ) { 
    _OSBASE_TRACE(4,("--- get_os_lastbootup() failed : was not able to set last boot time"));
    return NULL;
  }
  if( gmtime_r( &up, &uptm ) != NULL ) {
    uptime = (char*)malloc(26);
    strftime(uptime,26,"%Y%m%d%H%M%S.000000",&uptm);
    _cat_timezone(uptime, get_os_timezone());
  }

  _OSBASE_TRACE(4,("--- get_os_lastbootup() exited : %s",uptime));
  return uptime;
}


char * get_os_localdatetime() {
  char          * tm  = NULL;
  long            sec = 0;
  struct tm       cttm;

  _OSBASE_TRACE(4,("--- get_os_localdatetime() called"));

  sec=time(NULL) + get_os_timezone()*60;
  if( gmtime_r( &sec , &cttm) != NULL ) {
    tm = (char*)malloc(26);
    strftime(tm,26,"%Y%m%d%H%M%S.000000",&cttm);
    _cat_timezone(tm, get_os_timezone());
  }
  
  _OSBASE_TRACE(4,("--- get_os_localdatetime() exited : %s",tm));
  return tm;
}


unsigned long get_os_numOfProcesses() {
  char         ** hdout = NULL;
  int             rc    = 0;
  unsigned long   np    = 0;

  _OSBASE_TRACE(4,("--- get_os_numOfProcesses() called"));

  rc = runcommand( "ps -ef | wc -l" , NULL , &hdout , NULL );
  if( rc == 0 ) {
    np = atol(hdout[0]);
  }
  freeresultbuf(hdout);

  _OSBASE_TRACE(4,("--- get_os_numOfProcesses() exited : %lu",np));
  return np;
}

unsigned long get_os_numOfUsers() {
  char          ** hdout = NULL ;
  unsigned long    np    = 0;
  int              rc    = 0;

  _OSBASE_TRACE(4,("--- get_os_numOfUsers() called"));

  rc = runcommand( "who -u | wc -l" , NULL , &hdout , NULL );
  if( rc == 0 ) {
    np = atol(hdout[0]);
  }
  freeresultbuf(hdout);

  _OSBASE_TRACE(4,("--- get_os_numOfUsers() exited : %lu",np));
  return np;
}

unsigned long get_os_maxNumOfProc() {
  struct rlimit   rlim;
  unsigned long   max = 0;
  int             rc  = 0;

  _OSBASE_TRACE(4,("--- get_os_maxNumOfProc() called"));

  rc = getrlimit(RLIMIT_NPROC, &rlim);  /* same as calling 'ulimit -u' */
  if (rc == 0) { max = rlim.rlim_max; }

  _OSBASE_TRACE(4,("--- get_os_maxNumOfProc() exited : %lu",max));
  return max;
}

unsigned long long get_os_maxProcMemSize() {
  struct rlimit      rlim;
  unsigned long long max = 0;
  int                rc  = 0;

  _OSBASE_TRACE(4,("--- get_os_maxProcMemSize() called"));

  rc = getrlimit(RLIMIT_DATA,&rlim);
  if( rc == 0 ) { max = rlim.rlim_max; }

  _OSBASE_TRACE(4,("--- get_os_maxProcMemSize() exited : %Ld",max));
  return max;
}

char * get_os_codeSet() {
  char * codeSet = NULL;

  _OSBASE_TRACE(4,("--- get_os_codeSet() called"));

  codeSet = strdup(nl_langinfo(CODESET));

  _OSBASE_TRACE(4,("--- get_os_codeSet() exited : %s",codeSet));
  return codeSet;
}

char * get_os_langEd() {
  char ** hdout  = NULL;
  char *  langEd = NULL;
  char *  var    = NULL;
  char *  str    = NULL;
  char *  ptr    = NULL;
  int     rc     = 0;

  _OSBASE_TRACE(4,("--- get_os_langEd() called"));

  var = getenv("LANG");
  if(!var) {
    rc = runcommand( "locale | grep LC_CTYPE" , NULL , &hdout , NULL );
    if( rc == 0 ) { var = hdout[0]; }
  }
  if( var != NULL ) {

    if( (ptr=strchr(var,'=')) ) { var = ptr+1; }
    if( (ptr=strchr(var,'"')) ) {
      var = ptr+1;
    }
    str = strchr(var,'.');

    if(str) {
      langEd = calloc(1,strlen(var)-strlen(str)+1);
      strncpy(langEd,var,strlen(var)-strlen(str));
    }
    else {
      langEd = calloc(1,strlen(var)+1);
      strcpy(langEd,var);
    }

    if( (str = strchr(langEd,'\n')) ) { *str='\0'; }
    if( (str = strchr(langEd,'_')) )  { *str = '-'; }
  }
  freeresultbuf(hdout);

  _OSBASE_TRACE(4,("--- get_os_langEd() exited : %s",langEd));
  return langEd;
}


/* ---------------------------------------------------------------------------*/

void free_os_data( struct cim_operatingsystem * sptr ) {
    if(sptr==NULL) return;
    if(sptr->installDate) free(sptr->installDate);
    if(sptr->lastBootUp) free(sptr->lastBootUp);
    if(sptr->localDate) free(sptr->localDate);
    if(sptr->codeSet) free(sptr->codeSet);
    if(sptr->langEd) free(sptr->langEd);
    free(sptr);
}

/* ---------------------------------------------------------------------------*/
/*                    end of OSBase_OperatingSystem.c                         */
/* ---------------------------------------------------------------------------*/

