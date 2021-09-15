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

#ifndef VECTOR_H
#define VECTOR_H

#define V_ADD(vec, item) v_add(&vec, (void *) item)
#define V_GET(vec, type, id) (type) v_get(&vec, id)

typedef struct vector_t
{
    void **items;
    ulong capacity;
    ulong count;
} vector_t;

void v_init(vector_t*, ulong);
ulong v_count(vector_t*);
int v_itemIndex(vector_t*, void*);
static void v_resize(vector_t*, ulong);
void v_add(vector_t*, void*);
void v_set(vector_t*, ulong, void*);
void *v_get(vector_t*, ulong);
void v_delete(vector_t*, ulong);
void v_free(vector_t*);

#endif
