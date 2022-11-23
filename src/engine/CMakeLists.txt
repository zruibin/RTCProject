
message("Engine Module.")

set(engine_name engine)

file (GLOB_RECURSE engine_all_file "${src_dir}/engine/*")
sourceGroup("${sourceGroupPrefixName}" ${engine_all_file})

