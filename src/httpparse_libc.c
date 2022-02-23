/* HTTP Parse : LibC
 * Copyright(c) 2016 y2c2 */

#include "httpparse_allocator.h"
#include "httpparse_libc.h"

void *hp_memcpy(void *dest, const void *src, hp_size_t n)
{
    char *dest_p = dest;
    const char *src_p = src;
    while (n-- != 0)
    {
        *dest_p++ = *src_p++;
    }
    return dest;
}

hp_size_t hp_strlen(const char *s)
{
    hp_size_t len = 0;
    const char *p = s;
    while (*p++ != '\0') len++;
    return len;
}

void *hp_memset(void *s, int c, hp_size_t n)
{
    char *p = s;
    while (n-- != 0)
    {
        *p++ = (char)c;
    }
    return s;
}

int hp_strcmp(const char *s1, const char *s2)
{
    const char *p1 = s1, *p2 = s2;

    if ((p1 == NULL) || (p2 == NULL)) return -1;

    for (;;)
    {
        if ((*p1 != *p2) || (*p1 == '\0')) break;
        p1++; p2++;
    }
    return *p1 - *p2;
}

int hp_strncmp(const char *s1, const char *s2, hp_size_t n)
{
    const char *p1 = s1, *p2 = s2;
    
    if (n != 0)
    {
        do
        {
            if (*p1 != *p2++) break;
            if (*p1++ == '\0') return 0;
        }
        while (--n != 0);
        if (n > 0)
        {
            if (*p1 == '\0') return -1;
            if (*--p2 == '\0') return 1;
            return (unsigned char)*p1 - (unsigned char)*p2;
        }
    }
    return 0;
}

char *hp_strdup(const char *s)
{
    hp_size_t len;
    char *new_s;

    if (s == NULL) return NULL;

    len = hp_strlen(s);
    new_s = hp_malloc(sizeof(char) * (len + 1));
    if (new_s == NULL) return NULL;
    hp_memcpy(new_s, s, len);
    new_s[len] = '\0';

    return new_s;
}

const char *hp_strnstrn(const char *haystack, const hp_size_t haystack_len, \
        const char *needle, const hp_size_t needle_len)
{
    const char *haystack_p = haystack;
    const char *haystack_subp;
    const char *needle_p;
    hp_size_t needle_len_sub;
    hp_size_t haystack_remain = (hp_size_t)haystack_len;

    for (;;)
    {
        /* One Turn */
        if (haystack_remain < needle_len) return NULL;
        haystack_subp = haystack_p;
        needle_p = needle;
        needle_len_sub = needle_len;
        while (needle_len_sub-- != 0)
        {
            if (*needle_p != *haystack_subp)
            { /* Not Match */ goto next_turn; }
            needle_p++; haystack_subp++;
        }
        /* Match */
        return haystack_p;
next_turn:
        haystack_p++;
        haystack_remain--;
    }
}

