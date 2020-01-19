/*
 * OSBase_UnixProcess.c
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
 *               
 * Description:
 * This shared library provides resource access functionality for the class
 * Linux_UnixProcess. 
 * It is independent from any specific CIM technology.
*/

/* ---------------------------------------------------------------------------*/

#include "OSBase_Common.h"
#include "OSBase_UnixProcess.h"
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>  
#include <dirent.h>
#include <libgen.h>
#include <sys/stat.h>    
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>

/* ---------------------------------------------------------------------------*/
// private declarations


/* ---------------------------------------------------------------------------*/


static int _process_data( char * pid, struct cim_process ** sptr );
static char * _get_process_execpath( char * id, char * cmd );

/* returns a NULL terminated list of pointers; each listentry contains a pointer
 * to a structure, which contains necessary information about the currently
 * running processes
 */
int enum_all_process( struct processlist ** lptr ){
  struct processlist *  lptrhelp = NULL;
  char               ** hdout    = NULL;
  char               *  ptr      = NULL;
  int                   rc       = 0;
  int                   i        = 0;
  
  _OSBASE_TRACE(3,("--- enum_all_process() called"));
  
  rc = runcommand( "ps --no-headers -eo pid,ppid,tty,pri,nice,uid,gid,pmem,pcpu,cputime,session,state,args" , NULL, &hdout, NULL );

  if( rc == 0 ) {
    lptrhelp = (struct processlist *)calloc(1,sizeof(struct processlist));
    *lptr = lptrhelp;
    
    while( hdout[i] ) {
      if ( lptrhelp->p != NULL) { 
	lptrhelp->next = (struct processlist *)calloc(1,sizeof(struct processlist));
	lptrhelp = lptrhelp->next;
      }    
      if((ptr=strchr(hdout[i], '\n'))) { *ptr = '\0'; }
      rc = _process_data( hdout[i] , &(lptrhelp->p) );
      i++;
    }
  }
  freeresultbuf( hdout );
  _OSBASE_TRACE(3,("--- enum_all_process() exited"));
  return rc;
}

int get_process_data( char * pid , struct cim_process ** sptr ) {
  char ** hdout = NULL;
  char *  cmd   = NULL;
  char *  str   = NULL;
  char *  ptr   = NULL;
  char    buf[256];
  int     len   = 0;
  int     i     = 0;
  int     rc    = 0;

  _OSBASE_TRACE(3,("--- get_process_data() called"));

  sscanf(pid, "%[0-9] %n", buf, &len);
  if (len != strlen(pid)) {
      _OSBASE_TRACE(3,("--- get_process_data() failed : PID %s not valid", pid));
      return -1;
  }

  cmd = (char*)malloc(len + 100);
  sprintf(cmd, "ps -p %s --no-headers -o pid,ppid,tty,pri,nice,uid,gid,pmem,pcpu,cputime,session,state,args",buf);
  rc = runcommand( cmd , NULL, &hdout, NULL );
  if (rc == 0) {
    while( hdout[i] ) {
      if((ptr=strchr(hdout[i], '\n'))) { *ptr = '\0'; }
      str = hdout[i];
      while( *str == ' ' ) { str = str+1; }
      if( strncmp( str,buf,len) == 0 ) {
	rc = _process_data( hdout[i], sptr );
	break;
      }
      i++;
    }
    free ( cmd );
    freeresultbuf( hdout );
  }
  else { 
    _OSBASE_TRACE(3,("--- get_process_data() failed : PID %s not valid",pid));
    return -1 ;
  }
  _OSBASE_TRACE(3,("--- get_process_data() exited"));
  return rc;
}

static int _process_data( char * phd , struct cim_process ** sptr ){
  char               ** parr   = NULL;
  char               *  d      = NULL;
  char               *  end    = NULL;
  char               *  cmd    = NULL;
  FILE               *  fpstat = NULL;
  struct tm             tmdate;
  unsigned long long    kmtime = 0;        // Kernel Mode Time
  unsigned long long    umtime = 0;        // User Mode Time
  unsigned long         ctime  = 0;        // Start Time
  unsigned long         uptime = 0;
  int                   i      = 0;
  int                   j      = 0;
  int                   rc     = 0;

  _OSBASE_TRACE(4,("--- _process_data() called"));

  (*sptr) = (struct cim_process *)calloc(1,sizeof(struct cim_process));

  parr = calloc(1000,sizeof(char*));    
  end = phd + strlen(phd);
  while(phd < end) {
      d=strchr(phd,' ');
      if (d!=NULL)
	  *d=0;
      if (strlen(phd) > 0)
	  parr[i++]=strdup(phd);
      phd+=strlen(phd)+1;
  }
  
  /* Process related */
  (*sptr)->pid = strdup(parr[0]);

  (*sptr)->ppid = strdup(parr[1]);
  (*sptr)->ptty = strdup(parr[2]);
  (*sptr)->prio = atol(parr[3]);
  (*sptr)->nice = atol(parr[4]);

  (*sptr)->uid  = atoll(parr[5]);
  (*sptr)->gid  = atoll(parr[6]);

  (*sptr)->pmem = atoll(parr[7]);
  (*sptr)->pcpu = atol(parr[8]);

  (*sptr)->sid = atoll(parr[10]);

  /* state of Linux processes :
     D   uninterruptible sleep (usually IO) = Blocked (4)
     R   runnable (on run queue)            = Running (3)
     S   sleeping                           = Suspended Ready (6)
     T   traced or stopped                  = Stopped (8)
     Z   a defunct ("zombie") process       = Terminated (7)
  */
  /* Mapping between ExecutionState and integer value */
  /* 0 ... Unknown
   * 1 ... Other
   * 2 ... Ready
   * 3 ... Running
   * 4 ... Blocked
   * 5 ... Suspended Blocked
   * 6 ... Suspended Ready
   * 7 ... Terminated
   * 8 ... Stopped
   * 9 ... Growing
   */
  if(strcmp(parr[11],"R")==0)
    (*sptr)->state = 3 ;
  else if( strcmp(parr[11],"D")==0)
    (*sptr)->state = 4 ;
  else if( strcmp(parr[11],"S")==0)
    (*sptr)->state = 6 ;
  else if( strcmp(parr[11],"Z")==0)
    (*sptr)->state = 7 ;
  else if( strcmp(parr[11],"T")==0)
    (*sptr)->state = 8 ;

  /* values for array of Parameters */
  i=12;
    j=0;
    (*sptr)->args = calloc(100,sizeof(char*));  
    while(( parr[i] != NULL ) && (i < 100))  /* preventing buffer overflows */ {
      (*sptr)->args[j]=strdup(parr[i]);
      //     fprintf(stderr,"(*sptr)->args[%i] : %s\n",j,(*sptr)->args[j]);
      j++;
      i++;
    }
  if (parr[12][0]=='[' && parr[12][strlen(parr[12])-1]==']') {
    (*sptr)->pcmd = calloc(strlen(parr[12])-1,1);
    strncpy((*sptr)->pcmd,parr[12]+1,strlen(parr[12])-2);
  } else {
    (*sptr)->pcmd = strdup(basename(parr[12]));
  }
  freeresultbuf(parr);
  
  /* ModulePath */
  //  (*sptr)->path = _get_process_execpath((*sptr)->pcmd);
  (*sptr)->path = _get_process_execpath((*sptr)->pid,(*sptr)->pcmd);

  /* get UserModeTime, KernelModeTime and CreationDate */
  cmd = (char*)malloc( (strlen((*sptr)->pid)+12));
  strcpy( cmd, "/proc/");
  strcat( cmd, (*sptr)->pid);
  strcat( cmd, "/stat");

  if ( (fpstat=fopen(cmd,"r")) != NULL ) {
    fscanf(fpstat,"%*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s "
	   "%lld %lld %*s %*s %*s %*s %*s %ld", 
	   &umtime,&kmtime,&ctime );
    fclose(fpstat);
    (*sptr)->kmtime = kmtime*10;
    (*sptr)->umtime = umtime*10;

    /* char * createdate : yyyyMMddHHmmss.SSSSSSsutc , e.g. 20030112152345.000000+100 */
    uptime = _get_os_boottime();
    if( uptime == 0 ) { (*sptr)->createdate = NULL; }
    else {
      ctime = (ctime/100)+uptime;
      if( gmtime_r( &ctime , &tmdate) != NULL ) {
	(*sptr)->createdate = (char*)malloc(26);
	rc = strftime((*sptr)->createdate,26,"%Y%m%d%H%M%S.000000",&tmdate);
	_cat_timezone((*sptr)->createdate, get_os_timezone());
      }
    }
    //    fprintf(stderr,"(*sptr)->createdate: %s\n",(*sptr)->createdate);
    /*    fprintf(stderr,"PID: %s ... um:%lli; km:%lli;\num:%lli; km:%lli; cd_sec:%li\n",
    	    (*sptr)->pid,umtime,kmtime,(*sptr)->umtime,(*sptr)->kmtime,(*sptr)->createtime);
    */
  }
  else { (*sptr)->createdate = NULL; }
  if(cmd) free(cmd);

  _OSBASE_TRACE(4,("--- _process_data() exited"));
  return 0;
}

static char * _get_process_execpath( char * id , char * cmd ) {
  char *  buf   = NULL;
  char *  path  = NULL;
  int     rc    = 0;  

  _OSBASE_TRACE(4,("--- _get_process_execpath() called"));

  path = (char*)malloc((strlen(id)+11));
  strcpy(path, "/proc/");
  strcat(path, id);
  strcat(path, "/exe");

  buf = (char*)malloc(1024);
  memset(buf, 0, 1024);

  rc = readlink(path, buf, 1024);
  if( rc == -1 ) {
    free(buf);
    buf = strdup(cmd);
  }
  free ( path );

  _OSBASE_TRACE(4,("--- _get_process_execpath() exited : %s",buf));
  return buf; 
}

void free_processlist( struct processlist * lptr ) {
  struct processlist * ls = NULL ;

  if( lptr == NULL ) return;
  for( ; lptr ; ) {
    if(lptr->p) { free_process(lptr->p); }
    ls = lptr;
    lptr = lptr->next;
    free(ls);
  }
}

void free_process( struct cim_process * sptr ) {
  if( sptr == NULL ) return;
  if(sptr->pid)    free(sptr->pid); 
  if(sptr->ppid)   free(sptr->ppid); 
  if(sptr->ptty)   free(sptr->ptty); 
  if(sptr->pcmd)   free(sptr->pcmd);
  if(sptr->path)   free(sptr->path);
  if(sptr->createdate) free(sptr->createdate);
  freeresultbuf(sptr->args);
  if(sptr) free(sptr);
}


/* ---------------------------------------------------------------------------*/
/*                         end of OSBase_UnixProcess.c                        */
/* ---------------------------------------------------------------------------*/

