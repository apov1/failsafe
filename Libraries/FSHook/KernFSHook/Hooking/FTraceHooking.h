#pragma once

/* Based on
 * https://github.com/ilammy/ftrace-hook/blob/master/ftrace_hook.c
 * Original Author: ilammy <a.lozovsky@gmail.com>
 * Original License: GPLv2
 */

#include <linux/ftrace.h>
#include <linux/version.h>

#ifndef CONFIG_X86_64
#error "Architecture not supported"
#endif

#if ( LINUX_VERSION_CODE < KERNEL_VERSION( 4, 17, 0 ) )
#error "Kernel version not supported"
#endif

#define HOOK( _name, _hook, _orig )                                                                \
    {                                                                                              \
        .name = ( _name ), .function = ( _hook ), .original = ( _orig ),                           \
    }

struct FTraceHook {
    const char *name;
    void *function;
    void *original;

    unsigned long address;
    struct ftrace_ops ops;
};

int FSHook_FTraceInitHooks( struct FTraceHook *hooks, size_t count );
void FSHook_FTraceRemoveHooks( struct FTraceHook *hooks, size_t count );