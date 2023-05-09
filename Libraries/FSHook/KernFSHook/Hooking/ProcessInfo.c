#include "Hooking/ProcessInfo.h"

#include <linux/cred.h>
#include <linux/mm.h>
#include <linux/namei.h>
#include <linux/sched.h>
#include <linux/slab.h>

int GetProcessInfo( ProcessInfo *info )
{
    struct mm_struct *mm;

    mm = current->mm;
    if ( mm ) {
        down_read( &mm->mmap_lock );
        if ( mm->exe_file ) {
            info->_pathBuffer = kmalloc( PATH_MAX, GFP_ATOMIC );
            if ( info->_pathBuffer ) {
                info->exePath = d_path( &mm->exe_file->f_path, info->_pathBuffer, PATH_MAX );
                if ( unlikely( info->exePath == NULL ) ) {
                    return -ENOENT;
                }
            }
        }
        up_read( &mm->mmap_lock );
    }

    info->pid = task_tgid_nr( current );
    info->tid = task_pid_nr( current );
    info->uid = current_uid().val;

    return 0;
}

void FreeProcessInfo( ProcessInfo *info )
{
    kfree( info->_pathBuffer );
}