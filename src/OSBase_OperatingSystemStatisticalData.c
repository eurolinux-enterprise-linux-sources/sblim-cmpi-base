/*
 * Linux_OperatingSystemStatisticalData.c
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
 * This shared library provides resource access functionality for the class
 * Linux_OperatingSystemStatisticalData. 
 * It is independent from any specific CIM technology.
*/

/* ---------------------------------------------------------------------------*/

#include "OSBase_OperatingSystemStatisticalData.h"
#include "OSBase_Common.h" /* logging */

#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/utsname.h>

#define PROC_DIR "/proc"
#define DATA_FILE "/proc/stat"
#define PAGE_FILE_26 "/proc/vmstat"

/* ---------------------------------------------------------------------------*/
// private declarations


/* ---------------------------------------------------------------------------*/

/**
 * Checks if the currently running kernel is of version 2.6 
 * @return 1 if kernel is of version 2.6, 0 otherwise
 */
int is_kernel26() {

    int res = 0;
    _OSBASE_TRACE(3,("is_kernel26() called"));

    res = kernel_release() >= 26000;
    _OSBASE_TRACE(4,("is_kernel26(): %d",res));
    _OSBASE_TRACE(3,("is_kernel26() exited"));

    return res;
}

/**
 * Updates the field stats->wait_time supposing that the 
 * currently running kernel is of version 2.4.
 * @param stats the structure whose field will be updated
 * @return 1 on success, 0 otherwise
 */
int get_wait_time_24(struct os_statistics *stats) {

    int res = 0;
    double uptime = 0;
    FILE *f = NULL;

    _OSBASE_TRACE(3,("get_wait_time_24() called"));

    if ((f = fopen("/proc/uptime","r")) != NULL) {
        fscanf(f,"%lf",&uptime);
        stats->wait_time = 0;
        //stats->wait_time = uptime*1000 - stats->user_time - stats->system_time - stats->idle_time;
        fclose(f);
        res = 1;
    } else {
        _OSBASE_TRACE(1,("get_queue_info: could not open /proc/uptime: %s",strerror(errno)));
    }
    _OSBASE_TRACE(4,("get_wait_time_24() res: %d", res));
    _OSBASE_TRACE(3,("get_wait_time_24() exited"));
    return res;
}

/**
 * WARNING: block_queue_length is currently not supported and will be set to zero!
 * Updates the fields stats->run_queue_length and stats->block_queue_length
 * supposing that the currently running kernel is of version 2.4.
 * @param stats the structure whose fields will be updated. 
 * @return 1 on success, 0 otherwise
 */
int get_queue_info_24(struct os_statistics *stats) {

    int res = 0;
    FILE *f = NULL;
    char buf[127];
    char *strptr = NULL;

    _OSBASE_TRACE(3,("get_queue_info_24() called"));

    stats->run_queue_length = 0;

    /**
     * /proc/loadavg 
     * 0.11 0.10 0.09 <running_processes>/111 26002
     *
     */ 
    if ((f = fopen("/proc/loadavg","r")) != NULL) {
        fgets(buf,127,f);
        strptr = strstr(buf,"/");
        *strptr = '\0';
        while (*strptr != ' ') {
            strptr--;
        }
        stats->run_queue_length = atoll(strptr);
        fclose(f);
        res = 1;
    } else {
        _OSBASE_TRACE(1,("get_queue_info: could not open /proc/loadavg: %s",strerror(errno)));
    }
#warning block_queue_length for kernel 2.4 not supported

    // not supported
    stats->block_queue_length = 0;
    _OSBASE_TRACE(4,("get_queue_info_24() res: %d",res));
    _OSBASE_TRACE(3,("get_queue_info_24() exited"));
    return res;
}

/**
 * Updates the fields stats->user_time, stats->system_time, stats->idle_time,
 * stats->pages_in and stats->pages_out supposing that the 
 * currently running kernel is of version 2.4.
 * @param stats the structure whose fields will be updated.
 * @return 1 on success, 0 otherwise
 */
int get_cpu_page_data_24(struct os_statistics *stats) {

    int res = 0;
    FILE *f = NULL;

    char buf[4096];
    char *strptr = NULL;
    unsigned long long user = 0;
    unsigned long long userlow = 0;
    unsigned long long sys = 0;
    unsigned long long idle = 0;
    unsigned long long pages_in = 0;
    unsigned long long pages_out = 0;

    _OSBASE_TRACE(3,("get_cpu_page_data_24() called"));

    int read_count = 0;
    if ((f = fopen(DATA_FILE,"r")) != NULL) {
        read_count = fread(buf,sizeof(char),sizeof(buf)-1,f);
        buf[read_count] = '\0';

        /* read cpu data
         *
         * /proc/stat
         * cpu <user> <user_low_prio> <system> <idle> in jiffies (1/100ths of a second)
         */
        if (sscanf(buf,"cpu %lld %lld %lld %lld",&user,&userlow,&sys, &idle) == 4) {
            stats->user_time = (user + userlow) * 10;
            stats->system_time = sys * 10;
            stats->idle_time = idle * 10;
            stats->wait_time = 0;
            strptr = strstr(buf,"page");

            /* read page data
             *
             * page <in> <out>
            */
            if (sscanf(strptr,"page %lld %lld",&pages_in, &pages_out) == 2) {
                stats->pages_in = pages_in;
                stats->pages_out = pages_out;
                res = 1;
            } else {
                _OSBASE_TRACE(1,("get_cpu_page_data_24(): could not find page in/out data"));
            }
        } else {
            _OSBASE_TRACE(1,("get_cpu_page_data_24(): could not find cpu data"));
        }
    } else {
        _OSBASE_TRACE(1,("get_cpu_page_data_24():could not open %s: %s",DATA_FILE,strerror(errno)));
    }
    _OSBASE_TRACE(4,("get_cpu_page_data_24() res. %d",res));
    _OSBASE_TRACE(3,("get_cpu_page_data_24() exited"));
    return res;
}


/**
 * Updates the fields stats->user_time, stats->system_time, stats->idle_time,
 * stats->wait_time, stats->pages_in and stats->pages_out supposing that the 
 * currently running kernel is of version 2.6.
 * @param stats the structure whose fields will be updated.
 * They won't be changed if an error occured.
 * @return 1 on success, 0 otherwise
 */
int get_cpu_queue_data_26(struct os_statistics *stats) {

    int res = 0;
    FILE *f = NULL;

    char buf[4096];
    char *strptr = NULL;
    unsigned long long user = 0;
    unsigned long long userlow = 0;
    unsigned long long sys = 0;
    unsigned long long idle = 0;
    unsigned long long wait = 0;
    unsigned long long p_running = 0;
    unsigned long long p_blocked = 0;

    _OSBASE_TRACE(3,("get_cpu_queue_data_26() called"));

    int read_count = 0;
    if ((f = fopen(DATA_FILE,"r")) != NULL) {
        read_count = fread(buf,sizeof(char),sizeof(buf)-1,f);
        buf[read_count] = '\0';

        /* read cpu data
         *
         * /proc/stat
         * cpu <user> <user_low_prio> <system> <idle> <wait> in jiffies (1/100ths of a second)
         */
        if (sscanf(buf,"cpu %lld %lld %lld %lld %lld",&user,&userlow,&sys,&idle,&wait) == 5) {
            stats->user_time = (user + userlow) * 10;
            stats->system_time = sys * 10;
            stats->idle_time = idle * 10;
            stats->wait_time = wait * 10;
            /* read queue data
             * procs_running <number>
             * procs_blocked <number>
             */
            strptr = strstr(buf,"procs_running");
            if (strptr != NULL &&
                sscanf(strptr,"procs_running %lld\nprocs_blocked %lld",&p_running,&p_blocked) == 2) {
                stats->run_queue_length = p_running;
                stats->block_queue_length = p_blocked;
                res = 1;
            } else {
                _OSBASE_TRACE(1,("get_cpu_queue_data_26(): could not find queue info"));
            }

        } else {
            _OSBASE_TRACE(1,("get_cpu_queue_data_26(): could not find cpu info"));
        }
        fclose(f);
    } else {
        _OSBASE_TRACE(1,("get_cpu_queue_data_26(): could not open %s: %s",DATA_FILE,strerror(errno)));
    }
    _OSBASE_TRACE(4,("get_cpu_queue_data_26() res: %d",res));
    _OSBASE_TRACE(3,("get_cpu_queue_data_26() exited"));
    return res;
}

/**
 * Updates the fields stats->pages_in and stats->pages_out
 * supposing that the currently running kernel is of version 2.6.
 * @param stats the structure whose fields will be updated.
 * They won't be changed if an error occured.
 * @return 1 on success, 0 otherwise
 */
int get_pages_26(struct os_statistics *stats) {

    int res = 0;
    FILE *f = NULL;
    char buf[1024];
    char *strptr = NULL;
    int read_count = 0;

    _OSBASE_TRACE(3,("get_pages_26 called"));

    /*
     * pgpgin <number>
     * pgpgout <number>
     */
    if ((f = fopen(PAGE_FILE_26,"r")) != NULL) {
        read_count = fread(buf,sizeof(char),sizeof(buf)-1,f);
        buf[read_count] = '\0';
        strptr = strstr(buf,"pgpgin");
        if (strptr != NULL &&
            sscanf(strptr,"pgpgin %lld\npgpgout %lld",&(stats->pages_in),&(stats->pages_out)) == 2) {
            res = 1;
        } else {
            _OSBASE_TRACE(1,("get_pages_26(): could not find page in/out information"));
         }
        fclose(f);
    } else {
        _OSBASE_TRACE(1,("get_pages_26(): could not open %s: %s",PAGE_FILE_26,strerror(errno)));
    }
    _OSBASE_TRACE(4,("get_pages_26() res: %d",res));
    _OSBASE_TRACE(3,("get_pages_26() exited"));
    return res;
}


/**
 * Updates all fields of the given structure.
 * @param stats the structure whose fields will be updated.
 * They will be set to zero if an error occured.
 */
int get_os_statistics(struct os_statistics *stats) {

    int res = 1;
    _OSBASE_TRACE(3,("get_os_statistics() called"));
#ifdef CHECK_FCT_PARAMS
    if (stats == NULL) {
        return res;
        _OSBASE_TRACE(1,("get_os_statistics(): stats parameter is NULL"));
    }
#endif

    stats->user_time = 0;
    stats->system_time = 0;
    stats->wait_time = 0;
    stats->idle_time = 0;
    stats->run_queue_length = 0;
    stats->block_queue_length = 0;
    stats->pages_in = 0;
    stats->pages_out = 0;

    if (is_kernel26()) {
        res = get_cpu_queue_data_26(stats);
        res = res && get_pages_26(stats);
    } else {
        res = get_cpu_page_data_24(stats);
        res = res && get_queue_info_24(stats);
        res = res && get_wait_time_24(stats);
    }
    _OSBASE_TRACE(4,("get_os_statistics() res: %d",res));
    _OSBASE_TRACE(3,("get_os_statistics() exited"));

    return (res == 0);
}


/* ---------------------------------------------------------------------------*/
/*          end of OSBase_OperatingSystemStatisticalData.c                    */
/* ---------------------------------------------------------------------------*/

