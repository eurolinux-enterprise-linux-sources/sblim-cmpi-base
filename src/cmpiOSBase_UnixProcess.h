#ifndef _CMPIOSBASE_UNIXPROCESS_H_
#define _CMPIOSBASE_UNIXPROCESS_H_

/*
 * cmpiOSBase_UnixProcess.h
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
 *
 * Interface Type : Common Magabeablity Programming Interface ( CMPI )
 *
 * Description:
 * This file defines the interfaces for the factory implementation of the
 * CIM class Linux_UnixProcess.
 *
*/


/* ---------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "cmpidt.h"
#include "OSBase_UnixProcess.h"

/* ---------------------------------------------------------------------------*/

#ifndef _CMPIOSBASE_UTIL_H_
static char * _ClassName = "Linux_UnixProcess";
#endif

/* ---------------------------------------------------------------------------*/

#ifdef MODULE_SYSMAN

typedef struct _SysMan {
  unsigned long maxChildProc;
  unsigned long maxOpenFiles;
  unsigned long maxRealStack;
} SysMan;

int           moduleInstalled = 0;
static char   pathBuffer[PATH_MAX + 1]; 
static char * dirPath  = "/proc/sysman";
static char * fileName = "pid_rlimit";

int _sysman_data( char * pid , struct _SysMan * sm );

#endif

/* ---------------------------------------------------------------------------*/


/* method to create a CMPIObjectPath of this class                            */

CMPIObjectPath * _makePath_UnixProcess( const CMPIBroker * _broker,
                 const CMPIContext * ctx, 
                 const CMPIObjectPath * cop,
		 struct cim_process * sptr,
                 CMPIStatus * rc);


/* method to create a CMPIInstance of this class                              */

CMPIInstance * _makeInst_UnixProcess( const CMPIBroker * _broker,
               const CMPIContext * ctx, 
               const CMPIObjectPath * cop,
	       const char ** properties,
	       struct cim_process * sptr,
               CMPIStatus * rc);


/* ---------------------------------------------------------------------------*/

#endif

