
message("nlohmann json version was 3.10.5")

set(nlohmann_json_name nlohmann_json)

file(GLOB_RECURSE nlohmann_json_all_file "${libs_dir}/nlohmann/*")
sourceGroup("" ${nlohmann_json_all_file})

message("warning: 为啥.hpp的无法生成单独的库？")
