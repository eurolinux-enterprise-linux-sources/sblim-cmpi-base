/*
 * cmpiOSBase_OperatingSystemProvider.c
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
 * Interface Type : Common Manageability Programming Interface ( CMPI )
 *
 * Description: 
 * 
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmpidt.h"
#include "cmpift.h"
#include "cmpimacs.h"

#include "OSBase_Common.h"
#include "cmpiOSBase_Common.h"
#include "cmpiOSBase_OperatingSystem.h"

#ifndef NOEVENTS
#include <ind_helper.h>
#endif

static const CMPIBroker * _broker;

/* ---------------------------------------------------------------------------*/
/* private declarations                                                       */
#ifdef CMPI_VER_100
#define OSBase_OperatingSystemProviderSetInstance OSBase_OperatingSystemProviderModifyInstance 
#endif


/* ---------------------------------------------------------------------------*/


/* ---------------------------------------------------------------------------*/
/*                      Instance Provider Interface                           */
/* ---------------------------------------------------------------------------*/


CMPIStatus OSBase_OperatingSystemProviderCleanup( CMPIInstanceMI * mi, 
           const CMPIContext * ctx, CMPIBoolean terminate) { 
  _OSBASE_TRACE(1,("--- %s CMPI Cleanup() called",_ClassName));
  _OSBASE_TRACE(1,("--- %s CMPI Cleanup() exited",_ClassName));
  CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_OperatingSystemProviderEnumInstanceNames( CMPIInstanceMI * mi, 
           const CMPIContext * ctx, 
           const CMPIResult * rslt, 
           const CMPIObjectPath * ref) { 
  CMPIObjectPath * op = NULL;
  CMPIStatus       rc = {CMPI_RC_OK, NULL};
  
  _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() called",_ClassName));

  op = _makePath_OperatingSystem( _broker, ctx, ref, &rc );

  if( op == NULL ) {
    if( rc.msg != NULL )
      { _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() failed : %s",_ClassName,CMGetCharPtr(rc.msg))); }
    else
      { _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() failed",_ClassName)); }
    return rc;
  }

  CMReturnObjectPath( rslt, op );
  CMReturnDone( rslt );

  _OSBASE_TRACE(1,("--- %s CMPI EnumInstanceNames() exited",_ClassName));
  return rc;
}

CMPIStatus OSBase_OperatingSystemProviderEnumInstances( CMPIInstanceMI * mi, 
           const CMPIContext * ctx, 
           const CMPIResult * rslt, 
           const CMPIObjectPath * ref, 
           const char ** properties) { 
  const CMPIInstance * ci = NULL;
  CMPIStatus     rc = {CMPI_RC_OK, NULL};

  _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() called",_ClassName));

  ci = _makeInst_OperatingSystem( _broker, ctx, ref, properties, &rc );

  if( ci == NULL ) {
    if( rc.msg != NULL )
      { _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() failed : %s",_ClassName,CMGetCharPtr(rc.msg))); }
    else
      { _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() failed",_ClassName)); }
    return rc;
  }

  CMReturnInstance( rslt, ci );
  CMReturnDone( rslt );
  _OSBASE_TRACE(1,("--- %s CMPI EnumInstances() exited",_ClassName));
  return rc;
}

CMPIStatus OSBase_OperatingSystemProviderGetInstance( CMPIInstanceMI * mi, 
           const CMPIContext * ctx, 
           const CMPIResult * rslt, 
           const CMPIObjectPath * cop, 
           const char ** properties) {
  const CMPIInstance * ci = NULL;
  CMPIStatus     rc = {CMPI_RC_OK, NULL};

  _OSBASE_TRACE(1,("--- %s CMPI GetInstance() called",_ClassName));

  _check_system_key_value_pairs( _broker, cop, "CSCreationClassName", "CSName", &rc );
  if( rc.rc != CMPI_RC_OK ) { 
    _OSBASE_TRACE(1,("--- %s CMPI GetInstance() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
    return rc; 
  }
  _check_system_key_value_pairs( _broker, cop, "CreationClassName", "Name", &rc );
  if( rc.rc != CMPI_RC_OK ) { 
    _OSBASE_TRACE(1,("--- %s CMPI GetInstance() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
    return rc; 
  }

  ci = _makeInst_OperatingSystem( _broker, ctx, cop, properties, &rc );

  if( ci == NULL ) {
    if( rc.msg != NULL )
      { _OSBASE_TRACE(1,("--- %s CMPI GetInstance() failed : %s",_ClassName,CMGetCharPtr(rc.msg))); }
    else
      { _OSBASE_TRACE(1,("--- %s CMPI GetInstance() failed",_ClassName)); }
    return rc;
  }

  CMReturnInstance( rslt, ci );
  CMReturnDone(rslt);

  _OSBASE_TRACE(1,("--- %s CMPI GetInstance() exited",_ClassName));
  return rc;
}

CMPIStatus OSBase_OperatingSystemProviderCreateInstance( CMPIInstanceMI * mi, 
           const CMPIContext * ctx, 
           const CMPIResult * rslt, 
           const CMPIObjectPath * cop, 
           const CMPIInstance * ci) {
  CMPIStatus rc = {CMPI_RC_OK, NULL};

  _OSBASE_TRACE(1,("--- %s CMPI CreateInstance() called",_ClassName));

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED" ); 

  _OSBASE_TRACE(1,("--- %s CMPI CreateInstance() exited",_ClassName));
  return rc;
}

CMPIStatus OSBase_OperatingSystemProviderSetInstance( CMPIInstanceMI * mi, 
           const CMPIContext * ctx, 
           const CMPIResult * rslt, 
           const CMPIObjectPath * cop,
           const CMPIInstance * ci, 
           const char ** properties) {
  CMPIStatus rc = {CMPI_RC_OK, NULL};

  _OSBASE_TRACE(1,("--- %s CMPI SetInstance() called",_ClassName));

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED" ); 

  _OSBASE_TRACE(1,("--- %s CMPI SetInstance() exited",_ClassName));
  return rc;
}

CMPIStatus OSBase_OperatingSystemProviderDeleteInstance( CMPIInstanceMI * mi, 
           const CMPIContext * ctx, 
           const CMPIResult * rslt, 
           const CMPIObjectPath * cop) {
  CMPIStatus rc = {CMPI_RC_OK, NULL}; 

  _OSBASE_TRACE(1,("--- %s CMPI DeleteInstance() called",_ClassName));

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED" ); 

  _OSBASE_TRACE(1,("--- %s CMPI DeleteInstance() exited",_ClassName));
  return rc;
}

CMPIStatus OSBase_OperatingSystemProviderExecQuery( CMPIInstanceMI * mi, 
           const CMPIContext * ctx, 
           const CMPIResult * rslt, 
           const CMPIObjectPath * ref, 
           const char * lang, 
           const char * query) {
  CMPIStatus rc = {CMPI_RC_OK, NULL};

  _OSBASE_TRACE(1,("--- %s CMPI ExecQuery() called",_ClassName));

  CMSetStatusWithChars( _broker, &rc, 
			CMPI_RC_ERR_NOT_SUPPORTED, "CIM_ERR_NOT_SUPPORTED" ); 

  _OSBASE_TRACE(1,("--- %s CMPI ExecQuery() exited",_ClassName));
  return rc;
}


/* ---------------------------------------------------------------------------*/
/*                        Method Provider Interface                           */
/* ---------------------------------------------------------------------------*/

static char * _copy_buf( char ** hdbuf );

CMPIStatus OSBase_OperatingSystemProviderMethodCleanup( CMPIMethodMI * mi, 
           const CMPIContext * ctx, CMPIBoolean terminate) {
  _OSBASE_TRACE(1,("--- %s CMPI MethodCleanup() called",_ClassName));
  _OSBASE_TRACE(1,("--- %s CMPI MethodCleanup() exited",_ClassName));
  CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_OperatingSystemProviderInvokeMethod( CMPIMethodMI * mi,
           const CMPIContext * ctx,
           const CMPIResult * rslt,
           const CMPIObjectPath * ref,
           const char * methodName,
           const CMPIArgs * in,
           CMPIArgs * out) {
  CMPIData      incmd;
  CMPIValue     valrc;
  CMPIString  * class = NULL;
  CMPIStatus    rc    = {CMPI_RC_OK, NULL};
  char        * cmd   = NULL;
  char       ** hdout = NULL;
  char       ** hderr = NULL;
  char        * buf   = NULL;
  int           cmdrc = 0;

  _OSBASE_TRACE(1,("--- %s CMPI InvokeMethod() called",_ClassName));

  class = CMGetClassName(ref, &rc);

  /* "execCmd" */
  if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 &&
      strcasecmp(methodName, "execCmd") == 0 ) {

    valrc.uint8 = 0;

    incmd = CMGetArg( in, "cmd", &rc);
    if (incmd.type != CMPI_string) {
       valrc.uint8 = 2;
       CMSetStatusWithChars(_broker, &rc,
             CMPI_RC_ERR_INVALID_PARAMETER, "cmd parameter is not of type string.");
       _OSBASE_TRACE(1,("--- %s CMPI InvokeMethod() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
    }
    else if( incmd.value.string == NULL ) {    
      valrc.uint8 = 2;
      CMSetStatusWithChars( _broker, &rc, 
			    CMPI_RC_ERR_FAILED, "Could not get command string." ); 
      _OSBASE_TRACE(1,("--- %s CMPI InvokeMethod() failed : %s",_ClassName,CMGetCharPtr(rc.msg)));
    }
    else {

      cmd = calloc(1,(strlen(CMGetCharPtr(incmd.value.string))+1));
      strcpy(cmd, CMGetCharPtr(incmd.value.string) );
      cmdrc = runcommand( cmd, NULL, &hdout, &hderr );    
      if(cmd) free(cmd);

      /* command execution failed */
      if( cmdrc != 0 ) {
	valrc.uint8 = 2;
	_OSBASE_TRACE(1,("--- %s CMPI InvokeMethod() failed : runcommand() returned with %i",_ClassName,cmdrc));
	goto exitExecCmd;
      }

      /* command execution output on hderr */
      if( hderr[0] != NULL ) {
	valrc.uint8 = 1;
	buf = _copy_buf( hderr );
	if( buf != NULL ) {
	  rc = CMAddArg( out, "out", buf, CMPI_chars);
	  if( rc.rc != CMPI_RC_OK ) {
	    if( rc.msg != NULL ) {
	      _OSBASE_TRACE(1,("--- %s CMPI InvokeMethod() failed : stderr CMAddArg() %s",_ClassName,CMGetCharPtr(rc.msg)));
	    }
	    else {
	      _OSBASE_TRACE(1,("--- %s CMPI InvokeMethod() failed : stderr CMAddArg()",_ClassName));
	    }
	  }
	}
	goto exitExecCmd;
      }

      if( hdout[0] != NULL ) {
	buf = _copy_buf( hdout );
	if( buf != NULL ) {
	  rc = CMAddArg( out, "out", buf, CMPI_chars);
	  if( rc.rc != CMPI_RC_OK ) {
	    if( rc.msg != NULL ) {
	    _OSBASE_TRACE(1,("--- %s CMPI InvokeMethod() failed : stdout CMAddArg() %s",_ClassName,CMGetCharPtr(rc.msg)));
	    }
	    else {
	      _OSBASE_TRACE(1,("--- %s CMPI InvokeMethod() failed : stdout CMAddArg()",_ClassName));
	    }
	  }
	}
	goto exitExecCmd;
      }

    exitExecCmd:
      freeresultbuf(hdout);
      freeresultbuf(hderr);
      if(buf) free(buf);
      _OSBASE_TRACE(1,("--- %s CMPI InvokeMethod() %s exited",_ClassName,methodName));
      CMReturnData( rslt, &valrc, CMPI_uint8);
      CMReturnDone( rslt );
    }
  }
  else if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 &&
	   strcasecmp(methodName, "Reboot") == 0 ) {
    valrc.uint32 = runcommand("/sbin/reboot", NULL, NULL, NULL );
    CMReturnData(rslt, &valrc, CMPI_uint32);
    CMReturnDone(rslt);
  }
  else  if( strcasecmp(CMGetCharPtr(class), _ClassName) == 0 &&
	    strcasecmp(methodName, "Shutdown") == 0 ) {
    valrc.uint32 = runcommand("/sbin/halt", NULL, NULL, NULL );
    CMReturnData(rslt, &valrc, CMPI_uint32);
    CMReturnDone(rslt);
  }
  else {
    CMSetStatusWithChars( _broker, &rc, 
			  CMPI_RC_ERR_METHOD_NOT_FOUND, methodName ); 
  }
 
  _OSBASE_TRACE(1,("--- %s CMPI InvokeMethod() exited",_ClassName));
  return rc;
}


static char * _copy_buf( char ** hdbuf ) {
  char * buf      = NULL;
  int    i        = 0;
  int    buf_size = 0;

  /* allocate memory for return message of command */
  while ( hdbuf[i] ) {
    buf_size = buf_size + strlen(hdbuf[i]) +1 ;
    i++;	
  }	
  if(buf_size<1) { buf_size = 1; }

  buf = calloc (1, buf_size);

  /* write output of hdbuf to buf */
  i=0;
  while ( hdbuf[i] ) {
    strcat(buf,hdbuf[i]);
    i++;
  }
  if(buf_size == 1) { strcpy(buf,"\n"); }

  return buf;
}


/* ---------------------------------------------------------------------------*/
/*                       Indication Provider Interface                        */
/* ---------------------------------------------------------------------------*/

#ifndef NOEVENTS

#define INDCLASSNAME      "CIM_InstModification"
#define INDNAMESPACE      "root/cimv2"
#define IND_NUMBER_OF_DYNAMIC_PROPERTIES  5

static int ind_inited  = 0;
static int ind_enabled = 0;
static int ind_new     = 0;

static int ind_OperationalStatus = 2;

static CMPIInstance *sourceInstance   = NULL;
static CMPIInstance *previousInstance = NULL;


IndErrorT CIM_Indication_IndicationIdentifier(CMPIData *v)
{
  CMPIString *str;
  CMPIStatus rc;

  _OSBASE_TRACE(1,("--- %s CIM_Indication_IndicationIdentifier() called",_ClassName));

  str = CMNewString(_broker,INDCLASSNAME,&rc);
  v->type = CMPI_string;
  v->value.string = str;
  v->state = CMPI_goodValue;

  return IND_OK;
}

IndErrorT CIM_Indication_CorrelatedIndications(CMPIData *v)
{
  CMPIArray *a;
  CMPIStatus rc;
  
  _OSBASE_TRACE(1,("--- %s CIM_Indication_CorrelatedIndications() called",_ClassName));
  
  a = CMNewArray(_broker, 0, CMPI_string, &rc);
  v->type= CMPI_stringA;
  v->value.array = a;
  v->state = CMPI_goodValue;
  
  return IND_OK;
}

IndErrorT CIM_Indication_IndicationTime(CMPIData *v)
{
  CMPIStatus rc;
  CMPIDateTime *localTime = NULL;

  _OSBASE_TRACE(1,("--- %s CIM_Indication_IndicationTime() called",_ClassName));

  localTime = CMNewDateTime(_broker, &rc);
  v->type=CMPI_dateTime;
  v->value.dateTime = localTime;
  v->state = CMPI_goodValue;

  return IND_OK;
}

IndErrorT CIM_InstModification_SourceInstance(CMPIData *v)
{
  CMPIStatus rc;

  _OSBASE_TRACE(1,("--- %s CIM_InstModification_SourceInstance() called",_ClassName));

  v->type = CMPI_instance; 
  v->value.inst = CMClone(sourceInstance,&rc);
  v->state = CMPI_goodValue;

  return IND_OK;
}

IndErrorT CIM_InstModification_PreviousInstance(CMPIData *v)
{
  CMPIStatus rc;

  _OSBASE_TRACE(1,("--- %s CIM_InstModification_PreviousInstance() called",_ClassName));

  v->type = CMPI_instance; 
  v->value.inst = CMClone(previousInstance,&rc);
  v->state = CMPI_goodValue;

  return IND_OK;
}


IndErrorT check(CMPIData *v)
{
  CMPIObjectPath *cop = NULL;
  CMPIStatus      rc;

  _OSBASE_TRACE(1,("--- %s check() called",_ClassName));

  /* call function to check OperationalStatus */
  ind_new = check_OperationalStatus(&ind_OperationalStatus);
  if(ind_new!=0) 
    _OSBASE_TRACE(2,("--- %s check(): Event occured",_ClassName));

  if(previousInstance) {
    CMRelease(previousInstance);
    previousInstance=NULL; 
  }
  if(sourceInstance) {
    previousInstance = CMClone(sourceInstance,&rc);
    CMRelease(sourceInstance);
    sourceInstance=NULL;
  }

  cop = CMNewObjectPath( _broker,INDNAMESPACE,_ClassName,&rc);
  sourceInstance = CMClone(_makeInst_OperatingSystem(_broker,NULL,cop,NULL,&rc),&rc);

  v->state = CMPI_goodValue;
  v->type = CMPI_uint16;
  v->value.uint16 = ind_OperationalStatus;

  ind_new = 0;
  _OSBASE_TRACE(1,("--- %s check() exited",_ClassName));
  return IND_OK;
}


/* The list of property names for the functions */
static const char *Linux_OperatingSystemIndication_DYNAMIC_PROPERTIES[] =
  {"IndicationIdentifier",
   "CorrelatedIndications",
   "IndicationTime",
   "SourceInstance",
   "PreviousInstance"
  };

/* The list of property functions  */
static IndErrorT (* Linux_OperatingSystemIndication_DYNAMIC_FUNCTIONS[])(CMPIData *v) =
  {CIM_Indication_IndicationIdentifier,
   CIM_Indication_CorrelatedIndications,
   CIM_Indication_IndicationTime,
   CIM_InstModification_SourceInstance,
   CIM_InstModification_PreviousInstance
  };


static void ind_init(const CMPIContext *ctx) {

  CMPIObjectPath *cop = NULL;
  CMPIStatus      rc;

  if (ind_inited==0) {

    _OSBASE_TRACE(1,("--- %s ind_init() called",_ClassName));

    cop = CMNewObjectPath( _broker,INDNAMESPACE,_ClassName,&rc);
    sourceInstance = CMClone(_makeInst_OperatingSystem(_broker,NULL,cop,NULL,&rc),&rc);

    /* register the broker and context */
    if(ind_reg(_broker, ctx) != IND_OK) {
      _OSBASE_TRACE(1,("--- %s ind_init() failed",_ClassName));
      CMRelease(sourceInstance);
      return; 
    }

    /* register the poll function. check every 10 seconds. */
    if(ind_reg_pollfnc(INDCLASSNAME,
		       "Poll function",
		       check,
		       10,
		       IND_BEHAVIOR_LEVEL_EDGE) != IND_OK) {
      _OSBASE_TRACE(1,("--- %s ind_init() failed: register poll function",_ClassName));
      CMRelease(sourceInstance);
      return;
    }
    
    /* register the dynamic properties and its functions to retrieve data */
    if(ind_set_properties_f(INDNAMESPACE,INDCLASSNAME,
			    Linux_OperatingSystemIndication_DYNAMIC_PROPERTIES,
			    Linux_OperatingSystemIndication_DYNAMIC_FUNCTIONS,
			    IND_NUMBER_OF_DYNAMIC_PROPERTIES) != IND_OK) {
      _OSBASE_TRACE(1,("--- %s ind_init() failed: register functions of dynamic properties",_ClassName));
      CMRelease(sourceInstance);
      return;
    }
    
    /* tie the properties and polling function together */  
    if(ind_set_classes(INDNAMESPACE,
		       INDCLASSNAME,
		       INDCLASSNAME) != IND_OK) { 
      _OSBASE_TRACE(1,("--- %s ind_init() failed: set connection between poll function and dynamic properties",_ClassName));
      CMRelease(sourceInstance);
      return; 
    }

    ind_inited = 1; 
    _OSBASE_TRACE(1,("--- %s ind_init() exited",_ClassName));
  }
}

/* ---------------------------------------------------------------------------*/

CMPIStatus OSBase_OperatingSystemProviderIndicationCleanup( 
           CMPIIndicationMI * mi, 
           const CMPIContext * ctx, CMPIBoolean terminate) {
  _OSBASE_TRACE(1,("--- %s CMPI IndicationCleanup() called",_ClassName));

  if(sourceInstance)   { CMRelease(sourceInstance); }
  if(previousInstance) { CMRelease(previousInstance); }
  ind_shutdown();
  ind_inited = 0; 
  ind_enabled = 0;

  _OSBASE_TRACE(1,("--- %s CMPI IndicationCleanup() exited",_ClassName));
  CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_OperatingSystemProviderAuthorizeFilter( 
           CMPIIndicationMI * mi, 
           const CMPIContext * ctx, 
#ifndef CMPI_VER_100
           const CMPIResult * rslt,
#endif
           const CMPISelectExp * filter, 
           const char * indType, 
           const CMPIObjectPath * classPath,
           const char * owner) {
  _OSBASE_TRACE(1,("--- %s CMPI AuthorizeFilter() called",_ClassName));

#ifndef CMPI_VER_100
  if (strcasecmp(indType,INDCLASSNAME)==0) {     
    ind_init(ctx);
    _OSBASE_TRACE(1,("--- %s CMPI AuthorizeFilter(): successfully authorized filter",_ClassName));
    CMReturnData(rslt,(CMPIValue*)&(CMPI_true),CMPI_boolean);
  } else {
    _OSBASE_TRACE(1,("--- %s CMPI AuthorizeFilter(): refused to authorize filter",_ClassName));
    CMReturnData(rslt,(CMPIValue*)&(CMPI_false),CMPI_boolean);
  }
  CMReturnDone(rslt);
#endif

  _OSBASE_TRACE(1,("--- %s CMPI AuthorizeFilter() exited",_ClassName));
  CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_OperatingSystemProviderMustPoll(
           CMPIIndicationMI * mi, 
           const CMPIContext * ctx, 
#ifndef CMPI_VER_100
           const CMPIResult * rslt,
#endif
           const CMPISelectExp * filter, 
           const char * indType, 
           const CMPIObjectPath * classPath) {
  _OSBASE_TRACE(1,("--- %s CMPI MustPoll() called: NO POLLING",_ClassName));  
#ifndef CMPI_VER_100
  CMReturnData(rslt,(CMPIValue*)&(CMPI_false),CMPI_boolean);
  CMReturnDone(rslt);
#endif
  _OSBASE_TRACE(1,("--- %s CMPI MustPoll() exited",_ClassName));
  CMReturn(CMPI_RC_ERR_FAILED);
}

CMPIStatus OSBase_OperatingSystemProviderActivateFilter(
           CMPIIndicationMI * mi, 
           const CMPIContext * ctx, 
#ifndef CMPI_VER_100
           const CMPIResult * rslt,
#endif
           const CMPISelectExp * filter, 
           const char * indType, 
           const CMPIObjectPath * classPath,
           CMPIBoolean firstActivation) {

  CMPIStatus rc = {CMPI_RC_OK, NULL};
  _OSBASE_TRACE(1,("--- %s CMPI ActivateFilter() called",_ClassName));
  
  ind_init(ctx);
#ifndef CMPI_VER_100
  if (strcasecmp(indType,INDCLASSNAME)==0) {
#endif
    if(ind_set_select(INDNAMESPACE, INDCLASSNAME, filter) == IND_OK) {
      _OSBASE_TRACE(1,("--- %s CMPI ActivateFilter() exited: filter activated (%s)",
		       _ClassName,CMGetCharPtr(CMGetSelExpString(filter,&rc))));
      CMReturn(CMPI_RC_OK);
    }
#ifndef CMPI_VER_100
  }  
#endif
  _OSBASE_TRACE(1,("--- %s CMPI ActivateFilter() exited: filter not activated (%s)",
		   _ClassName,CMGetCharPtr(CMGetSelExpString(filter,&rc))));
  CMReturn(CMPI_RC_ERR_FAILED);
}

CMPIStatus OSBase_OperatingSystemProviderDeActivateFilter( 
           CMPIIndicationMI * mi, 
           const CMPIContext * ctx, 
#ifndef CMPI_VER_100
           const CMPIResult * rslt,
#endif
           const CMPISelectExp * filter, 
           const char * indType, 
           const CMPIObjectPath *classPath,
           CMPIBoolean lastActivation) {
  _OSBASE_TRACE(1,("--- %s CMPI DeActivateFilter() called",_ClassName));

#ifndef CMPI_VER_100
  if (strcasecmp(indType,INDCLASSNAME)==0) {
#endif
    if(ind_unreg_select(INDNAMESPACE, INDCLASSNAME, filter) == IND_OK) {
      _OSBASE_TRACE(1,("--- %s CMPI DeActivateFilter() exited: filter deactivated",_ClassName));
      CMReturn(CMPI_RC_OK);
    }
#ifndef CMPI_VER_100
  }  
#endif

  _OSBASE_TRACE(1,("--- %s CMPI DeActivateFilter() exited: filter not deactivated",_ClassName));
  CMReturn(CMPI_RC_ERR_FAILED);
}

CMPIStatus OSBase_OperatingSystemProviderEnableIndications(CMPIIndicationMI * mi, const CMPIContext * ctx) {
  _OSBASE_TRACE(1,("--- %s CMPI EnableIndications() called",_ClassName));

  if(!ind_enabled) {
    /* start the polling mechanism */
    if(ind_start() != IND_OK) { 
      _OSBASE_TRACE(1,("--- %s CMPI EnableIndications() failed: start indication helper",_ClassName));
      CMReturn(CMPI_RC_ERR_FAILED); 
    }
    ind_enabled = 1;
  }

  _OSBASE_TRACE(1,("--- %s CMPI EnableIndications() exited",_ClassName));
  CMReturn(CMPI_RC_OK);
}

CMPIStatus OSBase_OperatingSystemProviderDisableIndications(CMPIIndicationMI * mi, const CMPIContext * ctx) {
  _OSBASE_TRACE(1,("--- %s CMPI DisableIndications() called",_ClassName));

  if(ind_enabled) {
    /* stop the polling mechanism */
    if(ind_stop() != IND_OK) { 
      _OSBASE_TRACE(1,("--- %s CMPI DisableIndications() failed: stop indication helper",_ClassName));
      CMReturn(CMPI_RC_ERR_FAILED); 
    }
    ind_enabled = 0;
  }

  _OSBASE_TRACE(1,("--- %s CMPI DisableIndications() exited",_ClassName));
  CMReturn(CMPI_RC_OK);
}

#endif

/* ---------------------------------------------------------------------------*/
/*                              Provider Factory                              */
/* ---------------------------------------------------------------------------*/

CMInstanceMIStub( OSBase_OperatingSystemProvider, 
                  OSBase_OperatingSystemProvider, 
                  _broker, 
                  CMNoHook);

CMMethodMIStub( OSBase_OperatingSystemProvider,
                OSBase_OperatingSystemProvider, 
                _broker, 
                CMNoHook);

#ifndef NOEVENTS
CMIndicationMIStub( OSBase_OperatingSystemProvider,
                    OSBase_OperatingSystemProvider, 
                    _broker, 
                    CMNoHook);
#endif

/* ---------------------------------------------------------------------------*/
/*             end of cmpiOSBase_OperatingSystemProvider                      */
/* ---------------------------------------------------------------------------*/

