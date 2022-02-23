/* HTTP Parse : Allocator
 * Copyright(c) 2016 y2c2 */

#ifndef HTTPPARSE_ALLOCATOR_H
#define HTTPPARSE_ALLOCATOR_H

#include "httpparse_dt.h"

typedef void *(*hp_malloc_cb_t)(hp_size_t size);
typedef void (*hp_free_cb_t)(void *ptr);

void hp_allocator_set_malloc(hp_malloc_cb_t cb);
void hp_allocator_set_free(hp_free_cb_t cb);

void *hp_malloc(hp_size_t size);
void hp_free(void *ptr);

#endif

