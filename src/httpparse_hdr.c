/* HTTP Parse : Header
 * Copyright(c) 2016 y2c2 */

#include "httpparse_allocator.h"
#include "httpparse_libc.h"
#include "httpparse_hdr.h"

httpparse_header_field_t *httpparse_header_field_list_find( \
        httpparse_header_field_list_t *fields, const char *key, const hp_size_t key_len)
{
    hp_size_t i;

    for (i = 0; i != fields->fields_count; i++)
    {
        httpparse_header_field_t *cur = fields->fields + i;
        if ((cur->key_len == key_len) && \
                (hp_strncmp(cur->key, key, key_len) == 0))
        {
            return cur;
        }
    }

    return NULL;
}

void httpparse_request_header_init( \
        httpparse_request_header_t *request_header)
{
    hp_memset(request_header, 0, sizeof(httpparse_request_header_t));
}

void httpparse_response_header_init( \
        httpparse_response_header_t *response_header)
{
    hp_memset(response_header, 0, sizeof(httpparse_response_header_t));
}

static hp_size_t count_str(char *buf, hp_size_t len, \
        const char *needle, const hp_size_t needle_len)
{
    hp_size_t count = 0;
    const char *p = buf, *p1;

    while (len != 0)
    {
        p1 = hp_strnstrn(p, len, needle, needle_len);
        if (p == NULL) { break; }
        if (p1 == p)
        {
            /* Meet "\r\n\r\n" */
            break;
        }
        len -= (hp_size_t)(p1 - p) + needle_len;
        p = p1 + needle_len;

        count++;
    }

    return count;
}

static int copy_request_header( \
        httpparse_request_header_t *header, \
        char *buf, hp_size_t len)
{
    const char *p_rnrn;

    /* Locate to "\r\n\r\n" */
    p_rnrn = hp_strnstrn(buf, len, "\r\n\r\n", 4);
    if (p_rnrn == NULL) return -1;

    /* Copy the header
     * The header covers the first line and 
     * each line of field (including \r\n) */
    header->data_len = (hp_size_t)(p_rnrn - buf) + 2;
    if ((header->data = (char *)hp_malloc( \
                    sizeof(char) * (header->data_len + 1))) == NULL)
    { return -1; }
    hp_memcpy(header->data, buf, header->data_len);
    header->data[header->data_len] = '\0';

    return 0;
}

static int copy_response_header( \
        httpparse_response_header_t *header, \
        char *buf, hp_size_t len)
{
    const char *p_rnrn;

    /* Locate to "\r\n\r\n" */
    p_rnrn = hp_strnstrn(buf, len, "\r\n\r\n", 4);
    if (p_rnrn == NULL) return -1;

    /* Copy the header
     * The header covers the first line and 
     * each line of field (including \r\n) */
    header->data_len = (hp_size_t)(p_rnrn - buf) + 2;
    if ((header->data = (char *)hp_malloc( \
                    sizeof(char) * (header->data_len + 1))) == NULL)
    { return -1; }
    hp_memcpy(header->data, buf, header->data_len);
    header->data[header->data_len] = '\0';

    return 0;
}

/* Parse the first line of HTTP Request */

typedef enum
{
    PARSE_REQUEST_FIRST_LINE_STATE_INIT = 0,
    PARSE_REQUEST_FIRST_LINE_STATE_METHOD,
    PARSE_REQUEST_FIRST_LINE_STATE_PATH,
    PARSE_REQUEST_FIRST_LINE_STATE_FINISH,
    PARSE_REQUEST_FIRST_LINE_STATE_FAIL,
    PARSE_REQUEST_FIRST_LINE_STATE_EXIT,
} parse_request_first_line_state_t;

static int parse_request_first_line( \
        httpparse_request_header_t *header, \
        char **p_io, hp_size_t *len_io)
{
    int ret = 0;
    parse_request_first_line_state_t state = PARSE_REQUEST_FIRST_LINE_STATE_INIT;
    char *p = *p_io;
    hp_size_t len = *len_io;

    /* Method */
    header->method = p;
    header->method_len = 0;

    for (;;)
    {
        switch (state)
        {
            case PARSE_REQUEST_FIRST_LINE_STATE_INIT:
                if (len == 0) { state = PARSE_REQUEST_FIRST_LINE_STATE_FAIL; }
                else
                {
                    if (*p == ' ')
                    {
                        if (header->method_len == 0)
                        {
                            /* Empty method */
                            state = PARSE_REQUEST_FIRST_LINE_STATE_FAIL;
                        }
                        else
                        {
                            while ((len != 0) && (*p == ' '))
                            { p++; len--; }
                            header->path = p;
                            header->path_len = 0;
                            state = PARSE_REQUEST_FIRST_LINE_STATE_METHOD;
                        }
                    }
                    else
                    {
                        header->method_len++;
                        p++;
                        len--;
                    }
                }
                break;

            case PARSE_REQUEST_FIRST_LINE_STATE_METHOD:
                if (len == 0) { state = PARSE_REQUEST_FIRST_LINE_STATE_FAIL; }
                else
                {
                    if (*p == ' ')
                    {
                        if (header->path_len == 0)
                        {
                            /* Empty URL */
                            state = PARSE_REQUEST_FIRST_LINE_STATE_FAIL;
                        }
                        else
                        {
                            while ((len != 0) && (*p == ' '))
                            { p++; len--; }
                            state = PARSE_REQUEST_FIRST_LINE_STATE_PATH;
                        }
                    }
                    else
                    {
                        header->path_len++;
                        p++;
                        len--;
                    }
                }
                break;

            case PARSE_REQUEST_FIRST_LINE_STATE_PATH:
                if ((len >= 10) && (hp_strncmp("HTTP/0.9\r\n", p, 10) == 0))
                {
                    header->ver = HTTPPARSE_HTTP_VER_09;
                    p += 10;
                    len -= 10;
                    state = PARSE_REQUEST_FIRST_LINE_STATE_FINISH;
                }
                else if ((len >= 10) && (hp_strncmp("HTTP/1.0\r\n", p, 10) == 0))
                {
                    header->ver = HTTPPARSE_HTTP_VER_10;
                    p += 10;
                    len -= 10;
                    state = PARSE_REQUEST_FIRST_LINE_STATE_FINISH;
                }
                else if ((len >= 10) && (hp_strncmp("HTTP/1.1\r\n", p, 10) == 0))
                {
                    header->ver = HTTPPARSE_HTTP_VER_11;
                    p += 10;
                    len -= 10;
                    state = PARSE_REQUEST_FIRST_LINE_STATE_FINISH;
                }
                else if ((len >= 10) && (hp_strncmp("HTTP/2.0\r\n", p, 10) == 0))
                {
                    header->ver = HTTPPARSE_HTTP_VER_20;
                    p += 10;
                    len -= 10;
                    state = PARSE_REQUEST_FIRST_LINE_STATE_FINISH;
                }
                else
                {
                    state = PARSE_REQUEST_FIRST_LINE_STATE_FAIL;
                }
                break;

            case PARSE_REQUEST_FIRST_LINE_STATE_FINISH:
                *p_io = p;
                *len_io = len;
                state = PARSE_REQUEST_FIRST_LINE_STATE_EXIT;
                break;

            case PARSE_REQUEST_FIRST_LINE_STATE_FAIL:
                ret = -1;
                state = PARSE_REQUEST_FIRST_LINE_STATE_EXIT;
                break;

            case PARSE_REQUEST_FIRST_LINE_STATE_EXIT:
                break;
        }

        if (state == PARSE_REQUEST_FIRST_LINE_STATE_EXIT) break;
    }

    return ret;
}

typedef enum
{
    PARSE_RESPONSE_FIRST_LINE_STATE_INIT = 0,
    PARSE_RESPONSE_FIRST_LINE_STATE_VER,
    PARSE_RESPONSE_FIRST_LINE_STATE_STATUS_CODE,
    PARSE_RESPONSE_FIRST_LINE_STATE_FINISH,
    PARSE_RESPONSE_FIRST_LINE_STATE_FAIL,
    PARSE_RESPONSE_FIRST_LINE_STATE_EXIT,
} parse_response_first_line_state_t;

#define ISDIGIT(ch) (('0'<=(ch))&&((ch)<='9'))
#define DIGIT_TO_INT(ch) ((int)(ch) - (int)'0')

static int parse_response_first_line( \
        httpparse_response_header_t *header, \
        char **p_io, hp_size_t *len_io)
{
    int ret = 0;
    parse_response_first_line_state_t state = PARSE_RESPONSE_FIRST_LINE_STATE_INIT;
    char *p = *p_io;
    hp_size_t len = *len_io;

    /* Method */
    for (;;)
    {
        switch (state)
        {
            case PARSE_RESPONSE_FIRST_LINE_STATE_INIT:
                if ((len >= 9) && (hp_strncmp("HTTP/0.9 ", p, 9) == 0))
                {
                    header->ver = HTTPPARSE_HTTP_VER_09;
                    p += 9;
                    len -= 9;
                    state = PARSE_RESPONSE_FIRST_LINE_STATE_VER;
                }
                else if ((len >= 9) && (hp_strncmp("HTTP/1.0 ", p, 9) == 0))
                {
                    header->ver = HTTPPARSE_HTTP_VER_10;
                    p += 9;
                    len -= 9;
                    state = PARSE_RESPONSE_FIRST_LINE_STATE_VER;
                }
                else if ((len >= 9) && (hp_strncmp("HTTP/1.1 ", p, 9) == 0))
                {
                    header->ver = HTTPPARSE_HTTP_VER_11;
                    p += 9;
                    len -= 9;
                    state = PARSE_RESPONSE_FIRST_LINE_STATE_VER;
                }
                else if ((len >= 9) && (hp_strncmp("HTTP/2.0 ", p, 9) == 0))
                {
                    header->ver = HTTPPARSE_HTTP_VER_20;
                    p += 9;
                    len -= 9;
                    state = PARSE_RESPONSE_FIRST_LINE_STATE_VER;
                }
                else
                {
                    state = PARSE_RESPONSE_FIRST_LINE_STATE_FAIL;
                }
                break;

            case PARSE_RESPONSE_FIRST_LINE_STATE_VER:
                if (len == 0) { state = PARSE_RESPONSE_FIRST_LINE_STATE_FAIL; }
                else
                {
                    if (*p == ' ')
                    {
                        while ((len != 0) && (*p == ' '))
                        { p++; len--; }
                    }
                    else if ((len > 3) && \
                            (ISDIGIT(*p) && ISDIGIT(*(p + 1)) && ISDIGIT(*(p + 2))) && \
                            (*(p + 3) == ' '))
                    {
                        header->status_code = DIGIT_TO_INT(*p) * 100 + \
                                              DIGIT_TO_INT(*(p + 1)) * 10 + \
                                              DIGIT_TO_INT(*(p + 2));
                        p += 4; len -= 4;
                        header->desc = p;
                        header->desc_len = 0;
                        state = PARSE_RESPONSE_FIRST_LINE_STATE_STATUS_CODE;
                    }
                    else
                    { state = PARSE_RESPONSE_FIRST_LINE_STATE_FAIL; }
                }
                break;

            case PARSE_RESPONSE_FIRST_LINE_STATE_STATUS_CODE:
                if (len < 4)
                {
                    state = PARSE_RESPONSE_FIRST_LINE_STATE_FAIL;
                }
                else
                {
                    if ((*p == '\r') && (*(p + 1) == '\n'))
                    {
                        p += 2;
                        len -= 2;
                        state = PARSE_RESPONSE_FIRST_LINE_STATE_FINISH;
                    }
                    else
                    {
                        header->desc_len++;
                        p++; len--;
                    }
                }

                break;

            case PARSE_RESPONSE_FIRST_LINE_STATE_FINISH:
                *p_io = p;
                *len_io = len;
                state = PARSE_RESPONSE_FIRST_LINE_STATE_EXIT;
                break;

            case PARSE_RESPONSE_FIRST_LINE_STATE_FAIL:
                ret = -1;
                state = PARSE_RESPONSE_FIRST_LINE_STATE_EXIT;
                break;

            case PARSE_RESPONSE_FIRST_LINE_STATE_EXIT:
                break;
        }

        if (state == PARSE_RESPONSE_FIRST_LINE_STATE_EXIT) break;
    }

    return ret;
}

typedef enum
{
    PARSE_FIELDS_STATE_INIT = 0,
    PARSE_FIELDS_STATE_KEY_WS,
    PARSE_FIELDS_STATE_COLON,
    PARSE_FIELDS_STATE_FINISH,
    PARSE_FIELDS_STATE_FAIL,
    PARSE_FIELDS_STATE_EXIT,
} parse_fields_state_t;

#define IS_WS(ch) (((ch) == '\t') || ((ch) == ' '))

static int parse_fields( \
        httpparse_header_field_list_t *field_list, \
        char **p_io, hp_size_t *len_io)
{
    int ret = 0;
    parse_fields_state_t state = PARSE_FIELDS_STATE_INIT;
    char *p = *p_io;
    hp_size_t len = *len_io;
    httpparse_header_field_t *field_cur;
    hp_size_t parsed_fields_count = 0;

    /* Count \r\n */
    field_list->fields_count = count_str(p, len - 2, "\r\n", 2);
    if (field_list->fields_count == 0) return 0;

    if ((field_list->fields = (httpparse_header_field_t *)hp_malloc( \
                    sizeof(httpparse_header_field_t) * field_list->fields_count)) == NULL)
    { return -1; }
    field_cur = field_list->fields;

    /* First field */
    field_cur->key = p;
    field_cur->key_len = 0;

    for (;;)
    {
        switch (state)
        {
            case PARSE_FIELDS_STATE_INIT:
                if (len == 0)
                {
                    state = PARSE_FIELDS_STATE_FINISH;
                }
                else
                {
                    if (IS_WS(*p))
                    {
                        while ((len != 0) && (IS_WS(*p)))
                        {
                            p++; len--;
                        }
                        if (field_cur->key_len > 0)
                        {
                            state = PARSE_FIELDS_STATE_KEY_WS;
                        }
                    }
                    else if (*p == ':')
                    {
                        if (field_cur->key_len == 0)
                        {
                            /* Empty key */
                            state = PARSE_FIELDS_STATE_FAIL;
                        }
                        else
                        {
                            /* Skip ':' */
                            p++; len--;

                            while ((len != 0) && (*p == ' '))
                            { p++; len--; }
                            field_cur->value = p;
                            field_cur->value_len = 0;
                            state = PARSE_FIELDS_STATE_COLON;
                        }
                    }
                    else if ((*p == '\r') && (*p == '\n'))
                    {
                        state = PARSE_FIELDS_STATE_FAIL;
                    }
                    else
                    {
                        field_cur->key_len++;
                        p++;
                        len--;
                    }
                }
                break;

            case PARSE_FIELDS_STATE_KEY_WS:
                if (len == 0) { state = PARSE_FIELDS_STATE_FAIL; }
                else
                {
                    if (*p == ':')
                    {
                        /* Skip ':' */
                        p++; len--;

                        while ((len != 0) && (*p == ' '))
                        { p++; len--; }
                        field_cur->value = p;
                        field_cur->value_len = 0;
                        state = PARSE_FIELDS_STATE_COLON;
                    }
                    else
                    {
                        state = PARSE_FIELDS_STATE_FAIL;
                    }
                }
                break;

            case PARSE_FIELDS_STATE_COLON:
                if (len == 0)
                {
                    state = PARSE_FIELDS_STATE_FAIL;
                }
                else
                {
                    if (IS_WS(*p))
                    {
                        while ((len != 0) && (IS_WS(*p)))
                        {
                            p++; len--;
                        }
                    }
                    else if (*p == '\r')
                    {
                        if ((len >= 2) && (*(p + 1) == '\n'))
                        {
                            if (field_cur->value_len == 0)
                            {
                                /* Empty Value */
                                state = PARSE_FIELDS_STATE_FAIL;
                            }
                            else
                            {
                                p += 2; len -= 2;
                                parsed_fields_count++;
                                if (parsed_fields_count == field_list->fields_count)
                                {
                                    state = PARSE_FIELDS_STATE_EXIT;
                                }
                                else
                                {
                                    state = PARSE_FIELDS_STATE_INIT;
                                    field_cur++;
                                    field_cur->key = p;
                                    field_cur->key_len = 0;
                                }
                            }
                        }
                        else
                        {
                            state = PARSE_FIELDS_STATE_FAIL;
                        }
                    }
                    else
                    {
                        field_cur->value_len++;
                        p++;
                        len--;
                    }
                }

                break;

            case PARSE_FIELDS_STATE_FINISH:
                *p_io = p;
                *len_io = len;
                state = PARSE_FIELDS_STATE_EXIT;
                break;

            case PARSE_FIELDS_STATE_FAIL:
                ret = -1;
                state = PARSE_FIELDS_STATE_EXIT;
                break;
            case PARSE_FIELDS_STATE_EXIT:
                break;
        }

        if (state == PARSE_FIELDS_STATE_EXIT) break;
    }

    return ret;
}

/* Parse a HTTP request header */
int httpparse_parse_request_header( \
        httpparse_request_header_t *header, \
        char *buf, hp_size_t buf_len)
{
    char *p = buf;
    size_t len = buf_len;

    /* Clear */
    hp_memset(header, 0, sizeof(httpparse_request_header_t));

    /* Copy Header */
    if (copy_request_header(header, buf, len) != 0) { goto fail; }

    /* First line */
    if (parse_request_first_line(header, &p, &len) != 0) { goto fail; }

    /* Fields */
    if (parse_fields(&header->field_list, &p, &len) != 0) { goto fail; }

    return 0;

fail:
    if (header->field_list.fields != NULL) hp_free(header->field_list.fields);
    if (header->data != NULL) hp_free(header->data);
    return -1;
}

/* Parse a HTTP response header */
int httpparse_parse_response_header( \
        httpparse_response_header_t *header, \
        char *buf, hp_size_t buf_len)
{
    char *p = buf;
    size_t len = buf_len;

    /* Clear */
    hp_memset(header, 0, sizeof(httpparse_response_header_t));

    /* Copy Header */
    if (copy_response_header(header, buf, len) != 0) { goto fail; }

    /* First line */
    if (parse_response_first_line(header, &p, &len) != 0) { goto fail; }

    /* Fields */
    if (parse_fields(&header->field_list, &p, &len) != 0) { goto fail; }

    return 0;

fail:
    if (header->field_list.fields != NULL) hp_free(header->field_list.fields);
    if (header->data != NULL) hp_free(header->data);
    return -1;
}

/* Clear HTTP request header */
void httpparse_request_header_clear(httpparse_request_header_t *header)
{
    if (header->field_list.fields != NULL) 
    {
        hp_free(header->field_list.fields);
        header->field_list.fields = NULL;
    }
    if (header->data != 0) 
    {
        hp_free(header->data);
        header->data = NULL;
    }
}

/* Clear HTTP response header */
void httpparse_response_header_clear(httpparse_response_header_t *header)
{
    if (header->field_list.fields != NULL) 
    {
        hp_free(header->field_list.fields);
        header->field_list.fields = NULL;
    }
    if (header->data != 0) 
    {
        hp_free(header->data);
        header->data = NULL;
    }
}

