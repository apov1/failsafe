#include "Hooking/Hooks.h"

#include "Hooking/FTraceHooking.h"
#include "Hooking/Hooks/openat.h"
#include "Hooking/Hooks/renameat.h"
#include "Hooking/Hooks/unlinkat.h"
#include "Hooking/ResponseWaitMap.h"

static struct FTraceHook hooks[] = {
    HOOK( "do_sys_openat2", &HookedOpenat, &OriginalOpenat ),
    HOOK( "do_renameat2", &HookedRenameat, &OriginalRenameat ),
    HOOK( "do_unlinkat", &HookedUnlinkat, &OriginalUnlinkat ),
};

void FSHookInitAllHooks( void )
{
    int rc;
    rc = FSHook_FTraceInitHooks( hooks, ARRAY_SIZE( hooks ) );
    if ( rc != 0 ) {
        pr_err( "FSHookInitAllHooks(): error installing ftrace hooks." );
    }
}

void FSHookRemoveAllHooks( void )
{
    FSHook_FTraceRemoveHooks( hooks, ARRAY_SIZE( hooks ) );
    ClearMap();
}