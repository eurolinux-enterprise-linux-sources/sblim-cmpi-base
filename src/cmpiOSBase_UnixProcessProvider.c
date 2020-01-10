/*
 * cmpiOSBase_UnixProcessProvider.c
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
 * Contributors:
 *
 * Interface Type : Common Manageability Programming Interface ( CMPI )
 *
 * Description: 
 * 
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "cmpidt.h"
#include "cmpift.h"
#include "cmpimacs.h"

#include "OSBase_Common.h"
#include "cmpiOSBase_Common.h"
#include "cmpiOSBase_UnixProcess.h"

static const CMPIBroker * _broker;

/* ---------------------------------------------------------------------------*/
/* private declarations                                                       */
#ifdef CMPI_VER_100
#define OSBase_UnixProcessProviderSetInstance OSBase_UnixProcessProviderModifyInstance 
#endif


/* ---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------*/
/*                      Instance Provider Interface                           */
/* ---------------------------------------------------------------------------*/


CMPIStatus OSBase_UnixProcessProviderCleanup( CMPIInstanceMI * mi, 
           const CMPIContext * ctx, CMPIBoolean terminate) { 
  _OSBASE_TRACE(1,("--- %s CMPI Cleanup() called",_ClassName));
  _OSBASE_TRACE(1,("--- %s CMPI Cleanup() exited",_ClassName));
  CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_UnixProcessProviderEnumInstanceNames( CMPIInstanceMI * mi, 
           const CMPIContext * ctx, 
           const CMPIResult * rslt, 
           const CMPIObjectPath * ref) { 
  CMPIObjectPath     * op    = NULL;  
  CMPIStatus           rc    = {CMPI_RC_OK, NULL};
  struct processlist * lptr  = NULL;
  struct processlist * rm    = NULL;
  
  _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() called",_ClassName));

  if( enum_all_process( &lptr ) != 0 ) {
    CMSetStatusWithChars( _broker, &rc,
			  CMPI_RC_ERR_FAILED, "Could not list active processes." ); 
    _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
    return rc;
  }
  
  rm = lptr;
  // iterate process list
  if( lptr ) {
    for ( ; lptr && rc.rc == CMPI_RC_OK ; lptr = lptr->next) {
      // method call to create the CMPIInstance object
      op = _makePath_UnixProcess( _broker, ctx, ref, lptr->p, &rc );
      if( op == NULL || rc.rc != CMPI_RC_OK ) { 
	if( rc.msg != NULL ) {
	  _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
	}
	CMSetStatusWithChars( _broker, &rc,
			      CMPI_RC_ERR_FAILED, "Transformation from internal structure to CIM ObjectPath failed." ); 
	if(rm) free_processlist(rm);
	_OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
	return rc; 
      }
      else { CMReturnObjectPath( rslt, op ); }
    }  
    if(rm) free_processlist(rm);
  }
  
  CMReturnDone( rslt );
  _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() exited",_ClassName));
  return rc;
}

CMPIStatus OSBase_UnixProcessProviderEnumInstances( CMPIInstanceMI * mi, 
           const CMPIContext * ctx, 
           const CMPIResult * rslt, 
           const CMPIObjectPath * ref, 
           const char ** properties) { 
  CMPIInstance       * ci    = NULL;
  CMPIStatus           rc    = {CMPI_RC_OK, NULL};
  struct processlist * lptr  = NULL;
  struct processlist * rm    = NULL;

  _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() called",_ClassName));
  
  if( enum_all_process( &lptr ) != 0 ) {
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_FAILED, "Could not list active processes." ); 
    _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
    return rc;
  }

  rm = lptr;
  // iterate process list
  if( lptr ) {
    for ( ; lptr && rc.rc == CMPI_RC_OK ; lptr = lptr->next) {
      // method call to create the CMPIInstance object
      ci = _makeInst_UnixProcess( _broker, ctx, ref, properties, lptr->p, &rc );
      if( ci == NULL || rc.rc != CMPI_RC_OK ) { 
	if( rc.msg != NULL ) {
	  _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
	}
	CMSetStatusWithChars( _broker, &rc,
			      CMPI_RC_ERR_FAILED, "Transformation from internal structure to CIM Instance failed." ); 
	if(rm) free_processlist(rm);
	_OSBASE_TRACE(1,("--- %s CMPI EnumInstances() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
	return rc; 
      }
      else { CMReturnInstance( rslt, ci ); }
    }  
    if(rm) free_processlist(rm);
  }

  CMReturnDone( rslt );
  _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() exited",_ClassName));
  return rc;
}

CMPIStatus OSBase_UnixProcessProviderGetInstance( CMPIInstanceMI * mi, 
           const CMPIContext * ctx, 
           const CMPIResult * rslt, 
           const CMPIObjectPath * cop, 
           const char ** properties) {  
  CMPIInstance       * ci    = NULL;
  CMPIStatus           rc    = {CMPI_RC_OK, NULL};
  struct cim_process * sptr  = NULL;
  CMPIString         * name  = NULL;
  int                  cmdrc = 0;

  _OSBASE_TRACE(1,("--- %s CMPI GetInstance() called",_ClassName));

  _check_system_key_value_pairs( _broker, cop, "CSCreationClassName", "CSName", &rc );
  if( rc.rc != CMPI_RC_OK ) { 
    _OSBASE_TRACE(1,("--- %s CMPI GetInstance() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
    return rc; 
  }
  _check_system_key_value_pairs( _broker, cop, "OSCreationClassName", "OSName", &rc );
  if( rc.rc != CMPI_RC_OK ) { 
    _OSBASE_TRACE(1,("--- %s CMPI GetInstance() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
    return rc; 
  }

  name = CMGetKey( cop, "Handle", &rc).value.string;
  if( name == NULL ) {    
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_FAILED, "Could not get Process ID." ); 
    _OSBASE_TRACE(1,("--- %s CMPI GetInstance() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
    return rc;
  }

  cmdrc = get_process_data( CMGetCharPtr(name) , &sptr );
  if( cmdrc != 0 || sptr == NULL ) {    
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_FOUND, "Process ID does not exist." ); 
    _OSBASE_TRACE(1,("--- %s CMPI GetInstance() exited : %s",_ClassName,CMGetCharPtr(rc.msg)));
    return rc;
  }

  ci = _makeInst_UnixProcess( _broker, ctx, cop, properties, sptr, &rc );
  if(sptr) free_process(sptr);

  if( ci == NULL ) {
    if( rc.msg != NULL ) {
      _OSBASE_TRACE(1,("--- %s CMPI GetInstance() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
    }
    else {
      _OSBASE_TRACE(1,("--- %s CMPI GetInstance() failed",_ClassName));
    }
    return rc;
  }

  CMReturnInstance( rslt, ci );
  CMReturnDone(rslt);
  _OSBASE_TRACE(1,("--- %s CMPI GetInstance() exited",_ClassName));
  return rc;
}

CMPIStatus OSBase_UnixProcessProviderCreateInstance( CMPIInstanceMI * mi, 
           const CMPIContext * ctx, 
           const CMPIResult * rslt, 
           const CMPIObjectPath * cop, 
           const CMPIInstance * ci) {
  CMPIStatus rc = {CMPI_RC_OK, NULL};

  _OSBASE_TRACE(1,("--- %s CMPI CreateInstance() called",_ClassName));

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED" ); 

  _OSBASE_TRACE(1,("--- %s CMPI CreateInstance() exited",_ClassName));
  return rc;
}

CMPIStatus OSBase_UnixProcessProviderSetInstance( CMPIInstanceMI * mi, 
           const CMPIContext * ctx, 
           const CMPIResult * rslt, 
           const CMPIObjectPath * cop,
           const CMPIInstance * ci, 
           const char ** properties) {
  CMPIStatus           rc    = {CMPI_RC_OK, NULL};
#ifdef MODULE_SYSMAN
  struct cim_process * sptr  = NULL;
  CMPIString         * pid   = NULL;
  CMPIData             data;
  struct _SysMan       sm;
  int                  cmdrc = 0;
#endif

  _OSBASE_TRACE(1,("--- %s CMPI SetInstance() called",_ClassName));

#ifdef MODULE_SYSMAN
 
  if (!moduleInstalled)
    goto exit;
  
  pid = CMGetKey( cop, "Handle", &rc).value.string;
  if( pid == NULL ) {    
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_FAILED, "Could not get ProcessID." );
    _OSBASE_TRACE(1,("--- %s CMPI SetInstance() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
    return rc;
  }

  /* get data of new process instance */
  cmdrc = get_process_data( CMGetCharPtr(pid) , &sptr );
  if( sptr == NULL ) {    
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_FOUND, "Process ID does not exist." );
    _OSBASE_TRACE(1,("--- %s CMPI SetInstance() exited : %s",_ClassName,CMGetCharPtr(rc.msg)));
    return rc;
  }

  ci = _makeInst( _broker, ctx, cop, sptr, &rc );
  if(sptr) free_process(sptr);

  if( ci == NULL ) {
    if( rc.msg != NULL ) {
      _OSBASE_TRACE(1,("--- %s CMPI SetInstance() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
    }
    else {
      _OSBASE_TRACE(1,("--- %s CMPI SetInstance() failed",_ClassName));
    }
    CMReturn(CMPI_RC_ERR_FAILED);
  }

  /* get data of old process instance */
  cmdrc = _sysman_data( CMGetCharPtr(pid) , &sm );
  if( cmdrc != 0 ) goto exit;

  /* set new values for process */

  data = CMGetProperty( ci, "MaxNumberOfOpenFiles", &rc);
  if ( data.value.uint32 != sm.maxChildProc ) {
    sprintf(pathBuffer, "%s\t-1\t%ld", CMGetCharPtr(pid), data.value.uint32 );
    set_system_parameter(dirPath, fileName, pathBuffer);
  }

  data = CMGetProperty( ci, "MaxNumberOfChildProcesses", &rc);
  if ( data.value.uint32 != sm.maxChildProc ) {
    sprintf(pathBuffer, "%s\t-1\t%ld", CMGetCharPtr(pid), data.value.uint32 );
    set_system_parameter(dirPath, fileName, pathBuffer);
  }

  data = CMGetProperty( ci, "MaxRealStack", &rc);
  if ( data.value.uint32 != sm.maxChildProc ) {
    sprintf(pathBuffer, "%s\t-1\t%ld", CMGetCharPtr(pid), data.value.uint32 );
    set_system_parameter(dirPath, fileName, pathBuffer);
  }

 exit:
#else

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED" ); 
#endif

  _OSBASE_TRACE(1,("--- %s CMPI SetInstance() exited",_ClassName));
  return rc;
}

CMPIStatus OSBase_UnixProcessProviderDeleteInstance( CMPIInstanceMI * mi, 
           const CMPIContext * ctx, 
           const CMPIResult * rslt, 
           const CMPIObjectPath * cop) {
  CMPIStatus rc = {CMPI_RC_OK, NULL}; 

  _OSBASE_TRACE(1,("--- %s CMPI DeleteInstance() called",_ClassName));

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED" ); 

  _OSBASE_TRACE(1,("--- %s CMPI DeleteInstance() exited",_ClassName));
  return rc;
}

CMPIStatus OSBase_UnixProcessProviderExecQuery( CMPIInstanceMI * mi, 
           const CMPIContext * ctx, 
           const CMPIResult * rslt, 
           const CMPIObjectPath * ref, 
           const char * lang, 
           const char * query) {
  CMPIStatus rc = {CMPI_RC_OK, NULL};

  _OSBASE_TRACE(1,("--- %s CMPI ExecQuery() called",_ClassName));

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED" ); 

  _OSBASE_TRACE(1,("--- %s CMPI ExecQuery() exited",_ClassName));
  return rc;
}


/* ---------------------------------------------------------------------------*/
/*                        Method Provider Interface                           */
/* ---------------------------------------------------------------------------*/


CMPIStatus OSBase_UnixProcessProviderMethodCleanup( CMPIMethodMI * mi, 
           const CMPIContext * ctx, CMPIBoolean terminate) {
  _OSBASE_TRACE(1,("--- %s CMPI MethodCleanup() called",_ClassName));
  _OSBASE_TRACE(1,("--- %s CMPI MethodCleanup() exited",_ClassName));
  CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_UnixProcessProviderInvokeMethod( CMPIMethodMI * mi,
           const CMPIContext * ctx,
           const CMPIResult * rslt,
           const CMPIObjectPath * ref,
           const char * methodName,
           const CMPIArgs * in,
           CMPIArgs * out) {
  CMPIData         pid;
  CMPIValue        valrc;
  CMPIString     * class = NULL;
  CMPIStatus       rc    = {CMPI_RC_OK, NULL};
  char           * cmd   = NULL;
  char          ** hdout = NULL;
  char          ** hderr = NULL;
  int              cmdrc = 0;

  _OSBASE_TRACE(1,("--- %s CMPI InvokeMethod() called",_ClassName));
  
  class = CMGetClassName(ref, &rc);

  /* "terminate" */
  if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 && 
      strcasecmp(methodName, "terminate") == 0 ) {

    valrc.uint8 = 0;

    pid = CMGetKey( ref, "Handle", &rc);
    if( pid.value.string == NULL ) {    
      valrc.uint8 = 1;
      CMSetStatusWithChars( _broker, &rc, 
			    CMPI_RC_ERR_FAILED, "Could not get Process ID." ); 
      _OSBASE_TRACE(1,("--- %s CMPI InvokeMethod() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
    }
    else {
      cmd = calloc(1,(strlen(CMGetCharPtr(pid.value.string))+9));
      strcpy(cmd, "kill -9 ");
      strcat(cmd, CMGetCharPtr(pid.value.string));
      cmdrc = runcommand( cmd, NULL, &hdout, &hderr );
      free (cmd);

      /* something went wrong */
      if( cmdrc != 0 ||  hderr[0] != NULL ) {
	/* command execution failed */
	valrc.uint8 = 1;
	_OSBASE_TRACE(1,("--- %s CMPI InvokeMethod() failed : runcommand() returned with %i",_ClassName,cmdrc));
	if( hderr != NULL ) {
	  _OSBASE_TRACE(1,("--- %s CMPI InvokeMethod() failed : runcommand() hderr[0] %s",_ClassName,hderr[0]));
	}
      }
      
      freeresultbuf(hdout);
      freeresultbuf(hderr); 
    }
    CMReturnData( rslt, &valrc, CMPI_uint8);
    _OSBASE_TRACE(1,("--- %s CMPI InvokeMethod() %s exited",_ClassName,methodName));
    CMReturnDone( rslt );
  }
  else {
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_FOUND, methodName ); 
  }

  _OSBASE_TRACE(1,("--- %s CMPI InvokeMethod() exited",_ClassName));
  return rc;
}


/* ---------------------------------------------------------------------------*/
/*                              Provider Factory                              */
/* ---------------------------------------------------------------------------*/

CMInstanceMIStub( OSBase_UnixProcessProvider, 
                  OSBase_UnixProcessProvider, 
                  _broker, 
                  CMNoHook);

CMMethodMIStub( OSBase_UnixProcessProvider,
                OSBase_UnixProcessProvider, 
                _broker, 
                CMNoHook);


/* ---------------------------------------------------------------------------*/
/*                 end of cmpiOSBase_UnixProcessProvider                      */
/* ---------------------------------------------------------------------------*/

