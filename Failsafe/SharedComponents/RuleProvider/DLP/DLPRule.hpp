#pragma once

#include "Modules/DLP/Operations/DLPOperation.hpp"
#include "SharedComponents/RuleProvider/ActionType.hpp"
#include "SharedComponents/RuleProvider/DLP/Conditions/IDLPConditions.hpp"
#include "SharedComponents/RuleProvider/DLP/DLPOperationType.hpp"
#include "SharedComponents/RuleProvider/RuleBase.hpp"

#include "nlohmann/json.hpp"

class DLPRule : public RuleBase {
public:
    bool CheckRule( const DLPOperation &operation ) const;

protected:
    virtual void FromJsonSpecific( const nlohmann::json &j ) override;
    virtual void FromJsonConditions( const nlohmann::json &j ) override;

private:
    DLPOperationType operation_;
    std::shared_ptr< IDLPConditions > conditions_;
};
