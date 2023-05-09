#pragma once

#include <memory>

#include "Modules/DC/Operations/DCOperation.hpp"
#include "Modules/DLP/Operations/DLPOperation.hpp"
#include "SharedComponents/RuleProvider/ActionType.hpp"

#include "nlohmann/json.hpp"

class ILogger {
public:
    virtual ~ILogger() = default;

    virtual void SaveLog( ActionType takenAction, std::shared_ptr< DLPOperation > op ) = 0;
    virtual void SaveLog( ActionType takenAction, std::shared_ptr< DCOperation > op ) = 0;
};