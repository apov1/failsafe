#pragma once

#include <memory>

#include "FSHook/FSHook.h"
#include "Modules/DLP/FSMessages/FSMessage.hpp"

namespace FSMessageFactory {

std::shared_ptr< FSMessage > Construct( FSHookConnector *connector, FSHookMessage *event );

} // end namespace FSMessageFactory