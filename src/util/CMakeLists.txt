
message("Util Module.")

set(util_name util)

file (GLOB_RECURSE util_all_file "${src_dir}/util/*")
sourceGroup("${sourceGroupPrefixName}" ${util_all_file})

if(NOT USE_DEPENDS_MODEL)
    add_library(${util_name} ${util_all_file})
endif()
