
message("Platform Module.")

set(platform_name platform)

file (GLOB_RECURSE platform_all_file "${src_dir}/platform/*")
sourceGroup("${sourceGroupPrefixName}" ${platform_all_file})

