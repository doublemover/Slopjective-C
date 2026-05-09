add_library(objc3c_frontend STATIC
  c_api.cpp
  frontend_anchor.cpp
  objc3c_frontend_result_ownership.cpp
  objc3_cli_frontend.cpp
)
objc3c_apply_build_defaults(objc3c_frontend)
objc3c_apply_llvm_direct_config(objc3c_frontend)
target_link_libraries(objc3c_frontend PUBLIC
  objc3c_artifacts
  objc3c_pipeline
  objc3c_runtime_abi
  objc3c_json
  objc3c_support
)
