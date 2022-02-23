/* HTTP Parse : Allocator
 * Copyright(c) 2016 y2c2 */

#include "httpparse_allocator.h"

/* Global stuff */
static hp_malloc_cb_t g_hp_malloc = NULL;
static hp_free_cb_t g_hp_free = NULL;

void hp_allocator_set_malloc(hp_malloc_cb_t cb);
void hp_allocator_set_free(hp_free_cb_t cb);

/* Memory Management */

void hp_allocator_set_malloc(hp_malloc_cb_t cb)
{
    g_hp_malloc = cb;
}

void hp_allocator_set_free(hp_free_cb_t cb)
{
    g_hp_free = cb;
}

void *hp_malloc(hp_size_t size)
{
    return g_hp_malloc(size);
}

void hp_free(void *ptr)
{
    g_hp_free(ptr);
}

