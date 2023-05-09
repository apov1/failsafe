#pragma once

#include <pthread.h>
#include <stdbool.h>

#include "Messages/Message.h"

typedef struct FSHookConnectorStruct FSHookConnector;

/**
 * @brief Type of callback that will be called for any event received from the kernel.
 */
typedef void ( *FSHookEventHandlerBlock )( FSHookConnector *connector, FSHookMessage *event,
                                           void *data );

/**
 * @brief Struct representing internal state of the FSHookConnector.
 * The fields SHOULD NOT be accesed by the user and can be changed at any time.
 */
struct FSHookConnectorStruct {
    void *socket;
    int familyId;
    pthread_t processingThread;
    bool ending;
    pthread_mutex_t startedMtx;
    pthread_cond_t startedCv;
    FSHookEventHandlerBlock processingCallback;
    void *data;
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the previously allocated instance of FSHookConnector.
 *
 * @param connector FSHookConnector instance to initialize. If NULL, the function fails.
 * @param handler callback to call for every event received from the kernel.
 * @param data pointer to arbitrary user data that will be passed to the handler callback
 * @return int 0 on success, any positive integer on failure
 */
int FSHookConnectorInit( FSHookConnector *connector, FSHookEventHandlerBlock handler, void *data );

/**
 * @brief Registers the FSHookConnector with the kernel. After call to this function, the handler
 * specified in the FSHookConnectorInit will start being called.
 *
 * @param connector FSHookConnector to register. If NULL, the function fails.
 * @return int 0 on success, any positive integer on failure
 */
int FSHookRegister( FSHookConnector *connector );

/**
 * @brief Frees the FSHookConnector. After call to this function, the specified callback will not be
 * called anymore.
 *
 * @param connector FSHookConnector to free. If NULL, the function is a noop.
 */
void FSHookConnectorFree( FSHookConnector *connector );

/**
 * @brief Frees the FSHookMessage.
 *
 * @param event FSHookMessage to free. If NULL, the function is a noop.
 */
void FSHookFreeMessage( FSHookMessage *event );

/**
 * @brief Replies the authorization status to the kernel. All messages must be replied to.
 *
 * @param connector FSHookConnector from which the message was received.
 * @param event FSHookMessage to reply.
 * @param response authorization status, 1 to allow event, 0 to deny it.
 * @return int 0 on success, any positive integer on failure.
 */
int FSHookReplyToEvent( FSHookConnector *connector, FSHookMessage *event, bool response );

#ifdef __cplusplus
}
#endif
