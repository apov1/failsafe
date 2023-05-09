#include <fstream>
#include <signal.h>
#include <sstream>

#include "Modules/DC/DCModule.hpp"
#include "Modules/DLP/DLPModule.hpp"
#include "SharedComponents/IncidentLogger/LocalDBLogger.hpp"
#include "cxxopts/cxxopts.hpp"
#include "spdlog/spdlog.h"

int SleepUntilTerminationSignal()
{
    sigset_t sigset;
    sigemptyset( &sigset );
    sigaddset( &sigset, SIGINT );
    sigaddset( &sigset, SIGTERM );
    pthread_sigmask( SIG_BLOCK, &sigset, nullptr );

    int signum = 0;
    sigwait( &sigset, &signum );
    return signum;
}

std::string LoadRuleFile( const std::string &path )
{
    spdlog::info( "Loading rule file at {}", path );

    std::ifstream ruleFile( path );
    std::stringstream buffer;
    buffer << ruleFile.rdbuf();

    return buffer.str();
}

int main( int argc, const char *argv[] )
{
    cxxopts::Options options( "Failsafe; DLP proof-of-concept" );
    options.add_options()( "kernelModule", "Path to KernFSHook kerel module",
                           cxxopts::value< std::string >() );
    options.add_options()( "policy", "DLP policy specification file",
                           cxxopts::value< std::string >() );
    options.add_options()( "incidents", "Incident Database file", cxxopts::value< std::string >() );

    options.parse_positional( { "kernelModule" } );
    auto optResult = options.parse( argc, argv );

    try {
        LocalDBLogger logger( optResult[ "incidents" ].as< std::string >() );
        RuleProvider ruleProvider;
        ruleProvider.UpdateRules( LoadRuleFile( optResult[ "policy" ].as< std::string >() ) );

        DLPModule dlpModule( optResult[ "kernelModule" ].as< std::string >(), ruleProvider,
                             logger );
        DCModule dcModule( ruleProvider, logger );

        spdlog::info( "All modules started!" );
        auto signum = SleepUntilTerminationSignal();
        spdlog::info( "Received signal number {}. Shutting down!", signum );
    }
    catch ( const std::exception &e ) {
        spdlog::critical( "Uncaught exception: {}", e.what() );
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}