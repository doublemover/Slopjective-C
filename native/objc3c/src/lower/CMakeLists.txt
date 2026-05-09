add_library(objc3c_lower STATIC
  contracts/block_arc_lowering_plan.cpp
  contracts/function_method_lowering_state.cpp
  contracts/lowering_arc_contracts.cpp
  contracts/lowering_extended_contracts.cpp
  contracts/lowering_dispatch_and_arc_contracts.cpp
  contracts/lowering_diagnostics.cpp
  contracts/lowering_phase_io.cpp
  contracts/lowering_contract_validators.cpp
  contracts/lowering_summary_contracts.cpp
  contracts/lowering_summary_surfaces.cpp
  contracts/runtime_metadata_handoff.cpp
  contracts/runtime_dispatch_contracts.cpp
  core/lowering_primitive_ops.cpp
  metadata/lowering_metadata_helpers.cpp
  metadata/runtime_metadata_layout_policy.cpp
)
objc3c_apply_build_defaults(objc3c_lower)
objc3c_apply_llvm_direct_config(objc3c_lower)
target_link_libraries(objc3c_lower PUBLIC
  objc3c_ast
  objc3c_lower_model
  objc3c_sema_type_system
)
