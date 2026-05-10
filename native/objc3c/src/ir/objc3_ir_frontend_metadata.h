#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "lower/objc3_lowering_contract.h"
#include "ir/objc3_ir_frontend_metadata_block.h"
#include "ir/objc3_ir_frontend_metadata_concurrency.h"
#include "ir/objc3_ir_frontend_metadata_dispatch.h"
#include "ir/objc3_ir_frontend_metadata_error_handling.h"
#include "ir/objc3_ir_frontend_metadata_metaprogramming_bundles.h"
#include "ir/objc3_ir_frontend_metadata_module_source_linkage.h"
#include "ir/objc3_ir_frontend_metadata_ownership.h"
#include "ir/objc3_ir_frontend_metadata_pipeline_readiness.h"
#include "ir/objc3_ir_frontend_metadata_runtime_bundles.h"
#include "ir/objc3_ir_frontend_metadata_runtime_metadata.h"
#include "ir/objc3_ir_frontend_metadata_runtime_support.h"
#include "ir/objc3_ir_frontend_metadata_semantic_surface.h"
#include "ir/objc3_ir_frontend_metadata_type_system.h"
// Historical extraction contract marker:
// #include "parse/objc3_parser_contract.h"

struct Objc3Program;

struct Objc3IRFrontendMetadata : Objc3IRFrontendRuntimeSupportMetadata,
                                 Objc3IRFrontendPipelineReadinessMetadata,
                                 Objc3IRFrontendRuntimeMetadata,
                                 Objc3IRFrontendDispatchMetadata,
                                 Objc3IRFrontendOwnershipMetadata,
                                 Objc3IRFrontendBlockMetadata,
                                 Objc3IRFrontendTypeSystemMetadata,
                                 Objc3IRFrontendModuleSourceLinkageMetadata,
                                 Objc3IRFrontendErrorHandlingMetadata,
                                 Objc3IRFrontendSemanticSurfaceMetadata,
                                 Objc3IRFrontendConcurrencyMetadata {
  std::uint8_t language_version = 3u;
  std::string language_profile = "canonical";
  std::string arc_mode = "disabled";
  bool arc_mode_enabled = false;
  bool versioned_conformance_report_lowering_ready = false;
  std::string versioned_conformance_report_lowering_replay_key;
  std::size_t canonical_literal_yes_rejection_sites = 0;
  std::size_t canonical_literal_no_rejection_sites = 0;
  std::size_t canonical_literal_null_rejection_sites = 0;
  std::size_t declared_interfaces = 0;
  std::size_t declared_implementations = 0;
  std::size_t resolved_interface_symbols = 0;
  std::size_t resolved_implementation_symbols = 0;
  std::size_t interface_method_symbols = 0;
  std::size_t implementation_method_symbols = 0;
  std::size_t linked_implementation_symbols = 0;
  bool deterministic_interface_implementation_handoff = false;
  std::size_t declared_protocols = 0;
  std::size_t declared_categories = 0;
  std::size_t resolved_protocol_symbols = 0;
  std::size_t resolved_category_symbols = 0;
  std::size_t protocol_method_symbols = 0;
  std::size_t category_method_symbols = 0;
  std::size_t linked_category_symbols = 0;
  bool deterministic_protocol_category_handoff = false;
  std::size_t declared_class_interfaces = 0;
  std::size_t declared_class_implementations = 0;
  std::size_t resolved_class_interfaces = 0;
  std::size_t resolved_class_implementations = 0;
  std::size_t linked_class_method_symbols = 0;
  std::size_t linked_category_method_symbols = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t protocol_composition_symbols = 0;
  std::size_t category_composition_sites = 0;
  std::size_t category_composition_symbols = 0;
  std::size_t invalid_protocol_composition_sites = 0;
  bool deterministic_class_protocol_category_linking_handoff = false;
  std::size_t selector_method_declaration_entries = 0;
  std::size_t selector_normalized_method_declarations = 0;
  std::size_t selector_piece_entries = 0;
  std::size_t selector_piece_parameter_links = 0;
  bool deterministic_selector_normalization_handoff = false;
  std::size_t property_declaration_entries = 0;
  std::size_t property_attribute_entries = 0;
  std::size_t property_attribute_value_entries = 0;
  std::size_t property_accessor_modifier_entries = 0;
  std::size_t property_getter_selector_entries = 0;
  std::size_t property_setter_selector_entries = 0;
  bool deterministic_property_attribute_handoff = false;
  std::string lowering_dispatch_dispatch_control_replay_key;
  std::size_t dispatch_dispatch_control_lowering_direct_call_candidate_sites = 0;
  std::size_t dispatch_dispatch_control_lowering_direct_members_defaulted_sites =
      0;
  std::size_t dispatch_dispatch_control_lowering_dynamic_opt_out_sites = 0;
  std::size_t dispatch_dispatch_control_lowering_final_container_sites = 0;
  std::size_t dispatch_dispatch_control_lowering_sealed_container_sites = 0;
  std::size_t dispatch_dispatch_control_lowering_override_legality_sites = 0;
  std::size_t dispatch_dispatch_control_lowering_metadata_preserved_callable_sites =
      0;
  std::size_t
      dispatch_dispatch_control_lowering_metadata_preserved_container_sites = 0;
  std::size_t dispatch_dispatch_control_lowering_guard_blocked_sites = 0;
  std::size_t dispatch_dispatch_control_lowering_contract_violation_sites = 0;
  bool deterministic_dispatch_dispatch_control_lowering_handoff = false;
  std::string lowering_interop_interop_replay_key;
  std::size_t interop_interop_lowering_foreign_callable_sites = 0;
  std::size_t interop_interop_lowering_c_foreign_callable_sites = 0;
  std::size_t interop_interop_lowering_objc_runtime_parity_callable_sites = 0;
  std::size_t interop_interop_lowering_ownership_bridge_callable_sites = 0;
  std::size_t interop_interop_lowering_error_surface_sites = 0;
  std::size_t interop_interop_lowering_async_boundary_sites = 0;
  std::size_t interop_interop_lowering_swift_concurrency_metadata_sites = 0;
  std::size_t interop_interop_lowering_interface_preserved_foreign_callable_sites =
      0;
  std::size_t
      interop_interop_lowering_interface_preserved_metadata_annotation_sites = 0;
  std::size_t interop_interop_lowering_guard_blocked_sites = 0;
  std::size_t interop_interop_lowering_contract_violation_sites = 0;
  bool deterministic_interop_interop_lowering_handoff = false;
  std::string lowering_interop_foreign_call_lifetime_replay_key;
  std::size_t interop_foreign_call_lifetime_lowering_foreign_callable_sites = 0;
  std::size_t
      interop_foreign_call_lifetime_lowering_c_foreign_callable_sites = 0;
  std::size_t
      interop_foreign_call_lifetime_lowering_objc_runtime_parity_callable_sites =
          0;
  std::size_t interop_foreign_call_lifetime_lowering_ownership_bridge_sites = 0;
  std::size_t interop_foreign_call_lifetime_lowering_lifetime_bridge_sites = 0;
  std::size_t
      interop_foreign_call_lifetime_lowering_metadata_preservation_sites = 0;
  std::size_t interop_foreign_call_lifetime_lowering_guard_blocked_sites = 0;
  std::size_t
      interop_foreign_call_lifetime_lowering_contract_violation_sites = 0;
  bool deterministic_interop_foreign_call_lifetime_lowering_handoff = false;
  std::string lowering_interop_ffi_metadata_interface_preservation_key;
  std::size_t interop_ffi_metadata_interface_preservation_local_foreign_callable_count =
      0;
  std::size_t
      interop_ffi_metadata_interface_preservation_local_metadata_preservation_sites =
          0;
  std::size_t
      interop_ffi_metadata_interface_preservation_local_interface_annotation_sites =
          0;
  std::size_t interop_ffi_metadata_interface_preservation_imported_module_count =
      0;
  std::size_t
      interop_ffi_metadata_interface_preservation_imported_foreign_callable_count =
          0;
  std::size_t
      interop_ffi_metadata_interface_preservation_imported_metadata_preservation_sites =
          0;
  std::size_t
      interop_ffi_metadata_interface_preservation_imported_interface_annotation_sites =
          0;
  bool interop_ffi_metadata_interface_preservation_runtime_import_artifact_ready =
      false;
  bool interop_ffi_metadata_interface_preservation_separate_compilation_preservation_ready =
      false;
  bool deterministic_interop_ffi_metadata_interface_preservation_handoff =
      false;
  std::string lowering_interop_header_module_bridge_generation_key;
  std::string lowering_metaprogramming_expansion_replay_key;
  std::size_t metaprogramming_expansion_lowering_derive_inventory_sites = 0;
  std::size_t metaprogramming_expansion_lowering_derived_selector_artifact_sites = 0;
  std::size_t metaprogramming_expansion_lowering_macro_replay_visible_sites = 0;
  std::size_t metaprogramming_expansion_lowering_property_behavior_sites = 0;
  std::size_t metaprogramming_expansion_lowering_synthesized_binding_sites = 0;
  std::size_t metaprogramming_expansion_lowering_synthesized_getter_sites = 0;
  std::size_t metaprogramming_expansion_lowering_synthesized_setter_sites = 0;
  std::size_t metaprogramming_expansion_lowering_replay_visible_metadata_sites = 0;
  std::size_t metaprogramming_expansion_lowering_guard_blocked_sites = 0;
  std::size_t metaprogramming_expansion_lowering_contract_violation_sites = 0;
  bool deterministic_metaprogramming_expansion_lowering_handoff = false;
  std::string lowering_metaprogramming_synthesized_emission_replay_key;
  std::size_t metaprogramming_synthesized_emitted_derive_method_sites = 0;
  std::size_t metaprogramming_synthesized_emitted_macro_artifact_sites = 0;
  std::size_t metaprogramming_synthesized_emitted_property_behavior_artifact_sites = 0;
  std::size_t metaprogramming_synthesized_emitted_global_artifact_sites = 0;
  std::size_t metaprogramming_synthesized_emitted_runtime_method_list_sites = 0;
  std::size_t metaprogramming_synthesized_guard_blocked_sites = 0;
  std::size_t metaprogramming_synthesized_contract_violation_sites = 0;
  bool deterministic_metaprogramming_synthesized_emission_handoff = false;
  std::vector<Objc3IRMetaprogrammingDerivedMethodBundle>
      metaprogramming_derived_method_bundles_lexicographic;
  std::vector<Objc3IRMetaprogrammingMacroArtifactBundle>
      metaprogramming_macro_artifact_bundles_lexicographic;
  std::vector<Objc3IRMetaprogrammingPropertyBehaviorArtifactBundle>
      metaprogramming_property_behavior_artifact_bundles_lexicographic;
  std::string lowering_metaprogramming_module_interface_replay_preservation_key;
  std::size_t metaprogramming_module_replay_local_derive_method_count = 0;
  std::size_t metaprogramming_module_replay_local_macro_artifact_count = 0;
  std::size_t
      metaprogramming_module_replay_local_interface_property_behavior_artifact_count = 0;
  std::size_t
      metaprogramming_module_replay_local_implementation_property_behavior_artifact_count =
          0;
  std::size_t metaprogramming_module_replay_local_runtime_method_list_count = 0;
  std::size_t metaprogramming_module_replay_imported_module_count = 0;
  std::size_t metaprogramming_module_replay_imported_derive_method_count = 0;
  std::size_t metaprogramming_module_replay_imported_macro_artifact_count = 0;
  std::size_t
      metaprogramming_module_replay_imported_interface_property_behavior_artifact_count =
          0;
  std::size_t
      metaprogramming_module_replay_imported_implementation_property_behavior_artifact_count =
          0;
  std::size_t metaprogramming_module_replay_imported_runtime_method_list_count = 0;
  bool metaprogramming_module_replay_runtime_import_artifact_ready = false;
  bool metaprogramming_module_replay_separate_compilation_preservation_ready = false;
  bool deterministic_metaprogramming_module_interface_replay_handoff = false;
  std::string lowering_dispatch_dispatch_metadata_interface_preservation_key;
  std::size_t dispatch_dispatch_metadata_local_direct_callable_record_count = 0;
  std::size_t dispatch_dispatch_metadata_local_final_callable_record_count = 0;
  std::size_t dispatch_dispatch_metadata_local_final_container_record_count = 0;
  std::size_t dispatch_dispatch_metadata_local_sealed_container_record_count = 0;
  std::size_t dispatch_dispatch_metadata_imported_module_count = 0;
  std::size_t dispatch_dispatch_metadata_imported_direct_callable_record_count = 0;
  std::size_t dispatch_dispatch_metadata_imported_final_callable_record_count = 0;
  std::size_t dispatch_dispatch_metadata_imported_final_container_record_count = 0;
  std::size_t dispatch_dispatch_metadata_imported_sealed_container_record_count = 0;
  bool dispatch_dispatch_metadata_runtime_import_artifact_ready = false;
  bool dispatch_dispatch_metadata_separate_compilation_preservation_ready = false;
  bool deterministic_dispatch_dispatch_metadata_interface_handoff = false;
  std::string lowering_ownership_system_extension_replay_key;
  std::size_t ownership_system_extension_lowering_cleanup_hook_sites = 0;
  std::size_t ownership_system_extension_lowering_resource_local_sites = 0;
  std::size_t ownership_system_extension_lowering_cleanup_owned_local_sites = 0;
  std::size_t ownership_system_extension_lowering_resource_move_capture_sites = 0;
  std::size_t ownership_system_extension_lowering_borrowed_parameter_sites = 0;
  std::size_t
      ownership_system_extension_lowering_borrowed_return_callable_sites = 0;
  std::size_t
      ownership_system_extension_lowering_borrowed_escape_candidate_sites = 0;
  std::size_t ownership_system_extension_lowering_explicit_capture_item_sites = 0;
  std::size_t ownership_system_extension_lowering_retainable_family_callable_sites =
      0;
  std::size_t
      ownership_system_extension_lowering_retainable_family_operation_callable_sites =
          0;
  std::size_t
      ownership_system_extension_lowering_retainable_family_alias_callable_sites =
          0;
  std::size_t ownership_system_extension_lowering_guard_blocked_sites = 0;
  std::size_t ownership_system_extension_lowering_contract_violation_sites = 0;
  bool deterministic_ownership_system_extension_lowering_handoff = false;
  std::string ownership_borrowed_retainable_abi_completion_replay_key;
  std::size_t ownership_borrowed_retainable_returns_borrowed_attribute_sites = 0;
  std::size_t ownership_borrowed_retainable_family_retain_sites = 0;
  std::size_t ownership_borrowed_retainable_family_release_sites = 0;
  std::size_t ownership_borrowed_retainable_family_autorelease_sites = 0;
  std::size_t
      ownership_borrowed_retainable_compatibility_returns_retained_sites = 0;
  std::size_t
      ownership_borrowed_retainable_compatibility_returns_not_retained_sites = 0;
  std::size_t ownership_borrowed_retainable_compatibility_consumed_sites = 0;
  bool deterministic_ownership_borrowed_retainable_abi_completion_handoff = false;
  std::string lowering_task_runtime_interop_cancellation_replay_key;
  std::size_t task_runtime_interop_cancellation_lowering_sites = 0;
  std::size_t task_runtime_interop_cancellation_lowering_runtime_interop_sites =
      0;
  std::size_t
      task_runtime_interop_cancellation_lowering_cancellation_probe_sites = 0;
  std::size_t
      task_runtime_interop_cancellation_lowering_cancellation_handler_sites = 0;
  std::size_t task_runtime_interop_cancellation_lowering_runtime_resume_sites =
      0;
  std::size_t task_runtime_interop_cancellation_lowering_runtime_cancel_sites =
      0;
  std::size_t task_runtime_interop_cancellation_lowering_normalized_sites = 0;
  std::size_t task_runtime_interop_cancellation_lowering_guard_blocked_sites =
      0;
  std::size_t
      task_runtime_interop_cancellation_lowering_contract_violation_sites = 0;
  bool deterministic_task_runtime_interop_cancellation_lowering_handoff =
      false;
  std::string lowering_concurrency_replay_race_guard_replay_key;
  std::size_t concurrency_replay_race_guard_lowering_sites = 0;
  std::size_t concurrency_replay_race_guard_lowering_replay_proof_sites = 0;
  std::size_t concurrency_replay_race_guard_lowering_race_guard_sites = 0;
  std::size_t concurrency_replay_race_guard_lowering_task_handoff_sites = 0;
  std::size_t concurrency_replay_race_guard_lowering_actor_isolation_sites = 0;
  std::size_t concurrency_replay_race_guard_lowering_deterministic_schedule_sites = 0;
  std::size_t concurrency_replay_race_guard_lowering_guard_blocked_sites = 0;
  std::size_t concurrency_replay_race_guard_lowering_contract_violation_sites = 0;
  bool deterministic_concurrency_replay_race_guard_lowering_handoff = false;
  std::string lowering_unsafe_pointer_extension_replay_key;
  std::size_t unsafe_pointer_extension_lowering_sites = 0;
  std::size_t unsafe_pointer_extension_lowering_unsafe_keyword_sites = 0;
  std::size_t unsafe_pointer_extension_lowering_pointer_arithmetic_sites = 0;
  std::size_t unsafe_pointer_extension_lowering_raw_pointer_type_sites = 0;
  std::size_t unsafe_pointer_extension_lowering_unsafe_operation_sites = 0;
  std::size_t unsafe_pointer_extension_lowering_normalized_sites = 0;
  std::size_t unsafe_pointer_extension_lowering_gate_blocked_sites = 0;
  std::size_t unsafe_pointer_extension_lowering_contract_violation_sites = 0;
  bool deterministic_unsafe_pointer_extension_lowering_handoff = false;
  std::string lowering_inline_asm_intrinsic_governance_replay_key;
  std::size_t inline_asm_intrinsic_governance_lowering_sites = 0;
  std::size_t inline_asm_intrinsic_governance_lowering_inline_asm_sites = 0;
  std::size_t inline_asm_intrinsic_governance_lowering_intrinsic_sites = 0;
  std::size_t inline_asm_intrinsic_governance_lowering_governed_intrinsic_sites =
      0;
  std::size_t
      inline_asm_intrinsic_governance_lowering_privileged_intrinsic_sites = 0;
  std::size_t inline_asm_intrinsic_governance_lowering_normalized_sites = 0;
  std::size_t inline_asm_intrinsic_governance_lowering_gate_blocked_sites = 0;
  std::size_t
      inline_asm_intrinsic_governance_lowering_contract_violation_sites = 0;
  bool deterministic_inline_asm_intrinsic_governance_lowering_handoff = false;
  std::size_t canonical_literal_rejection_total() const {
    return canonical_literal_yes_rejection_sites +
           canonical_literal_no_rejection_sites +
           canonical_literal_null_rejection_sites;
  }
};

