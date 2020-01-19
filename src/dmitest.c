/*
 * $Id: dmitest.c,v 1.3 2009/07/25 00:37:31 tyreld Exp $
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
 * Contributors:
 *
 * Interface Type : Common Manageability Programming Interface ( CMPI )
 *
 * Description: Test Program for SMBIOS/DMI Resource Access.
 *
 */

#include "dmiinfo.h"

#include <stdio.h>

int main()
{
  DMI_BIOSPRODUCT *bp;
  DMI_BIOSFEATURE *bf;
  DMI_BIOSELEMENT *be;
  DMI_HWSECURITY  *hs;
  cimdmi_init();
  bp=cimdmi_getBiosProduct();
  if (bp)
    printf("BIOS Product Information: Name %s, Identifying Number %s, "
	   "Vendor %s, Version %s\n",
	   bp->dmi_Name,bp->dmi_IdentifyingNumber,bp->dmi_Vendor,
	   bp->dmi_Version );
  bf=cimdmi_getBiosFeature();
  if (bf)
    printf("BIOS Feature Information: Name %s, Product Name %s, "
	   "Identifying Number %s, Vendor %s, Version %s\n",
	   bf->dmi_Name,bf->dmi_ProductName, bf->dmi_IdentifyingNumber,
	   bf->dmi_Vendor, bf->dmi_Version );
  be=cimdmi_getBiosElement();
  if (be)
    printf("BIOS Element Information: Name %s, Version %s, "
	   "SWE State %d, SWE ID %s, Target OS %d, "
	   "Manufacturer %s, Starting Address 0x%08x, "
	   "Ending Address 0x%08x, Release Date %s\n",
	   be->dmi_Name,be->dmi_Version,be->dmi_SWEState,
	   be->dmi_SWEID, be->dmi_TargetOS, be->dmi_Manufacturer,
	   be->dmi_StartingAddress,be->dmi_EndingAddress,
	   ctime(&be->dmi_ReleaseDate));
  hs=cimdmi_getHwSecurity();
  if (hs && hs->dmi_Name) {
    char * popw;
    char * adpw;
    switch(hs->dmi_PowerOnPasswordState) {
    case 0:
      popw="not set";
      break;
    case 1:
      popw="set";
      break;
    case 2:
      popw="not implemented";
      break;
    case 3:
      popw="unknown";
      break;
    }
    switch(hs->dmi_AdminPasswordState) {
    case 0:
      adpw="not set";
      break;
    case 1:
      adpw="set";
      break;
    case 2:
      adpw="not implemented";
      break;
    case 3:
      adpw="unknown";
      break;
    }
    printf("HW Security Information: Name %s, "
	   "Power on password is %s, "
	   "Adminstrator password is %s\n ",
	   hs->dmi_Name,
	   popw,
	   adpw
	   );
  }
  cimdmi_term();
  return 0;
}
