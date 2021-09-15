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

#include "qos_dm_api.h"
#include "qos_dm_load.h"
#include "hal/classification.h"
#include "hal/queue.h"

QoS_t g_QoS = {0};

typedef struct qos_class qos_class_t;
typedef struct qos_queue qos_queue_t;

QoS_t* get_QoS()
{
    return &g_QoS;
}

typedef enum {
    Q_NODE_TYPE_CLASSIFICATION,
    Q_NODE_TYPE_QUEUE
} QosNodeType_t;

ulong qos_GetNewInstanceNumber(QosNodeType_t type)
{
    QoS_t* qos = get_QoS();

    switch (type)
    {
    case Q_NODE_TYPE_CLASSIFICATION:
        return qos->classInstanceCounter++;
    case Q_NODE_TYPE_QUEUE:
        return qos->queueInstanceCounter++;
    default:
        break;
    }

    return 0;
}

void qos_Init()
{
    QoS_t *qos = get_QoS();

    v_init(&qos->qu, 10);
    v_init(&qos->cl, 10);

    qos->classInstanceCounter = 1;
    qos->queueInstanceCounter = 1;
}

int parse_list_of_int(char *input, int *output, uint outputMaxSize )
{
    int count  = 0;
    char* pch    = strtok(input,", ");

    while (pch != NULL)
    {
        output[count++] = atoi(pch);
        pch = strtok(NULL,", ");

        if(count >= outputMaxSize)
            break;
    }

    return count;
}

void fill_qos_queue(qos_queue_t *pHalQu, const Queue_t *pQu)
{
    CHAR trafficClasses[256] = {0};
    strncpy(trafficClasses, pQu->TrafficClasses, sizeof(trafficClasses));
    pHalQu->class_size = parse_list_of_int(trafficClasses, pHalQu->class_list, QUEUE_MAX_TC);
    strncpy(pHalQu->device_name, pQu->Interface, sizeof(pHalQu->device_name));

    if(pQu->SchedulerAlgorithm == QOS_QUEUE_SCHEDULERALGORITHM_SP)
        pHalQu->alg = QUEUE_ALG_SP;
    else if (pQu->SchedulerAlgorithm == QOS_QUEUE_SCHEDULERALGORITHM_WRR)
        pHalQu->alg = QUEUE_ALG_WRR;
    else if (pQu->SchedulerAlgorithm == QOS_QUEUE_SCHEDULERALGORITHM_WFQ)
        pHalQu->alg = QUEUE_ALG_SP;

    pHalQu->priority = pQu->Precedence;
    pHalQu->weight = pQu->Weight;
    pHalQu->shaping_rate = pQu->ShapingRate;
}

int apply_qos_queue(Queue_t * pQueue)
{
    QoS_t *qos = get_QoS();
    qos_queue_t que = {0};
    ULONG i;

    if (qos_QueueItemIndex(pQueue, &i) == ANSC_STATUS_SUCCESS)
    {
        fill_qos_queue(&que, pQueue);

        if(pQueue->Enable)
            return qos_addQueue((int)i, &que);
        else
            return qos_removeQueue(&que);
    }

    return 0;
}

int remove_qos_queue(Queue_t * pQueue)
{
    if(!pQueue)
        return -1;

    qos_queue_t qu;
    fill_qos_queue(&qu, pQueue);

    return qos_removeQueue(&qu);
}

void qos_CommitQueue(Queue_t * pQueue)
{
    QoS_t *qos = get_QoS();

    if(apply_qos_queue(pQueue) == 0)
        printf("Success. Add Device.QoS.Queue.\n");
    else
        printf("Fail. Add Device.QoS.Queue.\n");

    qos_DmSaveQoS(qos);
}

int qos_ApplyQueues()
{
    QoS_t *qos = get_QoS();
    Queue_t *pQu = NULL;
    qos_queue_t qu;

    for (ulong i = 0; i < v_count(&qos->qu); i++)
    {
        pQu = V_GET(qos->qu, Queue_t*, i);
        fill_qos_queue(&qu, pQu);
        remove_qos_queue(pQu);
    }

    for (ulong i = 0; i < v_count(&qos->qu); i++)
    {
        pQu = V_GET(qos->qu, Queue_t*, i);

        if(!pQu->Enable)
            continue;

        fill_qos_queue(&qu, pQu);

        if(qos_addQueue(i, &qu) != 0)
        {
            printf("Fail. Add qos queue \n");
            return -1;
        }
    }

    return 0;
}

void qos_QueueFillDefaults(Queue_t * pQueue)
{
    pQueue->Enable = false;
    pQueue->Weight = 0;
    pQueue->Precedence = 1;
    pQueue->SchedulerAlgorithm = QOS_QUEUE_SCHEDULERALGORITHM_SP;
    pQueue->ShapingRate = -1;
}

ANSC_STATUS qos_QueueEntryCount(ULONG *pCount)
{
    ANSC_STATUS returnStatus = ANSC_STATUS_SUCCESS;
    QoS_t *qos = get_QoS();

    *pCount = (ULONG)v_count(&qos->qu);

    return returnStatus;
}

ANSC_STATUS qos_QueueGetEntry(ULONG entryIdx, Queue_t **ppClass)
{
    ANSC_STATUS returnStatus = ANSC_STATUS_SUCCESS;

    QoS_t *qos = get_QoS();

    if (entryIdx < v_count(&qos->qu))
        *ppClass = V_GET(qos->qu, Queue_t*, entryIdx);

    return returnStatus;
}

ANSC_STATUS  qos_QueueGetEntryInstance(ulong instanceNumber, Queue_t **ppQue)
{
    ANSC_STATUS returnStatus = ANSC_STATUS_FAILURE;
    QoS_t* qos = get_QoS();
    Queue_t* pQue = NULL;

    for(ulong i = 0; i < v_count(&qos->qu); i++)
    {
        pQue = V_GET(qos->qu,Queue_t*,i);

        if(pQue && pQue->srv_instanceNumber == instanceNumber)
         {
            *ppQue = pQue;
            returnStatus = ANSC_STATUS_SUCCESS;
         }
    }

    return returnStatus;
}

ANSC_STATUS qos_QueueAddEntry(Queue_t *pQue)
{
    ANSC_STATUS returnStatus = ANSC_STATUS_SUCCESS;

    QoS_t *qos = get_QoS();

    Queue_t *newQue = (Queue_t*) AnscAllocateMemory(sizeof(Queue_t));

    memcpy(newQue, pQue, sizeof(Queue_t));

    newQue->srv_instanceNumber = qos_GetNewInstanceNumber(Q_NODE_TYPE_QUEUE);
    pQue->srv_instanceNumber = newQue->srv_instanceNumber;

    V_ADD(qos->qu, newQue);

    return returnStatus;
}

ANSC_STATUS qos_QueueDeleteEntryItem(Queue_t *pQue)
{
    ANSC_STATUS returnStatus = ANSC_STATUS_FAILURE;
    QoS_t *qos = get_QoS();
    int entryIdx = v_itemIndex(&qos->qu, pQue);

    if(entryIdx > -1)
    {
        remove_qos_queue(pQue);

        AnscFreeMemory(V_GET(qos->qu, void*, entryIdx));
        v_delete(&qos->qu, entryIdx);

        qos_ApplyQueues();

        returnStatus = ANSC_STATUS_SUCCESS;
    }

    return returnStatus;
}

ANSC_STATUS qos_QueueItemIndex(Queue_t *pClass, ULONG *pEntryIdx)
{
    ANSC_STATUS returnStatus = ANSC_STATUS_FAILURE;
    QoS_t *qos = get_QoS();
    int entryIdx = -1;

    entryIdx = v_itemIndex(&qos->qu, pClass);

    if(entryIdx > -1) {
        *pEntryIdx = (ULONG)entryIdx;
        returnStatus = ANSC_STATUS_SUCCESS;
    }

    return returnStatus;
}

void fill_qos_class(qos_class_t *pHalClf, const Classification_t *pClf, ulong idx)
{
    pHalClf->id = idx;
    pHalClf->traffic_class = pClf->TrafficClass;
    strncpy(pHalClf->ip_src_addr, pClf->SourceIP, sizeof(pHalClf->ip_src_addr));
    pHalClf->ip_src_mask = atoi(pClf->SourceMask);
    strncpy(pHalClf->ip_dst_addr, pClf->DestIP, sizeof(pHalClf->ip_dst_addr));
    pHalClf->ip_dst_mask = atoi(pClf->DestMask);
    pHalClf->protocol = pClf->Protocol;
    pHalClf->port_src_range_start = pClf->SourcePort;
    pHalClf->port_src_range_end = pClf->SourcePortRangeMax;
    pHalClf->port_dst_range_start = pClf->DestPort;
    pHalClf->port_dst_range_end = pClf->DestPortRangeMax;
    strncpy(pHalClf->mac_src_addr, pClf->SourceMACAddress, sizeof(pHalClf->mac_src_addr));
    pHalClf->dscp_mark = pClf->DSCPMark;
    strncpy(pHalClf->chain_name, pClf->ChainName, sizeof(pHalClf->chain_name));
    strncpy(pHalClf->iface_in, pClf->IfaceIn, sizeof(pHalClf->iface_in));
    strncpy(pHalClf->iface_out, pClf->IfaceOut, sizeof(pHalClf->iface_out));
    pHalClf->tcp_flags = pClf->TcpFlags;
    pHalClf->tcp_psh = pClf->TcpPsh;
}

int apply_qos_class(Classification_t *pClf)
{
    QoS_t *qos = get_QoS();
    qos_class_t qclf = {0};

    if(!pClf->Enable)
        return -2;

    fill_qos_class(&qclf, pClf,  v_itemIndex(&qos->cl, pClf));

    if(qos_addClass(&qclf) != 0) {
        return -1;
    }

    return 0;
}

void qos_CommitClassification(Classification_t * pClf)
{
    QoS_t *qos = get_QoS();

    if(apply_qos_class(pClf) == 0)
        printf("Success. Add Device.QoS.Classification.\n");
    else
        printf("Fail. Add Device.QoS.Classification.\n");

    qos_DmSaveQoS(qos);
}

int qos_ApplyClassifications()
{
    QoS_t *qos = get_QoS();
    qos_class_t qclf = {0};
    Classification_t *pClfItr = NULL;

    qos_removeAllClasses();

    for (ulong i = 0; i < v_count(&qos->cl); i++)
    {
        pClfItr = V_GET(qos->cl, Classification_t*, i);

        if(!apply_qos_class(pClfItr))
            return -1;
    }

    return 0;
}

ANSC_STATUS qos_ClassificationEntryCount(ULONG *pCount)
{
    ANSC_STATUS returnStatus = ANSC_STATUS_SUCCESS;
    QoS_t *qos = get_QoS();

    *pCount = (ULONG)v_count(&qos->cl);

    return returnStatus;
}

ANSC_STATUS qos_ClassificationGetEntry(ULONG entryIdx, Classification_t **ppClass)
{
    ANSC_STATUS returnStatus = ANSC_STATUS_SUCCESS;

    QoS_t *qos = get_QoS();

    if (entryIdx < v_count(&qos->cl))
        *ppClass = V_GET(qos->cl, Classification_t*, entryIdx);

    return returnStatus;
}

ANSC_STATUS  qos_ClassificationGetEntryInstance(ulong instanceNumber, Classification_t **ppClf)
{
    ANSC_STATUS returnStatus = ANSC_STATUS_FAILURE;
    QoS_t* qos = get_QoS();
    Classification_t* pClf = NULL;

    for(ulong i = 0; i < v_count(&qos->cl); i++)
    {
        pClf = V_GET(qos->cl,Classification_t*,i);

        if(pClf && pClf->srv_instanceNumber == instanceNumber)
        {
            *ppClf = pClf;
            returnStatus = ANSC_STATUS_SUCCESS;
        }
    }

    return returnStatus;
}

ANSC_STATUS qos_ClassificationAddEntry(Classification_t *pClass)
{
    ANSC_STATUS returnStatus = ANSC_STATUS_SUCCESS;

    QoS_t *qos = get_QoS();

    Classification_t *newClf = (Classification_t*) AnscAllocateMemory(sizeof(Classification_t));

    memcpy(newClf, pClass, sizeof(Classification_t));

    newClf->srv_instanceNumber = qos_GetNewInstanceNumber(Q_NODE_TYPE_CLASSIFICATION);
    pClass->srv_instanceNumber = newClf->srv_instanceNumber;

    V_ADD(qos->cl, newClf);

    return returnStatus;
}

ANSC_STATUS qos_ClassificationDeleteEntryItem(Classification_t *pClass)
{
    ANSC_STATUS returnStatus = ANSC_STATUS_FAILURE;
    QoS_t *qos = get_QoS();
    int entryIdx = v_itemIndex(&qos->cl, pClass);

    if(entryIdx > -1)
    {
        AnscFreeMemory(V_GET(qos->cl, void*, entryIdx));
        v_delete(&qos->cl, entryIdx);

        qos_ApplyClassifications();

        returnStatus = ANSC_STATUS_SUCCESS;
    }

    return returnStatus;
}

ANSC_STATUS qos_ClassificationItemIndex(Classification_t *pClass, ULONG *pEntryIdx)
{
    ANSC_STATUS returnStatus = ANSC_STATUS_FAILURE;
    QoS_t *qos = get_QoS();
    int entryIdx = -1;

    entryIdx = v_itemIndex(&qos->cl, pClass);

    if(entryIdx > -1) {
        *pEntryIdx = (ULONG)entryIdx;
        returnStatus = ANSC_STATUS_SUCCESS;
    }

    return returnStatus;
}
