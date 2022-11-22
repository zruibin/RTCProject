
message("Engine Module.")

set(engine_name engine)

file (GLOB_RECURSE engine_all_file "${src_dir}/engine/*")
sourceGroup("/src" ${engine_all_file})

if(NOT USE_DEPENDS_MODEL)
    add_library(${engine_name} ${engine_all_file})
endif()
