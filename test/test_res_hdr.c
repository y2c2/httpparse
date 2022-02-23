#include <stdio.h>
#include <string.h>
#include "httpparse.h"

static int test_response_hdr(char *response, hp_size_t response_len, \
        httpparse_http_ver_t expected_ver, \
        int expected_status_code, \
        char *expected_desc, \
        hp_size_t fields_count)
{
    int ret = 0;
    httpparse_response_header_t header;
    hp_size_t expected_desc_len = strlen(expected_desc);

    if (httpparse_parse_response_header(&header, \
                response, response_len) != 0)
    { return -1; }

    if (header.ver != expected_ver)
    { ret = -1; goto fail; }
    if (header.status_code != expected_status_code)
    { ret = -1; goto fail; }
    if (strncmp(header.desc, expected_desc, expected_desc_len) != 0)
    { ret = -1; goto fail; }
    if (header.field_list.fields_count != fields_count)
    { ret = -1; goto fail; }

fail:
    httpparse_response_header_clear(&header);
    return ret;
}

#define TEST_RESPONSE_HDR( \
        response, \
        expected_ver, \
        expected_status_code, \
        expected_desc, \
        expected_fields_count) \
    do { \
        total++; \
        if (test_response_hdr(response, strlen(response), \
                    expected_ver, expected_status_code, expected_desc, expected_fields_count) != 0) \
        { \
            fprintf(stderr, "%s:%d: assert: test failed\n", __FILE__, __LINE__); \
        } \
        else \
        { \
            passed++; \
        } \
    } while (0);

int test_res_hdr(void)
{
    int total = 0;
    int passed = 0;

    printf("HTTP Response Header:\n");

    TEST_RESPONSE_HDR("HTTP/0.9 200 OK\r\n\r\n", HTTPPARSE_HTTP_VER_09, 200, "OK", 0);
    TEST_RESPONSE_HDR("HTTP/1.0 200 OK\r\n\r\n", HTTPPARSE_HTTP_VER_10, 200, "OK", 0);
    TEST_RESPONSE_HDR("HTTP/1.1 200 OK\r\n\r\n", HTTPPARSE_HTTP_VER_11, 200, "OK", 0);
    TEST_RESPONSE_HDR("HTTP/2.0 200 OK\r\n\r\n", HTTPPARSE_HTTP_VER_20, 200, "OK", 0);
    TEST_RESPONSE_HDR("HTTP/0.9 404 Not Found\r\n\r\n", HTTPPARSE_HTTP_VER_09, 404, "Not Found", 0);
    TEST_RESPONSE_HDR("HTTP/1.0 404 Not Found\r\n\r\n", HTTPPARSE_HTTP_VER_10, 404, "Not Found", 0);
    TEST_RESPONSE_HDR("HTTP/1.1 404 Not Found\r\n\r\n", HTTPPARSE_HTTP_VER_11, 404, "Not Found", 0);
    TEST_RESPONSE_HDR("HTTP/2.0 404 Not Found\r\n\r\n", HTTPPARSE_HTTP_VER_20, 404, "Not Found", 0);
    TEST_RESPONSE_HDR("HTTP/1.1 200 OK\r\nKey1:Value1\r\n\r\n", HTTPPARSE_HTTP_VER_11, 200, "OK", 1);
    TEST_RESPONSE_HDR("HTTP/1.1 200 OK\r\nKey1:Value1\r\nKey2:Value2\r\n\r\n", HTTPPARSE_HTTP_VER_11, 200, "OK", 2);
    TEST_RESPONSE_HDR("HTTP/1.1 200 OK\r\n Key1:Value1\r\nKey2:Value2\r\n\r\n", HTTPPARSE_HTTP_VER_11, 200, "OK", 2);
    TEST_RESPONSE_HDR("HTTP/1.1 200 OK\r\n  Key1:Value1\r\nKey2:Value2\r\n\r\n", HTTPPARSE_HTTP_VER_11, 200, "OK", 2);
    TEST_RESPONSE_HDR("HTTP/1.1 200 OK\r\nKey1 :Value1\r\nKey2:Value2\r\n\r\n", HTTPPARSE_HTTP_VER_11, 200, "OK", 2);
    TEST_RESPONSE_HDR("HTTP/1.1 200 OK\r\nKey1  :Value1\r\nKey2:Value2\r\n\r\n", HTTPPARSE_HTTP_VER_11, 200, "OK", 2);
    TEST_RESPONSE_HDR("HTTP/1.1 200 OK\r\nKey1: Value1\r\nKey2:Value2\r\n\r\n", HTTPPARSE_HTTP_VER_11, 200, "OK", 2);
    TEST_RESPONSE_HDR("HTTP/1.1 200 OK\r\nKey1:  Value1\r\nKey2:Value2\r\n\r\n", HTTPPARSE_HTTP_VER_11, 200, "OK", 2);
    TEST_RESPONSE_HDR("HTTP/1.1 200 OK\r\nKey1:Value1 \r\nKey2:Value2\r\n\r\n", HTTPPARSE_HTTP_VER_11, 200, "OK", 2);
    TEST_RESPONSE_HDR("HTTP/1.1 200 OK\r\nKey1:Value1  \r\nKey2:Value2\r\n\r\n", HTTPPARSE_HTTP_VER_11, 200, "OK", 2);

    printf("%d of %d cases passed\n", passed, total);

    return 0;
}

