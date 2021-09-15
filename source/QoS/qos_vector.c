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

#include <stdio.h>
#include <stdlib.h>
#include "qos_vector.h"

void v_init(vector_t *v, ulong capacity)
{
    v->capacity = capacity;
    v->count = 0;
    v->items = malloc(sizeof(void *) * v->capacity);
}

ulong v_count(vector_t *v)
{
    return v->count;
}

int v_itemIndex(vector_t *v, void *item)
{
    for (ulong i = 0; i < v->count; i++)
    {
        if(v->items[i] == item)
            return i;
    }
    return -1;
}

static void v_resize(vector_t *v, ulong capacity)
{
    void **items = realloc(v->items, sizeof(void *) * capacity);
    if (items)
    {
        v->items = items;
        v->capacity = capacity;
    }
}

void v_add(vector_t *v, void *item)
{
    if (v->capacity == v->count)
        v_resize(v, v->capacity * 2);

    v->items[v->count++] = item;
}

void v_set(vector_t *v, ulong index, void *item)
{
    if (index >= 0 && index < v->count)
        v->items[index] = item;
}

void *v_get(vector_t *v, ulong index)
{
    if (index >= 0 && index < v->count)
        return v->items[index];

    return NULL;
}

void v_delete(vector_t *v, ulong index)
{
    if (index < 0 || index >= v->count)
        return;

    v->items[index] = NULL;

    for (ulong i = index; i < v->count - 1; i++)
    {
        v->items[i] = v->items[i + 1];
        v->items[i + 1] = NULL;
    }

    v->count--;

    if (v->count > 0 && v->count == v->capacity / 4)
        v_resize(v, v->capacity / 2);
}

void v_free(vector_t *v)
{
    free(v->items);
}
