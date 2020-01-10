#ifndef _CMPIOSBASE_OPERATINGSYSTEM_H_
#define _CMPIOSBASE_OPERATINGSYSTEM_H_

/*
 * cmpiOSBase_OperatingSystem.h
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
 *
 * Interface Type : Common Magabeablity Programming Interface ( CMPI )
 *
 * Description:
 * This file defines the interfaces for the factory implementation of the
 * CIM class Linux_OperatingSystem.
 *
*/


/* ---------------------------------------------------------------------------*/

#include "cmpidt.h"

/* ---------------------------------------------------------------------------*/

#ifndef _CMPIOSBASE_UTIL_H_
static char * _ClassName = "Linux_OperatingSystem";
#endif

/* ---------------------------------------------------------------------------*/

/* method to create a CMPIObjectPath of this class                            */

CMPIObjectPath * _makePath_OperatingSystem( const CMPIBroker * _broker,
                 const CMPIContext * ctx, 
                 const CMPIObjectPath * cop,
                 CMPIStatus * rc);


/* method to create a CMPIInstance of this class                              */

CMPIInstance * _makeInst_OperatingSystem( const CMPIBroker * _broker,
               const CMPIContext * ctx, 
               const CMPIObjectPath * cop,
	       const char ** properties,
               CMPIStatus * rc);

/* ---------------------------------------------------------------------------*/

/* Indication support : check OperationalStatus */
#ifndef NOEVENTS
int check_OperationalStatus(int *OperationalStatus);
#endif

/* ---------------------------------------------------------------------------*/

#endif

