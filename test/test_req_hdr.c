#include <stdio.h>
#include <string.h>
#include "httpparse.h"

static int test_request_hdr(char *request, hp_size_t request_len, \
        char *expected_method, \
        char *expected_path, \
        httpparse_http_ver_t expected_ver, \
        hp_size_t fields_count)
{
    int ret = 0;
    httpparse_request_header_t header;
    hp_size_t expected_method_len = strlen(expected_method);
    hp_size_t expected_path_len = strlen(expected_path);

    if (httpparse_parse_request_header(&header, \
                request, request_len) != 0)
    { return -1; }

    if (header.path_len != expected_path_len)
    { ret = -1; goto fail; }
    if (strncmp(header.method, expected_method, expected_method_len) != 0)
    { ret = -1; goto fail; }
    if (strncmp(header.path, expected_path, expected_path_len) != 0)
    { ret = -1; goto fail; }
    if (header.ver != expected_ver)
    { ret = -1; goto fail; }
    if (header.field_list.fields_count != fields_count)
    { ret = -1; goto fail; }

fail:
    httpparse_request_header_clear(&header);
    return ret;
}

#define TEST_REQUEST_HDR( \
        request, \
        expected_method, \
        expected_path, \
        expected_ver, \
        expected_fields_count) \
    do { \
        total++; \
        if (test_request_hdr(request, strlen(request), \
                    expected_method, expected_path, expected_ver, expected_fields_count) != 0) \
        { \
            fprintf(stderr, "%s:%d: assert: test failed\n", __FILE__, __LINE__); \
        } \
        else \
        { \
            passed++; \
        } \
    } while (0);

int test_req_hdr(void)
{
    int total = 0;
    int passed = 0;

    printf("HTTP Request Header:\n");

    TEST_REQUEST_HDR("GET / HTTP/0.9\r\n\r\n", "GET", "/", HTTPPARSE_HTTP_VER_09, 0);
    TEST_REQUEST_HDR("GET / HTTP/1.0\r\n\r\n", "GET", "/", HTTPPARSE_HTTP_VER_10, 0);
    TEST_REQUEST_HDR("GET / HTTP/1.1\r\n\r\n", "GET", "/", HTTPPARSE_HTTP_VER_11, 0);
    TEST_REQUEST_HDR("GET / HTTP/2.0\r\n\r\n", "GET", "/", HTTPPARSE_HTTP_VER_20, 0);
    TEST_REQUEST_HDR("POST / HTTP/0.9\r\n\r\n", "POST", "/", HTTPPARSE_HTTP_VER_09, 0);
    TEST_REQUEST_HDR("POST / HTTP/1.0\r\n\r\n", "POST", "/", HTTPPARSE_HTTP_VER_10, 0);
    TEST_REQUEST_HDR("POST / HTTP/1.1\r\n\r\n", "POST", "/", HTTPPARSE_HTTP_VER_11, 0);
    TEST_REQUEST_HDR("POST / HTTP/2.0\r\n\r\n", "POST", "/", HTTPPARSE_HTTP_VER_20, 0);
    TEST_REQUEST_HDR("GET /index.html HTTP/0.9\r\n\r\n", "GET", "/index.html", HTTPPARSE_HTTP_VER_09, 0);
    TEST_REQUEST_HDR("GET /index.html HTTP/1.0\r\n\r\n", "GET", "/index.html", HTTPPARSE_HTTP_VER_10, 0);
    TEST_REQUEST_HDR("GET /index.html HTTP/1.1\r\n\r\n", "GET", "/index.html", HTTPPARSE_HTTP_VER_11, 0);
    TEST_REQUEST_HDR("GET /index.html HTTP/2.0\r\n\r\n", "GET", "/index.html", HTTPPARSE_HTTP_VER_20, 0);
    TEST_REQUEST_HDR("GET / HTTP/1.1\r\nKey1:Value1\r\n\r\n", "GET", "/", HTTPPARSE_HTTP_VER_11, 1);
    TEST_REQUEST_HDR("GET / HTTP/1.1\r\nKey1:Value1\r\nKey2:Value2\r\n\r\n", "GET", "/", HTTPPARSE_HTTP_VER_11, 2);
    TEST_REQUEST_HDR("GET / HTTP/1.1\r\n Key1:Value1\r\nKey2:Value2\r\n\r\n", "GET", "/", HTTPPARSE_HTTP_VER_11, 2);
    TEST_REQUEST_HDR("GET / HTTP/1.1\r\n  Key1:Value1\r\nKey2:Value2\r\n\r\n", "GET", "/", HTTPPARSE_HTTP_VER_11, 2);
    TEST_REQUEST_HDR("GET / HTTP/1.1\r\nKey1 :Value1\r\nKey2:Value2\r\n\r\n", "GET", "/", HTTPPARSE_HTTP_VER_11, 2);
    TEST_REQUEST_HDR("GET / HTTP/1.1\r\nKey1  :Value1\r\nKey2:Value2\r\n\r\n", "GET", "/", HTTPPARSE_HTTP_VER_11, 2);
    TEST_REQUEST_HDR("GET / HTTP/1.1\r\nKey1: Value1\r\nKey2:Value2\r\n\r\n", "GET", "/", HTTPPARSE_HTTP_VER_11, 2);
    TEST_REQUEST_HDR("GET / HTTP/1.1\r\nKey1:  Value1\r\nKey2:Value2\r\n\r\n", "GET", "/", HTTPPARSE_HTTP_VER_11, 2);
    TEST_REQUEST_HDR("GET / HTTP/1.1\r\nKey1:Value1 \r\nKey2:Value2\r\n\r\n", "GET", "/", HTTPPARSE_HTTP_VER_11, 2);
    TEST_REQUEST_HDR("GET / HTTP/1.1\r\nKey1:Value1  \r\nKey2:Value2\r\n\r\n", "GET", "/", HTTPPARSE_HTTP_VER_11, 2);

    printf("%d of %d cases passed\n", passed, total);

    return 0;
}

