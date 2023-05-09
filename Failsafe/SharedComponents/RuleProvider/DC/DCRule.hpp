#pragma once

#include <optional>
#include <string>

#include "Modules/DC/Operations/DCOperation.hpp"
#include "SharedComponents/RuleProvider/ActionType.hpp"
#include "SharedComponents/RuleProvider/RuleBase.hpp"

#include "nlohmann/json.hpp"

class DCRule : public RuleBase {
public:
    bool CheckRule( const DCOperation &operation ) const;

    virtual std::string ToString() const override;

protected:
    virtual void FromJsonSpecific( const nlohmann::json &j ) override;
    virtual void FromJsonConditions( const nlohmann::json &j ) override;

private:
    DCOperationType operation_;

    std::optional< int > vendorId_;
    std::optional< int > productId_;
    std::optional< int > deviceClass_;
    std::optional< int > deviceSubclass_;
    std::optional< int > deviceProtocol_;
    std::optional< std::string > serial_;
};
