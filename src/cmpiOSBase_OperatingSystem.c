/*
 * cmpiOSBase_OperatingSystem.c
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
 *               Tyrel Datwyler <tyreld@us.ibm.com>
 *
 * Interface Type : Common Magabeablity Programming Interface ( CMPI )
 *
 * Description:
 * This is the factory implementation for creating instances of CIM
 * class Linux_OperatingSystem.
 *
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "cmpidt.h"
#include "cmpimacs.h"

#include "cmpiOSBase_Common.h"
#include "OSBase_Common.h"

#include "cmpiOSBase_OperatingSystem.h"
#include "OSBase_OperatingSystem.h"


/* ---------------------------------------------------------------------------*/
/* private declarations                                                       */

/* ---------------------------------------------------------------------------*/

static CMPIInstance * _makeOS( const CMPIBroker * _broker,
                               const CMPIObjectPath * ref,
			       const char ** properties,
                               struct cim_operatingsystem * sptr,
                               CMPIStatus * rc);

/* ---------------------------------------------------------------------------*/
/* --- CPU utilization                                                        */
/* ---------------------------------------------------------------------------*/

typedef struct _CpuSample {
  unsigned long cpu;
  unsigned long total;
} CpuSample;

static int getcpu(CpuSample * cps);

static int getpctcpu(CpuSample * cps);

/* ---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------*/
/*                            Factory functions                               */
/* ---------------------------------------------------------------------------*/

/* ---------- method to create a CMPIObjectPath of this class ----------------*/

CMPIObjectPath * _makePath_OperatingSystem( const CMPIBroker * _broker,
	         const CMPIContext * ctx,  
                 const CMPIObjectPath * ref,
	         CMPIStatus * rc) {
  CMPIObjectPath * op = NULL;
 
  _OSBASE_TRACE(2,("--- _makePath_OperatingSystem() called"));

  /* the sblim-cmpi-base package offers some tool methods to get common
   * system data 
  */
  if( !get_system_name() ) {   
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "no host name found" );
    _OSBASE_TRACE(2,("--- _makePath_OperatingSystem() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }

  if( !get_os_name() ) {
    CMSetStatusWithChars( _broker, rc,
			  CMPI_RC_ERR_FAILED, "no OS name found" );
    _OSBASE_TRACE(2,("--- _makePath_OperatingSystem() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }

  op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(ref,rc)), 
			_ClassName, rc );
  if( CMIsNullObject(op) ) { 
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." ); 
    _OSBASE_TRACE(2,("--- _makePath_OperatingSystem() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit; 
  }

  CMAddKey(op, "CSCreationClassName", CSCreationClassName, CMPI_chars);
  CMAddKey(op, "CSName", get_system_name(), CMPI_chars);
  CMAddKey(op, "CreationClassName", _ClassName, CMPI_chars);
  CMAddKey(op, "Name", get_os_name(), CMPI_chars);

 exit:
  _OSBASE_TRACE(2,("--- _makePath_OperatingSystem() exited"));
  return op;                
}

/* ----------- method to create a CMPIInstance of this class ----------------*/


CMPIInstance * _makeInst_OperatingSystem( const CMPIBroker * _broker,
	       const CMPIContext * ctx, 
               const CMPIObjectPath * ref,
	       const char ** properties,
               CMPIStatus * rc) {
  CMPIInstance               * ci   = NULL;
  struct cim_operatingsystem * sptr = NULL;
  int                          frc  = 0;

  _OSBASE_TRACE(2,("--- _makeInst_OperatingSystem() called"));

  frc = get_operatingsystem_data(&sptr);
  if (frc==0)
    ci = _makeOS( _broker, ref, properties, sptr, rc );
  else {
    CMSetStatusWithChars( _broker, rc,
			  CMPI_RC_ERR_FAILED, "Could not get OS Data." );
    _OSBASE_TRACE(2,("--- _makeInst_OperatingSystem() failed : %s",CMGetCharPtr(rc->msg)));
  }

  free_os_data(sptr);

  _OSBASE_TRACE(2,("--- _makeInst_OperatingSystem() exited"));
  return ci;
}

static int getcpu(CpuSample * cps)
{
  unsigned long user, system, nice, idle;
  FILE * fcpuinfo = fopen("/proc/stat","r");
  if (cps && fcpuinfo) {
    fscanf(fcpuinfo,"%*s %ld %ld %ld %ld",&user,&nice,&system,&idle);
    fclose(fcpuinfo);
    cps->total=user+system+idle;
    cps->cpu=cps->total - idle;
    return 0;
  }
  else {
    return -1;
  }
}

static int getpctcpu(CpuSample * cps)
{
  static CpuSample ocps = {0,0};
  int  pctcpu = 0;
  static pthread_mutex_t cpumux = PTHREAD_MUTEX_INITIALIZER;
  if (cps) {
    if (cps->total == ocps.total) {
      if (cps->cpu == ocps.cpu) {
	pctcpu = 0;
      } else {
	pctcpu = 100;
      }
    } else {
      pctcpu = (100*(cps->cpu-ocps.cpu))/(cps->total-ocps.total);
    }
    ocps.total = cps->total;
    ocps.cpu = cps->cpu;
  }
  pthread_mutex_unlock(&cpumux);
  return pctcpu;
}

static CMPIInstance * _makeOS( const CMPIBroker * _broker,
                               const CMPIObjectPath * ref,
			       const char ** properties,
                               struct cim_operatingsystem * sptr,
                               CMPIStatus * rc) {
  CMPIObjectPath *   op        = NULL;
  CMPIInstance   *   ci        = NULL;
  CMPIDateTime   *   dt        = NULL;
  CpuSample          cs;
  const char     **  keys      = NULL;
  int                keyCount  = 0;
  unsigned long long totalSwap = 0;
  unsigned short     pctcpu    = 0;
#ifndef CIM26COMPAT
  CMPIArray      *   opstat    = NULL;
  unsigned short     status    = 2; /* Enabled */
  unsigned short     opstatval = 2; /* 2 ... OK ; 4 ... Stressed */
#endif

  _OSBASE_TRACE(2,("--- _makeOS() called"));

  /* the sblim-cmpi-base package offers some tool methods to get common
   * system data 
  */
  if( !get_system_name() ) {   
    CMSetStatusWithChars( _broker, rc, 
			  CMPI_RC_ERR_FAILED, "no host name found" );
    _OSBASE_TRACE(2,("--- _makeOS() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }

  if( !get_os_name() ) {
    CMSetStatusWithChars( _broker, rc,
			  CMPI_RC_ERR_FAILED, "no OS name found" );
    _OSBASE_TRACE(2,("--- _makeOS() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }

  op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(ref,rc)),
			_ClassName, rc );
  if( CMIsNullObject(op) ) {
    CMSetStatusWithChars( _broker, rc,
			  CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." );
    _OSBASE_TRACE(2,("--- _makeOS() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }

  ci = CMNewInstance( _broker, op, rc);
  if( CMIsNullObject(ci) ) {
    CMSetStatusWithChars( _broker, rc,
			  CMPI_RC_ERR_FAILED, "Create CMPIInstance failed." );
    _OSBASE_TRACE(2,("--- _makeOS() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }

  /* set property filter */
  keys = calloc(5,sizeof(char*));
  keys[0] = strdup("CSCreationClassName");
  keys[1] = strdup("CSName");
  keys[2] = strdup("CreationClassName");
  keys[3] = strdup("Name");
  CMSetPropertyFilter(ci,properties,keys);
  for( ;keys[keyCount]!=NULL;keyCount++) { free((char*)keys[keyCount]); }
  free(keys);

  /* calculate cpu percentage */
  if(getcpu(&cs) == 0) { pctcpu = getpctcpu(&cs); }

  CMSetProperty( ci, "CSCreationClassName", CSCreationClassName, CMPI_chars );
  CMSetProperty( ci, "CSName", get_system_name(), CMPI_chars );
  CMSetProperty( ci, "CreationClassName", _ClassName, CMPI_chars );
  CMSetProperty( ci, "Name", get_os_name(), CMPI_chars );

  CMSetProperty( ci, "Status", "NULL", CMPI_chars);
  CMSetProperty( ci, "Caption", "Operating System", CMPI_chars);
  CMSetProperty( ci, "Description", "A class derived from OperatingSystem to represents the running Linux OS.", CMPI_chars);

  CMSetProperty( ci, "OSType", (CMPIValue*)&(sptr->osType), CMPI_uint16);
  CMSetProperty( ci, "OtherTypeDescription", "NULL", CMPI_chars);

  CMSetProperty( ci, "Version", sptr->version, CMPI_chars);

  CMSetProperty( ci, "NumberOfUsers", (CMPIValue*)&(sptr->numOfUsers), CMPI_uint32);
  CMSetProperty( ci, "NumberOfProcesses", (CMPIValue*)&(sptr->numOfProcesses), CMPI_uint32);
  CMSetProperty( ci, "MaxProcessesPerUser", (CMPIValue*)&(sptr->maxNumOfProc), CMPI_uint32);
  CMSetProperty( ci, "MaxNumberOfProcesses", (CMPIValue*)&(sptr->maxNumOfProc), CMPI_uint32);
  CMSetProperty( ci, "MaxProcessMemorySize", (CMPIValue*)&(sptr->maxProcMemSize), CMPI_uint64);
  CMSetProperty( ci, "PctTotalCPUTime", (CMPIValue*)&pctcpu, CMPI_uint16);

  CMSetProperty( ci, "Distributed",(CMPIValue*)&CMPI_false, CMPI_boolean);
  CMSetProperty( ci, "TotalSwapSpaceSize",(CMPIValue*)&(sptr->totalSwapSize), CMPI_uint64);

  CMSetProperty( ci, "TotalVirtualMemorySize", (CMPIValue*)&(sptr->totalVirtMem), CMPI_uint64);
  CMSetProperty( ci, "FreeVirtualMemory", (CMPIValue*)&(sptr->freeVirtMem), CMPI_uint64);
  CMSetProperty( ci, "TotalVisibleMemorySize", (CMPIValue*)&(sptr->totalPhysMem), CMPI_uint64);
  CMSetProperty( ci, "FreePhysicalMemory", (CMPIValue*)&(sptr->freePhysMem), CMPI_uint64);
  CMSetProperty( ci, "SizeStoredInPagingFiles", (CMPIValue*)&(sptr->totalSwapMem), CMPI_uint64);
  CMSetProperty( ci, "FreeSpaceInPagingFiles", (CMPIValue*)&(sptr->freeSwapMem), CMPI_uint64);

  CMSetProperty( ci, "CurrentTimeZone", (CMPIValue*)&(sptr->curTimeZone), CMPI_sint16);

  if( sptr->localDate != NULL ) {
    dt = CMNewDateTimeFromChars(_broker,sptr->localDate,rc);
    if(dt == NULL) {
      CMSetStatusWithChars( _broker, rc,
			    CMPI_RC_ERR_FAILED, 
			    "CMNewDateTimeFromChars for property LocalDateTime failed." );
      _OSBASE_TRACE(2,("--- _makeOS() failed : %s",CMGetCharPtr(rc->msg)));
      goto exit;
    }
    else { CMSetProperty( ci, "LocalDateTime", (CMPIValue*)&(dt), CMPI_dateTime); }
  }

  if( sptr->installDate != NULL ) {
    dt = CMNewDateTimeFromChars(_broker,sptr->installDate,rc);
    if(dt == NULL) {
      CMSetStatusWithChars( _broker, rc,
			    CMPI_RC_ERR_FAILED, 
			    "CMNewDateTimeFromChars for property InstallDate failed." );
      _OSBASE_TRACE(2,("--- _makeOS() failed : %s",CMGetCharPtr(rc->msg)));
      goto exit;
    }
    else { CMSetProperty( ci, "InstallDate", (CMPIValue*)&(dt), CMPI_dateTime); }
  }

  if( sptr->lastBootUp != NULL ) {
    dt = CMNewDateTimeFromChars(_broker,sptr->lastBootUp,rc);    
    if(dt == NULL) {
      CMSetStatusWithChars( _broker, rc,
			    CMPI_RC_ERR_FAILED, 
			    "CMNewDateTimeFromChars for property LastBootUpTime failed." );
      _OSBASE_TRACE(2,("--- _makeOS() failed : %s",CMGetCharPtr(rc->msg)));
      goto exit;
    }
    else { CMSetProperty( ci, "LastBootUpTime", (CMPIValue*)&(dt), CMPI_dateTime); }
  }

  CMSetProperty( ci, "CodeSet", sptr->codeSet, CMPI_chars);
  CMSetProperty( ci, "LanguageEdition", sptr->langEd, CMPI_chars);

  CMSetProperty( ci, "DefaultPageSize", (CMPIValue*)&(sptr->defPageSize), CMPI_uint32);
  CMSetProperty( ci, "NumberOfLicensedUsers", (CMPIValue*)&(sptr->licensedUsers), CMPI_uint32);

  /* 2.7 */
#ifndef CIM26COMPAT
  opstat = CMNewArray(_broker,1,CMPI_uint16,rc);
  if( opstat == NULL ) {
    CMSetStatusWithChars( _broker, rc,
			  CMPI_RC_ERR_FAILED, "CMNewArray(_broker,1,CMPI_uint16,rc)" );
    _OSBASE_TRACE(2,("--- _makeOS() failed : %s",CMGetCharPtr(rc->msg)));
    goto exit;
  }
  else {
    if( pctcpu >= 90 ) { opstatval = 4; }
    CMSetArrayElementAt(opstat,0,(CMPIValue*)&(opstatval),CMPI_uint16);
    CMSetProperty( ci, "OperationalStatus", (CMPIValue*)&(opstat), CMPI_uint16A);
  }

  CMSetProperty( ci, "ElementName", CIM_OS_DISTRO, CMPI_chars);  
  CMSetProperty( ci, "EnabledState", (CMPIValue*)&(status), CMPI_uint16);
  CMSetProperty( ci, "OtherEnabledState", "NULL", CMPI_chars);
  CMSetProperty( ci, "RequestedState", (CMPIValue*)&(status), CMPI_uint16);
  CMSetProperty( ci, "EnabledDefault", (CMPIValue*)&(status), CMPI_uint16);
  CMSetProperty( ci, "HealthState", (CMPIValue*)&(sptr->healthState), CMPI_uint16);

#endif

 exit:
  _OSBASE_TRACE(2,("--- _makeOS() exited"));
  return ci;
}

/* ---------------------------------------------------------------------------*/
/* Indication support                                                         */
/* ---------------------------------------------------------------------------*/

#ifndef NOEVENTS

int check_OperationalStatus(int *OperationalStatus) {
  CpuSample      cs;
  unsigned short pctcpu = 0;

  if(getcpu(&cs) == 0) {
    pctcpu = getpctcpu(&cs);
    _OSBASE_TRACE(2,("--- _check_OperationalStatus(): TotalCPUTimePct %d",pctcpu));
    if(pctcpu>=90 && *OperationalStatus!=4) {
      *OperationalStatus = 4;
      return 1;
    }
    if(pctcpu<90 && *OperationalStatus==4) {
      *OperationalStatus = 2;
      return 1;
    }
  }
  return 0;
}

#endif

/* ---------------------------------------------------------------------------*/
/*                   end of cmpiOSBase_OperatingSystem.c                      */
/* ---------------------------------------------------------------------------*/

