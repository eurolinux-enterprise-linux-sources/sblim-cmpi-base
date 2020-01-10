/*
 * OSBase_Common.c
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
 *               Tyrel Datwyler <tyreld@us.ibm.com>
 *
 * Description:
 * This shared library provides common functionality for OS Base Instrumentation,
 * and all other SBLIM packages. It is independent from any specific CIM class
 * or CIM technology at all.
*/

/* ---------------------------------------------------------------------------*/

#define _GNU_SOURCE

#include "OSBase_Common.h"

#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>  
#include <dirent.h>
#include <sys/stat.h>    
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/utsname.h>

/* ---------------------------------------------------------------------------*/

char * CIM_HOST_NAME          = NULL;
char * CIM_OS_NAME            = NULL;
int    CIM_OS_TIMEZONE        = 999;
unsigned long CIM_OS_BOOTTIME = 0;

/* ---------------------------------------------------------------------------*/

int    _debug                    = 0;
int    _SBLIM_TRACE_ENABLED      = 0;
int    _SBLIM_TRACE_FILE_ENABLED = 0;
char * _SBLIM_TRACE_FILE         = NULL;

void _init_trace_level() {

  char *var = NULL;

  if(_SBLIM_TRACE_ENABLED == 1) return;

  var = getenv("SBLIM_TRACE");
  if(var != NULL) {
    _debug = atoi(var);
    fprintf(stderr,"SBLIM TRACE LEVEL: %i\n",_debug);
    _SBLIM_TRACE_ENABLED = 1;
  } 
  else { _debug = 0; }

  return;
}

void _init_trace_file() {

  char *var = NULL;
  FILE *fhd = NULL;

  if(_SBLIM_TRACE_FILE_ENABLED == 1) return;

  var = getenv("SBLIM_TRACE_FILE");
  if( var != NULL ) {
    if( ( ((fhd=fopen(var,"a")) == NULL) || fclose(fhd) ) ) {
      fprintf(stderr,"Couldn't open/create trace file: %s\n",var);
      return;
    }
    _SBLIM_TRACE_FILE = strdup(var);
    fprintf(stderr,"SBLIM TRACE FILE created: %s\n",_SBLIM_TRACE_FILE);
    _SBLIM_TRACE_FILE_ENABLED = 1;
  } 
  else { 
    _SBLIM_TRACE_FILE_ENABLED = 0;
    _SBLIM_TRACE_FILE = NULL; 
  }
  return;
}

/* initialization routine */
void __attribute__ ((constructor)) _osbase_common_init() {
  _init_trace_level();
  _init_trace_file();
  _init_system_name();
  _init_os_name();
  return;
}

/* deinitialization routine */
void __attribute__ ((destructor)) _osbase_common_fini() { 
  free ( CIM_HOST_NAME );
  free ( CIM_OS_NAME );
  _debug = 0;
  _SBLIM_TRACE_ENABLED = 0;
  _SBLIM_TRACE_FILE_ENABLED = 0;
  if(_SBLIM_TRACE_FILE) { 
    free(_SBLIM_TRACE_FILE);
    _SBLIM_TRACE_FILE = NULL;
  }
}

/* ---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------*/
/* get Kernel Version                                                         */
/* ---------------------------------------------------------------------------*/

#define KERNELRELEASE(maj,min,patch) ((maj) * 10000 + (min)*1000 + (patch))

int kernel_release() {
  struct utsname uts;
  int major, minor, patch;

  if (uname(&uts) < 0)
    return -1;
  if (sscanf(uts.release, "%d.%d.%d", &major, &minor, &patch) != 3)
    return -1;
  return KERNELRELEASE(major, minor, patch);
}

/* ---------------------------------------------------------------------------*/

/* initializes the variable CIM_HOST_NAME
 * contains the full qualified IP hostname of the system, e.g. host.domain
 */

void _init_system_name() {
  char *  host   = NULL;
  char *  domain = NULL;
  char *  ptr    = NULL;
  char ** hdout  = NULL;
  int     rc     = 0;

  if( !CIM_HOST_NAME ) {

    _OSBASE_TRACE(4,("--- _init_system_name() called : init"));

    host = calloc(1,255);
    if ( gethostname(host, 255 ) == -1 ) { return; }
    /* if host does not contain a '.' we can suppose, that the domain is not
     * available in the current value. but we try to get the full qualified
     * hostname.
     */
    if( strchr( host , '.') == NULL ) {
      /* get domain name */
      rc=runcommand("/bin/dnsdomainname",NULL,&hdout,NULL);
      if (rc == 0 && hdout != NULL) {
	if (hdout[0]) {
	  domain = strdup(hdout[0]);
	  ptr = strchr(domain, '\n');
	  *ptr = '\0';
	}
	freeresultbuf(hdout);
      }
    }

    /* initializes CIM_HOST_NAME */
    if( strlen(host) ) {
      if( !domain ) {
	CIM_HOST_NAME = calloc(1,(strlen(host)+1));
	strcpy( CIM_HOST_NAME, host);
      }
      else {
	CIM_HOST_NAME = calloc(1,(strlen(host)+strlen(domain)+2));
	strcpy( CIM_HOST_NAME, host);
	strcat( CIM_HOST_NAME, ".");
	strcat( CIM_HOST_NAME, domain );
      }
    }

    if(host) free(host);
    if(domain) free(domain);

    _OSBASE_TRACE(4,("--- _init_system_name() : CIM_HOST_NAME initialized with %s",CIM_HOST_NAME));
  }

}


/* ---------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------*/

void _init_os_name(){

  if(!CIM_OS_NAME) {

    _OSBASE_TRACE(4,("--- _init_os_name() called : init"));

    if(CIM_HOST_NAME) {
      if((CIM_OS_NAME=calloc(1,strlen(CIM_HOST_NAME)+1)) != NULL) {
	strcpy( CIM_OS_NAME, CIM_HOST_NAME );
      }
    }
    
    _OSBASE_TRACE(4,("--- _init_os_name() : CIM_OS_NAME initialized with %s",CIM_OS_NAME));
  }
}


/* ---------------------------------------------------------------------------*/

/* ---------------------------------------------------------------------------*/

signed short get_os_timezone() {
  if( CIM_OS_TIMEZONE == 999 ) {
    tzset();
    CIM_OS_TIMEZONE = -(timezone/60);
  }
  return CIM_OS_TIMEZONE;
}

unsigned long _get_os_boottime() {
  char ** hdout = NULL;
  int     rc    = 0;

  _OSBASE_TRACE(4,("--- get_os_boottime() called"));

  if( CIM_OS_BOOTTIME == 0 ) {
    rc = runcommand( "cat /proc/stat | grep btime" , NULL , &hdout , NULL );
    if( rc == 0 ) {
      CIM_OS_BOOTTIME = atol(hdout[0]+6);
      freeresultbuf(hdout);
    }
    else {
      _OSBASE_TRACE(4,("--- get_os_boottime() failed : was not able to get boottime - set to 0"));
    }
  }

  _OSBASE_TRACE(4,("--- get_os_boottime() exited : %i",CIM_OS_BOOTTIME));
  return CIM_OS_BOOTTIME;
}

/* add timezone to datetime 'str'of format %Y%m%d%H%M%S.000000
 * out : yyyyMMddHHmmss.SSSSSSsutc, e.g. 20030112152345.000000+100
 */
void _cat_timezone( char *str, signed short zone ) {
  char *tz = NULL;

  if((tz=calloc(1,sizeof(long long))) != NULL) {
    sprintf(tz, "%+04d", zone);
    if( str != NULL ) {
      if( str != NULL ) { strcat(str,tz); }
    }
    if(tz) free(tz);
  }
}

 
/* ---------------------------------------------------------------------------*/
/* tool function : executes commandlines and returns their output             */
/* ( stdout and stderr )                                                      */
/* ---------------------------------------------------------------------------*/


static void addstring(char ***buf, const char *str);

int runcommand(const char *cmd, char **in, char ***out, char ***err)
{
  int rc;
  char fcltinname[] = "/tmp/SBLIMXXXXXX";
  char fcltoutname[] = "/tmp/SBLIMXXXXXX";
  char fclterrname[] = "/tmp/SBLIMXXXXXX";
  int fdin, fdout, fderr;
  char * cmdstring;
  char * buffer;
  FILE * fcltin;
  FILE * fcltout;
  FILE * fclterr;

  /* No pipes -- real files */

  /* make the filenames for I/O redirection */
  if (in != NULL && ((fdin=mkstemp(fcltinname)) < 0 || close(fdin))) {
    _OSBASE_TRACE(4,("%s ... Couldn't create input file",strerror(errno)));
    return -1;
  }
  if (out != NULL && ((fdout=mkstemp(fcltoutname)) < 0 || close(fdout))) {
    _OSBASE_TRACE(4,("%s ... Couldn't create output file",strerror(errno)));
    return -1;
  }
  if (err != NULL && ((fderr=mkstemp(fclterrname)) < 0 || close(fderr))) {
    _OSBASE_TRACE(4,("%s ... Couldn't create error file",strerror(errno)));
    return -1;
  }

  /* if necessary feed in-buffer to child's stdin */
  if (in != NULL) {
    fcltin = fopen(fcltinname,"w");
    if (fcltin==NULL) {
      _OSBASE_TRACE(4,("%s ... Couldn't open client's input for writing",strerror(errno)));
      return -1;
    }
    while (*in != NULL)
      fprintf(fcltin,"%s\n",*in++);
    /* flush and close */
    fclose(fcltin);
  }

  /* prepare full command string */
  cmdstring = malloc(strlen(cmd) + 1 +
		     (in?strlen(fcltinname)+2:0) +
		     (out?strlen(fcltoutname)+3:0) +
		     (err?strlen(fclterrname)+3:0));
  strcpy(cmdstring,cmd);
  if (in) {
    strcat(cmdstring," <");
    strcat(cmdstring,fcltinname);
  }
  if (out) {
    strcat(cmdstring," >");
    strcat(cmdstring,fcltoutname);
  }
  if (err) {
    strcat(cmdstring," 2>");
    strcat(cmdstring,fclterrname);
  }
  /* perform the system call */
  _OSBASE_TRACE(4,("--- runcommand() : %s",cmdstring));
  rc=system(cmdstring);
  free(cmdstring);

  buffer = NULL;
  /* cleanup in file. if necessary */
  if (in != NULL)
    unlink(fcltinname);
  /* retrieve data from stdout and stderr */
  if (out != NULL) {
    *out=calloc(1,sizeof(char*));
    buffer = malloc(4096);
    fcltout = fopen(fcltoutname,"r");
    if (fcltout!=NULL) {
      while(!feof(fcltout)) {
	if (fgets(buffer,4096,fcltout) != NULL)
	  addstring(out,buffer);
	else
	  break;
      }
      /* close out file */
      fclose(fcltout);
    } else
      _OSBASE_TRACE(4,("%s ... Couldn't open client's output for reading",strerror(errno)));
    unlink(fcltoutname);
  }
  if (err != NULL) {
    *err=calloc(1,sizeof(char*));
    if (buffer == NULL)
      buffer = malloc(4096);
    fclterr = fopen(fclterrname,"r");
    if (fclterr!=NULL) {
	while(!feof(fclterr)) {
	  if (fgets(buffer,4096,fclterr) != NULL)
	    addstring(err,buffer);
	  else
	    break;
	}
	/* close err file */
	fclose(fclterr);
    } else
      _OSBASE_TRACE(4,("%s ... Couldn't open client's error for reading",strerror(errno)));
    unlink(fclterrname);
  }
  if (buffer != NULL)
    free(buffer);
  return rc;
}

void freeresultbuf(char ** buf)
{
  char ** bufstart = buf;
  if (buf == NULL)
    return;
  while(*buf != NULL) {
    free(*buf++);
  }
  free(bufstart);
}

static void addstring(char *** buf, const char * str)
{
  int i=0;
  size_t s;
  while ((*buf)[i++] != NULL);
  s = i == 1 ? 0 : strlen((*buf)[i-2]);
  if (i == 1 || (*buf)[i-2][s-1] == '\n') { 
    *buf=realloc(*buf,(i+1)*sizeof(char*));
    (*buf)[i-1] = strdup(str);
    (*buf)[i] = NULL;
  } else {
    fprintf(stderr, "******* append (%s)\n", str);
    (*buf)[i-2]=realloc((*buf)[i-2],s + strlen(str) + 1);
    strcpy((*buf)[i-2] + s, str);
  }
}

static void addstring_noconc(char *** buf, const char * str)
{
  int i=0;
  while ((*buf)[i++] != NULL);
  *buf=realloc(*buf,(i+1)*sizeof(char*));
  (*buf)[i-1] = strdup(str);
  (*buf)[i] = NULL;
}

/* ---------------------------------------------------------------------------*/
// end of tool function to executes commandlines
/* ---------------------------------------------------------------------------*/



/* ---------------------------------------------------------------------------*/
// tool function to convert ":"-seperated lines
/* ---------------------------------------------------------------------------*/

/* converts a line into an array of char * , while
 * c ( e.g. ':' ) acts as seperator
 *
 * example :
 * line  -> value1:value2:value3'\n'
 *
 * array -> value1
 *          value2
 *          value3
 *          NULL
 */

char ** line_to_array( char * buf , int c ){
  char ** data = NULL ;
  char *  str  = NULL ;
  char *  ptr  = NULL ;
  char *  ent  = NULL ;

  str = strdup(buf);
  data = calloc(1,sizeof(char*));
  ent = str ;

  while( ( ptr = strchr( ent , c )) != NULL ) {
    *ptr='\0';
    addstring_noconc( &data , ent );
    ent = ptr+1;
    ptr = NULL ;
  }
  addstring_noconc( &data , ent );

  if(str) free(str);
  return data;
}

/* ---------------------------------------------------------------------------*/
// end of tool function to convert ":"-seperated lines
/* ---------------------------------------------------------------------------*/



/* ---------------------------------------------------------------------------*/
/*
 * get system parameter from a file entry, such as /proc/sys/kernel/shmmax
 * and put it in the character array pointed by buffer
 * It returns the number of bytes read, or -1 in case of any error
 */

int get_system_parameter(char *path, char *entry, char *buffer, int size) {
  char * completePath = NULL;
  FILE * file         = NULL;
  int    res          = 0;

  _OSBASE_TRACE(4,("--- get_system_parameter() called"));

  if (path == NULL || entry == NULL || buffer == NULL)
    return -1;
  completePath = (char *)malloc(strlen(path) + strlen(entry) + 1);
  if (completePath == NULL)
    return -1;
  sprintf(completePath, "%s/%s", path, entry);
  if ((file = fopen(completePath, "r")) == NULL)
    return -1;
  res = fread(buffer, 1, size - 1, file);
  fclose(file);
  if (res > 0)
    buffer[res] = '\0';
  free(completePath);

  _OSBASE_TRACE(4,("--- get_system_parameter() exited"));
  return res;
}


/* ---------------------------------------------------------------------------*/
/*
 * set system parameter to a file entry, such as /proc/sys/kernel/shmmax
 * using the value string 
 * It returns the number of bytes written, or -1 in case of any error
 */

int set_system_parameter(char *path, char *entry, char *value) {
  char * completePath = NULL;
  FILE * file         = NULL;
  int    res          = 0;

  _OSBASE_TRACE(4,("--- set_system_parameter() called"));

  if (path == NULL || entry == NULL || value == NULL)
    return -1;
  completePath = (char *)malloc(strlen(path) + strlen(entry) + 1);
  if (completePath == NULL)
    return -1;
  sprintf(completePath, "%s/%s", path, entry);
  if ((file = fopen(completePath, "w")) == NULL)
    return -1;
  res = fwrite(value, 1, strlen(value), file);
  fclose(file);
  free(completePath);

  _OSBASE_TRACE(4,("--- set_system_parameter() exited"));
  return res;
}


/* ---------------------------------------------------------------------------*/



/* ---------------------------------------------------------------------------*/
/*                            trace facility                                  */
/* ---------------------------------------------------------------------------*/

char * _format_trace(char *fmt,...) {
   va_list ap;
   char *msg=(char*)malloc(1024);
   va_start(ap,fmt);
   vsnprintf(msg,1024,fmt,ap);
   va_end(ap);
   return msg;
}

void _osbase_trace( int level, char * file, int line, char * msg) {

  struct tm        cttm;
  long             sec  = 0;
  char           * tm   = NULL;
  FILE           * ferr = NULL;
  
  _init_trace_level();
  _init_trace_file();


  sec = time(NULL) + get_os_timezone()*60;
  tm = (char*)malloc(20);
  memset(tm, 0, 20*sizeof(char));
  if( gmtime_r( &sec , &cttm) != NULL ) {
    strftime(tm,20,"%m/%d/%Y %H:%M:%S",&cttm);
  }
  
  if( (_SBLIM_TRACE_FILE != NULL) ) {
    if( (ferr=fopen(_SBLIM_TRACE_FILE,"a")) == NULL ) {
      perror("perror: Couldn't open/create trace file %s\n");
      if(tm)
	fprintf(stderr,"[%s] Couldn't open/create trace file %s\n",tm,_SBLIM_TRACE_FILE);
      else
	fprintf(stderr,"Couldn't open/create trace file %s\n",_SBLIM_TRACE_FILE);	
      return;
    }
  }
  else { ferr = stderr; } 

  if(tm) {
    fprintf(ferr,"[%i] [%s] --- %s(%i) : %s\n", level, tm, file, line, msg);
  }
  else {
    fprintf(ferr,"[%i] --- %s(%i) : %s\n", level, file, line, msg);
  }
  
  if( (_SBLIM_TRACE_FILE != NULL) ) {
    fclose(ferr);
  }

  if(tm) free(tm);
  if(msg)  free(msg);
}



/* ---------------------------------------------------------------------------*/
