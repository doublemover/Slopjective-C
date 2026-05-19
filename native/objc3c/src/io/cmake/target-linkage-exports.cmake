add_library(objc3c_runtime_abi INTERFACE)

target_link_libraries(objc3c_runtime_abi_process PUBLIC
  objc3c_json
  objc3c_support
)

target_link_libraries(objc3c_runtime_abi_release_artifacts PUBLIC
  objc3c_runtime_abi_process
)

target_link_libraries(objc3c_runtime_abi_cross_module PUBLIC
  objc3c_runtime_abi_process
)

target_link_libraries(objc3c_runtime_abi_metadata PUBLIC
  objc3c_runtime_abi_process
)

target_link_libraries(objc3c_runtime_abi_release_artifacts PUBLIC
  objc3c_runtime_abi_metadata
)

target_link_libraries(objc3c_runtime_abi INTERFACE
  objc3c_runtime_abi_cross_module
  objc3c_runtime_abi_metadata
  objc3c_runtime_abi_process
  objc3c_runtime_abi_release_artifacts
)

target_link_libraries(objc3c_io PUBLIC
  objc3c_diag
  objc3c_json
  objc3c_runtime_abi
  objc3c_support
)
