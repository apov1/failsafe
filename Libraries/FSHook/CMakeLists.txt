
set(SOURCES
    FSHook/FSHook.c
    FSHook/KModuleHandler.c
    FSHook/NetlinkParser.c
)

set(HEADERS
    FSHook/FSHook.h
    FSHook/KModuleHandler.h
    FSHook/NetlinkParser.h
    FSHook/Messages/Message.h
)

add_library(FSHook STATIC ${SOURCES} ${HEADERS})

target_include_directories(FSHook PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(FSHook PUBLIC pthread)
target_link_libraries(FSHook PRIVATE nl-3)
target_link_libraries(FSHook PRIVATE nl-genl-3)
target_include_directories(FSHook PRIVATE "/usr/include/libnl3" "..")

add_subdirectory(KernFSHook)

# Tests
find_package(Catch2 REQUIRED)

set(TESTS
  FSHook/Tests/TestFSHook.cpp
  FSHook/Tests/TestNetlinkParser.cpp
)

add_executable(fshook_test ${TESTS})
target_link_libraries(fshook_test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(fshook_test PRIVATE FSHook)
target_link_libraries(fshook_test PRIVATE nl-3)
target_link_libraries(fshook_test PRIVATE nl-genl-3)
target_include_directories(fshook_test PRIVATE "/usr/include/libnl3" "..")

include(CTest)
include(Catch)
catch_discover_tests(fshook_test)
