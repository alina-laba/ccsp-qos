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

#ifndef  _COSA_APIS_PLUGINSAMPLEOBJ_H
#define  _COSA_APIS_PLUGINSAMPLEOBJ_H

#include "slap_definitions.h"


BOOL
Classification_IsUpdated
    (
        ANSC_HANDLE hInsContext
    );

ULONG
Classification_Synchronize
    (
        ANSC_HANDLE hInsContext
    );

ULONG
Classification_GetEntryCount
    (
        ANSC_HANDLE hInsContext
    );

ANSC_HANDLE
Classification_GetEntry
    (
        ANSC_HANDLE hInsContext, 
        ULONG nIndex, 
        ULONG* pInsNumber
    );

ANSC_HANDLE 
Classification_AddEntry
    (
        ANSC_HANDLE hInsContext, 
        ULONG* pInsNumber
    );

ULONG
Classification_DelEntry
    (
        ANSC_HANDLE hInsContext, 
        ANSC_HANDLE hInstance
    );

ULONG
Classification_GetParamStringValue
	(
		ANSC_HANDLE 				hInsContext,
		char*						pParamName,
		char*						pValue,
		ULONG*						pUlSize
	);

BOOL
Classification_GetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    );

BOOL
Classification_GetParamUlongValue
    (
 	    ANSC_HANDLE                 	hInsContext,
        char*                       pParamName,
        ULONG*                      puLong
    );

BOOL
Classification_GetParamIntValue
	(
		ANSC_HANDLE				 hInsContext,
		char*						 pParamName,
		int*						 pInt
	);

BOOL
Classification_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pParamName,
        char*                       pString
    );

BOOL
Classification_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pParamName,
        BOOL                        bValue
    );

BOOL
Classification_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pParamName,
       ULONG                      	uValue
    );

BOOL
Classification_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pParamName,
        int                      	iValue
    );

BOOL
Classification_Validate
    (
        ANSC_HANDLE hInsContext
    );

ULONG
Classification_Commit
    (
        ANSC_HANDLE                 hInsContext
    );

ULONG Classification_Rollback
    (
        ANSC_HANDLE hInsContext
    );

BOOL
Queue_IsUpdated
    (
        ANSC_HANDLE hInsContext
    );

ULONG
Queue_Synchronize
    (
        ANSC_HANDLE hInsContext
    );

ULONG
Queue_GetEntryCount
    (
        ANSC_HANDLE hInsContext
    );

ANSC_HANDLE
Queue_GetEntry
    (
        ANSC_HANDLE hInsContext, 
        ULONG nIndex, 
        ULONG* pInsNumber
    );

ANSC_HANDLE 
Queue_AddEntry
    (
        ANSC_HANDLE hInsContext, 
        ULONG* pInsNumber
    );

ULONG
Queue_DelEntry
    (
        ANSC_HANDLE hInsContext, 
        ANSC_HANDLE hInstance
    );

ULONG
Queue_GetParamStringValue
(
     ANSC_HANDLE hInsContext,
     char* pParamName,
     char* pValue,
     ULONG* pUlSize
);

BOOL
Queue_GetParamBoolValue
(
        ANSC_HANDLE                 hInsContext,
        char*                       ParamName,
        BOOL*                       pBool
    );

BOOL
Queue_GetParamUlongValue
    (
 	    ANSC_HANDLE                 	hInsContext,
        char*                       pParamName,
        ULONG*                      puLong
    );

BOOL
Queue_GetParamIntValue
	(
		ANSC_HANDLE				 hInsContext,
		char*						 pParamName,
		int*						 pInt
	);

BOOL
Queue_SetParamStringValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pParamName,
        char*                       pString
    );

BOOL
Queue_SetParamBoolValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pParamName,
        BOOL                        bValue
    );

BOOL
Queue_SetParamUlongValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pParamName,
       ULONG                      	uValue
    );

BOOL
Queue_SetParamIntValue
    (
        ANSC_HANDLE                 hInsContext,
        char*                       pParamName,
        int                      	iValue
    );

BOOL
Queue_Validate
    (
        ANSC_HANDLE hInsContext
    );

ULONG
Queue_Commit
    (
        ANSC_HANDLE                 hInsContext
    );

ULONG
Queue_Rollback
    (
        ANSC_HANDLE hInsContext
    );
#endif
