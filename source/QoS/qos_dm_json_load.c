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

#include "cjson/cJSON.h"
#include <rdk_error.h>
#include <rdk_debug.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include "ccsp_trace.h"

#include "qos_dm_load.h"
#include "qos_dm_api.h"

static int32_t json_parse_dm_defaults(void);
static void json_parse_Classification(uint32_t index, cJSON *classification);
static void json_parse_Queue(uint32_t index, cJSON *queue);
static void qos_copy_dm_config(char *src, char *dst);

int32_t qos_DmLoadDefaults(void)
{
    struct stat buffer;
    int32_t ret = -1;

    qos_Init();

    if (0 == stat( QOS_CONFIG_CURRENT_PATH QOS_CONFIG_CURRENT_NAME, &buffer ))
        ret = json_parse_dm_defaults();

    if (-1 == ret)
    {
        char filename[MAX_FILENAME_LENGTH] = {0};

        snprintf(filename, MAX_FILENAME_LENGTH, "%s%s", QOS_CONFIG_DEFAULT_PATH, QOS_CONFIG_DEFAULT_NAME);

        printf("copying %s to %s\n", filename, QOS_CONFIG_CURRENT_PATH QOS_CONFIG_CURRENT_NAME);

        qos_copy_dm_config(filename, QOS_CONFIG_CURRENT_PATH QOS_CONFIG_CURRENT_NAME);

        ret = json_parse_dm_defaults();
    }

    if(ret == 0)
    {
        printf("Apply qos configuration\n");
        qos_ApplyClassifications();
        qos_ApplyQueues();
    }
    else
    {
        printf("Fail. Can't load qos configuration\n");
    }

    return ret;
}

static int32_t json_parse_dm_defaults()
{
    cJSON *config = NULL, *qos = NULL, *classification = NULL, *queue = NULL;
    uint32_t numClassifications, numQueues, defaultDMSize, i, readBytes; void* pJsonConfig = NULL; FILE* fp;

    printf("%s:%s \n", __FILE__, __FUNCTION__);

    if (NULL == (fp = fopen(QOS_CONFIG_CURRENT_PATH QOS_CONFIG_CURRENT_NAME, "r")))
    {
        printf("Failed to open JSON defaults file %s\n", QOS_CONFIG_CURRENT_PATH QOS_CONFIG_CURRENT_NAME);
        return -1;
    }

    fseek(fp, 0, SEEK_END);

    pJsonConfig = malloc(defaultDMSize = ftell(fp));

    if (NULL == pJsonConfig)
    {
        printf("Failed to get memory for JSON file\n");
        fclose(fp);
        return -2;
    }
    fseek(fp, 0, SEEK_SET);
    if (0 == (readBytes = fread(pJsonConfig, 1, defaultDMSize, fp)))
    {
        printf("Failed to read JSON file %s of size %d. Read %d. errno %d \n",
            QOS_CONFIG_CURRENT_PATH QOS_CONFIG_CURRENT_NAME, defaultDMSize, readBytes, errno);
    }
    fclose(fp);

    if (NULL == (config = cJSON_Parse(pJsonConfig)))
    {
        printf("Failed to parse JSON file, error is at %p, start is at %p\n", cJSON_GetErrorPtr(), pJsonConfig);
        free(pJsonConfig);
        return -1;
    }

    free(pJsonConfig);

    if (NULL == (qos = cJSON_GetObjectItemCaseSensitive(config, DM_QOS)))
    {
        cJSON_Delete(config);
        return -1;
    }

    if (NULL != (classification = cJSON_GetObjectItemCaseSensitive(qos, DM_CLF)))
    {
        numClassifications = (uint32_t)cJSON_GetArraySize(classification);
        for ( i=0 ; i<numClassifications ; i++ )
        {
            json_parse_Classification(i, cJSON_GetArrayItem(classification, (uint32_t)i));
        }
    }

    if (NULL != (queue = cJSON_GetObjectItemCaseSensitive(qos, DM_QUEUE)))
    {
        numQueues = (uint32_t)cJSON_GetArraySize(queue);
        for ( i=0 ; i < numQueues ; i++ )
        {
            json_parse_Queue(i, cJSON_GetArrayItem(queue, (uint32_t)i));
        }
    }

    cJSON_Delete(config);
    printf("QoS. JSON parsed successfully!\n");
    return 0;
}

static void qos_copy_dm_config(char *src, char *dst)
{
    char            buffer[512];
    size_t          n;
    FILE *fpSrc, *fpDst;

    if (NULL == (fpSrc = fopen(src, "r")))
    {
        printf("Failed to open source %s for copy\n", src);
        return;
    }
    if (NULL == (fpDst = fopen(dst, "w")))
    {
        printf("Failed to open destination %s for copy, errno is %d\n", dst, errno);
        fclose(fpSrc);
        return;
    }
    while ((n = fread(buffer, sizeof(char), sizeof(buffer), fpSrc)) > 0)
    {
        if (fwrite(buffer, sizeof(char), n, fpDst) != n)
            printf("Copy failed\n");
    }
    fclose(fpSrc);
    fsync(fileno(fpDst));
    fclose(fpDst);
    sync();
    printf("Copy %s->%s succeded\n", src, dst);
}

#define ARR_NUM_OF_ELEMS(a) (sizeof(a)/sizeof(a[0]))

static void json_parse_Classification(uint32_t index, cJSON *pClassificationObj)
{
    cJSON *vsItem = NULL;
    Classification_t newClassification = {0};
    
    if (NULL == pClassificationObj)
    {
        printf("%s: NULL JSON object found!\n", __FUNCTION__);
        return;
    }

    if (NULL != (vsItem = cJSON_GetObjectItemCaseSensitive(pClassificationObj, DM_CLF_Enable)))
        newClassification.Enable = vsItem->valueint;

    if (NULL != (vsItem = cJSON_GetObjectItemCaseSensitive(pClassificationObj, DM_CLF_DestIP)))
        strncpy(newClassification.DestIP, vsItem->valuestring, ARR_NUM_OF_ELEMS(newClassification.DestIP));

    if (NULL != (vsItem = cJSON_GetObjectItemCaseSensitive(pClassificationObj, DM_CLF_DestMask)))
        strncpy(newClassification.DestMask, vsItem->valuestring, ARR_NUM_OF_ELEMS(newClassification.DestMask));

    if (NULL != (vsItem = cJSON_GetObjectItemCaseSensitive(pClassificationObj, DM_CLF_SourcePort)))
        newClassification.SourcePort = vsItem->valueint;

    if (NULL != (vsItem = cJSON_GetObjectItemCaseSensitive(pClassificationObj, DM_CLF_SourcePortRangeMax)))
        newClassification.SourcePortRangeMax = vsItem->valueint;

    if (NULL != (vsItem = cJSON_GetObjectItemCaseSensitive(pClassificationObj, DM_CLF_DestPort)))
        newClassification.DestPort = vsItem->valueint;

    if (NULL != (vsItem = cJSON_GetObjectItemCaseSensitive(pClassificationObj, DM_CLF_DestPortRangeMax)))
        newClassification.DestPortRangeMax = vsItem->valueint;

    if (NULL != (vsItem = cJSON_GetObjectItemCaseSensitive(pClassificationObj, DM_CLF_Protocol)))
        newClassification.Protocol = vsItem->valueint;

    if (NULL != (vsItem = cJSON_GetObjectItemCaseSensitive(pClassificationObj, DM_CLF_DSCPMark)))
        newClassification.DSCPMark = vsItem->valueint;

    if (NULL != (vsItem = cJSON_GetObjectItemCaseSensitive(pClassificationObj, DM_CLF_TrafficClass)))
        newClassification.TrafficClass = vsItem->valueint;

    if (NULL != (vsItem = cJSON_GetObjectItemCaseSensitive(pClassificationObj, DM_CLF_SourceIP)))
        strncpy(newClassification.SourceIP, vsItem->valuestring, ARR_NUM_OF_ELEMS(newClassification.SourceIP));

    if (NULL != (vsItem = cJSON_GetObjectItemCaseSensitive(pClassificationObj, DM_CLF_SourceMask)))
        strncpy(newClassification.SourceMask, vsItem->valuestring, ARR_NUM_OF_ELEMS(newClassification.SourceMask));

    if (NULL != (vsItem = cJSON_GetObjectItemCaseSensitive(pClassificationObj, DM_CLF_SourceMACAddress)))
        strncpy(newClassification.SourceMACAddress, vsItem->valuestring, ARR_NUM_OF_ELEMS(newClassification.SourceMACAddress));

    if (NULL != (vsItem = cJSON_GetObjectItemCaseSensitive(pClassificationObj, DM_CLF_ChainName)))
        strncpy(newClassification.ChainName, vsItem->valuestring, ARR_NUM_OF_ELEMS(newClassification.ChainName));

    if (NULL != (vsItem = cJSON_GetObjectItemCaseSensitive(pClassificationObj, DM_CLF_IfaceIn)))
        strncpy(newClassification.IfaceIn, vsItem->valuestring, ARR_NUM_OF_ELEMS(newClassification.IfaceIn));

    if (NULL != (vsItem = cJSON_GetObjectItemCaseSensitive(pClassificationObj, DM_CLF_IfaceOut)))
        strncpy(newClassification.IfaceOut, vsItem->valuestring, ARR_NUM_OF_ELEMS(newClassification.IfaceOut));

    if (NULL != (vsItem = cJSON_GetObjectItemCaseSensitive(pClassificationObj, DM_CLF_TcpFlags)))
        newClassification.TcpFlags = vsItem->valueint;

    if (NULL != (vsItem = cJSON_GetObjectItemCaseSensitive(pClassificationObj, DM_CLF_TcpPsh)))
        newClassification.TcpPsh = vsItem->valueint;

    qos_ClassificationAddEntry(&newClassification);
}

static void json_parse_Queue(uint32_t index, cJSON *pQueueObj)
{
    cJSON *vsItem = NULL;
    Queue_t newQueue = {0};

    if (NULL == pQueueObj)
    {
        printf("%s: NULL JSON object found!\n", __FUNCTION__);
        return;
    }

    if (NULL != (vsItem = cJSON_GetObjectItemCaseSensitive(pQueueObj, DM_QUEUE_Enable)))
        newQueue.Enable = vsItem->valueint;

    if (NULL != (vsItem = cJSON_GetObjectItemCaseSensitive(pQueueObj, DM_QUEUE_TrafficClasses)))
        strncpy(newQueue.TrafficClasses, vsItem->valuestring, ARR_NUM_OF_ELEMS(newQueue.TrafficClasses));

    if (NULL != (vsItem = cJSON_GetObjectItemCaseSensitive(pQueueObj, DM_QUEUE_Interface)))
        strncpy(newQueue.Interface, vsItem->valuestring, ARR_NUM_OF_ELEMS(newQueue.Interface));

    if (NULL != (vsItem = cJSON_GetObjectItemCaseSensitive(pQueueObj, DM_QUEUE_Weight)))
        newQueue.Weight = vsItem->valueint;

    if (NULL != (vsItem = cJSON_GetObjectItemCaseSensitive(pQueueObj, DM_QUEUE_Precedence)))
        newQueue.Precedence = vsItem->valueint;

    if (NULL != (vsItem = cJSON_GetObjectItemCaseSensitive(pQueueObj, DM_QUEUE_SchedulerAlgorithm)))
        newQueue.SchedulerAlgorithm = vsItem->valueint;

    if (NULL != (vsItem = cJSON_GetObjectItemCaseSensitive(pQueueObj, DM_QUEUE_ShapingRate)))
        newQueue.ShapingRate = vsItem->valueint;

    qos_QueueAddEntry(&newQueue);
}
