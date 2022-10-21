
message("websocketpp version was 0.8.2")

set(websocket_name websocketpp)

file(GLOB_RECURSE websocketpp_all_file "${libs_dir}/websocketpp/*.hpp")
# add_library(${websocket_name} ${websocketpp_all_file})
sourceGroup("" ${websocketpp_all_file})

add_definitions(-DASIO_STANDALONE)
add_definitions(-DASIO_HAS_STD_SYSTEM_ERROR)
message("warning: 为啥.hpp的无法生成单独的库？")
