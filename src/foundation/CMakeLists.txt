
message("Foundation Module.")

set(foundation_name foundation)

file (GLOB_RECURSE foundation_all_file "${src_dir}/foundation/*")
sourceGroup("${sourceGroupPrefixName}" ${foundation_all_file})

