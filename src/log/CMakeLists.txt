
message("Log Manager Module.")

set(log_name log)

file (GLOB_RECURSE log_all_file "${src_dir}/log/*")
sourceGroup("/src" ${log_all_file})

if(NOT USE_DEPENDS_MODEL)
    add_library(${log_name} ${log_all_file})
endif()
