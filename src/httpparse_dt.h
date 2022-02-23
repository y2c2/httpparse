/* HTTP Parse
 * Copyright(c) 2018 y2c2 */

#ifndef HTTPPARSE_DT_H
#define HTTPPARSE_DT_H

#ifndef __cplusplus 
typedef enum
{
    hp_false = 0,
    hp_true = 1,
} hp_bool;
#else
typedef enum
{
    hp_false = false,
    hp_true = true,
} hp_bool;
#endif

#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#include <stdint.h>
#include <stdio.h>
typedef uint64_t hp_u64;
typedef uint32_t hp_u32;
typedef int32_t hp_s32;
typedef int64_t hp_s64;
typedef uint16_t hp_u16;
typedef int16_t hp_s16;
typedef uint8_t hp_u8;
typedef int8_t hp_s8;
typedef size_t hp_size_t;
typedef ssize_t hp_ssize_t;
#else
typedef unsigned long long hp_u64;
typedef unsigned int hp_u32;
typedef unsigned short int hp_u16;
typedef unsigned char hp_u8;
typedef signed long long hp_s64;
typedef signed int hp_s32;
typedef signed short int hp_s16;
typedef char hp_s8;
typedef unsigned int hp_size_t;
typedef signed int hp_ssize_t;
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#endif

