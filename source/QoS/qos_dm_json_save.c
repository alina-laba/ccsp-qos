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

/*  To store data in the JSON file.
 */
#include "cJSON.h"
#include <stdint.h>
#include <string.h>
#include <rdk_error.h>
#include <rdk_debug.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include "ccsp_trace.h"
#include "qos_dm_load.h"
#include "qos_vector.h"

#include <sys/stat.h>
#include <fcntl.h>

cJSON* json_create_object(const char* pTableName)
{
    cJSON *clf = NULL, *queue = NULL;

    if (!strcmp(pTableName, DM_CLF))
    {
        clf = cJSON_CreateObject();
        cJSON_AddItemToObject(clf, DM_CLF_Enable, cJSON_CreateString(""));
        cJSON_AddItemToObject(clf, DM_CLF_DestIP, cJSON_CreateString(""));
        cJSON_AddItemToObject(clf, DM_CLF_DestMask, cJSON_CreateString(""));
        cJSON_AddItemToObject(clf, DM_CLF_SourceIP, cJSON_CreateString(""));
        cJSON_AddItemToObject(clf, DM_CLF_SourceMask, cJSON_CreateString(""));
        cJSON_AddItemToObject(clf, DM_CLF_SourcePort, cJSON_CreateString(""));
        cJSON_AddItemToObject(clf, DM_CLF_SourceMACAddress, cJSON_CreateString(""));
        cJSON_AddItemToObject(clf, DM_CLF_SourcePortRangeMax, cJSON_CreateString(""));
        cJSON_AddItemToObject(clf, DM_CLF_DestPort, cJSON_CreateString(""));
        cJSON_AddItemToObject(clf, DM_CLF_DestPortRangeMax, cJSON_CreateString(""));
        cJSON_AddItemToObject(clf, DM_CLF_Protocol, cJSON_CreateString(""));
        cJSON_AddItemToObject(clf, DM_CLF_DSCPMark, cJSON_CreateString(""));
        cJSON_AddItemToObject(clf, DM_CLF_TrafficClass, cJSON_CreateString(""));
        cJSON_AddItemToObject(clf, DM_CLF_ChainName, cJSON_CreateString(""));
        cJSON_AddItemToObject(clf, DM_CLF_IfaceIn, cJSON_CreateString(""));
        cJSON_AddItemToObject(clf, DM_CLF_IfaceOut, cJSON_CreateString(""));
        cJSON_AddItemToObject(clf, DM_CLF_TcpFlags, cJSON_CreateString(""));
        cJSON_AddItemToObject(clf, DM_CLF_TcpPsh, cJSON_CreateString(""));
    }
    else if (!strcmp(pTableName, DM_QUEUE))
    {
        queue = cJSON_CreateObject();
        cJSON_AddItemToObject(queue, DM_QUEUE_Enable, cJSON_CreateString(""));
        cJSON_AddItemToObject(queue, DM_QUEUE_Enable, cJSON_CreateString(""));
        cJSON_AddItemToObject(queue, DM_QUEUE_TrafficClasses, cJSON_CreateString(""));
        cJSON_AddItemToObject(queue, DM_QUEUE_Interface, cJSON_CreateString(""));
        cJSON_AddItemToObject(queue, DM_QUEUE_Weight, cJSON_CreateString(""));
        cJSON_AddItemToObject(queue, DM_QUEUE_Precedence, cJSON_CreateString(""));
        cJSON_AddItemToObject(queue, DM_QUEUE_SchedulerAlgorithm, cJSON_CreateString(""));
        cJSON_AddItemToObject(queue, DM_QUEUE_ShapingRate, cJSON_CreateString(""));
    }

    return clf;
}

cJSON* json_provide_object(cJSON *pParentNode, const char* pChildNodeName)
{
    cJSON *tmpObj = NULL;

    if (!(tmpObj = cJSON_GetObjectItemCaseSensitive(pParentNode, pChildNodeName)))
    {
        tmpObj = cJSON_CreateObject();
        cJSON_AddItemToObject(pParentNode, pChildNodeName, tmpObj);
    }

    return tmpObj;
}

cJSON_bool json_save_config_file(cJSON *obj);

bool json_set_value(const char *pValueName, cJSON *jsonObj, Value_t *pParam);

bool qos_DmSetString(const char *pTableName, uint32_t entryIdx, const char *valueName, const char *value)
{
    Value_t param;
    param.pValString = value;
    return qos_DmSave(pTableName, entryIdx, valueName, &param);
}

bool qos_DmSetInt(const char *pTableName, uint32_t entryIdx, const char *valueName, int32_t value)
{
    Value_t param;
    param.integer = value;
    return qos_DmSave(pTableName, entryIdx, valueName, &param);
}

bool qos_DmSetBool(const char *pTableName, uint32_t entryIdx, const char *valueName, bool value)
{
    Value_t param;
    param.boolean = value;
    return qos_DmSave(pTableName, entryIdx, valueName, &param);
}

cJSON* json_load_config()
{
    cJSON *config = NULL;
    uint32_t currentDMSize, readBytes;
    void* pJsonConfig = NULL; FILE* fp;

    if (NULL == (fp = fopen(QOS_CONFIG_CURRENT_PATH QOS_CONFIG_CURRENT_NAME, "r")))
    {
        printf("Failed to open JSON current file %s\n", QOS_CONFIG_CURRENT_PATH QOS_CONFIG_CURRENT_NAME);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    currentDMSize = ftell(fp);
    pJsonConfig = malloc(currentDMSize+10);
    if (NULL == pJsonConfig)
    {
        printf("Failed to get memory for JSON file\n");
        fclose(fp); 
        return NULL;
    }
    memset(pJsonConfig, (int)0, currentDMSize+3);
    fseek(fp, 0, SEEK_SET);
    if (0 == (readBytes = fread(pJsonConfig, 1, currentDMSize, fp)))  // Any error here will be picked up later
    {
        free(pJsonConfig);
        fclose(fp);
        return NULL;
    }
    fclose(fp);
    if (NULL == (config = cJSON_Parse(pJsonConfig)))
    {
        cJSON_Delete(config);
        free(pJsonConfig);
        return NULL;
    }

    free(pJsonConfig);

    return config;
}

bool qos_DmSave(const char *pTableName, uint32_t entryIdx, const char *pValueName, Value_t *pParam)
{
    cJSON *config = NULL, *currObj = NULL, *tmpObj = NULL;
    uint32_t currentDMSize, readBytes;

    config = json_load_config();

    if(!config)
    {
        printf("Can't open JSON config file\n");
        return false;
    }

    if (NULL != (currObj = json_provide_object(config, DM_QOS)))
    {
        if (NULL != (currObj = json_provide_object(currObj, pTableName)))
        {
            if(json_set_value(pValueName, cJSON_GetArrayItem(currObj, entryIdx), pParam))
            {
                json_save_config_file(config);
                cJSON_Delete(config);
                return true;
            }
        }
        else
        {
            printf("No %s section in JSON file\n", pTableName);
        } 
    }
    else
    {
        printf("No Qos section in JSON file???\n");
    } 

    printf("Unrecognised type: %s\n", pTableName);
    cJSON_Delete(config);
    return false;
}

bool qos_DmSaveQoS(QoS_t *pQos)
{
    cJSON *root, *qos, *clfs, *cl, *queues, *qu;
    Classification_t *pClf;
    Queue_t *pQueue;

    root = cJSON_CreateObject();
    qos = cJSON_CreateObject();

    cJSON_AddItemToObject(root, DM_QOS, qos);

    if(v_count(&pQos->cl) > 0)
    {
        clfs = cJSON_CreateArray();

        cJSON_AddItemToObject(qos, DM_CLF, clfs);

        for (ulong i = 0; i < v_count(&pQos->cl); i++)
        {
            cJSON_AddItemToArray(clfs, cl = cJSON_CreateObject());

            pClf = V_GET(pQos->cl, Classification_t*, i);

            cJSON_AddItemToObject(cl, DM_CLF_Enable,               cJSON_CreateBool  (pClf->Enable));
            cJSON_AddItemToObject(cl, DM_CLF_DestIP,               cJSON_CreateString(pClf->DestIP));
            cJSON_AddItemToObject(cl, DM_CLF_DestMask,             cJSON_CreateString(pClf->DestMask));
            cJSON_AddItemToObject(cl, DM_CLF_SourcePort,           cJSON_CreateNumber(pClf->SourcePort));
            cJSON_AddItemToObject(cl, DM_CLF_SourcePortRangeMax,   cJSON_CreateNumber(pClf->SourcePortRangeMax));
            cJSON_AddItemToObject(cl, DM_CLF_DestPort,             cJSON_CreateNumber(pClf->DestPort));
            cJSON_AddItemToObject(cl, DM_CLF_DestPortRangeMax,     cJSON_CreateNumber(pClf->DestPortRangeMax));
            cJSON_AddItemToObject(cl, DM_CLF_Protocol,             cJSON_CreateNumber(pClf->Protocol));
            cJSON_AddItemToObject(cl, DM_CLF_DSCPMark,             cJSON_CreateNumber(pClf->DSCPMark));
            cJSON_AddItemToObject(cl, DM_CLF_TrafficClass,         cJSON_CreateNumber(pClf->TrafficClass));
            cJSON_AddItemToObject(cl, DM_CLF_SourceIP,             cJSON_CreateString(pClf->SourceIP));
            cJSON_AddItemToObject(cl, DM_CLF_SourceMask,           cJSON_CreateString(pClf->SourceMask));
            cJSON_AddItemToObject(cl, DM_CLF_SourceMACAddress,     cJSON_CreateString(pClf->SourceMACAddress));
            cJSON_AddItemToObject(cl, DM_CLF_ChainName,            cJSON_CreateString(pClf->ChainName));
            cJSON_AddItemToObject(cl, DM_CLF_IfaceIn,              cJSON_CreateString(pClf->IfaceIn));
            cJSON_AddItemToObject(cl, DM_CLF_IfaceOut,             cJSON_CreateString(pClf->IfaceOut));
            cJSON_AddItemToObject(cl, DM_CLF_TcpFlags,             cJSON_CreateNumber(pClf->TcpFlags));
            cJSON_AddItemToObject(cl, DM_CLF_TcpPsh,               cJSON_CreateNumber(pClf->TcpPsh));
        }
    }
    if(v_count(&pQos->qu) > 0)
    {
        queues = cJSON_CreateArray();

        cJSON_AddItemToObject(qos, DM_QUEUE, queues);

        for (ulong i = 0; i < v_count(&pQos->qu); i++)
        {
            cJSON_AddItemToArray(queues, qu = cJSON_CreateObject());

            pQueue = V_GET(pQos->qu, Queue_t*, i);

            cJSON_AddItemToObject(qu, DM_QUEUE_Enable,             cJSON_CreateBool  (pQueue->Enable));
            cJSON_AddItemToObject(qu, DM_QUEUE_TrafficClasses,     cJSON_CreateString(pQueue->TrafficClasses));
            cJSON_AddItemToObject(qu, DM_QUEUE_Interface,          cJSON_CreateString(pQueue->Interface));
            cJSON_AddItemToObject(qu, DM_QUEUE_Weight,             cJSON_CreateNumber(pQueue->Weight));
            cJSON_AddItemToObject(qu, DM_QUEUE_Precedence,         cJSON_CreateNumber(pQueue->Precedence));
            cJSON_AddItemToObject(qu, DM_QUEUE_SchedulerAlgorithm, cJSON_CreateNumber(pQueue->SchedulerAlgorithm));
            cJSON_AddItemToObject(qu, DM_QUEUE_ShapingRate,        cJSON_CreateNumber(pQueue->ShapingRate));
        }
    }

    json_save_config_file(root);

    cJSON_Delete(root);

    return true;
}

bool json_set_value(const char *pValueName, cJSON *pNode, Value_t *pParam)
{
    if (pNode == NULL)
        return false;

    cJSON *pLeaf = NULL, *pNewValue = NULL;
    if ( NULL != (pLeaf = cJSON_GetObjectItem(pNode, pValueName) ))
    {
        if (cJSON_IsBool(pLeaf))
        {
            pNewValue = cJSON_CreateBool(pParam->boolean);
        }
        if (cJSON_IsString(pLeaf))
        {
            pNewValue = cJSON_CreateString(pParam->pValString);
        }
        if (cJSON_IsNumber(pLeaf))
        {
            pNewValue = cJSON_CreateNumber(pParam->integer);
        }
        cJSON_ReplaceItemInObject(pNode, pValueName, pNewValue);
        return true;
    }
    else
    {
        printf("Unknown parameter type for %s\n", pValueName);
        return false;  // No leaf in strucucture??
    }
}

cJSON_bool json_save_config_file(cJSON *obj)
{
    char *pJSON;
    int ret = 0;
    unsigned int confSize = 0;
    int fdNew;

    if (NULL == (pJSON = cJSON_Print(obj) ) )
    {
        printf("\n\nsaveCurrentFile error, JSON is <null>\n\n");
        return false;
    }
    confSize = strlen(pJSON);

    if ( -1 == (fdNew = open(QOS_CONFIG_CURRENT_PATH QOS_CONFIG_CURRENT_NAME, 
        O_WRONLY | O_SYNC | O_TRUNC | O_CREAT)))
    {
        printf("Could not open current file %s for writing!\n", 
            QOS_CONFIG_CURRENT_PATH QOS_CONFIG_CURRENT_NAME);
        free(pJSON);
        return false;
    }   
    /* Write out the memory image to current */
    ret = write(fdNew, pJSON, confSize);
    if (ret == confSize)
    {
        free(pJSON);        //Finished with this
        fsync(fdNew);       //Should not return until data close to flash
        close(fdNew);
        sync();
        return true;
        
    }
    else
    {
        printf("Could not update current file %s!\n", 
                      QOS_CONFIG_CURRENT_PATH QOS_CONFIG_CURRENT_NAME);
        free(pJSON);        //Finished with this
        close(fdNew);
        //next time we save try to recreate these files however if this api isnt called we accept losing the running config
        //as a consequence of a call to open with O_TRUNC the file was lost anyway
        unlink(QOS_CONFIG_CURRENT_PATH QOS_CONFIG_CURRENT_NAME);
        return false;
    }
}
