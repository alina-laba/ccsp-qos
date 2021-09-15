/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2021 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#include "ansc_platform.h"
#include "ansc_load_library.h"
#include "cosa_plugin_api.h"
#include "plugin_main.h"
#include "cosa_apis_qosplugin.h"
#include "qos_dm_load.h"

#define THIS_PLUGIN_VERSION                         1

int ANSC_EXPORT_API
COSA_Init
    (
        ULONG                       uMaxVersionSupported, 
        void*                       hCosaPlugInfo         /* PCOSA_PLUGIN_INFO passed in by the caller */
    )
{
    PCOSA_PLUGIN_INFO               pPlugInfo  = (PCOSA_PLUGIN_INFO)hCosaPlugInfo;

    if ( uMaxVersionSupported < THIS_PLUGIN_VERSION )
    {
      /* this version is not supported */
        return -1;
    }   
    
    pPlugInfo->uPluginVersion       = THIS_PLUGIN_VERSION;
    /* register the back-end apis for the data model */

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Classification_IsUpdated", Classification_IsUpdated);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Classification_Synchronize", Classification_Synchronize);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Classification_GetEntryCount", Classification_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Classification_GetEntry", Classification_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Classification_AddEntry", Classification_AddEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Classification_DelEntry", Classification_DelEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Classification_GetParamUlongValue", Classification_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Classification_SetParamUlongValue", Classification_SetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Classification_GetParamStringValue", Classification_GetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Classification_SetParamStringValue", Classification_SetParamStringValue);
	pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Classification_GetParamBoolValue", Classification_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Classification_SetParamBoolValue", Classification_SetParamBoolValue);
	pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Classification_GetParamIntValue", Classification_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Classification_SetParamIntValue", Classification_SetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Classification_Validate", Classification_Validate);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Classification_Commit", Classification_Commit);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Classification_Rollback", Classification_Rollback);

    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Queue_IsUpdated", Queue_IsUpdated);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Queue_Synchronize", Queue_Synchronize);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Queue_GetEntryCount", Queue_GetEntryCount);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Queue_GetEntry", Queue_GetEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Queue_AddEntry", Queue_AddEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Queue_DelEntry", Queue_DelEntry);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Queue_GetParamUlongValue", Queue_GetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Queue_SetParamUlongValue", Queue_SetParamUlongValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Queue_GetParamStringValue", Queue_GetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Queue_SetParamStringValue", Queue_SetParamStringValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Queue_GetParamBoolValue", Queue_GetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Queue_SetParamBoolValue", Queue_SetParamBoolValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Queue_GetParamIntValue", Queue_GetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Queue_SetParamIntValue", Queue_SetParamIntValue);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Queue_Validate", Queue_Validate);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Queue_Commit", Queue_Commit);
    pPlugInfo->RegisterFunction(pPlugInfo->hContext, "Queue_Rollback", Queue_Rollback);

    qos_DmLoadDefaults();

    return  0;
}

BOOL ANSC_EXPORT_API
COSA_IsObjectSupported
    (
        char*                        pObjName
    )
{
    
    return TRUE;
}

void ANSC_EXPORT_API
COSA_Unload
    (
        void
    )
{
    /* unload the memory here */
}
