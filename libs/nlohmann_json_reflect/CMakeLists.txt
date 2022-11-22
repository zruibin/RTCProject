
message("nlohmann_json_reflect github was https://github.com/1261385937/nlohmann_json_reflect")
message(WARNING "fix json_reflect.hpp")

set(nlohmann_json_reflect_name nlohmann_json_reflect)

file(GLOB_RECURSE nlohmann_json_reflect_all_file "${libs_dir}/nlohmann_json_reflect/*")
sourceGroup("" ${nlohmann_json_reflect_all_file})

message("warning: 为啥.hpp的无法生成单独的库？")
