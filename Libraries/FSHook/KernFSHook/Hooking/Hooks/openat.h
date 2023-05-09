#pragma once

#include <linux/module.h>
#include <uapi/linux/openat2.h>

extern asmlinkage long ( *OriginalOpenat )( int dfd, const char __user *filename,
                                            struct open_how *how );
asmlinkage long HookedOpenat( int dfd, const char __user *filename, struct open_how *how );