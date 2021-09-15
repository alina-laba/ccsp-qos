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

#ifndef QOS_DM_API_H
#define QOS_DM_API_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>
#include "ansc_platform.h"
#include "ansc_string_util.h"
#include "qos_vector.h"

enum {
    QOS_QUEUE_STATUS_Disabled           = 1,
    QOS_QUEUE_STATUS_Enabled,
    QOS_QUEUE_STATUS_Error_Misconfigured,
    QOS_QUEUE_STATUS_Error
};


enum {
    QOS_QUEUE_DROPALGORITHM_RED = 1,
    QOS_QUEUE_DROPALGORITHM_DT,
    QOS_QUEUE_DROPALGORITHM_WRED,
    QOS_QUEUE_DROPALGORITHM_BLUE
};

enum {
    QOS_QUEUE_SCHEDULERALGORITHM_WFQ = 1,
    QOS_QUEUE_SCHEDULERALGORITHM_WRR,
    QOS_QUEUE_SCHEDULERALGORITHM_SP,
};


typedef struct {
    BOOL Enable;
    CHAR TrafficClasses[256];
    CHAR Interface[256];
    UINT Weight;
    UINT Precedence;
    UINT SchedulerAlgorithm;
    INT ShapingRate;

    // Service member
    ULONG srv_instanceNumber;
} Queue_t;

typedef struct {
    BOOL Enable;
    CHAR DestIP[45];
    CHAR DestMask[49];
    CHAR SourceIP[45];
    CHAR SourceMask[49];
    INT SourcePort;
    CHAR SourceMACAddress[17];
    INT SourcePortRangeMax;
    INT DestPort;
    INT DestPortRangeMax;
    INT Protocol;
    INT DSCPMark;
    INT TrafficClass;

    // Custom
    CHAR ChainName[16];
    CHAR IfaceIn[16];
    CHAR IfaceOut[16];
    INT TcpFlags;
    INT TcpPsh;

    // Service member
    ULONG srv_instanceNumber;
} Classification_t;

typedef struct {
    vector_t qu;
    vector_t cl;

    ulong classInstanceCounter;
    ulong queueInstanceCounter;
} QoS_t;

void qos_Init();

void qos_CommitQueue(Queue_t * pQueue);
int qos_ApplyQueues();
void qos_QueueFillDefaults(Queue_t * pQueue);

ANSC_STATUS qos_QueueEntryCount(ULONG *pCount);
ANSC_STATUS qos_QueueGetEntry(ULONG entryIdx, Queue_t **ppClass);
ANSC_STATUS qos_QueueGetEntryInstance(ulong instanceNumber, Queue_t **ppQue);
ANSC_STATUS qos_QueueAddEntry(Queue_t *pClass);
ANSC_STATUS qos_QueueDeleteEntryItem(Queue_t *pClass);
ANSC_STATUS qos_QueueItemIndex(Queue_t *pClass, ULONG *pEntryIdx);

void qos_CommitClassification(Classification_t * pClf);
int qos_ApplyClassifications();

ANSC_STATUS qos_ClassificationEntryCount(ULONG *pCount);
ANSC_STATUS qos_ClassificationGetEntry(ULONG entryIdx, Classification_t **ppClass);
ANSC_STATUS qos_ClassificationGetEntryInstance(ulong instanceNumber, Classification_t **ppClf);
ANSC_STATUS qos_ClassificationAddEntry(Classification_t *pClass);
ANSC_STATUS qos_ClassificationDeleteEntryItem(Classification_t *pClass);
ANSC_STATUS qos_ClassificationItemIndex(Classification_t *pClass, ULONG *pEntryIdx);

#endif
