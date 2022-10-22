
message("httplib version was v0.11.2")
# https://github.com/yhirose/cpp-httplib.git

set(httplib_name httplib)

file(GLOB_RECURSE httplib_all_file "${libs_dir}/httplib/*")
sourceGroup("" ${httplib_all_file})

