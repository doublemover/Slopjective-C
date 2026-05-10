         handoff.nullability_flow_warning_precision_summary.normalized_sites <=
             handoff.nullability_flow_warning_precision_summary.nullability_flow_sites &&
         handoff.nullability_flow_warning_precision_summary.contract_violation_sites <=
             handoff.nullability_flow_warning_precision_summary.nullability_flow_sites &&
         handoff.nullability_flow_warning_precision_summary.nullability_suffix_sites ==
             handoff.nullability_flow_warning_precision_summary.nullable_suffix_sites +
                 handoff.nullability_flow_warning_precision_summary.nonnull_suffix_sites &&
         handoff.protocol_qualified_object_type_summary.deterministic &&
         handoff.protocol_qualified_object_type_summary.protocol_qualified_object_type_sites ==
             protocol_qualified_object_type_summary.protocol_qualified_object_type_sites &&
         handoff.protocol_qualified_object_type_summary.protocol_composition_sites ==
             protocol_qualified_object_type_summary.protocol_composition_sites &&
         handoff.protocol_qualified_object_type_summary.object_pointer_type_sites ==
             protocol_qualified_object_type_summary.object_pointer_type_sites &&
         handoff.protocol_qualified_object_type_summary.terminated_protocol_composition_sites ==
             protocol_qualified_object_type_summary.terminated_protocol_composition_sites &&
         handoff.protocol_qualified_object_type_summary.pointer_declarator_sites ==
             protocol_qualified_object_type_summary.pointer_declarator_sites &&
         handoff.protocol_qualified_object_type_summary.normalized_protocol_composition_sites ==
             protocol_qualified_object_type_summary.normalized_protocol_composition_sites &&
         handoff.protocol_qualified_object_type_summary.contract_violation_sites ==
             protocol_qualified_object_type_summary.contract_violation_sites &&
         handoff.protocol_qualified_object_type_summary.terminated_protocol_composition_sites <=
             handoff.protocol_qualified_object_type_summary.protocol_composition_sites &&
         handoff.protocol_qualified_object_type_summary.normalized_protocol_composition_sites <=
             handoff.protocol_qualified_object_type_summary.protocol_qualified_object_type_sites &&
         handoff.protocol_qualified_object_type_summary.contract_violation_sites <=
             handoff.protocol_qualified_object_type_summary.protocol_qualified_object_type_sites &&
         handoff.variance_bridge_cast_summary.deterministic &&
         handoff.variance_bridge_cast_summary.variance_bridge_cast_sites ==
             variance_bridge_cast_summary.variance_bridge_cast_sites &&
         handoff.variance_bridge_cast_summary.protocol_composition_sites ==
             variance_bridge_cast_summary.protocol_composition_sites &&
         handoff.variance_bridge_cast_summary.ownership_qualifier_sites ==
             variance_bridge_cast_summary.ownership_qualifier_sites &&
         handoff.variance_bridge_cast_summary.object_pointer_type_sites ==
             variance_bridge_cast_summary.object_pointer_type_sites &&
         handoff.variance_bridge_cast_summary.pointer_declarator_sites ==
             variance_bridge_cast_summary.pointer_declarator_sites &&
         handoff.variance_bridge_cast_summary.normalized_sites ==
             variance_bridge_cast_summary.normalized_sites &&
         handoff.variance_bridge_cast_summary.contract_violation_sites ==
             variance_bridge_cast_summary.contract_violation_sites &&
         handoff.variance_bridge_cast_summary.protocol_composition_sites <=
             handoff.variance_bridge_cast_summary.variance_bridge_cast_sites &&
         handoff.variance_bridge_cast_summary.normalized_sites <=
             handoff.variance_bridge_cast_summary.variance_bridge_cast_sites &&
         handoff.variance_bridge_cast_summary.contract_violation_sites <=
             handoff.variance_bridge_cast_summary.variance_bridge_cast_sites &&
         handoff.generic_metadata_abi_summary.deterministic &&
         handoff.generic_metadata_abi_summary.generic_metadata_abi_sites ==
             generic_metadata_abi_summary.generic_metadata_abi_sites &&
         handoff.generic_metadata_abi_summary.generic_suffix_sites ==
             generic_metadata_abi_summary.generic_suffix_sites &&
         handoff.generic_metadata_abi_summary.protocol_composition_sites ==
             generic_metadata_abi_summary.protocol_composition_sites &&
         handoff.generic_metadata_abi_summary.ownership_qualifier_sites ==
             generic_metadata_abi_summary.ownership_qualifier_sites &&
         handoff.generic_metadata_abi_summary.object_pointer_type_sites ==
             generic_metadata_abi_summary.object_pointer_type_sites &&
         handoff.generic_metadata_abi_summary.pointer_declarator_sites ==
             generic_metadata_abi_summary.pointer_declarator_sites &&
         handoff.generic_metadata_abi_summary.normalized_sites ==
             generic_metadata_abi_summary.normalized_sites &&
         handoff.generic_metadata_abi_summary.contract_violation_sites ==
             generic_metadata_abi_summary.contract_violation_sites &&
         handoff.generic_metadata_abi_summary.generic_suffix_sites <=
             handoff.generic_metadata_abi_summary.generic_metadata_abi_sites &&
         handoff.generic_metadata_abi_summary.protocol_composition_sites <=
             handoff.generic_metadata_abi_summary.generic_metadata_abi_sites &&
         handoff.generic_metadata_abi_summary.normalized_sites <=
             handoff.generic_metadata_abi_summary.generic_metadata_abi_sites &&
         handoff.generic_metadata_abi_summary.contract_violation_sites <=
             handoff.generic_metadata_abi_summary.generic_metadata_abi_sites &&
         handoff.module_import_graph_summary.deterministic &&
         handoff.module_import_graph_summary.module_import_graph_sites ==
             module_import_graph_summary.module_import_graph_sites &&
         handoff.module_import_graph_summary.import_edge_candidate_sites ==
             module_import_graph_summary.import_edge_candidate_sites &&
         handoff.module_import_graph_summary.namespace_segment_sites ==
             module_import_graph_summary.namespace_segment_sites &&
         handoff.module_import_graph_summary.object_pointer_type_sites ==
             module_import_graph_summary.object_pointer_type_sites &&
         handoff.module_import_graph_summary.pointer_declarator_sites ==
             module_import_graph_summary.pointer_declarator_sites &&
         handoff.module_import_graph_summary.normalized_sites ==
             module_import_graph_summary.normalized_sites &&
         handoff.module_import_graph_summary.contract_violation_sites ==
             module_import_graph_summary.contract_violation_sites &&
         handoff.module_import_graph_summary.import_edge_candidate_sites <=
             handoff.module_import_graph_summary.module_import_graph_sites &&
         handoff.module_import_graph_summary.namespace_segment_sites <=
             handoff.module_import_graph_summary.module_import_graph_sites &&
         handoff.module_import_graph_summary.normalized_sites <=
             handoff.module_import_graph_summary.module_import_graph_sites &&
         handoff.module_import_graph_summary.contract_violation_sites <=
             handoff.module_import_graph_summary.module_import_graph_sites &&
         handoff.namespace_collision_shadowing_summary.deterministic &&
         handoff.namespace_collision_shadowing_summary.namespace_collision_shadowing_sites ==
             namespace_collision_shadowing_summary.namespace_collision_shadowing_sites &&
         handoff.namespace_collision_shadowing_summary.namespace_segment_sites ==
             namespace_collision_shadowing_summary.namespace_segment_sites &&
         handoff.namespace_collision_shadowing_summary.import_edge_candidate_sites ==
             namespace_collision_shadowing_summary.import_edge_candidate_sites &&
         handoff.namespace_collision_shadowing_summary.object_pointer_type_sites ==
             namespace_collision_shadowing_summary.object_pointer_type_sites &&
         handoff.namespace_collision_shadowing_summary.pointer_declarator_sites ==
             namespace_collision_shadowing_summary.pointer_declarator_sites &&
         handoff.namespace_collision_shadowing_summary.normalized_sites ==
             namespace_collision_shadowing_summary.normalized_sites &&
         handoff.namespace_collision_shadowing_summary.contract_violation_sites ==
             namespace_collision_shadowing_summary.contract_violation_sites &&
         handoff.namespace_collision_shadowing_summary.namespace_segment_sites <=
             handoff.namespace_collision_shadowing_summary
                 .namespace_collision_shadowing_sites &&
         handoff.namespace_collision_shadowing_summary.import_edge_candidate_sites <=
             handoff.namespace_collision_shadowing_summary
                 .namespace_collision_shadowing_sites &&
         handoff.namespace_collision_shadowing_summary.normalized_sites <=
             handoff.namespace_collision_shadowing_summary
                 .namespace_collision_shadowing_sites &&
         handoff.namespace_collision_shadowing_summary.contract_violation_sites <=
             handoff.namespace_collision_shadowing_summary
                 .namespace_collision_shadowing_sites &&
         handoff.public_private_api_partition_summary.deterministic &&
         handoff.public_private_api_partition_summary.public_private_api_partition_sites ==
             public_private_api_partition_summary.public_private_api_partition_sites &&
         handoff.public_private_api_partition_summary.namespace_segment_sites ==
             public_private_api_partition_summary.namespace_segment_sites &&
         handoff.public_private_api_partition_summary.import_edge_candidate_sites ==
             public_private_api_partition_summary.import_edge_candidate_sites &&
         handoff.public_private_api_partition_summary.object_pointer_type_sites ==
             public_private_api_partition_summary.object_pointer_type_sites &&
         handoff.public_private_api_partition_summary.pointer_declarator_sites ==
             public_private_api_partition_summary.pointer_declarator_sites &&
         handoff.public_private_api_partition_summary.normalized_sites ==
             public_private_api_partition_summary.normalized_sites &&
         handoff.public_private_api_partition_summary.contract_violation_sites ==
             public_private_api_partition_summary.contract_violation_sites &&
         handoff.public_private_api_partition_summary.namespace_segment_sites <=
             handoff.public_private_api_partition_summary.public_private_api_partition_sites &&
         handoff.public_private_api_partition_summary.import_edge_candidate_sites <=
             handoff.public_private_api_partition_summary.public_private_api_partition_sites &&
         handoff.public_private_api_partition_summary.normalized_sites <=
             handoff.public_private_api_partition_summary.public_private_api_partition_sites &&
         handoff.public_private_api_partition_summary.contract_violation_sites <=
             handoff.public_private_api_partition_summary.public_private_api_partition_sites &&
         handoff.incremental_module_cache_invalidation_summary.deterministic &&
         handoff.incremental_module_cache_invalidation_summary
                 .incremental_module_cache_invalidation_sites ==
             incremental_module_cache_invalidation_summary
                 .incremental_module_cache_invalidation_sites &&
         handoff.incremental_module_cache_invalidation_summary
                 .namespace_segment_sites ==
             incremental_module_cache_invalidation_summary.namespace_segment_sites &&
         handoff.incremental_module_cache_invalidation_summary
                 .import_edge_candidate_sites ==
             incremental_module_cache_invalidation_summary
                 .import_edge_candidate_sites &&
         handoff.incremental_module_cache_invalidation_summary
                 .object_pointer_type_sites ==
             incremental_module_cache_invalidation_summary
                 .object_pointer_type_sites &&
         handoff.incremental_module_cache_invalidation_summary
                 .pointer_declarator_sites ==
             incremental_module_cache_invalidation_summary
                 .pointer_declarator_sites &&
         handoff.incremental_module_cache_invalidation_summary.normalized_sites ==
             incremental_module_cache_invalidation_summary.normalized_sites &&
         handoff.incremental_module_cache_invalidation_summary
                 .cache_invalidation_candidate_sites ==
             incremental_module_cache_invalidation_summary
                 .cache_invalidation_candidate_sites &&
         handoff.incremental_module_cache_invalidation_summary
                 .contract_violation_sites ==
             incremental_module_cache_invalidation_summary.contract_violation_sites &&
         handoff.incremental_module_cache_invalidation_summary
                 .namespace_segment_sites <=
             handoff.incremental_module_cache_invalidation_summary
                 .incremental_module_cache_invalidation_sites &&
         handoff.incremental_module_cache_invalidation_summary
                 .import_edge_candidate_sites <=
             handoff.incremental_module_cache_invalidation_summary
                 .incremental_module_cache_invalidation_sites &&
         handoff.incremental_module_cache_invalidation_summary.normalized_sites <=
             handoff.incremental_module_cache_invalidation_summary
                 .incremental_module_cache_invalidation_sites &&
         handoff.incremental_module_cache_invalidation_summary
                 .cache_invalidation_candidate_sites <=
             handoff.incremental_module_cache_invalidation_summary
                 .incremental_module_cache_invalidation_sites &&
         handoff.incremental_module_cache_invalidation_summary
                 .contract_violation_sites <=
             handoff.incremental_module_cache_invalidation_summary
                 .incremental_module_cache_invalidation_sites &&
         handoff.cross_module_conformance_summary.deterministic &&
         handoff.cross_module_conformance_summary.cross_module_conformance_sites ==
             cross_module_conformance_summary.cross_module_conformance_sites &&
         handoff.cross_module_conformance_summary.namespace_segment_sites ==
             cross_module_conformance_summary.namespace_segment_sites &&
         handoff.cross_module_conformance_summary.import_edge_candidate_sites ==
             cross_module_conformance_summary.import_edge_candidate_sites &&
         handoff.cross_module_conformance_summary.object_pointer_type_sites ==
             cross_module_conformance_summary.object_pointer_type_sites &&
         handoff.cross_module_conformance_summary.pointer_declarator_sites ==
             cross_module_conformance_summary.pointer_declarator_sites &&
         handoff.cross_module_conformance_summary.normalized_sites ==
             cross_module_conformance_summary.normalized_sites &&
         handoff.cross_module_conformance_summary.cache_invalidation_candidate_sites ==
             cross_module_conformance_summary.cache_invalidation_candidate_sites &&
         handoff.cross_module_conformance_summary.contract_violation_sites ==
             cross_module_conformance_summary.contract_violation_sites &&
         handoff.cross_module_conformance_summary.namespace_segment_sites <=
             handoff.cross_module_conformance_summary.cross_module_conformance_sites &&
         handoff.cross_module_conformance_summary.import_edge_candidate_sites <=
             handoff.cross_module_conformance_summary.cross_module_conformance_sites &&
         handoff.cross_module_conformance_summary.normalized_sites <=
             handoff.cross_module_conformance_summary.cross_module_conformance_sites &&
         handoff.cross_module_conformance_summary.cache_invalidation_candidate_sites <=
             handoff.cross_module_conformance_summary.cross_module_conformance_sites &&
         handoff.cross_module_conformance_summary.normalized_sites +
                 handoff.cross_module_conformance_summary.cache_invalidation_candidate_sites ==
             handoff.cross_module_conformance_summary.cross_module_conformance_sites &&
         handoff.cross_module_conformance_summary.contract_violation_sites <=
             handoff.cross_module_conformance_summary.cross_module_conformance_sites &&
         handoff.throws_propagation_summary.deterministic &&
         handoff.throws_propagation_summary.throws_propagation_sites ==
             throws_propagation_summary.throws_propagation_sites &&
         handoff.throws_propagation_summary.namespace_segment_sites ==
             throws_propagation_summary.namespace_segment_sites &&
         handoff.throws_propagation_summary.import_edge_candidate_sites ==
             throws_propagation_summary.import_edge_candidate_sites &&
         handoff.throws_propagation_summary.object_pointer_type_sites ==
             throws_propagation_summary.object_pointer_type_sites &&
         handoff.throws_propagation_summary.pointer_declarator_sites ==
             throws_propagation_summary.pointer_declarator_sites &&
         handoff.throws_propagation_summary.normalized_sites ==
             throws_propagation_summary.normalized_sites &&
         handoff.throws_propagation_summary.cache_invalidation_candidate_sites ==
             throws_propagation_summary.cache_invalidation_candidate_sites &&
         handoff.throws_propagation_summary.contract_violation_sites ==
             throws_propagation_summary.contract_violation_sites &&
         handoff.throws_propagation_summary.namespace_segment_sites <=
             handoff.throws_propagation_summary.throws_propagation_sites &&
         handoff.throws_propagation_summary.import_edge_candidate_sites <=
             handoff.throws_propagation_summary.throws_propagation_sites &&
         handoff.throws_propagation_summary.normalized_sites <=
             handoff.throws_propagation_summary.throws_propagation_sites &&
         handoff.throws_propagation_summary.cache_invalidation_candidate_sites <=
             handoff.throws_propagation_summary.throws_propagation_sites &&
         handoff.throws_propagation_summary.normalized_sites +
                 handoff.throws_propagation_summary.cache_invalidation_candidate_sites ==
             handoff.throws_propagation_summary.throws_propagation_sites &&
         handoff.throws_propagation_summary.contract_violation_sites <=
             handoff.throws_propagation_summary.throws_propagation_sites &&
#include "sema/objc3_semantic_passes_body_validation_core_concurrency.inc"
         handoff.unsafe_pointer_extension_summary.deterministic &&
         handoff.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites ==
             unsafe_pointer_extension_summary.unsafe_pointer_extension_sites &&
         handoff.unsafe_pointer_extension_summary.unsafe_keyword_sites ==
             unsafe_pointer_extension_summary.unsafe_keyword_sites &&
         handoff.unsafe_pointer_extension_summary.pointer_arithmetic_sites ==
             unsafe_pointer_extension_summary.pointer_arithmetic_sites &&
         handoff.unsafe_pointer_extension_summary.raw_pointer_type_sites ==
             unsafe_pointer_extension_summary.raw_pointer_type_sites &&
         handoff.unsafe_pointer_extension_summary.unsafe_operation_sites ==
             unsafe_pointer_extension_summary.unsafe_operation_sites &&
         handoff.unsafe_pointer_extension_summary.normalized_sites ==
             unsafe_pointer_extension_summary.normalized_sites &&
         handoff.unsafe_pointer_extension_summary.gate_blocked_sites ==
             unsafe_pointer_extension_summary.gate_blocked_sites &&
         handoff.unsafe_pointer_extension_summary.contract_violation_sites ==
             unsafe_pointer_extension_summary.contract_violation_sites &&
         handoff.unsafe_pointer_extension_summary.unsafe_keyword_sites <=
             handoff.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites &&
         handoff.unsafe_pointer_extension_summary.pointer_arithmetic_sites <=
             handoff.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites &&
         handoff.unsafe_pointer_extension_summary.raw_pointer_type_sites <=
             handoff.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites &&
         handoff.unsafe_pointer_extension_summary.unsafe_operation_sites <=
             handoff.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites &&
         handoff.unsafe_pointer_extension_summary.normalized_sites <=
             handoff.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites &&
         handoff.unsafe_pointer_extension_summary.gate_blocked_sites <=
             handoff.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites &&
         handoff.unsafe_pointer_extension_summary.contract_violation_sites <=
             handoff.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites &&
         handoff.unsafe_pointer_extension_summary.normalized_sites +
                 handoff.unsafe_pointer_extension_summary.gate_blocked_sites ==
             handoff.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites &&
         handoff.inline_asm_intrinsic_governance_summary.deterministic &&
         handoff.inline_asm_intrinsic_governance_summary
                 .inline_asm_intrinsic_sites ==
             inline_asm_intrinsic_governance_summary.inline_asm_intrinsic_sites &&
         handoff.inline_asm_intrinsic_governance_summary.inline_asm_sites ==
             inline_asm_intrinsic_governance_summary.inline_asm_sites &&
         handoff.inline_asm_intrinsic_governance_summary.intrinsic_sites ==
             inline_asm_intrinsic_governance_summary.intrinsic_sites &&
         handoff.inline_asm_intrinsic_governance_summary
                 .governed_intrinsic_sites ==
             inline_asm_intrinsic_governance_summary.governed_intrinsic_sites &&
         handoff.inline_asm_intrinsic_governance_summary
                 .privileged_intrinsic_sites ==
             inline_asm_intrinsic_governance_summary
                 .privileged_intrinsic_sites &&
         handoff.inline_asm_intrinsic_governance_summary.normalized_sites ==
             inline_asm_intrinsic_governance_summary.normalized_sites &&
         handoff.inline_asm_intrinsic_governance_summary.gate_blocked_sites ==
             inline_asm_intrinsic_governance_summary.gate_blocked_sites &&
         handoff.inline_asm_intrinsic_governance_summary
                 .contract_violation_sites ==
             inline_asm_intrinsic_governance_summary.contract_violation_sites &&
         handoff.inline_asm_intrinsic_governance_summary.inline_asm_sites <=
             handoff.inline_asm_intrinsic_governance_summary
                 .inline_asm_intrinsic_sites &&
         handoff.inline_asm_intrinsic_governance_summary.intrinsic_sites <=
             handoff.inline_asm_intrinsic_governance_summary
                 .inline_asm_intrinsic_sites &&
         handoff.inline_asm_intrinsic_governance_summary
                 .governed_intrinsic_sites <=
             handoff.inline_asm_intrinsic_governance_summary.intrinsic_sites &&
         handoff.inline_asm_intrinsic_governance_summary
                 .privileged_intrinsic_sites <=
             handoff.inline_asm_intrinsic_governance_summary
                 .governed_intrinsic_sites &&
         handoff.inline_asm_intrinsic_governance_summary.normalized_sites <=
             handoff.inline_asm_intrinsic_governance_summary
                 .inline_asm_intrinsic_sites &&
         handoff.inline_asm_intrinsic_governance_summary.gate_blocked_sites <=
             handoff.inline_asm_intrinsic_governance_summary
                 .inline_asm_intrinsic_sites &&
         handoff.inline_asm_intrinsic_governance_summary
                 .contract_violation_sites <=
             handoff.inline_asm_intrinsic_governance_summary
                 .inline_asm_intrinsic_sites &&
         handoff.inline_asm_intrinsic_governance_summary.inline_asm_sites ==
             handoff.throws_propagation_summary
                 .cache_invalidation_candidate_sites &&
         handoff.inline_asm_intrinsic_governance_summary.intrinsic_sites ==
             handoff.unsafe_pointer_extension_summary.unsafe_operation_sites &&
         handoff.inline_asm_intrinsic_governance_summary
                 .governed_intrinsic_sites <=
             handoff.throws_propagation_summary.normalized_sites &&
         handoff.inline_asm_intrinsic_governance_summary
                 .privileged_intrinsic_sites <=
             handoff.unsafe_pointer_extension_summary.normalized_sites &&
         handoff.inline_asm_intrinsic_governance_summary.normalized_sites >=
             handoff.inline_asm_intrinsic_governance_summary.inline_asm_sites &&
         handoff.inline_asm_intrinsic_governance_summary.normalized_sites -
                 handoff.inline_asm_intrinsic_governance_summary
                     .inline_asm_sites ==
             handoff.inline_asm_intrinsic_governance_summary
                 .governed_intrinsic_sites &&
         handoff.inline_asm_intrinsic_governance_summary.gate_blocked_sites ==
             handoff.inline_asm_intrinsic_governance_summary.intrinsic_sites -
                 handoff.inline_asm_intrinsic_governance_summary
                     .governed_intrinsic_sites &&
         handoff.inline_asm_intrinsic_governance_summary.normalized_sites +
                 handoff.inline_asm_intrinsic_governance_summary
                     .gate_blocked_sites ==
             handoff.inline_asm_intrinsic_governance_summary
                 .inline_asm_intrinsic_sites &&
         handoff.ns_error_bridging_summary.deterministic &&
         handoff.ns_error_bridging_summary.ns_error_bridging_sites ==
             ns_error_bridging_summary.ns_error_bridging_sites &&
         handoff.ns_error_bridging_summary.ns_error_parameter_sites ==
             ns_error_bridging_summary.ns_error_parameter_sites &&
         handoff.ns_error_bridging_summary.ns_error_out_parameter_sites ==
             ns_error_bridging_summary.ns_error_out_parameter_sites &&
         handoff.ns_error_bridging_summary.ns_error_bridge_path_sites ==
             ns_error_bridging_summary.ns_error_bridge_path_sites &&
         handoff.ns_error_bridging_summary.failable_call_sites ==
             ns_error_bridging_summary.failable_call_sites &&
         handoff.ns_error_bridging_summary.normalized_sites ==
             ns_error_bridging_summary.normalized_sites &&
         handoff.ns_error_bridging_summary.bridge_boundary_sites ==
             ns_error_bridging_summary.bridge_boundary_sites &&
         handoff.ns_error_bridging_summary.contract_violation_sites ==
             ns_error_bridging_summary.contract_violation_sites &&
         handoff.ns_error_bridging_summary.ns_error_parameter_sites <=
             handoff.ns_error_bridging_summary.ns_error_bridging_sites &&
         handoff.ns_error_bridging_summary.ns_error_out_parameter_sites <=
             handoff.ns_error_bridging_summary.ns_error_bridging_sites &&
         handoff.ns_error_bridging_summary.ns_error_bridge_path_sites <=
             handoff.ns_error_bridging_summary.ns_error_bridging_sites &&
         handoff.ns_error_bridging_summary.failable_call_sites <=
             handoff.ns_error_bridging_summary.ns_error_bridging_sites &&
         handoff.ns_error_bridging_summary.normalized_sites <=
             handoff.ns_error_bridging_summary.ns_error_bridging_sites &&
         handoff.ns_error_bridging_summary.bridge_boundary_sites <=
             handoff.ns_error_bridging_summary.ns_error_bridging_sites &&
         handoff.ns_error_bridging_summary.normalized_sites +
                 handoff.ns_error_bridging_summary.bridge_boundary_sites ==
             handoff.ns_error_bridging_summary.ns_error_bridging_sites &&
         handoff.ns_error_bridging_summary.contract_violation_sites <=
             handoff.ns_error_bridging_summary.ns_error_bridging_sites &&
         handoff.error_diagnostics_recovery_summary.deterministic &&
         handoff.error_diagnostics_recovery_summary
                 .error_diagnostics_recovery_sites ==
             error_diagnostics_recovery_summary
                 .error_diagnostics_recovery_sites &&
         handoff.error_diagnostics_recovery_summary.diagnostic_emit_sites ==
             error_diagnostics_recovery_summary.diagnostic_emit_sites &&
         handoff.error_diagnostics_recovery_summary.recovery_anchor_sites ==
             error_diagnostics_recovery_summary.recovery_anchor_sites &&
         handoff.error_diagnostics_recovery_summary.recovery_boundary_sites ==
             error_diagnostics_recovery_summary.recovery_boundary_sites &&
         handoff.error_diagnostics_recovery_summary
                 .fail_closed_diagnostic_sites ==
             error_diagnostics_recovery_summary.fail_closed_diagnostic_sites &&
         handoff.error_diagnostics_recovery_summary.normalized_sites ==
             error_diagnostics_recovery_summary.normalized_sites &&
         handoff.error_diagnostics_recovery_summary.gate_blocked_sites ==
             error_diagnostics_recovery_summary.gate_blocked_sites &&
         handoff.error_diagnostics_recovery_summary.contract_violation_sites ==
             error_diagnostics_recovery_summary.contract_violation_sites &&
         handoff.error_diagnostics_recovery_summary.diagnostic_emit_sites <=
             handoff.error_diagnostics_recovery_summary
                 .error_diagnostics_recovery_sites &&
         handoff.error_diagnostics_recovery_summary.recovery_anchor_sites <=
             handoff.error_diagnostics_recovery_summary
                 .error_diagnostics_recovery_sites &&
         handoff.error_diagnostics_recovery_summary.recovery_boundary_sites <=
             handoff.error_diagnostics_recovery_summary
                 .error_diagnostics_recovery_sites &&
         handoff.error_diagnostics_recovery_summary
                 .fail_closed_diagnostic_sites <=
             handoff.error_diagnostics_recovery_summary
                 .error_diagnostics_recovery_sites &&
         handoff.error_diagnostics_recovery_summary.normalized_sites <=
             handoff.error_diagnostics_recovery_summary
                 .error_diagnostics_recovery_sites &&
         handoff.error_diagnostics_recovery_summary.gate_blocked_sites <=
             handoff.error_diagnostics_recovery_summary
                 .error_diagnostics_recovery_sites &&
         handoff.error_diagnostics_recovery_summary.contract_violation_sites <=
             handoff.error_diagnostics_recovery_summary
                 .error_diagnostics_recovery_sites &&
         handoff.error_diagnostics_recovery_summary.normalized_sites +
                 handoff.error_diagnostics_recovery_summary.gate_blocked_sites ==
             handoff.error_diagnostics_recovery_summary
                 .error_diagnostics_recovery_sites &&
         handoff.result_like_lowering_summary.deterministic &&
         handoff.result_like_lowering_summary.result_like_sites ==
             result_like_lowering_summary.result_like_sites &&
         handoff.result_like_lowering_summary.result_success_sites ==
             result_like_lowering_summary.result_success_sites &&
         handoff.result_like_lowering_summary.result_failure_sites ==
             result_like_lowering_summary.result_failure_sites &&
         handoff.result_like_lowering_summary.result_branch_sites ==
             result_like_lowering_summary.result_branch_sites &&
         handoff.result_like_lowering_summary.result_payload_sites ==
             result_like_lowering_summary.result_payload_sites &&
         handoff.result_like_lowering_summary.normalized_sites ==
             result_like_lowering_summary.normalized_sites &&
         handoff.result_like_lowering_summary.branch_merge_sites ==
             result_like_lowering_summary.branch_merge_sites &&
         handoff.result_like_lowering_summary.contract_violation_sites ==
             result_like_lowering_summary.contract_violation_sites &&
         handoff.result_like_lowering_summary.result_success_sites <=
             handoff.result_like_lowering_summary.result_like_sites &&
         handoff.result_like_lowering_summary.result_failure_sites <=
             handoff.result_like_lowering_summary.result_like_sites &&
         handoff.result_like_lowering_summary.result_branch_sites <=
             handoff.result_like_lowering_summary.result_like_sites &&
         handoff.result_like_lowering_summary.result_payload_sites <=
             handoff.result_like_lowering_summary.result_like_sites &&
         handoff.result_like_lowering_summary.normalized_sites <=
             handoff.result_like_lowering_summary.result_like_sites &&
         handoff.result_like_lowering_summary.branch_merge_sites <=
             handoff.result_like_lowering_summary.result_like_sites &&
         handoff.result_like_lowering_summary.contract_violation_sites <=
             handoff.result_like_lowering_summary.result_like_sites &&
         handoff.result_like_lowering_summary.result_success_sites +
                 handoff.result_like_lowering_summary.result_failure_sites ==
             handoff.result_like_lowering_summary.normalized_sites &&
         handoff.result_like_lowering_summary.normalized_sites +
                 handoff.result_like_lowering_summary.branch_merge_sites ==
             handoff.result_like_lowering_summary.result_like_sites &&
         handoff.unwind_cleanup_summary.deterministic &&
         handoff.unwind_cleanup_summary.unwind_cleanup_sites ==
             unwind_cleanup_summary.unwind_cleanup_sites &&
         handoff.unwind_cleanup_summary.exceptional_exit_sites ==
             unwind_cleanup_summary.exceptional_exit_sites &&
         handoff.unwind_cleanup_summary.cleanup_action_sites ==
             unwind_cleanup_summary.cleanup_action_sites &&
         handoff.unwind_cleanup_summary.cleanup_scope_sites ==
             unwind_cleanup_summary.cleanup_scope_sites &&
         handoff.unwind_cleanup_summary.cleanup_resume_sites ==
             unwind_cleanup_summary.cleanup_resume_sites &&
         handoff.unwind_cleanup_summary.normalized_sites ==
             unwind_cleanup_summary.normalized_sites &&
         handoff.unwind_cleanup_summary.fail_closed_sites ==
             unwind_cleanup_summary.fail_closed_sites &&
         handoff.unwind_cleanup_summary.contract_violation_sites ==
             unwind_cleanup_summary.contract_violation_sites &&
         handoff.unwind_cleanup_summary.exceptional_exit_sites <=
             handoff.unwind_cleanup_summary.unwind_cleanup_sites &&
         handoff.unwind_cleanup_summary.cleanup_action_sites <=
             handoff.unwind_cleanup_summary.unwind_cleanup_sites &&
         handoff.unwind_cleanup_summary.cleanup_scope_sites <=
             handoff.unwind_cleanup_summary.unwind_cleanup_sites &&
         handoff.unwind_cleanup_summary.cleanup_resume_sites <=
             handoff.unwind_cleanup_summary.unwind_cleanup_sites &&
         handoff.unwind_cleanup_summary.normalized_sites <=
             handoff.unwind_cleanup_summary.unwind_cleanup_sites &&
         handoff.unwind_cleanup_summary.fail_closed_sites <=
             handoff.unwind_cleanup_summary.unwind_cleanup_sites &&
         handoff.unwind_cleanup_summary.contract_violation_sites <=
             handoff.unwind_cleanup_summary.unwind_cleanup_sites &&
         handoff.unwind_cleanup_summary.normalized_sites +
                 handoff.unwind_cleanup_summary.fail_closed_sites ==
             handoff.unwind_cleanup_summary.unwind_cleanup_sites &&
         handoff.symbol_graph_scope_resolution_summary.deterministic &&
         handoff.symbol_graph_scope_resolution_summary.global_symbol_nodes ==
             symbol_graph_scope_summary.global_symbol_nodes &&
         handoff.symbol_graph_scope_resolution_summary.function_symbol_nodes ==
             symbol_graph_scope_summary.function_symbol_nodes &&
         handoff.symbol_graph_scope_resolution_summary.interface_symbol_nodes ==
             symbol_graph_scope_summary.interface_symbol_nodes &&
         handoff.symbol_graph_scope_resolution_summary.implementation_symbol_nodes ==
             symbol_graph_scope_summary.implementation_symbol_nodes &&
         handoff.symbol_graph_scope_resolution_summary.interface_property_symbol_nodes ==
             symbol_graph_scope_summary.interface_property_symbol_nodes &&
         handoff.symbol_graph_scope_resolution_summary.implementation_property_symbol_nodes ==
             symbol_graph_scope_summary.implementation_property_symbol_nodes &&
         handoff.symbol_graph_scope_resolution_summary.interface_method_symbol_nodes ==
             symbol_graph_scope_summary.interface_method_symbol_nodes &&
         handoff.symbol_graph_scope_resolution_summary.implementation_method_symbol_nodes ==
             symbol_graph_scope_summary.implementation_method_symbol_nodes &&
         handoff.symbol_graph_scope_resolution_summary.top_level_scope_symbols ==
             symbol_graph_scope_summary.top_level_scope_symbols &&
         handoff.symbol_graph_scope_resolution_summary.nested_scope_symbols ==
             symbol_graph_scope_summary.nested_scope_symbols &&
         handoff.symbol_graph_scope_resolution_summary.scope_frames_total ==
             symbol_graph_scope_summary.scope_frames_total &&
         handoff.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites ==
             symbol_graph_scope_summary.implementation_interface_resolution_sites &&
         handoff.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits ==
             symbol_graph_scope_summary.implementation_interface_resolution_hits &&
         handoff.symbol_graph_scope_resolution_summary.implementation_interface_resolution_misses ==
             symbol_graph_scope_summary.implementation_interface_resolution_misses &&
         handoff.symbol_graph_scope_resolution_summary.method_resolution_sites ==
             symbol_graph_scope_summary.method_resolution_sites &&
         handoff.symbol_graph_scope_resolution_summary.method_resolution_hits ==
             symbol_graph_scope_summary.method_resolution_hits &&
         handoff.symbol_graph_scope_resolution_summary.method_resolution_misses ==
             symbol_graph_scope_summary.method_resolution_misses &&
         handoff.symbol_graph_scope_resolution_summary.symbol_nodes_total() ==
             symbol_graph_scope_summary.symbol_nodes_total() &&
         handoff.symbol_graph_scope_resolution_summary.resolution_hits_total() ==
             symbol_graph_scope_summary.resolution_hits_total() &&
         handoff.symbol_graph_scope_resolution_summary.resolution_misses_total() ==
             symbol_graph_scope_summary.resolution_misses_total() &&
         handoff.method_lookup_override_conflict_summary.deterministic &&
         handoff.method_lookup_override_conflict_summary.method_lookup_sites ==
             method_lookup_override_conflict_summary.method_lookup_sites &&
         handoff.method_lookup_override_conflict_summary.method_lookup_hits ==
             method_lookup_override_conflict_summary.method_lookup_hits &&
         handoff.method_lookup_override_conflict_summary.method_lookup_misses ==
             method_lookup_override_conflict_summary.method_lookup_misses &&
         handoff.method_lookup_override_conflict_summary.override_lookup_sites ==
             method_lookup_override_conflict_summary.override_lookup_sites &&
         handoff.method_lookup_override_conflict_summary.override_lookup_hits ==
             method_lookup_override_conflict_summary.override_lookup_hits &&
         handoff.method_lookup_override_conflict_summary.override_lookup_misses ==
             method_lookup_override_conflict_summary.override_lookup_misses &&
         handoff.method_lookup_override_conflict_summary.override_conflicts ==
             method_lookup_override_conflict_summary.override_conflicts &&
         handoff.method_lookup_override_conflict_summary.unresolved_base_interfaces ==
             method_lookup_override_conflict_summary.unresolved_base_interfaces &&
         handoff.method_lookup_override_conflict_summary.method_lookup_hits <=
             handoff.method_lookup_override_conflict_summary.method_lookup_sites &&
         handoff.method_lookup_override_conflict_summary.method_lookup_hits +
                 handoff.method_lookup_override_conflict_summary.method_lookup_misses ==
             handoff.method_lookup_override_conflict_summary.method_lookup_sites &&
         handoff.method_lookup_override_conflict_summary.override_lookup_hits <=
             handoff.method_lookup_override_conflict_summary.override_lookup_sites &&
         handoff.method_lookup_override_conflict_summary.override_lookup_hits +
                 handoff.method_lookup_override_conflict_summary.override_lookup_misses ==
             handoff.method_lookup_override_conflict_summary.override_lookup_sites &&
         handoff.method_lookup_override_conflict_summary.override_conflicts <=
             handoff.method_lookup_override_conflict_summary.override_lookup_hits &&
         handoff.property_synthesis_ivar_binding_summary.deterministic &&
         handoff.property_synthesis_ivar_binding_summary.property_synthesis_sites ==
             property_synthesis_ivar_binding_summary.property_synthesis_sites &&
         handoff.property_synthesis_ivar_binding_summary.property_synthesis_explicit_ivar_bindings ==
             property_synthesis_ivar_binding_summary.property_synthesis_explicit_ivar_bindings &&
         handoff.property_synthesis_ivar_binding_summary.property_synthesis_default_ivar_bindings ==
             property_synthesis_ivar_binding_summary.property_synthesis_default_ivar_bindings &&
         handoff.property_synthesis_ivar_binding_summary.interface_owned_property_synthesis_sites ==
             property_synthesis_ivar_binding_summary.interface_owned_property_synthesis_sites &&
         handoff.property_synthesis_ivar_binding_summary.implementation_property_redeclaration_sites ==
             property_synthesis_ivar_binding_summary.implementation_property_redeclaration_sites &&
         handoff.property_synthesis_ivar_binding_summary.ivar_binding_sites ==
             property_synthesis_ivar_binding_summary.ivar_binding_sites &&
         handoff.property_synthesis_ivar_binding_summary.ivar_binding_resolved ==
             property_synthesis_ivar_binding_summary.ivar_binding_resolved &&
         handoff.property_synthesis_ivar_binding_summary.ivar_binding_missing ==
             property_synthesis_ivar_binding_summary.ivar_binding_missing &&
         handoff.property_synthesis_ivar_binding_summary.ivar_binding_conflicts ==
             property_synthesis_ivar_binding_summary.ivar_binding_conflicts &&
         handoff.property_synthesis_ivar_binding_summary.property_synthesis_explicit_ivar_bindings +
                 handoff.property_synthesis_ivar_binding_summary.property_synthesis_default_ivar_bindings ==
             handoff.property_synthesis_ivar_binding_summary.property_synthesis_sites &&
         handoff.property_synthesis_ivar_binding_summary.interface_owned_property_synthesis_sites ==
             handoff.property_synthesis_ivar_binding_summary.property_synthesis_sites &&
         handoff.property_synthesis_ivar_binding_summary.implementation_property_redeclaration_sites <=
             handoff.property_synthesis_ivar_binding_summary.property_synthesis_sites &&
         handoff.property_synthesis_ivar_binding_summary.ivar_binding_sites ==
             handoff.property_synthesis_ivar_binding_summary.property_synthesis_sites &&
         handoff.property_synthesis_ivar_binding_summary.ivar_binding_resolved +
                 handoff.property_synthesis_ivar_binding_summary.ivar_binding_missing +
                 handoff.property_synthesis_ivar_binding_summary.ivar_binding_conflicts ==
             handoff.property_synthesis_ivar_binding_summary.ivar_binding_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.deterministic &&
         handoff.id_class_sel_object_pointer_type_checking_summary.canonical_reference_form_count ==
             id_class_sel_object_pointer_type_checking_summary.canonical_reference_form_count &&
         handoff.id_class_sel_object_pointer_type_checking_summary.canonical_message_scalar_form_count ==
             id_class_sel_object_pointer_type_checking_summary.canonical_message_scalar_form_count &&
         handoff.id_class_sel_object_pointer_type_checking_summary.canonical_bridge_top_form_count ==
             id_class_sel_object_pointer_type_checking_summary.canonical_bridge_top_form_count &&
         handoff.id_class_sel_object_pointer_type_checking_summary.canonical_reference_forms_unique ==
             id_class_sel_object_pointer_type_checking_summary.canonical_reference_forms_unique &&
         handoff.id_class_sel_object_pointer_type_checking_summary.canonical_message_scalar_forms_unique ==
             id_class_sel_object_pointer_type_checking_summary.canonical_message_scalar_forms_unique &&
         handoff.id_class_sel_object_pointer_type_checking_summary.canonical_bridge_top_forms_unique ==
             id_class_sel_object_pointer_type_checking_summary.canonical_bridge_top_forms_unique &&
         handoff.id_class_sel_object_pointer_type_checking_summary.canonical_bridge_top_subset_of_reference ==
             id_class_sel_object_pointer_type_checking_summary.canonical_bridge_top_subset_of_reference &&
         handoff.id_class_sel_object_pointer_type_checking_summary.canonical_type_form_scaffold_ready ==
             id_class_sel_object_pointer_type_checking_summary.canonical_type_form_scaffold_ready &&
         handoff.id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_diagnostics_hardening_consistent ==
             id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_diagnostics_hardening_consistent &&
         handoff.id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_diagnostics_hardening_ready ==
             id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_diagnostics_hardening_ready &&
         handoff.id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_diagnostics_hardening_key ==
             id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_diagnostics_hardening_key &&
         handoff.id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_recovery_determinism_consistent ==
             id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_recovery_determinism_consistent &&
         handoff.id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_recovery_determinism_ready ==
             id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_recovery_determinism_ready &&
         handoff.id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_recovery_determinism_key ==
             id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_recovery_determinism_key &&
         handoff.id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_conformance_matrix_consistent ==
             id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_conformance_matrix_consistent &&
         handoff.id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_conformance_matrix_ready ==
             id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_conformance_matrix_ready &&
         handoff.id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_conformance_matrix_key ==
             id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_conformance_matrix_key &&
         handoff.id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_conformance_corpus_case_count ==
             id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_conformance_corpus_case_count &&
         handoff.id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_conformance_corpus_passed_case_count ==
             id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_conformance_corpus_passed_case_count &&
         handoff.id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_conformance_corpus_failed_case_count ==
             id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_conformance_corpus_failed_case_count &&
         handoff.id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_conformance_corpus_consistent ==
             id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_conformance_corpus_consistent &&
         handoff.id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_conformance_corpus_ready ==
             id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_conformance_corpus_ready &&
         handoff.id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_conformance_corpus_key ==
             id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_conformance_corpus_key &&
         handoff.id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_performance_quality_required_guardrail_count ==
             id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_performance_quality_required_guardrail_count &&
         handoff.id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_performance_quality_passed_guardrail_count ==
             id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_performance_quality_passed_guardrail_count &&
         handoff.id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_performance_quality_failed_guardrail_count ==
             id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_performance_quality_failed_guardrail_count &&
         handoff.id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_performance_quality_guardrails_consistent ==
             id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_performance_quality_guardrails_consistent &&
         handoff.id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_performance_quality_guardrails_ready ==
             id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_performance_quality_guardrails_ready &&
         handoff.id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_performance_quality_guardrails_key ==
             id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_performance_quality_guardrails_key &&
         handoff.id_class_sel_object_pointer_type_checking_summary.param_type_sites ==
             id_class_sel_object_pointer_type_checking_summary.param_type_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.param_id_spelling_sites ==
             id_class_sel_object_pointer_type_checking_summary.param_id_spelling_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.param_class_spelling_sites ==
             id_class_sel_object_pointer_type_checking_summary.param_class_spelling_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.param_sel_spelling_sites ==
             id_class_sel_object_pointer_type_checking_summary.param_sel_spelling_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.param_instancetype_spelling_sites ==
             id_class_sel_object_pointer_type_checking_summary.param_instancetype_spelling_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.param_object_pointer_type_sites ==
             id_class_sel_object_pointer_type_checking_summary.param_object_pointer_type_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.return_type_sites ==
             id_class_sel_object_pointer_type_checking_summary.return_type_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.return_id_spelling_sites ==
             id_class_sel_object_pointer_type_checking_summary.return_id_spelling_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.return_class_spelling_sites ==
             id_class_sel_object_pointer_type_checking_summary.return_class_spelling_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.return_sel_spelling_sites ==
             id_class_sel_object_pointer_type_checking_summary.return_sel_spelling_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.return_instancetype_spelling_sites ==
             id_class_sel_object_pointer_type_checking_summary.return_instancetype_spelling_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.return_object_pointer_type_sites ==
             id_class_sel_object_pointer_type_checking_summary.return_object_pointer_type_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.property_type_sites ==
             id_class_sel_object_pointer_type_checking_summary.property_type_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.property_id_spelling_sites ==
             id_class_sel_object_pointer_type_checking_summary.property_id_spelling_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.property_class_spelling_sites ==
             id_class_sel_object_pointer_type_checking_summary.property_class_spelling_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.property_sel_spelling_sites ==
             id_class_sel_object_pointer_type_checking_summary.property_sel_spelling_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.property_instancetype_spelling_sites ==
             id_class_sel_object_pointer_type_checking_summary.property_instancetype_spelling_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.property_object_pointer_type_sites ==
             id_class_sel_object_pointer_type_checking_summary.property_object_pointer_type_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.param_id_spelling_sites +
                 handoff.id_class_sel_object_pointer_type_checking_summary.param_class_spelling_sites +
                 handoff.id_class_sel_object_pointer_type_checking_summary.param_sel_spelling_sites +
                 handoff.id_class_sel_object_pointer_type_checking_summary.param_instancetype_spelling_sites +
                 handoff.id_class_sel_object_pointer_type_checking_summary.param_object_pointer_type_sites <=
             handoff.id_class_sel_object_pointer_type_checking_summary.param_type_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.return_id_spelling_sites +
                 handoff.id_class_sel_object_pointer_type_checking_summary.return_class_spelling_sites +
                 handoff.id_class_sel_object_pointer_type_checking_summary.return_sel_spelling_sites +
                 handoff.id_class_sel_object_pointer_type_checking_summary.return_instancetype_spelling_sites +
                 handoff.id_class_sel_object_pointer_type_checking_summary.return_object_pointer_type_sites <=
             handoff.id_class_sel_object_pointer_type_checking_summary.return_type_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.property_id_spelling_sites +
                 handoff.id_class_sel_object_pointer_type_checking_summary.property_class_spelling_sites +
                 handoff.id_class_sel_object_pointer_type_checking_summary.property_sel_spelling_sites +
                 handoff.id_class_sel_object_pointer_type_checking_summary.property_instancetype_spelling_sites +
                 handoff.id_class_sel_object_pointer_type_checking_summary.property_object_pointer_type_sites <=
             handoff.id_class_sel_object_pointer_type_checking_summary.property_type_sites &&
         handoff.id_class_sel_object_pointer_type_checking_summary.canonical_reference_form_count ==
             kObjc3CanonicalReferenceTypeForms.size() &&
         handoff.id_class_sel_object_pointer_type_checking_summary.canonical_message_scalar_form_count ==
             kObjc3CanonicalScalarMessageSendTypeForms.size() &&
         handoff.id_class_sel_object_pointer_type_checking_summary.canonical_bridge_top_form_count ==
             kObjc3CanonicalBridgeTopReferenceTypeForms.size() &&
         handoff.id_class_sel_object_pointer_type_checking_summary.canonical_reference_forms_unique &&
         handoff.id_class_sel_object_pointer_type_checking_summary.canonical_message_scalar_forms_unique &&
         handoff.id_class_sel_object_pointer_type_checking_summary.canonical_bridge_top_forms_unique &&
         handoff.id_class_sel_object_pointer_type_checking_summary.canonical_bridge_top_subset_of_reference &&
         handoff.id_class_sel_object_pointer_type_checking_summary.canonical_type_form_scaffold_ready &&
         handoff.id_class_sel_object_pointer_type_checking_summary
             .canonical_type_form_diagnostics_hardening_consistent &&
         handoff.id_class_sel_object_pointer_type_checking_summary
             .canonical_type_form_diagnostics_hardening_ready &&
         !handoff.id_class_sel_object_pointer_type_checking_summary
              .canonical_type_form_diagnostics_hardening_key.empty() &&
         handoff.id_class_sel_object_pointer_type_checking_summary
             .canonical_type_form_recovery_determinism_consistent &&
         handoff.id_class_sel_object_pointer_type_checking_summary
             .canonical_type_form_recovery_determinism_ready &&
         !handoff.id_class_sel_object_pointer_type_checking_summary
              .canonical_type_form_recovery_determinism_key.empty() &&
         handoff.id_class_sel_object_pointer_type_checking_summary
             .canonical_type_form_conformance_matrix_consistent &&
         handoff.id_class_sel_object_pointer_type_checking_summary
             .canonical_type_form_conformance_matrix_ready &&
         !handoff.id_class_sel_object_pointer_type_checking_summary
              .canonical_type_form_conformance_matrix_key.empty() &&
         handoff.id_class_sel_object_pointer_type_checking_summary
             .canonical_type_form_conformance_corpus_case_count > 0u &&
         handoff.id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_conformance_corpus_passed_case_count ==
             handoff.id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_conformance_corpus_case_count &&
         handoff.id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_conformance_corpus_failed_case_count ==
             0u &&
         handoff.id_class_sel_object_pointer_type_checking_summary
             .canonical_type_form_conformance_corpus_consistent &&
         handoff.id_class_sel_object_pointer_type_checking_summary
             .canonical_type_form_conformance_corpus_ready &&
         !handoff.id_class_sel_object_pointer_type_checking_summary
              .canonical_type_form_conformance_corpus_key.empty() &&
         handoff.id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_performance_quality_required_guardrail_count >
             0u &&
         handoff.id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_performance_quality_passed_guardrail_count ==
             handoff.id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_performance_quality_required_guardrail_count &&
         handoff.id_class_sel_object_pointer_type_checking_summary
                 .canonical_type_form_performance_quality_failed_guardrail_count ==
             0u &&
         handoff.id_class_sel_object_pointer_type_checking_summary
             .canonical_type_form_performance_quality_guardrails_consistent &&
         handoff.id_class_sel_object_pointer_type_checking_summary
             .canonical_type_form_performance_quality_guardrails_ready &&
         !handoff.id_class_sel_object_pointer_type_checking_summary
              .canonical_type_form_performance_quality_guardrails_key.empty() &&
#include "sema/objc3_semantic_passes_body_validation_core_blocks.inc"
#include "sema/objc3_semantic_passes_body_validation_core_message_dispatch.inc"
#include "sema/objc3_semantic_passes_body_validation_core_runtime_arc.inc"
