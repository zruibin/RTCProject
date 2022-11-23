
message("Core Module.")

set(core_name core)

file (GLOB_RECURSE core_all_file "${src_dir}/core/*")
sourceGroup("${sourceGroupPrefixName}" ${core_all_file})

if(NOT USE_DEPENDS_MODEL)
    add_library(${core_name} ${core_all_file})
endif()
