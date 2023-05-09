#pragma once

#include <filesystem>
#include <map>
#include <memory>
#include <set>

#include "FSHook/FSHook.h"
#include "Modules/DLP/FSMessages/OpenFSMessage.hpp"
#include "Modules/DLP/Handlers/GenericHandler.hpp"
#include "Modules/DLP/Handlers/HandlerBase.hpp"
#include "Modules/DLP/PathResolver/PathResolver.hpp"
#include "SharedComponents/IncidentLogger/ILogger.hpp"
#include "SharedComponents/RuleProvider/RuleProvider.hpp"
#include "Utils/ThreadPool.hpp"

class DLPModule {
public:
    DLPModule( std::filesystem::path pathToKmod, RuleProvider &ruleProvider, ILogger &logger );
    ~DLPModule();

    void HandleFSEvent( FSHookConnector *connector, FSHookMessage *event );

private:
    RuleProvider &ruleProvider_;
    ILogger &logger_;
    ThreadPool threadPool_;
    FSHookConnector fsHookConnector_;
    PathResolver pathResolver_;
    GenericHandler genericHandler_;

    std::map< std::string, std::unique_ptr< HandlerBase >, std::less<> > handlers_;
    std::filesystem::path pathToKmod_;
};
