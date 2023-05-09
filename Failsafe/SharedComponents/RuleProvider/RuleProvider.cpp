#include "SharedComponents/RuleProvider/RuleProvider.hpp"

#include <mutex>

void RuleProvider::UpdateRules( const std::string &serialized )
{
    auto json = nlohmann::json::parse( serialized );

    std::unique_lock lock( mtx_ );

    defaultAction_ = ActionFromString( json.at( "defaultAction" ).get< std::string >() );
    json.at( "DLPRules" ).get_to( dlpRules_ );
    json.at( "DCRules" ).get_to( dcRules_ );
    std::sort( dlpRules_.begin(), dlpRules_.end() );
    std::sort( dcRules_.begin(), dcRules_.end() );
}

ActionType RuleProvider::DefaultAction() const noexcept
{
    std::shared_lock lock( mtx_ );

    return defaultAction_;
}

std::optional< DLPRule > RuleProvider::GetRule( const DLPOperation &op ) const
{
    std::shared_lock lock( mtx_ );

    for ( const auto &rule : dlpRules_ ) {
        if ( rule.CheckRule( op ) )
            return std::make_optional( rule );
    }

    return std::nullopt;
}

std::optional< DCRule > RuleProvider::GetRule( const DCOperation &op ) const
{
    std::shared_lock lock( mtx_ );

    for ( const auto &rule : dcRules_ ) {
        if ( rule.CheckRule( op ) )
            return std::make_optional( rule );
    }

    return std::nullopt;
}

int RuleProvider::NumDLPRules() const
{
    std::shared_lock lock( mtx_ );
    return dlpRules_.size();
}

int RuleProvider::NumDCRules() const
{
    std::shared_lock lock( mtx_ );
    return dcRules_.size();
}
