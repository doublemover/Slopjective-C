set(OBJC3C_IR_PUBLIC_DEPS
  objc3c_ast
  objc3c_lower
  objc3c_sema_type_system
  objc3c_support
)

add_library(objc3c_ir_core_models STATIC
  objc3_ir_c_string.cpp
  objc3_ir_function_signature_model.cpp
  objc3_ir_literal_parsing.cpp
  objc3_ir_module_identity.cpp
  objc3_ir_symbol_model.cpp
  objc3_ir_type_model.cpp
)
objc3c_apply_build_defaults(objc3c_ir_core_models)
objc3c_apply_llvm_direct_config(objc3c_ir_core_models)
target_link_libraries(objc3c_ir_core_models PUBLIC
  ${OBJC3C_IR_PUBLIC_DEPS}
)

add_library(objc3c_ir_runtime_surface STATIC
  objc3_ir_block_runtime_contracts.cpp
  objc3_ir_concurrency_identity.cpp
  objc3_ir_concurrency_runtime_call_emission.cpp
  objc3_ir_control_flow_ops.cpp
  objc3_ir_direct_call_emission.cpp
  objc3_ir_entry_point_emission.cpp
  objc3_ir_expression_emission.cpp
  objc3_ir_function_definition_emission.cpp
  objc3_ir_message_send_emission.cpp
  objc3_ir_message_send_lowering.cpp
  objc3_ir_message_send_validation.cpp
  objc3_ir_method_definition_plan.cpp
  objc3_ir_prototype_declarations.cpp
  objc3_ir_receiver_dispatch_policy.cpp
  objc3_ir_receiver_identity_contracts.cpp
  objc3_ir_runtime_dispatch_calls.cpp
  objc3_ir_runtime_dispatch_declarations.cpp
  objc3_ir_runtime_helper_calls.cpp
  objc3_ir_runtime_dispatch_state.cpp
  objc3_ir_scope_cleanup_emission.cpp
  objc3_ir_runtime_bootstrap_global_emission.cpp
  objc3_ir_runtime_metadata_emission.cpp
  objc3_ir_statement_emission.cpp
  objc3_ir_synthesized_property_accessors.cpp
)
objc3c_apply_build_defaults(objc3c_ir_runtime_surface)
objc3c_apply_llvm_direct_config(objc3c_ir_runtime_surface)
target_link_libraries(objc3c_ir_runtime_surface PUBLIC
  ${OBJC3C_IR_PUBLIC_DEPS}
  objc3c_ir_core_models
)

add_library(objc3c_ir_publication_surface STATIC
  objc3_ir_canonical_literal_pools.cpp
  objc3_ir_deterministic_publication.cpp
  objc3_ir_emission_prologue.cpp
  objc3_ir_emission_readiness_publication.cpp
  objc3_ir_frontend_core_metadata_nodes.cpp
  objc3_ir_frontend_metadata_publication.cpp
  objc3_ir_lowering_extension_metadata_publication.cpp
  objc3_ir_module_emission_surface.cpp
  objc3_ir_static_data_emission.cpp
  objc3_ir_surface_serialization.cpp
)
objc3c_apply_build_defaults(objc3c_ir_publication_surface)
objc3c_apply_llvm_direct_config(objc3c_ir_publication_surface)
target_link_libraries(objc3c_ir_publication_surface PUBLIC
  ${OBJC3C_IR_PUBLIC_DEPS}
  objc3c_ir_core_models
  objc3c_ir_runtime_surface
)

add_library(objc3c_ir STATIC
  objc3_ir_emitter.cpp
)
objc3c_apply_build_defaults(objc3c_ir)
objc3c_apply_llvm_direct_config(objc3c_ir)
target_link_libraries(objc3c_ir PUBLIC
  ${OBJC3C_IR_PUBLIC_DEPS}
  objc3c_ir_core_models
  objc3c_ir_runtime_surface
  objc3c_ir_publication_surface
)
