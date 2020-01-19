#ifndef _LINUX_OPERATINGSYSTEMSTATISTICALDATA_H_
#define _LINUX_OPERATINGSYSTEMSTATISTICALDATA_H_

/*
 * Linux_OperatingSystemStatisticalData.h
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
 * Description: 
 * This file defines the interfaces for the resource access implementation 
 * of the CIM class Linux_OperatingSystemStatisticalData.
 * 
*/

/* ---------------------------------------------------------------------------*/

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------------------*/

struct os_statistics {
    unsigned long long user_time;
    unsigned long long system_time;
    unsigned long long wait_time;
    unsigned long long idle_time;
    unsigned long long run_queue_length;
    unsigned long long block_queue_length;
    unsigned long long pages_in;
    unsigned long long pages_out;
};

/**
 * Returns the current values for the following OS properties:
 * CPU user time (msec)
 * CPU system time (msec)
 * CPU wait time (msec)
 * CPU idle time (msec)
 * run queue length
 * block queue length
 * page in rate (absolute amount of pages since startup)
 * page out rate (absolute amount of pages since startup)
 * param stats on success points to a structure with current values
 * return 0 if data is available, 1 if an error occured 
 */
int get_os_statistics(struct os_statistics *stats);

/* ---------------------------------------------------------------------------*/

#ifdef __cplusplus
   }
#endif

/* ---------------------------------------------------------------------------*/

#endif


