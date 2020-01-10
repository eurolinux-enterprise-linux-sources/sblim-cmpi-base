/*
 * cmpiOSBase_OperatingSystemStatisticalData.c
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
 * Contributors: Tyrel Datwyler <tyreld@us.ibm.com>
 *
 * Interface Type : Common Magabeablity Programming Interface ( CMPI )
 *
 * Description:
 * This is the factory implementation for creating instances of CIM
 * class Linux_OperatingSystemStatisticalData.
 *
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "cmpidt.h"
#include "cmpimacs.h"

#include "cmpiOSBase_Common.h"
#include "OSBase_Common.h"

#include "cmpiOSBase_OperatingSystemStatisticalData.h"

/* ---------------------------------------------------------------------------*/
/* private declarations                                                       */


/* ---------------------------------------------------------------------------*/

// stores the time when ResetSelectedStats has been called (function call not yet supported) */
// static unsigned long long last_reset = 0;


/* ---------------------------------------------------------------------------*/
/*                            Factory functions                               */
/* ---------------------------------------------------------------------------*/

/* ---------- method to create a CMPIObjectPath of this class ----------------*/

CMPIObjectPath * _makePath_OperatingSystemStatisticalData( const CMPIBroker * _broker,
                                                           const CMPIContext * ctx, 
                                                           const CMPIObjectPath * ref,
                                                           CMPIStatus * rc) {
    CMPIObjectPath * op = NULL;
    char * instanceID   = NULL;
    char * hostname     = NULL;
    _OSBASE_TRACE(2,("--- _makePath_OperatingSystemStatisticalData() called"));



    if (!get_os_name()) {
        CMSetStatusWithChars(_broker, rc, 
                             CMPI_RC_ERR_FAILED, "no OS name found");
        _OSBASE_TRACE(2,("--- _makePath_OperatingSystemStatisticalData() failed : %s",CMGetCharPtr(rc->msg)));
        goto exit;
    }

    op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(ref,rc)), 
                          _ClassName, rc );

    if (CMIsNullObject(op)) {
        CMSetStatusWithChars( _broker, rc, 
                              CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." ); 
        _OSBASE_TRACE(2,("--- _makePath_OperatingSystemStatisticalData() failed : %s",CMGetCharPtr(rc->msg)));
        goto exit; 
    }

    /* keys are unique identifiers for an instance */
    // instanceID: Linux:<hostname>
    hostname = get_system_name();
    instanceID = calloc(_INSTANCE_PREFIX_LENGTH + strlen(hostname) + 1, sizeof(char));
    strcpy(instanceID, _INSTANCE_PREFIX);
    strcat(instanceID, hostname);
    CMAddKey(op, "InstanceID", instanceID, CMPI_chars);

    if (instanceID) free(instanceID);
    exit:
    _OSBASE_TRACE(2,("--- _makePath_OperatingSystemStatisticalData() exited"));
    return op;                
}

/* ----------- method to create a CMPIInstance of this class ----------------*/

CMPIInstance * _makeInst_OperatingSystemStatisticalData( const CMPIBroker * _broker,
                                                         const CMPIContext * ctx, 
                                                         const CMPIObjectPath * ref,
                                                         const char ** properties,
                                                         CMPIStatus * rc) {
    CMPIObjectPath *  op        = NULL;
    CMPIInstance   *  ci        = NULL;
    const char     ** keys      = NULL;
    int               keyCount  = 0;
    unsigned short    status    = 2;       /* Enabled */
    int stats_avail             = 0;
    CMPIDateTime   *  dt        = NULL;
    char           * instanceID = NULL;
    char           * hostname   = NULL;
    unsigned long long usecs    = 0;


    struct os_statistics statistics;

    _OSBASE_TRACE(2,("--- _makeInst_OperatingSystemStatisticalData() called"));

    stats_avail = get_os_statistics(&statistics);

    if (stats_avail != 0) {
        _OSBASE_TRACE(2,("--- _makeInst_OperatingSystemStatisticalData() get_os_statistics failed"));
        CMSetStatusWithChars(_broker, rc, 
                             CMPI_RC_ERR_FAILED, "no OS statistics available");
        _OSBASE_TRACE(2,("--- _makeInst_OperatingSystemStatisticalData() failed : %s",CMGetCharPtr(rc->msg)));
        goto exit;
    }

    op = CMNewObjectPath( _broker, CMGetCharPtr(CMGetNameSpace(ref,rc)), 
                          _ClassName, rc );

    if (CMIsNullObject(op)) {
        CMSetStatusWithChars( _broker, rc, 
                              CMPI_RC_ERR_FAILED, "Create CMPIObjectPath failed." );
        _OSBASE_TRACE(2,("--- _makeInst_OperatingSystemStatisticalData() failed : %s",CMGetCharPtr(rc->msg))); 
        goto exit; 
    }

    ci = CMNewInstance( _broker, op, rc);
    if (CMIsNullObject(ci)) {
        CMSetStatusWithChars( _broker, rc, 
                              CMPI_RC_ERR_FAILED, "Create CMPIInstance failed." ); 
        _OSBASE_TRACE(2,("--- _makeInst_OperatingSystemStatisticalData() failed : %s",CMGetCharPtr(rc->msg)));
        goto exit; 
    }

    /* set property filter */
    /*
    keys = calloc(3,sizeof(char*));
    keys[0] = strdup("CreationClassName");
    keys[1] = strdup("Name");
    keys[2] = NULL;
    CMSetPropertyFilter(ci,properties,keys);
    for( ;keys[keyCount]!=NULL;keyCount++) { free((char*)keys[keyCount]); }
    free(keys);
    */

    // Linux:<hostname>
    hostname = get_system_name();
    instanceID = calloc(_INSTANCE_PREFIX_LENGTH + strlen(hostname) + 1, sizeof(char));
    strcpy(instanceID, _INSTANCE_PREFIX);
    strcat(instanceID, hostname);
    CMSetProperty(ci, "InstanceID", instanceID, CMPI_chars);

    if (instanceID) free(instanceID);

    /* from CIM_ManagedElement */
    CMSetProperty(ci, "Caption", _ClassName, CMPI_chars);
    CMSetProperty(ci, "Description", "statistical information about operating system", CMPI_chars);
    CMSetProperty(ci, "ElementName", get_os_name(), CMPI_chars);

    /* ResetSelectedStats is not yet supported so StartStatisticTime is always set to boottime */
    usecs = _get_os_boottime();
    usecs *= 1000000;

    /* the following can be used when ResetSelectedStats is supported 
     * (last_reset should be updated in reset function) */
    /*
    if (last_reset == 0) {
        usecs = _get_os_boottime();
        usecs *= 1000000;
        _OSBASE_TRACE(5,("--- _makeInst_OperatingSystemStatisticalData(): 1st run: boottime:%llu us",
                         usecs));

    } else {
        usecs = last_reset;
        _OSBASE_TRACE(5,("--- _makeInst_OperatingSystemStatisticalData(): using last update: %llu us",
                         usecs));
    }
    */

    dt = CMNewDateTimeFromBinary(_broker,(CMPIUint64)usecs,CMPI_false,rc);

    /* from CIM_StatisticalData */
    CMSetProperty(ci, "StartStatisticTime", (CMPIValue*)&(dt), CMPI_dateTime);

    dt = CMNewDateTime(_broker,rc);

    CMSetProperty(ci, "StatisticTime", (CMPIValue*)&(dt), CMPI_dateTime);

    // there's no consistent time interval
    //CMSetProperty(ci, "SampleInterval", NULL, CMPI_uint64);


    /* additional properties */
    CMSetProperty(ci, "CPUUserTime",      (CMPIValue*)&statistics.user_time,          CMPI_uint64);
    CMSetProperty(ci, "CPUSystemTime",    (CMPIValue*)&statistics.system_time,        CMPI_uint64);
    CMSetProperty(ci, "CPUWaitTime",      (CMPIValue*)&statistics.wait_time,          CMPI_uint64);
    CMSetProperty(ci, "CPUIdleTime",      (CMPIValue*)&statistics.idle_time,          CMPI_uint64);
    CMSetProperty(ci, "RunQueueLength",   (CMPIValue*)&statistics.run_queue_length,   CMPI_uint64);
    CMSetProperty(ci, "BlockQueueLength", (CMPIValue*)&statistics.block_queue_length, CMPI_uint64);
    CMSetProperty(ci, "PagesPagedIn",     (CMPIValue*)&statistics.pages_in,           CMPI_uint64);
    CMSetProperty(ci, "PagesPagedOut",    (CMPIValue*)&statistics.pages_out,          CMPI_uint64);

    exit:
    _OSBASE_TRACE(2,("--- _makeInst_OperatingSystemStatisticalData() exited"));
    return ci;
}

/* ---------------------------------------------------------------------------*/
/*          end of cmpiOSBase_OperatingSystemStatisticalData.c                */
/* ---------------------------------------------------------------------------*/

