
message("Platform Module.")

set(platform_name platform)

file (GLOB_RECURSE platform_all_file "${src_dir}/platform/*")
sourceGroup("${sourceGroupPrefixName}" ${platform_all_file})

if(NOT USE_DEPENDS_MODEL)
    add_library(${platform_name} ${platform_all_file})
endif()
