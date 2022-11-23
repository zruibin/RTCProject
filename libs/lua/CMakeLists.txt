
message("lua version was 5.4.3")

set(lua_name lua)

file(GLOB_RECURSE lua_all_file "${libs_dir}/lua/*")
sourceGroup("" ${lua_all_file})


# serach Lua main in source code
