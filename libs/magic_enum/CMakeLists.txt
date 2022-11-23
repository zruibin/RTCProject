
message("magic_enum version was 0.8.1")
message("magic_enum github was https://github.com/Neargye/magic_enum")

set(magic_enum_name magic_enum)

file(GLOB_RECURSE magic_enum_all_file "${libs_dir}/magic_enum/*")
sourceGroup("" ${magic_enum_all_file})

