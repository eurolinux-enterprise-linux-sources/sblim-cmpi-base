#ifndef _OSBASE_OPERATINGSYSTEM_H_
#define _OSBASE_OPERATINGSYSTEM_H_

/*
 * OSBase_OperatingSystem.h
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
 *               C. Eric Wu <cwu@us.ibm.com>
 *               Tyrel Datwyler <tyreld@us.ibm.com>
 *
 * Description: 
 * This file defines the interfaces for the resource access implementation 
 * of the CIM class Linux_OperatingSystem.
 * 
*/

/* ---------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------------------*/

extern char * CIM_OS_DISTRO;

/* ---------------------------------------------------------------------------*/

struct cim_operatingsystem {
  char *         version;              // major.minor
  unsigned short osType;               // 36
  unsigned long  numOfProcesses;       // ps
  unsigned long  numOfUsers;           // who -u

  char *       installDate;
  char *       lastBootUp;
  char *       localDate;
  signed short curTimeZone;            // CurrentTimeZone

  unsigned long maxNumOfProc;          // MaxNumberOfProcesses
  unsigned long long maxProcMemSize;   // MaxProcessMemorySize

  unsigned long long totalVirtMem;     // TotalVirtualMemorySize
  unsigned long long freeVirtMem;      // FreeVirtualMemory
  unsigned long long totalPhysMem;     // TotalVisibleMemorySize
  unsigned long long freePhysMem;      // FreePhysicalMemory
  unsigned long long totalSwapMem;     // SizeStoredInPagingFiles
  unsigned long long freeSwapMem;      // FreeSpaceInPagingFiles

  char * codeSet;                      // CodeSet
  char * langEd;                       // LanguageEdition

  unsigned long defPageSize;           // DefaultPageSize
  
  unsigned long licensedUsers;         // 0
  unsigned long long totalSwapSize;
  unsigned short healthState;          // 5
};

/* ---------------------------------------------------------------------------*/

int get_operatingsystem_data( struct cim_operatingsystem ** );
void free_os_data( struct cim_operatingsystem * );

void _init_os_distro();
char * get_os_installdate();
char * get_os_lastbootup();
char * get_os_localdatetime();
char * get_os_codeSet();
char * get_os_langEd();

unsigned long get_os_numOfProcesses();
unsigned long get_os_numOfUsers();
unsigned long get_os_maxNumOfProc();

unsigned long long get_os_maxProcMemSize();
unsigned long long get_os_totalSwapSize();

char * get_kernel_version();

/* ---------------------------------------------------------------------------*/

#ifdef __cplusplus
   }
#endif

/* ---------------------------------------------------------------------------*/

#endif


