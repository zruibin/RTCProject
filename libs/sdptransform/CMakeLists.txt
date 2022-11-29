
message("sdptransform github was https://github.com/ibc/libsdptransform")
message(WARNING "fix sdptransform.hpp for swap json header include")

set(sdptransform_name sdptransform)

file(GLOB_RECURSE sdptransform_all_file "${libs_dir}/sdptransform/*")
sourceGroup("" ${sdptransform_all_file})

