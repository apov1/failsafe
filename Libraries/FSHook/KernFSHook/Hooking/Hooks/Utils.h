#pragma once

#include "Hooking/ProcessInfo.h"
#include <linux/fs.h>
#include <stddef.h>

char *ResolvePath( int dfd, const char __user *openPathRaw, char *buffer, size_t bufferSize );
void OurPutname( struct filename *name );
int ShouldFilterPath( const char *path );
int ShouldFilterProcess( ProcessInfo *process );
