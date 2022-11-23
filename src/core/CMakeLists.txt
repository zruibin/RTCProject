
message("Core Module.")

set(core_name core)

file (GLOB_RECURSE core_all_file "${src_dir}/core/*")
sourceGroup("${sourceGroupPrefixName}" ${core_all_file})

