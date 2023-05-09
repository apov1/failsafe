#pragma once

#include <linux/fs.h>
#include <linux/module.h>

extern asmlinkage int ( *OriginalRenameat )( int olddfd, struct filename *from, int newdfd,
                                             struct filename *to, unsigned int flags );

asmlinkage int HookedRenameat( int olddfd, struct filename *from, int newdfd, struct filename *to,
                               unsigned int flags );