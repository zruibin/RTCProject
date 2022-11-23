
message("Network Module.")

set(network_name network)

file (GLOB_RECURSE network_all_file "${src_dir}/network/*")
sourceGroup("${sourceGroupPrefixName}" ${network_all_file})

