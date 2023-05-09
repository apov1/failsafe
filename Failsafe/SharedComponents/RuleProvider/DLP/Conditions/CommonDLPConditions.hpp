#pragma once

#include <regex>

#include "SharedComponents/RuleProvider/DLP/Conditions/IDLPConditions.hpp"

#include "nlohmann/json.hpp"

class CommonDLPConditions : public IDLPConditions {
public:
    CommonDLPConditions( const nlohmann::json &j );

    virtual bool CheckConditions( const DLPOperation &operation ) override;

private:
    std::optional< std::string > fileExtension_;
    std::optional< std::string > pathPrefix_;
    std::optional< std::regex > pathRegex_;
};