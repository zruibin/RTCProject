if(ENABLE_UNIT_TESTS)


message("This is Unit Test.")

set(TEST_NAME "Test_${LIB_NAME}")

enable_testing()

message(WARNING "This is Unit Test->${TEST_NAME}")

include(FetchContent)
FetchContent_Declare(
  googletest
  URL "${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest-release-1.12.1.tar.gz"
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
set(BUILD_GTEST ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


file(GLOB_RECURSE TEST_ALL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/test/*")
sourceGroup("" ${TEST_ALL_FILES})

add_executable(${TEST_NAME} ${TEST_ALL_FILES})

target_link_libraries(
    ${TEST_NAME}
    gtest 
    gtest_main
    gmock
    gmock_main
    pthread
    ${all_list} ${deps_list} ${TARGET_LIB}
)


endif()