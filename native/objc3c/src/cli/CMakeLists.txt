add_executable(objc3c-native
  objc3c_native_entrypoint.cpp
  "${CMAKE_CURRENT_LIST_DIR}/../main.cpp"
)
objc3c_apply_build_defaults(objc3c-native)
objc3c_apply_llvm_direct_config(objc3c-native)
objc3c_apply_runtime_output(objc3c-native)
