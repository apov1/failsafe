#include "SharedComponents/RuleProvider/DC/DCRule.hpp"

bool DCRule::CheckRule( const DCOperation &operation ) const
{
    bool doesApply = ( operation.Operation() == operation_ );

    if ( productId_.has_value() )
        doesApply = doesApply && ( productId_.value() == operation.Device().ProductID() );

    if ( vendorId_.has_value() )
        doesApply = doesApply && ( vendorId_.value() == operation.Device().VendorID() );

    if ( deviceClass_.has_value() )
        doesApply = doesApply && ( deviceClass_.value() == operation.Device().DeviceClass() );

    if ( deviceSubclass_.has_value() )
        doesApply = doesApply && ( deviceSubclass_.value() == operation.Device().DeviceSubclass() );

    if ( deviceProtocol_.has_value() )
        doesApply = doesApply && ( deviceProtocol_.value() == operation.Device().DeviceProtocol() );

    if ( serial_.has_value() )
        doesApply = doesApply && ( serial_.value() == operation.Device().Serial() );

    return doesApply;
}

/* virtual */ void DCRule::FromJsonSpecific( const nlohmann::json &j )
{
    operation_ = DCOperationFromString( j.at( "operationType" ).get< std::string >() );
}

/* virtual */ void DCRule::FromJsonConditions( const nlohmann::json &j )
{
    if ( j.contains( "VendorID" ) )
        j[ "VendorID" ].get_to( vendorId_ );

    if ( j.contains( "ProductID" ) )
        j[ "ProductID" ].get_to( productId_ );

    if ( j.contains( "DeviceClass" ) )
        j[ "DeviceClass" ].get_to( deviceClass_ );

    if ( j.contains( "DeviceSubclass" ) )
        j[ "DeviceSubclass" ].get_to( deviceSubclass_ );

    if ( j.contains( "DeviceProtocol" ) )
        j[ "DeviceProtocol" ].get_to( deviceProtocol_ );

    if ( j.contains( "Serial" ) )
        j[ "Serial" ].get_to( serial_ );
}

/* virtual */ std::string DCRule::ToString() const
{
    std::stringstream ss;
    ss << "Rule (" << RuleBase::ToString() << ", conditions:";

    if ( vendorId_.has_value() )
        ss << "\n\tVendorID: " << vendorId_.value();

    if ( productId_.has_value() )
        ss << "\n\tProductID: " << productId_.value();

    if ( deviceClass_.has_value() )
        ss << "\n\tDeviceClass: " << deviceClass_.value();

    if ( deviceSubclass_.has_value() )
        ss << "\n\tDeviceSubclass: " << deviceSubclass_.value();

    if ( deviceProtocol_.has_value() )
        ss << "\n\tDeviceProtocol: " << deviceProtocol_.value();

    if ( serial_.has_value() )
        ss << "\n\tSerial: " << serial_.value();

    ss << "\n)";
    return ss.str();
}
