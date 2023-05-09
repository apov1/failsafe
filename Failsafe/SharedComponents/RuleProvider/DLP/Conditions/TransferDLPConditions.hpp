#pragma once

#include <optional>

#include "Modules/DLP/PathResolver/PathType.hpp"
#include "SharedComponents/RuleProvider/DLP/Conditions/CommonDLPConditions.hpp"

class TransferDLPConditions : public CommonDLPConditions {
public:
    TransferDLPConditions( const nlohmann::json &j );

    virtual bool CheckConditions( const DLPOperation &operation ) override;

private:
    std::optional< PathType > sourceType_;
    std::optional< PathType > destinationType_;
};