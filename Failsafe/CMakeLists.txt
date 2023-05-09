add_subdirectory(ThirdParty/nlohmann)
add_subdirectory(ThirdParty/cxxopts)

set(SOURCES
    Modules/DC/DCModule.cpp
    Modules/DLP/DLPModule.cpp
    Modules/DLP/FSMessages/FSMessageFactory.cpp
    Modules/DLP/FSMessages/FSMessage.cpp
    Modules/DLP/FSMessages/OpenFSMessage.cpp
    Modules/DLP/FSMessages/RenameFSMessage.cpp
    Modules/DLP/FSMessages/UnlinkFSMessage.cpp
    Modules/DLP/Handlers/HandlerBase.cpp
    Modules/DLP/Handlers/NautilusHandler.cpp
    Modules/DLP/Handlers/SlackHandler.cpp
    Modules/DLP/Handlers/ChromeHandler.cpp
    Modules/DLP/Handlers/GenericHandler.cpp
    Modules/DLP/PathResolver/PathResolver.cpp
    Modules/DLP/PathResolver/PathType.cpp
    Modules/DLP/PathResolver/MountPointCache.cpp
    Modules/DLP/PathResolver/CloudCache.cpp
    SharedComponents/RuleProvider/ActionType.cpp 
    SharedComponents/RuleProvider/RuleBase.cpp
    SharedComponents/RuleProvider/RuleProvider.cpp
    SharedComponents/RuleProvider/DC/DCRule.cpp
    SharedComponents/RuleProvider/DC/DCOperationType.cpp
    SharedComponents/RuleProvider/DLP/DLPRule.cpp
    SharedComponents/RuleProvider/DLP/DLPOperationType.cpp
    SharedComponents/RuleProvider/DLP/Conditions/CommonDLPConditions.cpp
    SharedComponents/RuleProvider/DLP/Conditions/TransferDLPConditions.cpp
    SharedComponents/IncidentLogger/LocalDBLogger.cpp
)

set(HEADERS
    Modules/DC/DCModule.hpp
    Modules/DC/USBDevice.hpp
    Modules/DC/DCEvent.hpp
    Modules/DC/Operations/DCOperation.hpp
    Modules/DLP/DLPModule.hpp
    Modules/DLP/Operations/DLPOperation.hpp
    Modules/DLP/Operations/DLPOperationOnePath.hpp
    Modules/DLP/Operations/DLPOperationTwoPath.hpp
    Modules/DLP/FSMessages/FSMessageFactory.hpp
    Modules/DLP/FSMessages/FSMessage.hpp
    Modules/DLP/FSMessages/OpenFSMessage.hpp
    Modules/DLP/FSMessages/RenameFSMessage.hpp
    Modules/DLP/FSMessages/UnlinkFSMessage.hpp
    Modules/DLP/Handlers/HandlerBase.hpp
    Modules/DLP/Handlers/NautilusHandler.hpp
    Modules/DLP/Handlers/SlackHandler.hpp
    Modules/DLP/Handlers/ChromeHandler.hpp
    Modules/DLP/Handlers/GenericHandler.hpp
    Modules/DLP/PathResolver/ResolvedPath.hpp
    Modules/DLP/PathResolver/PathResolver.hpp
    Modules/DLP/PathResolver/PathType.hpp
    Modules/DLP/PathResolver/MountPointCache.hpp
    Modules/DLP/PathResolver/CloudCache.hpp
    SharedComponents/RuleProvider/ActionType.hpp
    SharedComponents/RuleProvider/RuleProvider.hpp
    SharedComponents/RuleProvider/RuleBase.hpp
    SharedComponents/RuleProvider/DC/DCRule.hpp
    SharedComponents/RuleProvider/DC/DCOperationType.hpp
    SharedComponents/RuleProvider/DLP/DLPRule.hpp
    SharedComponents/RuleProvider/DLP/DLPOperationType.hpp
    SharedComponents/RuleProvider/DLP/Conditions/IDLPConditions.hpp
    SharedComponents/RuleProvider/DLP/Conditions/CommonDLPConditions.hpp
    SharedComponents/RuleProvider/DLP/Conditions/TransferDLPConditions.hpp
    SharedComponents/IncidentLogger/ILogger.hpp
    SharedComponents/IncidentLogger/LocalDBLogger.hpp
    Utils/ThreadPool.hpp
    Utils/AtExit.hpp
)

add_library(failsafe_lib ${SOURCES} ${HEADERS})
target_include_directories(failsafe_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package(spdlog REQUIRED)
target_link_libraries(failsafe_lib PUBLIC spdlog::spdlog)

find_package(SQLiteCpp REQUIRED)
target_link_libraries(failsafe_lib PUBLIC SQLiteCpp)

target_link_libraries(failsafe_lib LINK_PUBLIC nlohmann)
target_link_libraries(failsafe_lib LINK_PUBLIC cxxopts)
target_link_libraries(failsafe_lib PUBLIC USBControl)
target_link_libraries(failsafe_lib PUBLIC FSHook)

add_executable(failsafe main.cpp)
target_link_libraries(failsafe PRIVATE failsafe_lib)

# Tests
find_package(Catch2 REQUIRED)

set(TESTS
  Utils/TestThreadPool.cpp
  SharedComponents/RuleProvider/Tests/TestActionType.cpp
  SharedComponents/RuleProvider/Tests/TestDLPRules.cpp
  SharedComponents/RuleProvider/Tests/TestDLPOperationType.cpp
  SharedComponents/RuleProvider/Tests/TestPathType.cpp
  SharedComponents/RuleProvider/Tests/TestDCRules.cpp
  SharedComponents/RuleProvider/Tests/TestRuleProvider.cpp
)

add_executable(failsafe_test ${TESTS})
target_link_libraries(failsafe_test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(failsafe_test PRIVATE failsafe_lib)
include(CTest)
include(Catch)
catch_discover_tests(failsafe_test)
