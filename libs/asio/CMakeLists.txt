
message("asio version was 1.24.0")

set(asio_name asio)

add_definitions(-DASIO_HEADER_ONLY)

file (GLOB_RECURSE asio_all_file "${libs_dir}/asio/*")
sourceGroup("" ${asio_all_file})



