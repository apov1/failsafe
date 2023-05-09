#include "SharedComponents/RuleProvider/DLP/Conditions/TransferDLPConditions.hpp"

#include "Modules/DLP/Operations/DLPOperationTwoPath.hpp"

TransferDLPConditions::TransferDLPConditions( const nlohmann::json &j )
    : CommonDLPConditions( j )
{
    if ( j.contains( "sourceType" ) )
        sourceType_ =
            std::make_optional( StringToPathType( j[ "sourceType" ].get< std::string >() ) );

    if ( j.contains( "destinationType" ) )
        destinationType_ =
            std::make_optional( StringToPathType( j[ "destinationType" ].get< std::string >() ) );
}

/* virtual */ bool TransferDLPConditions::CheckConditions( const DLPOperation &operation )
{
    if ( operation.Operation() != DLPOperationType::FileTransfer )
        return false;

    bool doesMatch = CommonDLPConditions::CheckConditions( operation );
    if ( !doesMatch )
        return false;

    const auto &transferOp = static_cast< const DLPOperationTwoPath & >( operation );

    if ( sourceType_.has_value() )
        doesMatch = doesMatch && ( sourceType_.value() == transferOp.SourcePath().Type() );

    if ( destinationType_.has_value() )
        doesMatch =
            doesMatch && ( destinationType_.value() == transferOp.DestinationPath().Type() );

    return doesMatch;
}