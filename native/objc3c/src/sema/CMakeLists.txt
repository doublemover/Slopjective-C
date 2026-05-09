set(OBJC3C_SEMA_PUBLIC_DEPS
  objc3c_ast
  objc3c_parse
  objc3c_diag
  objc3c_sema_model
  objc3c_support
)

add_library(objc3c_sema_contracts INTERFACE)
target_link_libraries(objc3c_sema_contracts INTERFACE
  ${OBJC3C_SEMA_PUBLIC_DEPS}
)
target_sources(objc3c_sema_contracts INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/model/frontend_concurrency_symbol_graph_summaries.h
  ${CMAKE_CURRENT_LIST_DIR}/model/frontend_linkage_summaries.h
  ${CMAKE_CURRENT_LIST_DIR}/model/frontend_type_source_closure.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_parser_sema_handoff_contract.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_parser_sema_handoff_scaffold.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_sema_canonical_literal_contract.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_sema_contract.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_sema_contract_core.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_sema_contract_feature_surfaces.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_sema_contract_type_handoff.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_sema_pass_manager_contract.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_sema_pass_manager_contract_flow.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_sema_pass_manager_contract_results.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_sema_pass_manager_contract_surface_validation.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_semantic_body_validation.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_semantic_integration_surface.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_semantic_type_helpers.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_semantic_type_lowering_contract.h
  ${CMAKE_CURRENT_LIST_DIR}/objc3_semantic_type_metadata_handoff.h
)

add_library(objc3c_semantic_type_surface STATIC
  objc3_semantic_collection_helpers.cpp
  objc3_semantic_context.cpp
  objc3_semantic_feature_claim_context.cpp
  objc3_semantic_feature_claim_rules.cpp
  objc3_semantic_feature_claims.cpp
  objc3_semantic_pass_helpers.cpp
  objc3_semantic_symbol_lookup.cpp
  objc3_semantic_type_factory.cpp
  objc3_semantic_type_relations.cpp
  objc3_semantic_type_predicates.cpp
)
objc3c_apply_build_defaults(objc3c_semantic_type_surface)
objc3c_apply_llvm_direct_config(objc3c_semantic_type_surface)
target_link_libraries(objc3c_semantic_type_surface PUBLIC
  ${OBJC3C_SEMA_PUBLIC_DEPS}
  objc3c_sema_contracts
)

add_library(objc3c_sema_equivalence STATIC
  objc3_sema_control_flow_equivalence.cpp
  objc3_sema_dispatch_ownership_equivalence.cpp
  objc3_sema_pass_block_equivalence.cpp
  objc3_sema_symbol_binding_equivalence.cpp
  objc3_sema_type_summary_equivalence.cpp
)
objc3c_apply_build_defaults(objc3c_sema_equivalence)
objc3c_apply_llvm_direct_config(objc3c_sema_equivalence)
target_link_libraries(objc3c_sema_equivalence PUBLIC
  ${OBJC3C_SEMA_PUBLIC_DEPS}
  objc3c_sema_contracts
  objc3c_semantic_type_surface
)

add_library(objc3c_sema_diagnostics_surface STATIC
  objc3_sema_diagnostic_contract.cpp
  objc3_sema_diagnostic_publication.cpp
  objc3_sema_diagnostics_bus.cpp
  objc3_sema_parser_handoff_publication.cpp
  objc3_sema_pass_diagnostics_driver.cpp
  objc3_sema_pass_flow_publication.cpp
  objc3_sema_pass_flow_scaffold.cpp
)
objc3c_apply_build_defaults(objc3c_sema_diagnostics_surface)
objc3c_apply_llvm_direct_config(objc3c_sema_diagnostics_surface)
target_link_libraries(objc3c_sema_diagnostics_surface PUBLIC
  ${OBJC3C_SEMA_PUBLIC_DEPS}
  objc3c_sema_contracts
  objc3c_sema_equivalence
  objc3c_semantic_type_surface
)

add_library(objc3c_semantic_passes_core STATIC
  objc3_pure_contract.cpp
  objc3_semantic_passes.cpp
)
objc3c_apply_build_defaults(objc3c_semantic_passes_core)
objc3c_apply_llvm_direct_config(objc3c_semantic_passes_core)
target_link_libraries(objc3c_semantic_passes_core PUBLIC
  ${OBJC3C_SEMA_PUBLIC_DEPS}
  objc3c_sema_contracts
  objc3c_semantic_type_surface
)

target_link_libraries(objc3c_sema_diagnostics_surface PUBLIC
  objc3c_semantic_passes_core
)

add_library(objc3c_sema STATIC
  objc3_sema_pass_manager.cpp
  objc3_type_form_scaffold.cpp
  objc3_static_analysis.cpp
)
objc3c_apply_build_defaults(objc3c_sema)
objc3c_apply_llvm_direct_config(objc3c_sema)
target_link_libraries(objc3c_sema PUBLIC
  ${OBJC3C_SEMA_PUBLIC_DEPS}
  objc3c_sema_contracts
  objc3c_sema_diagnostics_surface
  objc3c_sema_equivalence
  objc3c_semantic_passes_core
  objc3c_semantic_type_surface
)
# M141-B001 sema/type-system surface target for downstream topology hardening.
add_library(objc3c_sema_type_system INTERFACE)
target_link_libraries(objc3c_sema_type_system INTERFACE
  objc3c_sema
)
