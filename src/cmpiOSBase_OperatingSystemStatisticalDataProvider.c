/*
 * cmpiOSBase_OperatingSystemStatisticalDataProvider.c
 *
 * (C) Copyright IBM Corp. 2005, 2009
 *
 * THIS FILE IS PROVIDED UNDER THE TERMS OF THE ECLIPSE PUBLIC LICENSE 
 * ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE 
 * CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
 *
 * You can obtain a current copy of the Eclipse Public License from
 * http://www.opensource.org/licenses/eclipse-1.0.php
 *
 * Author:       Michael Schuele <schuelem@de.ibm.com>
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
#include "cmpiOSBase_OperatingSystemStatisticalData.h"

static const CMPIBroker * _broker;


/* ---------------------------------------------------------------------------*/
/* private declarations                                                       */
#ifdef CMPI_VER_100
#define OSBase_OperatingSystemStatisticalDataProviderSetInstance OSBase_OperatingSystemStatisticalDataProviderModifyInstance 
#endif


/* ---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------*/
/*                      Instance Provider Interface                           */
/* ---------------------------------------------------------------------------*/


CMPIStatus OSBase_OperatingSystemStatisticalDataProviderCleanup( CMPIInstanceMI * mi, 
                                                                 const CMPIContext * ctx,
								 CMPIBoolean terminate) { 
    _OSBASE_TRACE(1,("--- %s CMPI Cleanup() called",_ClassName));
    _OSBASE_TRACE(1,("--- %s CMPI Cleanup() exited",_ClassName));

    CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_OperatingSystemStatisticalDataProviderEnumInstanceNames( CMPIInstanceMI * mi, 
                                                                           const CMPIContext * ctx, 
                                                                           const CMPIResult * rslt, 
                                                                           const CMPIObjectPath * ref) { 
    CMPIStatus rc = {CMPI_RC_OK, NULL};
    CMPIObjectPath *op = NULL;

    _OSBASE_TRACE(3,("CMPI EnumInstanceNames() called",_ClassName));

    op = _makePath_OperatingSystemStatisticalData(_broker,ctx,ref,&rc);
    if (op == NULL) {
        if (rc.msg != NULL) {
            _OSBASE_TRACE(1,("--%s CMPI EnumInstanceNames() failed : %s", _ClassName,CMGetCharPtr(rc.msg)));
        } else {
            _OSBASE_TRACE(1,("--%s CMPI EnumInstanceNames() failed", _ClassName));
        }
        return rc;
    }

    CMReturnObjectPath(rslt, op);

    CMReturnDone(rslt);

    _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() exited",_ClassName));
    return rc;
}

CMPIStatus OSBase_OperatingSystemStatisticalDataProviderEnumInstances( CMPIInstanceMI * mi, 
                                                                       const CMPIContext * ctx, 
                                                                       const CMPIResult * rslt, 
                                                                       const CMPIObjectPath * ref, 
                                                                       const char ** properties) { 
    CMPIStatus rc = {CMPI_RC_OK, NULL};
    const CMPIInstance * ci  = NULL;

    _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() called",_ClassName));

    ci = _makeInst_OperatingSystemStatisticalData(_broker, ctx, ref, properties, &rc);

    if (ci == NULL) {
        if (rc.msg != NULL) {
            _OSBASE_TRACE(1,("--%s CMPI EnumInstances() failed : %s", _ClassName,CMGetCharPtr(rc.msg)));
        } else {
            _OSBASE_TRACE(1,("--%s CMPI EnumInstances() failed", _ClassName));
        }
        return rc;
    }

    CMReturnInstance(rslt, ci);

    CMReturnDone( rslt );
    _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() exited",_ClassName));
    return rc;
}

CMPIStatus OSBase_OperatingSystemStatisticalDataProviderGetInstance( CMPIInstanceMI * mi, 
                                                                     const CMPIContext * ctx, 
                                                                     const CMPIResult * rslt, 
                                                                     const CMPIObjectPath * cop, 
                                                                     const char ** properties) {
    const CMPIInstance * ci = NULL;
    CMPIStatus     rc = {CMPI_RC_OK, NULL};

    _OSBASE_TRACE(1,("--- %s CMPI GetInstance() called",_ClassName));

    ci = _makeInst_OperatingSystemStatisticalData(_broker, ctx, cop, properties, &rc);

    if (ci == NULL) {
        if (rc.msg != NULL) {
            _OSBASE_TRACE(1,("--%s CMPI GetInstance() failed : %s", _ClassName,CMGetCharPtr(rc.msg)));
        } else {
            _OSBASE_TRACE(1,("--%s CMPI GetInstance() failed", _ClassName));
        }
        return rc;
    }


    CMReturnInstance( rslt, ci );
    CMReturnDone(rslt);
    _OSBASE_TRACE(1,("--- %s CMPI GetInstance() exited",_ClassName));
    return rc;
}

CMPIStatus OSBase_OperatingSystemStatisticalDataProviderCreateInstance( CMPIInstanceMI * mi, 
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

CMPIStatus OSBase_OperatingSystemStatisticalDataProviderSetInstance( CMPIInstanceMI * mi, 
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

CMPIStatus OSBase_OperatingSystemStatisticalDataProviderDeleteInstance( CMPIInstanceMI * mi, 
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

CMPIStatus OSBase_OperatingSystemStatisticalDataProviderExecQuery( CMPIInstanceMI * mi, 
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


CMPIStatus OSBase_OperatingSystemStatisticalDataProviderMethodCleanup( CMPIMethodMI * mi, 
                                                                       const CMPIContext * ctx,
								       CMPIBoolean terminate) {
  _OSBASE_TRACE(1,("--- %s CMPI MethodCleanup() called",_ClassName));
  _OSBASE_TRACE(1,("--- %s CMPI MethodCleanup() exited",_ClassName));
  CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_OperatingSystemStatisticalDataProviderInvokeMethod( CMPIMethodMI * mi,
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
      strcasecmp("ResetSelectedStats",methodName) == 0 ) {  
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

CMInstanceMIStub( OSBase_OperatingSystemStatisticalDataProvider, 
                  OSBase_OperatingSystemStatisticalDataProvider, 
                  _broker, 
                  CMNoHook);

CMMethodMIStub( OSBase_OperatingSystemStatisticalDataProvider,
                OSBase_OperatingSystemStatisticalDataProvider, 
                _broker, 
                CMNoHook);

/* ---------------------------------------------------------------------------*/
/*          end of cmpiOSBase_OperatingSystemStatisticalDataProvider          */
/* ---------------------------------------------------------------------------*/

