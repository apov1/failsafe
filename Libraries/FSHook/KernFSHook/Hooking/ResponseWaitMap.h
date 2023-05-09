#pragma once

#include <linux/types.h>

typedef struct {
    uint32_t id;
    uint8_t responded;
    uint8_t response;
} WaitContext;

WaitContext *InitWaitContext( void );
uint8_t WaitForResponse( WaitContext *ctx );
void SetResponseForContext( uint32_t id, uint8_t response );
void ClearMap( void );
