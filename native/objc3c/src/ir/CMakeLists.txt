add_library(objc3c_ir STATIC
  objc3_ir_emission_helpers.cpp
  objc3_ir_emitter.cpp
)
objc3c_apply_build_defaults(objc3c_ir)
objc3c_apply_llvm_direct_config(objc3c_ir)
target_link_libraries(objc3c_ir PUBLIC
  objc3c_lower
  objc3c_sema_type_system
  objc3c_support
)
