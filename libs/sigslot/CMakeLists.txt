
message("sigslot version was 1.0.0")

set(sigslot_name sigslot)

file(GLOB_RECURSE sigslot_all_file "${libs_dir}/sigslot/*")
sourceGroup("" ${sigslot_all_file})

