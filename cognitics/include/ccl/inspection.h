/****************************************************************************
Copyright (c) 2016 Cognitics, Inc.
****************************************************************************/
#pragma once

// COGNITICS_INSPECTION is enabled with cmake: -DCOGNITICS_INSPECTION=1
#ifdef COGNITICS_INSPECTION

#include <new>

void *operator new(size_t size, const char *file, int line);
void *operator new(size_t size);
void *operator new[](size_t size, const char *file, int line);
void *operator new[](size_t size);
void operator delete(void *p, const char *file, int line);
void operator delete(void *p);
void operator delete[](void *p, const char *file, int line);
void operator delete[](void *p);

#define new new(__FILE__, __LINE__)

bool cognitics_inspection_make_ready(void);

#endif

