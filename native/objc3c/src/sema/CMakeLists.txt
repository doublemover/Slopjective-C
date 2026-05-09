add_library(objc3c_sema STATIC
  objc3_parser_sema_handoff_contract.h
  objc3_parser_sema_handoff_scaffold.h
  objc3_semantic_type_helpers.h
  objc3_sema_diagnostics_bus.cpp
  objc3_sema_pass_block_equivalence.cpp
  objc3_sema_pass_flow_scaffold.cpp
  objc3_semantic_pass_helpers.cpp
  objc3_semantic_type_predicates.cpp
  objc3_sema_pass_manager.cpp
  objc3_semantic_passes.cpp
  objc3_type_form_scaffold.cpp
  objc3_static_analysis.cpp
  objc3_pure_contract.cpp
)
objc3c_apply_build_defaults(objc3c_sema)
objc3c_apply_llvm_direct_config(objc3c_sema)
target_link_libraries(objc3c_sema PUBLIC
  objc3c_ast
  objc3c_parse
  objc3c_diag
  objc3c_sema_model
  objc3c_support
)
# M141-B001 sema/type-system surface target for downstream topology hardening.
add_library(objc3c_sema_type_system INTERFACE)
target_link_libraries(objc3c_sema_type_system INTERFACE
  objc3c_sema
)
