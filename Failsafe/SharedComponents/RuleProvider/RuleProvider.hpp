#pragma once

#include <optional>
#include <shared_mutex>
#include <string>
#include <vector>

#include "Modules/DC/Operations/DCOperation.hpp"
#include "Modules/DLP/Operations/DLPOperation.hpp"

#include "SharedComponents/RuleProvider/ActionType.hpp"
#include "SharedComponents/RuleProvider/DC/DCRule.hpp"
#include "SharedComponents/RuleProvider/DLP/DLPRule.hpp"

class RuleProvider {
public:
    void UpdateRules( const std::string &serialized );

    ActionType DefaultAction() const noexcept;

    std::optional< DLPRule > GetRule( const DLPOperation &op ) const;
    std::optional< DCRule > GetRule( const DCOperation &op ) const;

    int NumDLPRules() const;
    int NumDCRules() const;

private:
    mutable std::shared_mutex mtx_;

    ActionType defaultAction_;
    std::vector< DLPRule > dlpRules_;
    std::vector< DCRule > dcRules_;
};