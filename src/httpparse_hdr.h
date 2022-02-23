/* HTTP Parse : Header
 * Copyright(c) 2016 y2c2 */

#ifndef HTTPPARSE_HDR_H
#define HTTPPARSE_HDR_H

#include "httpparse_dt.h"

/* Headers */
struct httpparse_header_field
{
    char *key;
    hp_size_t key_len;

    char *value;
    hp_size_t value_len;
};
typedef struct httpparse_header_field httpparse_header_field_t;

struct httpparse_header_field_list
{
    httpparse_header_field_t *fields;
    hp_size_t fields_count;
};
typedef struct httpparse_header_field_list httpparse_header_field_list_t;

httpparse_header_field_t *httpparse_header_field_list_find( \
        httpparse_header_field_list_t *fields, const char *key, const hp_size_t key_len);

/* HTTP Version */
typedef enum
{
    HTTPPARSE_HTTP_VER_09 = 9,
    HTTPPARSE_HTTP_VER_10 = 10,
    HTTPPARSE_HTTP_VER_11 = 11,
    HTTPPARSE_HTTP_VER_20 = 20,
} httpparse_http_ver_t;

/* Request Header */
struct httpparse_request_header
{
    /* First line */
    char *method;
    hp_size_t method_len;
    char *path;
    hp_size_t path_len;
    httpparse_http_ver_t ver;

    /* Headers */
    httpparse_header_field_list_t field_list;

    /* Raw data of header */
    char *data;
    hp_size_t data_len;
};
typedef struct httpparse_request_header httpparse_request_header_t;

/* Response Header */
struct httpparse_response_header
{
    /* First line */
    httpparse_http_ver_t ver;
    int status_code;
    char *desc;
    hp_size_t desc_len;

    /* Headers */
    httpparse_header_field_list_t field_list;

    /* Raw data of header */
    char *data;
    hp_size_t data_len;
};
typedef struct httpparse_response_header httpparse_response_header_t;

void httpparse_request_header_init( \
        httpparse_request_header_t *request_header);

void httpparse_response_header_init( \
        httpparse_response_header_t *response_header);

/* Parse a HTTP request header */
int httpparse_parse_request_header( \
        httpparse_request_header_t *request_header, \
        char *buf, hp_size_t len);

/* Parse a HTTP response header */
int httpparse_parse_response_header( \
        httpparse_response_header_t *response_header, \
        char *buf, hp_size_t len);

/* Clear HTTP request header */
void httpparse_request_header_clear(httpparse_request_header_t *header);


/* Clear HTTP response header */
void httpparse_response_header_clear(httpparse_response_header_t *header);

#endif


