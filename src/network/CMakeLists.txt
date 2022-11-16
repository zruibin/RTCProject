
message("Network Module.")

set(network_name log)

file (GLOB_RECURSE network_all_file "${src_dir}/network/*")
sourceGroup("/src" ${network_all_file})

if(NOT USE_DEPENDS_MODEL)
    add_library(${network_name} ${network_all_file})
endif()
