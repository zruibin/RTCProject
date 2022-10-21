
message("lua version was 5.4.3")

set(lua_name lua)

file(GLOB_RECURSE lua_all_file "${libs_dir}/lua/*")

if(USE_DEPENDS_MODEL)
    sourceGroup("" ${lua_all_file})
else()
    add_library(${lua_name} ${lua_all_file})
    sourceGroup("/libs/lua" ${lua_all_file})
endif()


# serach Lua main in source code
