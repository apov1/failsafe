set(SOURCES
  USBControl/UCDeviceMonitor.c
  USBControl/UCDeviceEnumerate.c
  USBControl/Events/UCDeviceEvent.c
  USBControl/Events/UCDeviceInfo.c
)

set(HEADERS
  USBControl/USBControl.h
  USBControl/UCDeviceMonitor.h
  USBControl/UCDeviceEnumerate.h
  USBControl/Events/UCDeviceEvent.h
  USBControl/Events/UCDeviceInfo.h
)

add_library(USBControl STATIC ${SOURCES} ${HEADERS})

target_include_directories(USBControl PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(USBControl PUBLIC pthread)
target_link_libraries(USBControl PUBLIC udev)

# Tests
find_package(Catch2 REQUIRED)

set(TESTS
  USBControl/Tests/TestDeviceMonitor.cpp
  USBControl/Tests/TestDeviceEnumerate.cpp
)

add_executable(usbcontrol_test ${TESTS})
target_link_libraries(usbcontrol_test PRIVATE Catch2::Catch2WithMain)
target_link_libraries(usbcontrol_test PRIVATE USBControl)
include(CTest)
include(Catch)
catch_discover_tests(usbcontrol_test)