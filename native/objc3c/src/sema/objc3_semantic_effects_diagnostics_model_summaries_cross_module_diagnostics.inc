Objc3CrossModuleSemanticContractsDiagnosticsSummary
BuildCrossModuleSemanticContractsDiagnosticsSummary(
    const Objc3SemanticIntegrationSurface &surface,
    const Objc3InteropInteropSemanticModelSummary &interop_summary) {
  Objc3CrossModuleSemanticContractsDiagnosticsSummary summary;
  const auto &module_import = surface.module_import_graph_summary;
  const auto &namespace_collision =
      surface.namespace_collision_shadowing_summary;
  const auto &api_partition = surface.public_private_api_partition_summary;
  const auto &incremental_cache =
      surface.incremental_module_cache_invalidation_summary;
  const auto &cross_module = surface.cross_module_conformance_summary;
  const auto &diagnostics = surface.error_diagnostics_recovery_summary;

  summary.module_import_graph_sites = module_import.module_import_graph_sites;
  summary.import_edge_candidate_sites =
      module_import.import_edge_candidate_sites;
  summary.namespace_segment_sites = module_import.namespace_segment_sites;
  summary.object_pointer_type_sites = module_import.object_pointer_type_sites;
  summary.pointer_declarator_sites = module_import.pointer_declarator_sites;
  summary.namespace_collision_shadowing_sites =
      namespace_collision.namespace_collision_shadowing_sites;
  summary.public_private_api_partition_sites =
      api_partition.public_private_api_partition_sites;
  summary.incremental_module_cache_invalidation_sites =
      incremental_cache.incremental_module_cache_invalidation_sites;
  summary.cross_module_conformance_sites =
      cross_module.cross_module_conformance_sites;
  summary.normalized_cross_module_sites = cross_module.normalized_sites;
  summary.cache_invalidation_candidate_sites =
      cross_module.cache_invalidation_candidate_sites;
  summary.diagnostic_recovery_sites =
      diagnostics.error_diagnostics_recovery_sites;
  summary.diagnostic_emit_sites = diagnostics.diagnostic_emit_sites;
  summary.recovery_anchor_sites = diagnostics.recovery_anchor_sites;
  summary.recovery_boundary_sites = diagnostics.recovery_boundary_sites;
  summary.fail_closed_diagnostic_sites =
      diagnostics.fail_closed_diagnostic_sites;
  summary.diagnostic_normalized_sites = diagnostics.normalized_sites;
  summary.diagnostic_gate_blocked_sites = diagnostics.gate_blocked_sites;
  summary.interop_import_module_annotation_sites =
      interop_summary.import_module_annotation_sites;
  summary.interop_imported_module_name_sites =
      interop_summary.imported_module_name_sites;
  summary.contract_violation_sites =
      module_import.contract_violation_sites +
      namespace_collision.contract_violation_sites +
      api_partition.contract_violation_sites +
      incremental_cache.contract_violation_sites +
      cross_module.contract_violation_sites +
      diagnostics.contract_violation_sites;

  summary.module_import_graph_semantics_landed =
      module_import.deterministic &&
      module_import.normalized_sites ==
          module_import.module_import_graph_sites &&
      module_import.import_edge_candidate_sites <=
          module_import.module_import_graph_sites &&
      module_import.namespace_segment_sites <=
          module_import.module_import_graph_sites;
  summary.namespace_collision_semantics_landed =
      namespace_collision.deterministic &&
      namespace_collision.namespace_segment_sites <=
          namespace_collision.namespace_collision_shadowing_sites &&
      namespace_collision.import_edge_candidate_sites <=
          namespace_collision.namespace_collision_shadowing_sites &&
      namespace_collision.normalized_sites <=
          namespace_collision.namespace_collision_shadowing_sites;
  summary.public_private_partition_semantics_landed =
      api_partition.deterministic &&
      api_partition.namespace_segment_sites <=
          api_partition.public_private_api_partition_sites &&
      api_partition.import_edge_candidate_sites <=
          api_partition.public_private_api_partition_sites &&
      api_partition.normalized_sites <=
          api_partition.public_private_api_partition_sites;
  summary.incremental_cache_semantics_landed =
      incremental_cache.deterministic &&
      incremental_cache.normalized_sites +
              incremental_cache.cache_invalidation_candidate_sites ==
          incremental_cache.incremental_module_cache_invalidation_sites;
  summary.cross_module_conformance_semantics_landed =
      cross_module.deterministic &&
      cross_module.normalized_sites +
              cross_module.cache_invalidation_candidate_sites ==
          cross_module.cross_module_conformance_sites &&
      cross_module.contract_violation_sites <=
          cross_module.cross_module_conformance_sites;
  summary.diagnostic_recovery_semantics_landed =
      diagnostics.deterministic &&
      diagnostics.diagnostic_emit_sites <=
          diagnostics.error_diagnostics_recovery_sites &&
      diagnostics.recovery_anchor_sites <=
          diagnostics.error_diagnostics_recovery_sites &&
      diagnostics.recovery_boundary_sites <=
          diagnostics.error_diagnostics_recovery_sites &&
      diagnostics.fail_closed_diagnostic_sites <=
          diagnostics.error_diagnostics_recovery_sites &&
      diagnostics.normalized_sites + diagnostics.gate_blocked_sites ==
          diagnostics.error_diagnostics_recovery_sites;
  summary.interop_import_semantics_landed =
      interop_summary.deterministic &&
      interop_summary.imported_module_name_sites <=
          interop_summary.import_module_annotation_sites;
  summary.deterministic =
      summary.contract_violation_sites == 0 &&
      summary.module_import_graph_semantics_landed &&
      summary.namespace_collision_semantics_landed &&
      summary.public_private_partition_semantics_landed &&
      summary.incremental_cache_semantics_landed &&
      summary.cross_module_conformance_semantics_landed &&
      summary.diagnostic_recovery_semantics_landed &&
      summary.interop_import_semantics_landed;
  summary.ready_for_lowering_and_runtime = summary.deterministic;
  if (!summary.deterministic) {
    summary.failure_reason =
        "cross-module semantic contracts and diagnostic recovery packets must remain deterministic and violation-free";
  }

  std::ostringstream out;
  out << summary.contract_id
      << ";module-import=" << summary.module_import_graph_sites << ":"
      << summary.import_edge_candidate_sites << ":" << summary.namespace_segment_sites
      << ";namespace=" << summary.namespace_collision_shadowing_sites
      << ";api-partition=" << summary.public_private_api_partition_sites
      << ";incremental-cache="
      << summary.incremental_module_cache_invalidation_sites << ":"
      << summary.cache_invalidation_candidate_sites
      << ";cross-module=" << summary.cross_module_conformance_sites << ":"
      << summary.normalized_cross_module_sites
      << ";diagnostics=" << summary.diagnostic_recovery_sites << ":"
      << summary.diagnostic_emit_sites << ":" << summary.fail_closed_diagnostic_sites
      << ";interop-import="
      << summary.interop_import_module_annotation_sites << ":"
      << summary.interop_imported_module_name_sites
      << ";violations=" << summary.contract_violation_sites;
  summary.replay_key = out.str();
  return summary;
}
