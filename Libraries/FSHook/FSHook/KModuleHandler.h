#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Loads the neccessary KernFSHook kernel module.
 *
 * @return int 0 on success, any positive integer on failure
 */
int LoadKernelModule( const char *path );

/**
 * @brief Unloads the neccessary KernFSHook kernel module.
 *
 * @return int 0 on success, any positive integer on failure
 */
int UnloadKernelModule( const char *name );

#ifdef __cplusplus
}
#endif