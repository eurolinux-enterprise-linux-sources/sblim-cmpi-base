/*
 * $Id: dmiinfo.c,v 1.4 2009/07/25 00:37:31 tyreld Exp $
 *
 * (C) Copyright IBM Corp. 2003, 2009
 *
 * THIS FILE IS PROVIDED UNDER THE TERMS OF THE ECLIPSE PUBLIC LICENSE 
 * ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE 
 * CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
 *
 * You can obtain a current copy of the Eclipse Public License from
 * http://www.opensource.org/licenses/eclipse-1.0.php
 *
 * Author:       Viktor Mihajlovski <mihajlov@de.ibm.com>
 * Contributors: James Jeffers <jamesjef@us.ibm.com>
 *
 * Interface Type : Common Manageability Programming Interface ( CMPI )
 *
 * Description: Implementation of SMBIOS/DMI Resource Access.
 *              BIOS and System Information is retrieved by scanning the
 *              SMBIOS blocks pointed to by the DMI anchor. Works on 
 *              x86 architectures only!
 *     
 */

#include "dmiinfo.h"
#include "dmifind.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>

static DMI_BIOSPRODUCT *biosprod = NULL;
static DMI_BIOSFEATURE *biosfeat = NULL;
static DMI_BIOSELEMENT *bioselem = NULL;
static DMI_HWSECURITY  *hwsecurity = NULL;

static void fillinstruct(struct DMI_HDR * dmih, int num); 


/*
 * Init call scans the SMBIOS structures and filles the info structures.
 */
void cimdmi_init()
{
  biosprod = calloc(sizeof(DMI_BIOSPRODUCT),1);
  biosfeat = calloc(sizeof(DMI_BIOSFEATURE),1);
  bioselem = calloc(sizeof(DMI_BIOSELEMENT),1);
  hwsecurity = calloc(sizeof(DMI_HWSECURITY),1);
  if (biosprod && biosfeat && bioselem && hwsecurity) {
    struct DMI_FIND *dmif = NULL;
    struct DMI_HDR  *dmih = NULL;
    int    memfile = open("/dev/mem",O_RDONLY);
    void * biosptr=NULL, * smbiosptr=NULL;
    size_t pagesz = getpagesize();
    unsigned offbios = 0xe0000, offsmbios;
    size_t bioslen = 0x20000;
    
    if (memfile < 0 ) {
      perror("Error opening /dev/mem");
      cimdmi_term();
    } else {
      /* map /dev/mem BIOS portion to virtual memory */
      biosptr=mmap(NULL,bioslen,PROT_READ,MAP_SHARED,memfile,offbios);
      if (biosptr ==  MAP_FAILED) {
	perror("Error mapping /dev/mem BIOS");
	cimdmi_term();
      } else {
	/* find DMI anchor */
	for (dmif=biosptr;(void*)dmif<(biosptr+bioslen);dmif++) {
	  if (memcmp("_DMI_",dmif->dmi_sig,5)==0) {
#ifdef DEBUG
	    printf("DMI Signature found at %08x\n",
		   (void*)dmif-biosptr+offbios);
	    printf("SMBIOS Structures reside at %08x\n",
		   dmif->dmi_base);
#endif
	    break;
	  }
	}
	/* map SMBIOS structure to virtual memory (page-aligned) */
	if (pagesz != 0 && dmif->dmi_base%pagesz != 0) {
	  offsmbios = dmif->dmi_base/pagesz * pagesz;
	} else {
	  offsmbios = dmif->dmi_base;
	}
	smbiosptr = mmap(NULL,dmif->dmi_len + dmif->dmi_base - offsmbios,
			 PROT_READ,MAP_SHARED, memfile,offsmbios);
	if (smbiosptr == MAP_FAILED) {
	  perror ("Error mapping /dev/mem SMBIOS"); 
	  cimdmi_term();
	} else {
	  dmih = dmif->dmi_base - offsmbios + smbiosptr;
	  /* fill in data from SMBIOS Structures */
	  fillinstruct(dmih,dmif->dmi_num);
	}
      }
    }
    if (smbiosptr) 
      munmap(smbiosptr,dmif->dmi_len + dmif->dmi_base - offsmbios);
    if (biosptr) 
      munmap(biosptr,bioslen);
    if (memfile > 0) 
      close(memfile);    
  } else {
    cimdmi_term();
  }
}

/*
 * Termination call frees up all the allocated memory.
 */
void cimdmi_term()
{
  if (biosprod) {
    free(biosprod->dmi_Name);
    free(biosprod->dmi_IdentifyingNumber);
    free(biosprod->dmi_Vendor);
    free(biosprod->dmi_Version);
    free(biosprod);
    biosprod=NULL;
  }
  if (biosfeat) {
    free(biosfeat->dmi_Name);
    free(biosfeat->dmi_ProductName);
    free(biosfeat->dmi_IdentifyingNumber);
    free(biosfeat->dmi_Vendor);
    free(biosfeat->dmi_Version);
    free(biosfeat);
    biosfeat=NULL;
  }
  if (bioselem) {
    free(bioselem->dmi_Name);
    free(bioselem->dmi_Version);
    free(bioselem->dmi_SWEID);
    free(bioselem->dmi_Manufacturer);
    free(bioselem);
    bioselem=NULL;
  }
  if (hwsecurity) {
    free(hwsecurity->dmi_Name);
    hwsecurity=NULL;
  }
}


/*
 * get functions return pointers to structures filled by cimdmi_init
 */
DMI_BIOSPRODUCT * cimdmi_getBiosProduct()
{
  return biosprod;
}

DMI_BIOSFEATURE * cimdmi_getBiosFeature()
{
  return biosfeat;
}

DMI_BIOSELEMENT * cimdmi_getBiosElement()
{
  return bioselem;
}

DMI_HWSECURITY * cimdmi_getHwSecurity()
{
  return hwsecurity;
}

/*
 * decodes the string tables for an SMBIOS structure  and advances
 * to the next SMBIOS structure
 */
static void * nexthdr(struct DMI_HDR *dmih, struct DMI_STRING_MAP *dmis)
{
  char *stringbase = ((char*)dmih) + dmih->dmi_hdrlen;
  int i;
  memset(dmis,0,sizeof(struct DMI_STRING_MAP));
  dmis->dmi_string[0]="unknown";
  if (stringbase[0]==0 && stringbase[1]==0) 
    return stringbase + 2;
  for (i=1;*stringbase != 0 && i < 256; i++) {
    dmis->dmi_string[i] = stringbase;
    stringbase += strlen(stringbase) + 1;
  }
  return stringbase+1;
}

/*
 * fills info structures from SMBIOS structures
 */
static void fillinstruct(struct DMI_HDR * dmih, int num)
{
  struct DMI_HDR               *dmihn;
  struct DMI_STRING_MAP         dmis;
  struct DMI_BIOS_INFO         *biosinfo=NULL;
  struct DMI_SYSTEM_INFO       *sysinfo=NULL;
  struct DMI_HWSECURITY_INFO   *secinfo=NULL;
  char cvbuf[100];
  int i;
  struct tm date = {0};

  for (i=0; i < num;i++) {
    dmihn = nexthdr(dmih, &dmis);

    /* 	Try to determine the DMI struct length.
       	0-lengths are typically bogus headers, and should be skipped. */
	if (dmih->dmi_hdrlen == 0) {
		return;
	}
		

    switch(dmih->dmi_type) {
    case 0:
      /* bios */
      biosinfo = (void*)dmih + sizeof(struct DMI_HDR); 
      bioselem->dmi_Name=strdup("BIOS");
      sprintf(cvbuf,"%04x",dmih->dmi_handle);
      bioselem->dmi_SWEID=strdup(cvbuf);
      bioselem->dmi_Manufacturer=
	strdup(dmis.dmi_string[biosinfo->bios_vendor_idx]);
      bioselem->dmi_Version=
	strdup(dmis.dmi_string[biosinfo->bios_version_idx]);
      bioselem->dmi_StartingAddress=biosinfo->bios_segment << 4;
      bioselem->dmi_EndingAddress=0x100000;
      sscanf(dmis.dmi_string[biosinfo->bios_date_idx],"%d/%d/%d",
	     &date.tm_mon,
	     &date.tm_mday,
	     &date.tm_year);
      /* perform date fixups */
      if (date.tm_year>1900) 
	date.tm_year-=1900;
      date.tm_mon-=1;
      /* making up time portion as needed for CIM DateTime */
      date.tm_hour=11;
      date.tm_min=0;
      date.tm_sec=0;
      bioselem->dmi_ReleaseDate=mktime(&date);
      break;
    case 1:
      /* system */
      sysinfo =  (void*)dmih + sizeof(struct DMI_HDR); 
      biosprod->dmi_Vendor=
	strdup(dmis.dmi_string[sysinfo->system_manufacturer_idx]);
      biosprod->dmi_Name=strdup(dmis.dmi_string[sysinfo->system_product_idx]);
      biosprod->dmi_Version=
	strdup(dmis.dmi_string[sysinfo->system_version_idx]);
      biosprod->dmi_IdentifyingNumber=
	strdup(dmis.dmi_string[sysinfo->system_serial_idx]);
      biosfeat->dmi_Vendor=strdup(biosprod->dmi_Vendor);
      biosfeat->dmi_ProductName=strdup(biosprod->dmi_Name);
      biosfeat->dmi_Version=strdup(biosprod->dmi_Version);
      biosfeat->dmi_IdentifyingNumber=strdup(biosprod->dmi_IdentifyingNumber);
      biosfeat->dmi_Name=strdup("BIOS");
      break;
    case 24:
      /* hardware security */
      secinfo =  (void*)dmih + sizeof(struct DMI_HDR); 
      hwsecurity->dmi_Name=strdup("HW Security");
      hwsecurity->dmi_PowerOnPasswordState=secinfo->hwsecurity_setting >> 6;
      hwsecurity->dmi_AdminPasswordState=secinfo->hwsecurity_setting >> 2;
      break;
    default:
      break;
    }
    dmih=dmihn;
  } 
  /* fill in fixed items */
  bioselem->dmi_SWEState=2; /* always installed */
  bioselem->dmi_TargetOS=36;/* Linux, what else */
}

