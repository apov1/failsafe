#pragma once

#include <linux/module.h>

struct filename;

extern asmlinkage int ( *OriginalUnlinkat )( int dfd, struct filename *name );
asmlinkage int HookedUnlinkat( int dfd, struct filename *name );