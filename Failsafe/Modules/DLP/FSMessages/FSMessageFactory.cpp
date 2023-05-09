#include "Modules/DLP/FSMessages/FSMessageFactory.hpp"

#include <cassert>

#include "Modules/DLP/FSMessages/OpenFSMessage.hpp"
#include "Modules/DLP/FSMessages/RenameFSMessage.hpp"
#include "Modules/DLP/FSMessages/UnlinkFSMessage.hpp"

namespace FSMessageFactory {
std::shared_ptr< FSMessage > Construct( FSHookConnector *connector, FSHookMessage *event )
{
    switch ( event->eventType ) {
    case FSH_EVENT_OPEN:
        return std::make_shared< OpenFSMessage >( connector, event );
    case FSH_EVENT_RENAME:
        return std::make_shared< RenameFSMessage >( connector, event );
    case FSH_EVENT_UNLINK:
        return std::make_shared< UnlinkFSMessage >( connector, event );
    default:
        assert( false );
    }
}
} // end namespace FSMessageFactory