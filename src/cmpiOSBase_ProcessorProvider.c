/*
 * cmpiOSBase_ProcessorProvider.c
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

#include "cmpidt.h"
#include "cmpift.h"
#include "cmpimacs.h"

#include "OSBase_Common.h"
#include "cmpiOSBase_Common.h"
#include "cmpiOSBase_Processor.h"
#include "OSBase_Processor.h"

static const CMPIBroker * _broker;

/* ---------------------------------------------------------------------------*/
/* private declarations                                                       */
#ifdef CMPI_VER_100
#define OSBase_ProcessorProviderSetInstance OSBase_ProcessorProviderModifyInstance 
#endif


/* ---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------*/
/*                      Instance Provider Interface                           */
/* ---------------------------------------------------------------------------*/


CMPIStatus OSBase_ProcessorProviderCleanup( CMPIInstanceMI * mi, 
           const CMPIContext * ctx, CMPIBoolean terminate) { 
  _OSBASE_TRACE(1,("--- %s CMPI Cleanup() called",_ClassName));
  proc_cancel_thread();
  _OSBASE_TRACE(1,("--- %s CMPI Cleanup() exited",_ClassName));
  CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_ProcessorProviderEnumInstanceNames( CMPIInstanceMI * mi, 
           const CMPIContext * ctx, 
           const CMPIResult * rslt, 
           const CMPIObjectPath * ref) { 
  CMPIObjectPath       * op    = NULL;  
  CMPIStatus             rc    = {CMPI_RC_OK, NULL};
  struct processorlist * lptr  = NULL ;
  struct processorlist * rm    = NULL ;
  
  _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() called",_ClassName));
  
  if( enum_all_processor( &lptr ) != 0 ) {
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_FAILED, "Could not list processors." ); 
    _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
    return rc;
  }
  
  rm = lptr;
  // iterate process list
  if( lptr->sptr ) {
    for ( ; lptr && rc.rc == CMPI_RC_OK ; lptr = lptr->next) {
      // method call to create the CIMInstance object
      op = _makePath_Processor( _broker, ctx, ref,lptr->sptr, &rc );
      if( op == NULL || rc.rc != CMPI_RC_OK ) {
	if( rc.msg != NULL ) {
	  _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
	}
	CMSetStatusWithChars( _broker, &rc,
			      CMPI_RC_ERR_FAILED, "Transformation from internal structure to CIM ObjectPath failed." ); 
	if(rm) free_processorlist(rm);
	_OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
	return rc; 
      }
      else { CMReturnObjectPath( rslt, op ); }
    }
    if(rm) free_processorlist(rm);
  }
  
  CMReturnDone( rslt );
  _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() exited",_ClassName));
  return rc;
}

CMPIStatus OSBase_ProcessorProviderEnumInstances( CMPIInstanceMI * mi, 
           const CMPIContext * ctx, 
           const CMPIResult * rslt, 
           const CMPIObjectPath * ref, 
           const char ** properties) { 
  CMPIInstance         * ci    = NULL;  
  CMPIStatus             rc    = {CMPI_RC_OK, NULL};
  struct processorlist * lptr  = NULL ;
  struct processorlist * rm    = NULL ;

  _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() called",_ClassName));
  
  if( enum_all_processor( &lptr ) != 0 ) {
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_FAILED, "Could not list processors." ); 
    _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
    free_processorlist ( lptr );
    return rc;
  }

  rm = lptr;
  // iterate process list
  if( lptr->sptr != NULL ) {
    for ( ; lptr && rc.rc == CMPI_RC_OK ; lptr = lptr->next) {
      // method call to create the CIMInstance object
      ci = _makeInst_Processor( _broker, ctx, ref, properties, lptr->sptr, &rc );
      if( ci == NULL || rc.rc != CMPI_RC_OK ) { 
	if( rc.msg != NULL ) {
	  _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
	}
	CMSetStatusWithChars( _broker, &rc,
			      CMPI_RC_ERR_FAILED, "Transformation from internal structure to CIM Instance failed." ); 
	if(rm) free_processorlist(rm);
	_OSBASE_TRACE(1,("--- %s CMPI EnumInstances() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
	return rc; 
      }
      else { CMReturnInstance( rslt, ci ); }
    }
    if(rm) free_processorlist(rm);
  }

  CMReturnDone( rslt );
  _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() exited",_ClassName));
  return rc;
}

CMPIStatus OSBase_ProcessorProviderGetInstance( CMPIInstanceMI * mi, 
           const CMPIContext * ctx, 
           const CMPIResult * rslt, 
           const CMPIObjectPath * cop, 
           const char ** properties) {
  CMPIInstance         * ci    = NULL;
  CMPIString           * name  = NULL;
  struct cim_processor * sptr  = NULL;
  CMPIStatus             rc    = {CMPI_RC_OK, NULL};
  int                    cmdrc = 0;

  _OSBASE_TRACE(1,("--- %s CMPI GetInstance() called",_ClassName));

  _check_system_key_value_pairs( _broker, cop, "SystemCreationClassName", "SystemName", &rc );
  if( rc.rc != CMPI_RC_OK ) { 
    _OSBASE_TRACE(1,("--- %s CMPI GetInstance() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
    return rc; 
  }

  name = CMGetKey( cop, "DeviceID", &rc).value.string;
  if( CMGetCharPtr(name) == NULL ) {    
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_FAILED, "Could not get Processor ID." ); 
    _OSBASE_TRACE(1,("--- %s CMPI GetInstance() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
    return rc;
  }

  cmdrc = get_processor_data( CMGetCharPtr(name) , &sptr );
  if( cmdrc != 0 || sptr == NULL ) {    
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_FOUND, "Processor ID does not exist." ); 
    _OSBASE_TRACE(1,("--- %s CMPI GetInstance() exited : %s",_ClassName,CMGetCharPtr(rc.msg)));
    if(sptr) free_processor(sptr);
    return rc;
  }

  ci = _makeInst_Processor( _broker, ctx, cop, properties, sptr, &rc );
  if(sptr) free_processor(sptr);

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

CMPIStatus OSBase_ProcessorProviderCreateInstance( CMPIInstanceMI * mi, 
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

CMPIStatus OSBase_ProcessorProviderSetInstance( CMPIInstanceMI * mi, 
           const CMPIContext * ctx, 
           const CMPIResult * rslt, 
           const CMPIObjectPath * cop,
           const CMPIInstance * ci, 
           const char **properties) {
  CMPIStatus rc = {CMPI_RC_OK, NULL};

  _OSBASE_TRACE(1,("--- %s CMPI SetInstance() called",_ClassName));

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED" ); 

  _OSBASE_TRACE(1,("--- %s CMPI SetInstance() exited",_ClassName));
  return rc;
}

CMPIStatus OSBase_ProcessorProviderDeleteInstance( CMPIInstanceMI * mi, 
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

CMPIStatus OSBase_ProcessorProviderExecQuery( CMPIInstanceMI * mi, 
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


CMPIStatus OSBase_ProcessorProviderMethodCleanup( CMPIMethodMI * mi, 
           const CMPIContext * ctx, CMPIBoolean terminate) {
  _OSBASE_TRACE(1,("--- %s CMPI MethodCleanup() called",_ClassName));
  _OSBASE_TRACE(1,("--- %s CMPI MethodCleanup() exited",_ClassName));
  CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_ProcessorProviderInvokeMethod( CMPIMethodMI * mi,
           const CMPIContext * ctx,
           const CMPIResult * rslt,
           const CMPIObjectPath * ref,
           const char * methodName,
           const CMPIArgs * in,
           CMPIArgs * out) {
  CMPIString * class = NULL; 
  CMPIStatus   rc    = {CMPI_RC_OK, NULL};

  _OSBASE_TRACE(1,("--- %s CMPI InvokeMethod() called",_ClassName));

  class = CMGetClassName(ref, &rc);

  if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 && 
      strcasecmp("SetPowerState",methodName) == 0 ) {   
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_SUPPORTED, methodName ); 
  }
  else if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 && 
	   strcasecmp("Reset",methodName) == 0 ) {   
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_SUPPORTED, methodName ); 
  }
  else if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 && 
	   strcasecmp("EnableDevice",methodName) == 0 ) {   
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_SUPPORTED, methodName ); 
  }
  else if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 && 
	   strcasecmp("OnlineDevice",methodName) == 0 ) {   
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_SUPPORTED, methodName ); 
  }
  else if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 && 
	   strcasecmp("QuiesceDevice",methodName) == 0 ) {   
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_SUPPORTED, methodName ); 
  }
  else if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 && 
	   strcasecmp("SaveProperties",methodName) == 0 ) {   
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_SUPPORTED, methodName ); 
  }
  else if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 && 
	   strcasecmp("RestoreProperties",methodName) == 0 ) {  
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_NOT_SUPPORTED, methodName ); 
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

CMInstanceMIStub( OSBase_ProcessorProvider, 
                  OSBase_ProcessorProvider, 
                  _broker, 
                  CMNoHook);

CMMethodMIStub( OSBase_ProcessorProvider,
                OSBase_ProcessorProvider, 
                _broker, 
                CMNoHook);


/* ---------------------------------------------------------------------------*/
/*                   end of cmpiOSBase_ProcessorProvider                      */
/* ---------------------------------------------------------------------------*/

