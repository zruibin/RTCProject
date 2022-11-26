/*
 * foundation_define.h
 *
 * Created by Ruibin.Chow on 2022/11/26.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef FOUNDATION_DEFINE_H
#define FOUNDATION_DEFINE_H

#if defined(__clang__) || defined(__GNUC__)
    #define CPP_STANDARD __cplusplus
#elif defined(_MSC_VER)
    #define CPP_STANDARD _MSVC_LANG
#endif
 
#if CPP_STANDARD >= 199711L
    #define HAS_CPP_03 1
#endif
#if CPP_STANDARD >= 201103L
    #define HAS_CPP_11 1
#endif
#if CPP_STANDARD >= 201402L
    #define HAS_CPP_14 1
#endif
#if CPP_STANDARD >= 201703L
    #define HAS_CPP_17 1
#endif

#endif /* !FOUNDATION_DEFINE_H */
