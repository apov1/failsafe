#include "SharedComponents/RuleProvider/DLP/DLPRule.hpp"

#include "SharedComponents/RuleProvider/DLP/Conditions/CommonDLPConditions.hpp"
#include "SharedComponents/RuleProvider/DLP/Conditions/TransferDLPConditions.hpp"

bool DLPRule::CheckRule( const DLPOperation &operation ) const
{
    bool doesMatch = ( operation.Operation() == operation_ );
    if ( !doesMatch )
        return false;

    if ( conditions_ )
        return conditions_->CheckConditions( operation );
    return true;
}

/* virtual */ void DLPRule::FromJsonSpecific( const nlohmann::json &j )
{
    operation_ = DLPOperationFromString( j.at( "operationType" ).get< std::string >() );
}

/* virtual */ void DLPRule::FromJsonConditions( const nlohmann::json &j )
{
    if ( operation_ == DLPOperationType::FileTransfer )
        conditions_ = std::make_shared< TransferDLPConditions >( j );
    else
        conditions_ = std::make_shared< CommonDLPConditions >( j );
}