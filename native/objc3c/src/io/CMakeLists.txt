add_library(objc3c_json STATIC
  json/json_error.cpp
  json/json_parser.cpp
  json/json_schema.cpp
  json/json_value.cpp
  json/json_writer.cpp
  objc3_json.cpp
)
objc3c_apply_build_defaults(objc3c_json)
objc3c_apply_llvm_direct_config(objc3c_json)

add_library(objc3c_io STATIC
  objc3_diagnostics_artifacts.cpp
  objc3_file_io.cpp
  objc3_manifest_artifacts.cpp
)
objc3c_apply_build_defaults(objc3c_io)
objc3c_apply_llvm_direct_config(objc3c_io)

add_library(objc3c_runtime_abi STATIC
  objc3_process.cpp
)
objc3c_apply_build_defaults(objc3c_runtime_abi)
objc3c_apply_llvm_direct_config(objc3c_runtime_abi)
target_link_libraries(objc3c_runtime_abi PUBLIC
  objc3c_json
)
target_link_libraries(objc3c_io PUBLIC
  objc3c_diag
  objc3c_json
  objc3c_runtime_abi
)
