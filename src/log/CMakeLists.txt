
message("Log Manager Module.")

set(log_name log)

file (GLOB_RECURSE log_all_file "${src_dir}/log/*")
sourceGroup("${sourceGroupPrefixName}" ${log_all_file})

