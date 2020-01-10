/*
 * cmpiOSBase_Processor.c
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
 * Contributors: Tyrel Datwyler <tyreld@us.ibm.com>
 *
 * Interface Type : Common Magabeablity Programming Interface ( CMPI )
 *
 * Description:
 * This is the factory implementation for creating instances of CIM
 * class Linux_Processor.
 *
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmpidt.h"
#include "cmpimacs.h"

#include "cmpiOSBase_Common.h"
#include "OSBase_Common.h"

#include "cmpiOSBase_Processor.h"
#include "OSBase_Processor.h"


/* ---------------------------------------------------------------------------*/
/* private declarations                                                       */


/* ---------------------------------------------------------------------------*/



/* ---------------------------------------------------------------------------*/
/*                            Factory functions                               */
/* ---------------------------------------------------------------------------*/

/* ---------- method to create a CMPIObjectPath of this class ----------------*/

CMPIObjectPath * _makePath_Processor( const CMPIBroker * _broker,
                 const CMPIContext * ctx, 
                 const CMPIObjectPath * ref,
		 struct cim_processor * sptr,
		 CMPIStatus * rc) {
  CMPIObjectPath * op = NULL;
 
  _OSBASE_TRACE(2,("--- _makePath_Processor() called"));

  /* the sblim-cmpi-base package offers some tool methods to get common
   * system data
  */ 
  if( !get_system_name() ) {   
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "no host name found" );
    _OSBASE_TRACE(2,("--- _makePath_Processor() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }

  op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(ref,rc)), 
			_ClassName, rc );
  if( CMIsNullObject(op) ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." ); 
    _OSBASE_TRACE(2,("--- _makePath_Processor() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit; 
  }

  CMAddKey(op, "SystemCreationClassName", CSCreationClassName, CMPI_chars);  
  CMAddKey(op, "SystemName", get_system_name(), CMPI_chars);
  CMAddKey(op, "CreationClassName", _ClassName, CMPI_chars);  
  CMAddKey(op, "DeviceID", sptr->id, CMPI_chars);
    
 exit:
  _OSBASE_TRACE(2,("--- _makePath_Processor() exited"));
  return op;                
}

/* ----------- method to create a CMPIInstance of this class ----------------*/

CMPIInstance * _makeInst_Processor( const CMPIBroker * _broker,
               const CMPIContext * ctx, 
               const CMPIObjectPath * ref,
	       const char ** properties,
	       struct cim_processor * sptr,
	       CMPIStatus * rc) {
  CMPIObjectPath *  op       = NULL;
  CMPIInstance   *  ci       = NULL;
  const char     ** keys     = NULL;
  int               keyCount = 0;
#ifndef CIM26COMPAT
  unsigned short    status   = 2; /* Enabled */
#endif

  _OSBASE_TRACE(2,("--- _makeInst_Processor() called"));

  /* the sblim-cmpi-base package offers some tool methods to get common
   * system data
  */
  if( !get_system_name() ) {   
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "no host name found" );
    _OSBASE_TRACE(2,("--- _makeInst_Processor() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }

  op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(ref,rc)), 
			_ClassName, rc );
  if( CMIsNullObject(op) ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." ); 
    _OSBASE_TRACE(2,("--- _makeInst_Processor() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit; 
  }

  ci = CMNewInstance( _broker, op, rc);
  if( CMIsNullObject(ci) ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "Create CMPIInstance failed." ); 
    _OSBASE_TRACE(2,("--- _makeInst_Processor() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit; 
  }

  /* set property filter */
  keys = calloc(5,sizeof(char*));
  keys[0] = strdup("SystemCreationClassName");
  keys[1] = strdup("SystemName");
  keys[2] = strdup("CreationClassName");
  keys[3] = strdup("DeviceID");
  CMSetPropertyFilter(ci,properties,keys);
  for( ;keys[keyCount]!=NULL;keyCount++) { free((char*)keys[keyCount]); }
  free(keys);
      
  CMSetProperty( ci, "SystemCreationClassName",CSCreationClassName , CMPI_chars ); 
  CMSetProperty( ci, "SystemName", get_system_name(), CMPI_chars );
  CMSetProperty( ci, "CreationClassName", _ClassName, CMPI_chars ); 
  CMSetProperty( ci, "DeviceID", sptr->id, CMPI_chars );

  CMSetProperty( ci, "Caption", "Linux Processor", CMPI_chars);
  CMSetProperty( ci, "Description", "This class represents instances of available processors.", CMPI_chars);
  CMSetProperty( ci, "Status", "NULL", CMPI_chars);

  CMSetProperty( ci, "Role", "Central Processor", CMPI_chars);  
  CMSetProperty( ci, "CPUStatus", (CMPIValue*)&(sptr->stat), CMPI_uint16);
  CMSetProperty( ci, "LoadPercentage", (CMPIValue*)&(sptr->loadPct), CMPI_uint16);
  CMSetProperty( ci, "Stepping", sptr->step, CMPI_chars);
  CMSetProperty( ci, "Family", (CMPIValue*)&(sptr->family), CMPI_uint16);
  CMSetProperty( ci, "OtherFamilyDescription", "NULL", CMPI_chars);

  if (sptr->maxClockSpeed > 0) {
    CMSetProperty( ci, "MaxClockSpeed", (CMPIValue*)&(sptr->maxClockSpeed), CMPI_uint32);
  }
  if (sptr->curClockSpeed > 0) {
    CMSetProperty( ci, "CurrentClockSpeed", (CMPIValue*)&(sptr->curClockSpeed), CMPI_uint32);
  }
  CMSetProperty( ci, "Name", sptr->id, CMPI_chars );

  /* 2.7 */
#ifndef CIM26COMPAT
  CMSetProperty( ci, "ElementName", sptr->name, CMPI_chars);  
  CMSetProperty( ci, "EnabledState", (CMPIValue*)&(status), CMPI_uint16);
  CMSetProperty( ci, "OtherEnabledState", "NULL", CMPI_chars);
  CMSetProperty( ci, "RequestedState", (CMPIValue*)&(status), CMPI_uint16);
  CMSetProperty( ci, "EnabledDefault", (CMPIValue*)&(status), CMPI_uint16);
#endif

 exit:
  _OSBASE_TRACE(2,("--- _makeInst_Processor() exited"));
  return ci;
}



/* ---------------------------------------------------------------------------*/
/*                      end of cmpiOSBase_Processor.c                         */
/* ---------------------------------------------------------------------------*/

