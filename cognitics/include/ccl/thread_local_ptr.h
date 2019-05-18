/****************************************************************************
Copyright (c) 2016 Cognitics, Inc.
****************************************************************************/
#pragma once

#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace cognitics
{
    template <typename T>
    class thread_local_ptr
    {
    private:
#ifdef WIN32
        DWORD index;
#else
        pthread_key_t index;
#endif
    public:
        thread_local_ptr(void)
        {
#ifdef WIN32
            index = TlsAlloc();
            assert(index != TLS_OUT_OF_INDEXES);
#else
            pthread_key_create(&index, NULL);
#endif
            set(NULL);
        }

        void set(T *ptr)
        {
#ifdef WIN32
            TlsSetValue(index, (LPVOID)ptr);
#else
            pthread_setspecific(index, (void *)ptr);
#endif
        }

        T *get(void) const
        {
#ifdef WIN32
            return (T *)TlsGetValue(index);
#else
            return (T *)pthread_getspecific(index);
#endif
        }

        thread_local_ptr &operator=(T *ptr)
        {
            set(ptr);
            return *this;
        }

        thread_local_ptr &operator=(const thread_local_ptr &other)
        {
            set(other.get());
            return *this;
        }

        T &operator*(void) const
        {
            return *get();
        }

        T *operator->(void) const
        {
            return get();
        }

        ~thread_local_ptr(void)
        {
#ifdef WIN32
            TlsFree(index);
#else
            pthread_key_delete(index);
#endif
        }

    };


}

