#include "SharedComponents/RuleProvider/RuleBase.hpp"

#include <sstream>

void from_json( const nlohmann::json &j, RuleBase &r )
{
    j.at( "name" ).get_to( r.name_ );
    j.at( "priority" ).get_to( r.priority_ );

    r.action_ = ActionFromString( j.at( "action" ).get< std::string >() );
    r.FromJsonSpecific( j );

    const auto &conditions = j.at( "conditions" );
    r.FromJsonConditions( conditions );
}

ActionType RuleBase::Action() const noexcept
{
    return action_;
}

const std::string &RuleBase::Name() const noexcept
{
    return name_;
}

bool RuleBase::operator<( const RuleBase &o ) const
{
    return ( priority_ < o.priority_ );
}

std::string RuleBase::ToString() const
{
    std::stringstream ss;
    ss << "name: " << name_ << ", priority: " << priority_
       << ", action: " << ActionToString( action_ );

    return ss.str();
}
