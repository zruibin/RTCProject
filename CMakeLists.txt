cmake_minimum_required (VERSION 3.0)

# projectname is the same as the main-executable
project(RTCProject)
set(TARGET ${PROJECT_NAME})
set(LIB_NAME "engine")

#set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_BUILD_TYPE "Release")

set(BUILD_WITH_VIEW false)

# 生成compile_commands.json,用于在VSCode等编辑器/IDE中给C/C++代码做函数定义跳转支持
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
message(WARNING "Build type: ${CMAKE_BUILD_TYPE}")

# 设置c++的编译选项
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
if(COMPILER_SUPPORTS_CXX17)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has C++17 support.")
else()
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support. Please use a different C++ compiler.")
endif()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99") 


if(CMAKE_BUILD_TYPE MATCHES "Debug")
    message(WARNING "Debug Model.")
    add_definitions("-Wall")
    add_definitions("-fPIC")
    add_definitions("-Wl,-lm")
    add_definitions("-Wno-dev")
    if(NOT WIN32)
        add_definitions("-Wextra")
        add_definitions("-Wpedantic")
    endif()
endif()

#-------------------------------------------------------------------------------

function(deleteFiles)
    foreach(FILE IN LISTS ARGN) 
        if ("${FILE}" MATCHES ".DS_Store")
            message(WARNING "删除文件: ${FILE}")
            file(REMOVE ${FILE})
        endif()
    endforeach()
endfunction()
file(GLOB_RECURSE delete_seerch_all_file "*")
deleteFiles(${delete_seerch_all_file})

# 按文件层次结构显示
function(sourceGroup prefix)
    message("prefix: ${prefix}")
    foreach(FILE IN LISTS ARGN) 
        get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")
        string(REPLACE "${prefix}" "" GROUP "${GROUP}")
        string(REPLACE "/" "\\" GROUP "${GROUP}")

        if ("${FILE}" MATCHES ".*\\.cpp" AND ".*\\.cc" AND ".*\\.c")
            set(GROUP "Source Files${GROUP}")
        elseif("${FILE}" MATCHES ".*\\.h" AND ".*\\.hpp")
            set(GROUP "Header Files${GROUP}")
        endif()

        source_group("${GROUP}" FILES "${FILE}")
    endforeach()
endfunction()


#-------------------------------------------------------------------------------

if(WIN32)
    message(WARNING "Now is windows")
elseif(APPLE)
    message(WARNING "Now is Apple systens.")
    set(CMAKE_OSX_ARCHITECTURES "x86_64")
    #set(CMAKE_OSX_ARCHITECTURES arm64)
    message(STATUS "osx architectures was ${CMAKE_OSX_ARCHITECTURES}")
    add_definitions("-D__x86_64__=1")
    add_definitions("-DWEBRTC_MAC=1")
    add_definitions("-DWEBRTC_POSIX=1")

    link_libraries("-framework Foundation")
    link_libraries("-framework Foundation")
    link_libraries("-framework CoreServices")
    link_libraries("-framework Metal")
    link_libraries("-framework AVFoundation")
    link_libraries("-framework VideoToolbox")
    link_libraries("-framework AudioUnit")
    link_libraries("-framework AudioToolbox")
    link_libraries("-framework CoreAudio")
    link_libraries("-framework ApplicationServices")
elseif(UNIX)
    message(WARNING "Now is UNIX-like OS's. Including aPPLE os x  and CygWin")
endif()
message(WARNING "operation system is ${CMAKE_SYSTEM}")

#-------------------------------------------------------------------------------


# 开关，是否要源码依赖方式
set(USE_DEPENDS_MODEL true)

set(src_dir "${PROJECT_SOURCE_DIR}/src")
set(libs_dir "${PROJECT_SOURCE_DIR}/libs")

include_directories(
    "${src_dir}"
    "${libs_dir}"
)

# 相当于g++命令的-L选项的作用
link_directories(
    "/usr/local"
    "${src_dir}"
    "${libs_dir}"
)

# 添加依赖
if(EXISTS "${PROJECT_SOURCE_DIR}/deps.cmake")
    include("deps.cmake")
endif()

# modules
include("${src_dir}/core/CMakeLists.txt")
include("${src_dir}/log/CMakeLists.txt")
include("${src_dir}/platform/CMakeLists.txt")
include("${src_dir}/util/CMakeLists.txt")
include("${src_dir}/network/CMakeLists.txt")
include("${src_dir}/engine/CMakeLists.txt")
include("${src_dir}/view/CMakeLists.txt")
# libs
include("${libs_dir}/nlohmann/CMakeLists.txt")
include("${libs_dir}/nlohmann_json_reflect/CMakeLists.txt")
include("${libs_dir}/asio/CMakeLists.txt")
include("${libs_dir}/websocketpp/CMakeLists.txt")
include("${libs_dir}/kcp/CMakeLists.txt")
include("${libs_dir}/lua/CMakeLists.txt")
include("${libs_dir}/sigslot/CMakeLists.txt")
include("${libs_dir}/httplib/CMakeLists.txt")


if(USE_DEPENDS_MODEL)

set(libs_list 
)

set(modules_list 
)

set(DEPENDS_LIBS 
    ${core_all_file}
    ${log_all_file}
    ${platform_all_file}
    ${util_all_file}
    ${network_all_file}
    ${engine_all_file}
    ${view_all_file}

    ${asio_all_file}
    ${kcp_all_file}
    ${lua_all_file}
    ${websocketpp_all_file}
    ${nlohmann_json_all_file}
    ${nlohmann_json_reflect_all_file}
    ${sigslot_all_file}
    ${httplib_all_file}
)

else()

set(libs_list 
    asio
    kcp
    lua
)

set(modules_list 
    core
    log
    platform
    util
    network
    engine
    view
)

set(DEPENDS_LIBS 
    ${websocketpp_all_file}
    ${nlohmann_json_all_file}
    ${nlohmann_json_reflect_all_file}
    ${sigslot_all_file}
    ${httplib_all_file}
)

endif()

set(all_list ${libs_list} ${modules_list})
message("all_list: ${all_list}")


#-------------------------------------------------------------------------------


file (GLOB_RECURSE abseil ./webrtc/abseil/**/*.h)
file (GLOB_RECURSE WebRTC ./webrtc/include/**/*.h)
file (GLOB_RECURSE include_third_party ./webrtc/include_third_party/**/*.h)

set(HEADERS
    ${abseil}
    ${WebRTC}
    ${include_third_party}
    ${Deps_include}
    src/app.h
)

set(SOURCES
    src/app.cc
)

set(EXECUTE_SOURCES
    src/main.cc
)

source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${HEADERS} ${SOURCES} ${EXECUTE_SOURCES})

#-------------------------------------------------------------------------------

# 相当于g++选项中的-I参数的作用
include_directories(
    "${CMAKE_SOURCE_DIR}/webrtc/abseil"
    "${CMAKE_SOURCE_DIR}/webrtc/include"
    "${CMAKE_SOURCE_DIR}/webrtc/include_third_party"
)

set(WebRTCLib "${CMAKE_SOURCE_DIR}/webrtc/lib/libwebrtc.a")
message(STATUS "WebRTC lis was ${CMAKE_BUILD_TYPE}: ${WebRTCLib}")
link_libraries(${WebRTCLib})

#-------------------------------------------------------------------------------


set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR}/Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR}/Lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR}/Bin)
message(STATUS "OUTPUT_DIR: ${OUTPUT_DIR}")
message(STATUS "ARCHIVE_OUTPUT_DIRECTORY: ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
message(STATUS "LIBRARY_OUTPUT_DIRECTORY: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
message(STATUS "RUNTIME_OUTPUT_DIRECTORY: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

set(TARGET_STATIC_LIB "${LIB_NAME}_static")
# add_library(${TARGET_STATIC_LIB} STATIC ${DEPENDS_LIBS} ${HEADERS} ${SOURCES} ${all_list})
set(TARGET_SHARED_LIB "${LIB_NAME}_shared")
# add_library(${TARGET_SHARED_LIB} SHARED ${DEPENDS_LIBS} ${HEADERS} ${SOURCES} ${all_list})

if(BUILD_WITH_VIEW)

file (GLOB_RECURSE ASSETS "assets/*")
sourceGroup("" ${ASSETS})

qt_add_executable(${TARGET} MANUAL_FINALIZATION ${DEPENDS_LIBS} ${HEADERS} ${SOURCES} ${EXECUTE_SOURCES} ${ASSETS})

set_target_properties(${TARGET} PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

if(APPLE AND NOT IOS)
    set_target_properties(${TARGET} PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/plistInfo.plist.in"
    )
elseif(IOS)
    set_target_properties(${TARGET} PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/plist/Info.plist.in"
        MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
        #MACOSX_BUNDLE_SHORT_VERSION_STRING ${VERSION}
    )
endif()


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../Resources/assets"
)

target_link_libraries(${TARGET} PRIVATE ${all_list} ${deps_list} ${QtModuleLibs})

qt_finalize_executable(${TARGET})

else()

add_executable(${TARGET} ${DEPENDS_LIBS} ${HEADERS} ${SOURCES} ${EXECUTE_SOURCES})
target_link_libraries(${TARGET} PRIVATE ${all_list} ${deps_list})

endif()

#-------------------------------------------------------------------------------

#[[
message(STATUS "origin installed directory: ${CMAKE_INSTALL_PREFIX}")
if(${CMAKE_INSTALL_PREFIX} MATCHES "/usr/local")
    set(install_directory "${PROJECT_SOURCE_DIR}/install")
    file(MAKE_DIRECTORY ${install_directory})
    set(CMAKE_INSTALL_PREFIX ${install_directory})
    message(STATUS "Create installed directory: ${CMAKE_INSTALL_PREFIX}")
endif()
message(STATUS "Project will be installed to ${CMAKE_INSTALL_PREFIX}")

install(
    TARGETS ${TARGET} ${TARGET_STATIC_LIB} ${TARGET_SHARED_LIB}
    RUNTIME DESTINATION bin 
    LIBRARY DESTINATION lib
)
set(output_head_files
    "${src_dir}/app.h"
)
install(FILES ${output_head_files} DESTINATION include)
]]

#-------------------------------------------------------------------------------


add_custom_target(run
    COMMAND ./${TARGET}
    WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMENT "Intermediate Run target: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)


