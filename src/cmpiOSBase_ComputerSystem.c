/*
 * cmpiOSBase_ComputerSystem.c
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
 * class Linux_ComputerSystem.
 *
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmpidt.h"
#include "cmpimacs.h"

#include "cmpiOSBase_Common.h"
#include "OSBase_Common.h"

#include "cmpiOSBase_ComputerSystem.h"
#include "OSBase_ComputerSystem.h"


/* ---------------------------------------------------------------------------*/
/* private declarations                                                       */

/* ---------------------------------------------------------------------------*/



/* ---------------------------------------------------------------------------*/
/*                            Factory functions                               */
/* ---------------------------------------------------------------------------*/

/* ---------- method to create a CMPIObjectPath of this class ----------------*/

CMPIObjectPath * _makePath_ComputerSystem( const CMPIBroker * _broker,
		 const CMPIContext * ctx, 
	         const CMPIObjectPath * ref,
	         CMPIStatus * rc) {
  CMPIObjectPath * op = NULL;
 
  _OSBASE_TRACE(2,("--- _makePath_ComputerSystem() called"));

  /* the sblim-cmpi-base package offers some tool methods to get common
   * system data 
  */
  if( !get_system_name() ) {   
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "no host name found" );
    _OSBASE_TRACE(2,("--- _makePath_ComputerSystem() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }

  op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(ref,rc)), 
			_ClassName, rc );
  if( CMIsNullObject(op) ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." ); 
    _OSBASE_TRACE(2,("--- _makePath_ComputerSystem() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit; 
  }

  CMAddKey(op, "CreationClassName", _ClassName, CMPI_chars);
  CMAddKey(op, "Name", get_system_name(), CMPI_chars);
    
 exit:
  _OSBASE_TRACE(2,("--- _makePath_ComputerSystem() exited"));
  return op;                
}

/* ----------- method to create a CMPIInstance of this class ----------------*/

CMPIInstance * _makeInst_ComputerSystem( const CMPIBroker * _broker,
	       const CMPIContext * ctx, 
	       const CMPIObjectPath * ref,
	       const char ** properties,
	       CMPIStatus * rc) {
  CMPIObjectPath *  op        = NULL;
  CMPIInstance   *  ci        = NULL;
  char           *  owner     = NULL;
  char           *  contact   = NULL;
  char              lparid[255];
  const char     ** keys      = NULL;
  int               keyCount  = 0;
#ifndef CIM26COMPAT
  CMPIArray      *  dedic     = NULL;
  unsigned short    status    = 2; /* Enabled */
  unsigned short    dedicated = 0; /* Not Dedicated */
#endif

  _OSBASE_TRACE(2,("--- _makeInst_ComputerSystem() called"));

  /* the sblim-cmpi-base package offers some tool methods to get common
   * system data
  */
  if( !get_system_name() ) {   
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "no host name found" );
    _OSBASE_TRACE(2,("--- _makeInst_ComputerSystem() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }

  op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(ref,rc)), 
			_ClassName, rc );
  if( CMIsNullObject(op) ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." ); 
    _OSBASE_TRACE(2,("--- _makeInst_ComputerSystem() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit; 
  }

  ci = CMNewInstance( _broker, op, rc);
  if( CMIsNullObject(ci) ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "Create CMPIInstance failed." ); 
    _OSBASE_TRACE(2,("--- _makeInst_ComputerSystem() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit; 
  }

  /* set property filter */
  keys = calloc(3,sizeof(char*));
  keys[0] = strdup("CreationClassName");
  keys[1] = strdup("Name");
  CMSetPropertyFilter(ci,properties,keys);
  for( ;keys[keyCount]!=NULL;keyCount++) { free((char*)keys[keyCount]); }
  free(keys);

  CMSetProperty( ci, "CreationClassName", _ClassName, CMPI_chars );
  CMSetProperty( ci, "Name", get_system_name(), CMPI_chars );
  CMSetProperty( ci, "Status", "NULL", CMPI_chars);
  CMSetProperty( ci, "NameFormat", "IP", CMPI_chars);
  CMSetProperty( ci, "Caption", "Computer System", CMPI_chars);
  CMSetProperty( ci, "Description", "A class derived from ComputerSystem that represents the single node container of the Linux OS.", CMPI_chars);

  if( (owner=get_cs_primownername()) != NULL ) {
    CMSetProperty( ci, "PrimaryOwnerName", owner, CMPI_chars);
    free(owner);
  }
  if( (contact=get_cs_primownercontact()) != NULL) {
    CMSetProperty( ci, "PrimaryOwnerContact", contact, CMPI_chars);
    free(contact);
  }

  if(get_cs_lparid(lparid, sizeof(lparid)) == 0) {
    CMSetProperty( ci, "LPARID", lparid, CMPI_chars);
  }

  /* 2.7 */
#ifndef CIM26COMPAT
  CMSetProperty( ci, "ElementName", get_system_name(), CMPI_chars);
  CMSetProperty( ci, "EnabledState", (CMPIValue*)&(status), CMPI_uint16);
  CMSetProperty( ci, "OtherEnabledState", "NULL", CMPI_chars);
  CMSetProperty( ci, "RequestedState", (CMPIValue*)&(status), CMPI_uint16);
  CMSetProperty( ci, "EnabledDefault", (CMPIValue*)&(status), CMPI_uint16);

  dedic = CMNewArray(_broker,1,CMPI_uint16,rc);
  if(dedic == NULL) {
    CMSetStatusWithChars( _broker, rc,
			  CMPI_RC_ERR_FAILED, "Create Array for Property Dedicated failed." );
    _OSBASE_TRACE(2,("--- _makeInst_ComputerSystem() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }
  else {
    CMSetArrayElementAt(dedic,0,(CMPIValue*)&(dedicated),CMPI_uint16);
    CMSetProperty( ci, "Dedicated", (CMPIValue*)&(dedic), CMPI_uint16A);
  }

#endif

 exit:
  _OSBASE_TRACE(2,("--- _makeInst_ComputerSystem() exited"));
  return ci;
}



/* ---------------------------------------------------------------------------*/
/*                  end of cmpiOSBase_ComputerSystem.c                        */
/* ---------------------------------------------------------------------------*/

