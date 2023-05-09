# based on https://musteresel.github.io/posts/2020/02/cmake-template-linux-kernel-module.html

execute_process(COMMAND uname -r OUTPUT_VARIABLE uname_r OUTPUT_STRIP_TRAILING_WHITESPACE)
set(KERN_BUILD_DIR "/lib/modules/${uname_r}/build")

find_file(KERN_MAKEFILE NAMES Makefile PATHS ${KERN_BUILD_DIR} NO_DEFAULT_PATH)
if(NOT KERN_MAKEFILE)
    message(FATAL_ERROR "There is no Makefile in KERN_BUILD_DIR!")
endif()

set(SOURCES
    main.c
    Hooking/FTraceHooking.c
    Hooking/Hooks.c
    Hooking/ResponseWaitMap.c
    Hooking/ProcessInfo.c
    Hooking/Variables.c
    Hooking/Hooks/openat.c
    Hooking/Hooks/renameat.c
    Hooking/Hooks/unlinkat.c
    Hooking/Hooks/Utils.c
    Netlink/Connection.c
    Netlink/MessageFactory.c
)

set(HEADERS
    Hooking/FTraceHooking.h
    Hooking/Hooks.h
    Hooking/ResponseWaitMap.h
    Hooking/ProcessInfo.h
    Hooking/Variables.h
    Hooking/Hooks/openat.h
    Hooking/Hooks/renameat.h
    Hooking/Hooks/unlinkat.h
    Hooking/Hooks/Utils.h
    Netlink/Connection.h
    Netlink/NLMsg.h
    Netlink/MessageFactory.h
    ../Common/EventType.h
    ../Common/NetlinkFamily.h
)

foreach (src ${SOURCES})
    string(REGEX REPLACE "[.]c$" ".o" obj ${src})
    set(OBJ_FILES ${OBJ_FILES} ${obj})
endforeach ()


string(REPLACE ";" " " MODULE_STRING_SOURCES "${OBJ_FILES}")

configure_file(Kbuild.in Kbuild @ONLY)

foreach(src ${SOURCES})
    configure_file(${src} ${src} COPYONLY)
endforeach()

foreach(hdr ${HEADERS})
    configure_file(${hdr} ${hdr} COPYONLY)
endforeach()

set(module_cmd ${CMAKE_MAKE_PROGRAM} -C ${KERN_BUILD_DIR} M=${CMAKE_CURRENT_BINARY_DIR})
add_custom_command(OUTPUT kern_fshook.ko
  COMMAND ${module_cmd} modules
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS ${SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/Kbuild
  VERBATIM)
add_custom_target(KernFSHook ALL DEPENDS kern_fshook.ko)
add_custom_target(KernFSHook-clean COMMAND ${module_cmd} clean)