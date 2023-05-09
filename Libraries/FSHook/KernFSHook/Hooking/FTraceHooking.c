/* Based on
 * https://github.com/ilammy/ftrace-hook/blob/master/ftrace_hook.c
 * Original Author: ilammy <a.lozovsky@gmail.com>
 * Original License: GPLv2
 */

#include "Hooking/FTraceHooking.h"

#include <linux/kprobes.h>
#include <linux/linkage.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#pragma GCC optimize( "-fno-optimize-sibling-calls" )

static int FSHookResolveHookAddress( struct FTraceHook *hook )
{
    struct kprobe kp = { .symbol_name = hook->name };

    int rc;
    rc = register_kprobe( &kp );

    if ( rc < 0 )
        return 0;

    hook->address = ( unsigned long )kp.addr;
    unregister_kprobe( &kp );

    if ( hook->address == 0 ) {
        pr_err( "Can't resolve address of: %s\n", hook->name );
        return -ENOENT;
    }

    *( ( unsigned long * )hook->original ) = hook->address;

    return 0;
}

static void notrace __FSHookFtraceThunk( unsigned long ip, unsigned long parentIp,
                                         struct ftrace_ops *ops, struct ftrace_regs *regs )
{
    struct FTraceHook *hook = container_of( ops, struct FTraceHook, ops );

    if ( !within_module( parentIp, THIS_MODULE ) ) {
        regs->regs.ip = ( unsigned long )hook->function;
    }
}

static int FSHookInitHook( struct FTraceHook *hook )
{
    int rc;
    rc = FSHookResolveHookAddress( hook );
    if ( rc != 0 )
        return rc;

    hook->ops.func = __FSHookFtraceThunk;
    hook->ops.flags = FTRACE_OPS_FL_SAVE_REGS | FTRACE_OPS_FL_RECURSION | FTRACE_OPS_FL_IPMODIFY;

    rc = ftrace_set_filter_ip( &hook->ops, hook->address, 0, 0 );
    if ( rc != 0 ) {
        pr_err( "ftrace_set_filter_ip() failed: %d\n", rc );
        return rc;
    }

    rc = register_ftrace_function( &hook->ops );
    if ( rc != 0 ) {
        pr_err( "register_ftrace_function() failed: %d\n", rc );
        ftrace_set_filter_ip( &hook->ops, hook->address, 1, 0 );
        return rc;
    }

    return 0;
}

static void FSHookRemoveHook( struct FTraceHook *hook )
{
    int rc;
    rc = unregister_ftrace_function( &hook->ops );
    if ( rc != 0 )
        pr_err( "unregister_ftrace_function() failed: %d\n", rc );

    rc = ftrace_set_filter_ip( &hook->ops, hook->address, 1, 0 );
    if ( rc != 0 )
        pr_err( "ftrace_set_filter_ip() failed: %d\n", rc );
}

int FSHook_FTraceInitHooks( struct FTraceHook *hooks, size_t count )
{
    int rc;
    size_t i;

    for ( i = 0; i < count; i++ ) {
        rc = FSHookInitHook( hooks + i );
        if ( rc != 0 )
            goto bail;
    }
    return 0;

bail:
    while ( i != 0 ) {
        --i;
        FSHookRemoveHook( hooks - i );
    }
    return rc;
}

void FSHook_FTraceRemoveHooks( struct FTraceHook *hooks, size_t count )
{
    size_t i;

    for ( i = 0; i < count; i++ )
        FSHookRemoveHook( hooks + i );
}