/* HTTP Parse
 * Copyright(c) 2016 y2c2 */

#ifndef HTTPPARSE_H
#define HTTPPARSE_H

#include "httpparse_dt.h"
#include "httpparse_allocator.h"
#include "httpparse_hdr.h"

void hp_allocator_set_malloc(hp_malloc_cb_t cb);
void hp_allocator_set_free(hp_free_cb_t cb);

/* Parse a HTTP request header */
int httpparse_parse_request_header( \
        httpparse_request_header_t *request_header, \
        char *buf, hp_size_t len);

/* Parse a HTTP response header */
int httpparse_parse_response_header( \
        httpparse_response_header_t *response_header, \
        char *buf, hp_size_t len);

/* Clear HTTP request header */
void httpparse_request_header_clear( \
        httpparse_request_header_t *header);


/* Clear HTTP response header */
void httpparse_response_header_clear( \
        httpparse_response_header_t *header);


#endif
