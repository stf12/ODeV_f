#include <stdlib.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
/*
void *operator new(size_t size) throw() {
    return malloc(size);
}

void operator delete(void *p) throw() {
    free(p);
}
*/
/*
extern "C" int __aeabi_atexit(void *object,
                              void (*destructor)(void *),
                              void *dso_handle)
{
    object = object; destructor=destructor; dso_handle=dso_handle;

    return 0;
}
*/
extern "C"
{
    /*
    void *malloc(size_t s) {
        while(1);
        return (void *)0;
    }
    void free(void *) {
        while(1);
    }
    void *calloc(size_t a, size_t b) {
        return (void *)0;
    }
    */
    extern int errno;
#define FreeRTOS
#define MAX_STACK_SIZE 0x2000

    extern int __io_putchar(int ch) __attribute__((weak));
    extern int __io_getchar(void) __attribute__((weak));

#ifndef FreeRTOS
    register char* stack_ptr asm("sp");
#endif

    register char* stack_ptr asm("sp");

    char* __env[1] = { 0 };
    char** environ = __env;

    //void* __dso_handle = (void*)NULL;
    void __cxa_atexit(void(*Arg)(void*), void* Arg2, void* Arg3) {}
    void  __cxa_guard_acquire(void) {}
    void  __cxa_guard_release(void) {}

    void __cxa_pure_virtual()
    {
        while (1);
    }
    int _read(int fd, void* buf, int count)
    {
        return 0;
    }

    int _lseek(int fildes, int offset, int whence)
    {
        return 0;
    }

    int _isatty(int fildes)
    {
        return 0;
    }

    int _fstat(int fildes, void* buf)
    {
        return 0;
    }

    int _close(int fildes)
    {
        return 0;
    }

    int _write(int fd, const void* buf, int count)
    {
        return 0;
    }

    int _kill(int a, int b)
    {
        while (1);
        return 0;
    }

    int _getpid(void)
    {
        return 0;
    }
    caddr_t _sbrk(int incr)
    {
        extern char end asm("end");
        static char* heap_end;
        char* prev_heap_end;

        if (heap_end == 0)
        {
            heap_end = &end;
        }

        prev_heap_end = heap_end;
        if (heap_end + incr > stack_ptr)
        {
            //      write(1, "Heap and stack collision\n", 25);
            //      abort();
            errno = ENOMEM;
            return (caddr_t) -1;
        }

        heap_end += incr;

        return (caddr_t)prev_heap_end;
    }
    void _exit(int a)
    {
        while (1);
    }
    void __assert_func(const char* file, int line, const char* func, const char* failedexpr)
    {
        while (1) {}
    }
}
