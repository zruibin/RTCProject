/*
 * gl_defines.h
 *
 * Created by Ruibin.Chow on 2022/12/28.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef GL_DEFINES_H
#define GL_DEFINES_H

#if defined(__APPLE__)
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#include <GLUT/glut.h>
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#include <GL/glew.h>
#endif

#endif /* !GL_DEFINES_H */
