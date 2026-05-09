add_library(objc3c_ir STATIC
  objc3_ir_c_string.cpp
  objc3_ir_function_signature_model.cpp
  objc3_ir_literal_parsing.cpp
  objc3_ir_module_identity.cpp
  objc3_ir_symbol_model.cpp
  objc3_ir_type_model.cpp
  objc3_ir_emitter.cpp
  objc3_runtime_call_lowering.cpp
)
objc3c_apply_build_defaults(objc3c_ir)
objc3c_apply_llvm_direct_config(objc3c_ir)
target_link_libraries(objc3c_ir PUBLIC
  objc3c_ast
  objc3c_lower
  objc3c_sema_type_system
  objc3c_support
)
