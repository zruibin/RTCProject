
message("asio version was 1.18.1")

set(asio_name asio)

add_definitions(-DASIO_HEADER_ONLY)

file (GLOB_RECURSE asio_all_file "${libs_dir}/asio/*")

if(USE_DEPENDS_MODEL)
    sourceGroup("" ${asio_all_file})
else()
    add_library(${asio_name} ${asio_all_file})
    sourceGroup("/libs/asio" ${asio_all_file})
endif()



