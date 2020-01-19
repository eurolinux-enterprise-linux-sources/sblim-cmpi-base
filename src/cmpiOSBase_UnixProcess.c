/*
 * cmpiOSBase_UnixProcess.c
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
 *               Eric C. Wu <cwu@us.ibm.com>
 *               Tyrel Datwyler <tyreld@us.ibm.com>
 *
 * Interface Type : Common Magabeablity Programming Interface ( CMPI )
 *
 * Description:
 * This is the factory implementation for creating instances of CIM
 * class Linux_UnixProcess.
 *
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmpidt.h"
#include "cmpimacs.h"

#include "cmpiOSBase_Common.h"
#include "OSBase_Common.h"

#include "cmpiOSBase_UnixProcess.h"
#include "OSBase_UnixProcess.h"


/* ---------------------------------------------------------------------------*/
/* private declarations                                                       */


/* ---------------------------------------------------------------------------*/

#ifdef MODULE_SYSMAN

static char   tempBuffer[PATH_MAX + 1];
static char   makeBuffer[PATH_MAX + 1]; 
static char * fileEntry = "/proc/sysman/pid_rlimit";

static void _sysman_init();
static int _sysman_CI( CMPIInstance * ci , char * pid );
static char * searchPath(char * paths, char * entry);
static int installModule(char *namep);

#endif

/* ---------------------------------------------------------------------------*/
/*                            Factory functions                               */
/* ---------------------------------------------------------------------------*/

/* ---------- method to create a CMPIObjectPath of this class ----------------*/

CMPIObjectPath * _makePath_UnixProcess( const CMPIBroker * _broker,
                 const CMPIContext * ctx, 
                 const CMPIObjectPath * ref,
		 struct cim_process * sptr,
		 CMPIStatus * rc) {
  CMPIObjectPath * op = NULL;
 
  _OSBASE_TRACE(2,("--- _makePath_UnixProcess() called"));

  /* the sblim-cmpi-base package offers some tool methods to get common
   * system data 
  */
  if( !get_system_name() ) {   
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "no host name found" );
    _OSBASE_TRACE(2,("--- _makePath_UnixProcess() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }

  if( !get_os_name() ) {
    CMSetStatusWithChars( _broker, rc,
			  CMPI_RC_ERR_FAILED, "no OS name found" );
    _OSBASE_TRACE(2,("--- _makePath_UnixProcess() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }

  op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(ref,rc)), 
			_ClassName, rc );
  if( CMIsNullObject(op) ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." ); 
    _OSBASE_TRACE(2,("--- _makePath_UnixProcess() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit; 
  }

  CMAddKey(op, "CSCreationClassName", CSCreationClassName, CMPI_chars);
  CMAddKey(op, "CSName", get_system_name(), CMPI_chars);
  CMAddKey(op, "OSCreationClassName", OSCreationClassName, CMPI_chars);
  CMAddKey(op, "OSName", get_os_name(), CMPI_chars);
  CMAddKey(op, "CreationClassName", _ClassName, CMPI_chars);
  CMAddKey(op, "Handle", sptr->pid, CMPI_chars);
    
 exit:
  _OSBASE_TRACE(2,("--- _makePath_UnixProcess() exited"));
  return op;                
}

/* ----------- method to create a CMPIInstance of this class ----------------*/

CMPIInstance * _makeInst_UnixProcess( const CMPIBroker * _broker,
               const CMPIContext * ctx, 
               const CMPIObjectPath * ref,
	       const char ** properties,
	       struct cim_process * sptr,
	       CMPIStatus * rc) {
  CMPIObjectPath *  op       = NULL;
  CMPIInstance   *  ci       = NULL;
  CMPIDateTime   *  cdt      = NULL;
  CMPIArray      *  args     = NULL;
  CMPIString     *  val      = NULL;
  const char     ** keys     = NULL;
  int               keyCount = 0;
  int               i        = 0;
  int               max      = 0;
#ifndef CIM26COMPAT
  unsigned short    status   = 2; /* Enabled */
#endif

  _OSBASE_TRACE(2,("--- _makeInst_UnixProcess() called"));

#ifdef MODULE_SYSMAN
  int               sysrc  = 0;

  if (!moduleInstalled) { _sysman_init(); }
#endif

  /* the sblim-cmpi-base package offers some tool methods to get common
   * system data
  */
  if( !get_system_name() ) {   
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "no host name found" );
    _OSBASE_TRACE(2,("--- _makeInst_UnixProcess() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }

  if( !get_os_name() ) {
    CMSetStatusWithChars( _broker, rc,
			  CMPI_RC_ERR_FAILED, "no OS name found" );
    _OSBASE_TRACE(2,("--- _makeInst_UnixProcess() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }

  op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(ref,rc)), 
			_ClassName, rc );
  if( CMIsNullObject(op) ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." ); 
    _OSBASE_TRACE(2,("--- _makeInst_UnixProcess() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit; 
  }

  ci = CMNewInstance( _broker, op, rc);
  if( CMIsNullObject(ci) ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "Create CMPIInstance failed." ); 
    _OSBASE_TRACE(2,("--- _makeInst_UnixProcess() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit; 
  }

  /* set property filter */
  keys = calloc(7,sizeof(char*));
  keys[0] = strdup("CSCreationClassName");
  keys[1] = strdup("CSName");
  keys[2] = strdup("OSCreationClassName");
  keys[3] = strdup("OSName");
  keys[4] = strdup("CreationClassName");
  keys[5] = strdup("Handle");
  CMSetPropertyFilter(ci,properties,keys);
  for( ;keys[keyCount]!=NULL;keyCount++) { free((char*)keys[keyCount]); }
  free(keys);

  CMSetProperty( ci, "CSCreationClassName",CSCreationClassName , CMPI_chars );
  CMSetProperty( ci, "CSName", get_system_name(), CMPI_chars );
  CMSetProperty( ci, "OSCreationClassName", OSCreationClassName, CMPI_chars );
  CMSetProperty( ci, "OSName", get_os_name(), CMPI_chars );
  CMSetProperty( ci, "CreationClassName", _ClassName, CMPI_chars );
  CMSetProperty( ci, "Handle", sptr->pid, CMPI_chars );

  CMSetProperty( ci, "Caption", "Linux (Unix) Process", CMPI_chars);
  CMSetProperty( ci, "Description", "This class represents instances of currently running programms.", CMPI_chars);

  CMSetProperty( ci, "Status", "NULL", CMPI_chars);

  CMSetProperty( ci, "ParentProcessID", sptr->ppid, CMPI_chars);
  CMSetProperty( ci, "ProcessTTY", sptr->ptty, CMPI_chars);
  CMSetProperty( ci, "Name", sptr->pcmd, CMPI_chars);
  CMSetProperty( ci, "ModulePath", sptr->path, CMPI_chars);

  CMSetProperty( ci, "Priority", (CMPIValue*)&(sptr->prio), CMPI_uint32);

  CMSetProperty( ci, "RealUserID", (CMPIValue*)&(sptr->uid), CMPI_uint64);
  CMSetProperty( ci, "ProcessGroupID", (CMPIValue*)&(sptr->gid), CMPI_uint64);
  CMSetProperty( ci, "ProcessSessionID", (CMPIValue*)&(sptr->sid), CMPI_uint64);
  CMSetProperty( ci, "ProcessNiceValue", (CMPIValue*)&(sptr->nice), CMPI_uint32);

  /* CIM Mapping between ExecutionState and int value */
  /* 0 ... Unknown
   * 1 ... Other
   * 2 ... Ready
   * 3 ... Running (supported)
   * 4 ... Blocked (supported)
   * 5 ... Suspended Blocked
   * 6 ... Suspended Ready (supported)
   * 7 ... Terminated (supported)
   * 8 ... Stopped (supported)
   * 9 ... Growing
   */
  CMSetProperty( ci, "ExecutionState", (CMPIValue*)&(sptr->state), CMPI_uint16);

  CMSetProperty( ci, "KernelModeTime", (CMPIValue*)&(sptr->kmtime), CMPI_uint64);
  CMSetProperty( ci, "UserModeTime", (CMPIValue*)&(sptr->umtime), CMPI_uint64);

  if( sptr->createdate != NULL ) {
    cdt = CMNewDateTimeFromChars(_broker,sptr->createdate,rc);
    if(cdt == NULL) {
      CMSetStatusWithChars( _broker, rc,
			    CMPI_RC_ERR_FAILED, 
			    "CMNewDateTimeFromChars for property CreationDate failed." );
      _OSBASE_TRACE(2,("--- _makeInst_UnixProcess() failed : %s",CMGetCharPtr(rc->msg)));
      goto exit;
    }
    else { CMSetProperty( ci, "CreationDate", (CMPIValue*)&(cdt), CMPI_dateTime); }
  }

  /* Parameters */
  while( sptr->args[max] != NULL ) { max++; }
  if( max > 25 ) { max=25; }
  args = CMNewArray(_broker,max,CMPI_string,rc); 
  if(args == NULL) {
    CMSetStatusWithChars( _broker, rc,
			  CMPI_RC_ERR_FAILED, "Create Array for Property Parameters failed." );
    _OSBASE_TRACE(2,("--- _makeInst_ComputerSystem() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }
  else {
    for( ; i< max; i++ ) {
      /* BMMU: Prevent segfaults */
      if (sptr->args[i]) {
	val = CMNewString(_broker,sptr->args[i],rc);
	CMSetArrayElementAt(args,i,(CMPIValue*)&(val),CMPI_string);
      }
      else break;
    }
    //  fprintf(stderr,"args %s\n",CMGetCharPtr(CDToString(_broker,args,rc)));
    CMSetProperty( ci, "Parameters", (CMPIValue*)&(args), CMPI_stringA);
  }
  
  /* 2.7 */
#ifndef CIM26COMPAT
  CMSetProperty( ci, "ElementName", sptr->pcmd, CMPI_chars);
  CMSetProperty( ci, "EnabledState", (CMPIValue*)&(status), CMPI_uint16);
  CMSetProperty( ci, "OtherEnabledState", "NULL", CMPI_chars);
  CMSetProperty( ci, "RequestedState", (CMPIValue*)&(status), CMPI_uint16);
  CMSetProperty( ci, "EnabledDefault", (CMPIValue*)&(status), CMPI_uint16);
#endif

#ifdef MODULE_SYSMAN
  sysrc = _sysman_CI( ci, sptr->pid );
#endif	

 exit:
  _OSBASE_TRACE(2,("--- _makeInst_UnixProcess() exited"));
  return ci;
}


/* ---------------------------------------------------------------------------*/
/* ---------------------- methods of module SYSMAN ---------------------------*/

#ifdef MODULE_SYSMAN

static void _sysman_init() {
  FILE *file;
  if ((file = fopen(fileEntry, "r")) != NULL) {
    moduleInstalled = 1;
    fclose(file);
  } else
    moduleInstalled = installModule("sysman.o");
  if (moduleInstalled < 0) {
    fprintf(stderr, "Warning: kernel module sysman.o not found\n");
    moduleInstalled = 0;
  }
}

int _sysman_data( char * pid , struct _SysMan * sm ) {
  char * tokp = NULL;

  sprintf(makeBuffer, "%s", pid);
  set_system_parameter(dirPath, fileName, makeBuffer);
  
  get_system_parameter(dirPath, fileName, makeBuffer, PATH_MAX + 1);
  tokp = strtok(makeBuffer, " \t");

  /* return if the pid is an invalid process id */
  if (strcmp(tokp, pid))
    return -1;

  tokp = strtok(0, " \t");
  sm->maxChildProc = atol(tokp);
  tokp = strtok(0, " \t");
  sm->maxOpenFiles = atol(tokp);
  tokp = strtok(0, " \t");
  sm->maxRealStack = atol(tokp);

  return 0;
}

static int _sysman_CI( CMPIInstance * ci , char * pid ) {
  struct _SysMan  sm;
  int             rc   = 0;

  rc = _sysman_data( pid , &sm );
  if( rc != 0 ) return -1;

  CMSetProperty( ci, "MaxNumberOfChildProcesses", (CMPIValue*)&(sm.maxChildProc), CMPI_uint32);
  CMSetProperty( ci, "MaxNumberOfOpenFiles", (CMPIValue*)&(sm.maxOpenFiles), CMPI_uint32);
  CMSetProperty( ci, "MaxRealStack", (CMPIValue*)&(sm.maxRealStack), CMPI_uint32);

  return 0;
}


/*
 * searchPath() searches paths defined in paths for the entry 
 */

static char *searchPath(char *paths, char *entry) {
  char *tokp;
  FILE *filp;
  
  tokp = (char *) strtok(paths, ", :");
  do {
    sprintf(pathBuffer, "%s/%s", tokp, entry);
    if ((filp = fopen(pathBuffer, "r")) != NULL)
      break;
  } while ((tokp = (char *) strtok(0, ", :")) != NULL);
  if (filp != NULL) {
    fclose(filp);
    return tokp;
  }
  return NULL;
}


/*
 * installModule() searches paths defined in MODPATH and LD_LIBRARY_PATH
 * for the module sysman.o
 */

static int installModule(char *namep) {
  int pid, status;
  char *cp, *sysdirp;
  
  sysdirp = getenv("MODPATH");
  cp = searchPath(sysdirp, namep);
  if (cp == NULL) {
    sysdirp = getenv("LD_LIBRARY_PATH");
    cp = searchPath(sysdirp, namep);
  }
  if (cp == NULL) return -1;
  pid = fork();
  if (pid == -1) return -1;
  if (pid == 0) { /* child */
    char *argv[4];

    argv[0] = "sh";
    argv[1] = "-c";
    sprintf(tempBuffer, "/sbin/insmod %s/%s", cp, namep);
    fprintf(stderr,"%s\n", tempBuffer);
    argv[2] = tempBuffer;
    argv[3] = 0;
    execve("/bin/sh", argv, NULL);
    exit(127);
  }
  if (waitpid(pid, &status, 0) == -1) return -1;
  return 1;
}

#endif


/* ---------------------------------------------------------------------------*/
/*                     end of cmpiOSBase_UnixProcess.c                        */
/* ---------------------------------------------------------------------------*/

