#pragma once

#include "SharedComponents/RuleProvider/ActionType.hpp"

#include "nlohmann/json.hpp"

class RuleBase {
public:
    virtual ~RuleBase() = default;

    friend void from_json( const nlohmann::json &j, RuleBase &r );

    ActionType Action() const noexcept;
    const std::string &Name() const noexcept;

    bool operator<( const RuleBase &o ) const;

    virtual std::string ToString() const;

protected:
    virtual void FromJsonSpecific( const nlohmann::json &j ) = 0;
    virtual void FromJsonConditions( const nlohmann::json &j ) = 0;

private:
    std::string name_ = "unnamed";

    unsigned priority_ = -1;
    ActionType action_ = ActionType::Ignore;
};
