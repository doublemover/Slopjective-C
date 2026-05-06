add_library(objc3c_lower STATIC
  core/lowering_primitive_ops.cpp
  objc3_lowering_contract.cpp
)
objc3c_apply_build_defaults(objc3c_lower)
objc3c_apply_llvm_direct_config(objc3c_lower)
target_link_libraries(objc3c_lower PUBLIC
  objc3c_lower_model
  objc3c_sema_type_system
)