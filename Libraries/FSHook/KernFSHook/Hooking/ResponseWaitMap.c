#include "Hooking/ResponseWaitMap.h"

#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <linux/xarray.h>

static DEFINE_XARRAY_ALLOC( waitMap );
static DECLARE_WAIT_QUEUE_HEAD( wq );

WaitContext *InitWaitContext( void )
{
    int rc;
    WaitContext *ctx;

    ctx = ( WaitContext * )kmalloc( sizeof( WaitContext ), GFP_KERNEL );
    if ( unlikely( ctx == NULL ) )
        return NULL;

    ctx->responded = 0;
    // default is allowed
    ctx->response = 1;

    rc = xa_alloc( &waitMap, &( ctx->id ), ctx, xa_limit_32b, GFP_KERNEL );
    if ( rc != 0 )
        return NULL;

    return ctx;
}

uint8_t WaitForResponse( WaitContext *ctx )
{
    uint8_t response;
    wait_event_killable_timeout( wq, ctx->responded == 1, 10 * HZ );

    xa_erase( &waitMap, ctx->id );

    response = ctx->response;

    kfree( ctx );

    return response;
}

void SetResponseForContext( uint32_t id, uint8_t response )
{
    WaitContext *ctx;
    ctx = xa_load( &waitMap, id );
    if ( unlikely( ctx == NULL ) )
        return;

    ctx->response = response;
    ctx->responded = 1;

    wake_up( &wq );
}

void ClearMap( void )
{
    XA_STATE( xas, &waitMap, 0 );
    WaitContext *ctx;
    rcu_read_lock();
    xas_for_each( &xas, ctx, ULONG_MAX )
    {
        ctx->responded = 1;
    }
    rcu_read_unlock();

    wake_up( &wq );
}
