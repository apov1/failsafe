#pragma once

#include "Modules/DLP/Operations/DLPOperation.hpp"

class IDLPConditions {
public:
    virtual ~IDLPConditions() = default;
    virtual bool CheckConditions( const DLPOperation &operation ) = 0;
};