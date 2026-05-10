static Objc3VarianceBridgeCastSummary BuildVarianceBridgeCastSummaryFromTypeAnnotationAndProtocolSummary(
    const Objc3TypeAnnotationSurfaceSummary &type_annotation_summary,
    const Objc3ProtocolQualifiedObjectTypeSummary &protocol_summary) {
  Objc3VarianceBridgeCastSummary summary;
  summary.variance_bridge_cast_sites = protocol_summary.protocol_qualified_object_type_sites;
  summary.protocol_composition_sites = protocol_summary.protocol_composition_sites;
  summary.ownership_qualifier_sites = type_annotation_summary.ownership_qualifier_sites;
  summary.object_pointer_type_sites = type_annotation_summary.object_pointer_type_sites;
  summary.pointer_declarator_sites = protocol_summary.pointer_declarator_sites;
  summary.normalized_sites = std::min(protocol_summary.normalized_protocol_composition_sites,
                                      summary.object_pointer_type_sites);
  summary.contract_violation_sites = protocol_summary.contract_violation_sites +
                                     type_annotation_summary.invalid_ownership_qualifier_sites;
  summary.deterministic =
      type_annotation_summary.deterministic && protocol_summary.deterministic &&
      summary.contract_violation_sites == 0u &&
      summary.protocol_composition_sites <= summary.variance_bridge_cast_sites &&
      summary.normalized_sites <= summary.variance_bridge_cast_sites &&
      summary.contract_violation_sites <= summary.variance_bridge_cast_sites;
  return summary;
}

static Objc3GenericMetadataAbiSummary BuildGenericMetadataAbiSummaryFromTypeAnnotationAndConstraintSummaries(
    const Objc3TypeAnnotationSurfaceSummary &type_annotation_summary,
    const Objc3LightweightGenericConstraintSummary &lightweight_summary,
    const Objc3VarianceBridgeCastSummary &variance_summary) {
  Objc3GenericMetadataAbiSummary summary;
  summary.generic_metadata_abi_sites = lightweight_summary.generic_constraint_sites;
  summary.generic_suffix_sites = lightweight_summary.generic_suffix_sites;
  summary.protocol_composition_sites = variance_summary.protocol_composition_sites;
  summary.ownership_qualifier_sites = variance_summary.ownership_qualifier_sites;
  summary.object_pointer_type_sites = type_annotation_summary.object_pointer_type_sites;
  summary.pointer_declarator_sites = type_annotation_summary.pointer_declarator_sites;
  summary.normalized_sites =
      std::min(std::min(lightweight_summary.normalized_constraint_sites,
                        variance_summary.normalized_sites),
               summary.object_pointer_type_sites);
  summary.contract_violation_sites =
      lightweight_summary.contract_violation_sites +
      variance_summary.contract_violation_sites +
      type_annotation_summary.invalid_generic_suffix_sites;
  summary.deterministic =
      type_annotation_summary.deterministic && lightweight_summary.deterministic &&
      variance_summary.deterministic && summary.contract_violation_sites == 0u &&
      summary.generic_suffix_sites <= summary.generic_metadata_abi_sites &&
      summary.protocol_composition_sites <= summary.generic_metadata_abi_sites &&
      summary.normalized_sites <= summary.generic_metadata_abi_sites &&
      summary.contract_violation_sites <= summary.generic_metadata_abi_sites;
  return summary;
}

static Objc3ModuleImportGraphSummary BuildModuleImportGraphSummaryFromTypeAnnotationAndGenericMetadataSummary(
    const Objc3TypeAnnotationSurfaceSummary &type_annotation_summary,
    const Objc3GenericMetadataAbiSummary &generic_metadata_summary) {
  Objc3ModuleImportGraphSummary summary;
  summary.module_import_graph_sites = generic_metadata_summary.generic_metadata_abi_sites;
  summary.import_edge_candidate_sites = generic_metadata_summary.generic_suffix_sites;
  summary.namespace_segment_sites =
      std::min(generic_metadata_summary.protocol_composition_sites +
                   generic_metadata_summary.object_pointer_type_sites,
               summary.module_import_graph_sites);
  summary.object_pointer_type_sites = type_annotation_summary.object_pointer_type_sites;
  summary.pointer_declarator_sites = type_annotation_summary.pointer_declarator_sites;
  summary.normalized_sites =
      std::min(summary.import_edge_candidate_sites, summary.object_pointer_type_sites);
  summary.contract_violation_sites =
      generic_metadata_summary.contract_violation_sites +
      type_annotation_summary.invalid_generic_suffix_sites;
  summary.deterministic =
      type_annotation_summary.deterministic && generic_metadata_summary.deterministic &&
      summary.contract_violation_sites == 0u &&
      summary.import_edge_candidate_sites <= summary.module_import_graph_sites &&
      summary.namespace_segment_sites <= summary.module_import_graph_sites &&
      summary.normalized_sites <= summary.module_import_graph_sites &&
      summary.contract_violation_sites <= summary.module_import_graph_sites;
  return summary;
}

static Objc3NamespaceCollisionShadowingSummary
BuildNamespaceCollisionShadowingSummaryFromModuleImportGraphSummary(
    const Objc3ModuleImportGraphSummary &module_import_graph_summary) {
  Objc3NamespaceCollisionShadowingSummary summary;
  summary.namespace_collision_shadowing_sites =
      module_import_graph_summary.module_import_graph_sites;
  summary.namespace_segment_sites = module_import_graph_summary.namespace_segment_sites;
  summary.import_edge_candidate_sites =
      module_import_graph_summary.import_edge_candidate_sites;
  summary.object_pointer_type_sites =
      module_import_graph_summary.object_pointer_type_sites;
  summary.pointer_declarator_sites =
      module_import_graph_summary.pointer_declarator_sites;
  summary.normalized_sites = module_import_graph_summary.normalized_sites;
  summary.contract_violation_sites =
      module_import_graph_summary.contract_violation_sites;
  summary.deterministic =
      module_import_graph_summary.deterministic &&
      summary.namespace_segment_sites <=
          summary.namespace_collision_shadowing_sites &&
      summary.import_edge_candidate_sites <=
          summary.namespace_collision_shadowing_sites &&
      summary.normalized_sites <= summary.namespace_collision_shadowing_sites &&
      summary.contract_violation_sites <=
          summary.namespace_collision_shadowing_sites;
  return summary;
}

static Objc3PublicPrivateApiPartitionSummary
BuildPublicPrivateApiPartitionSummaryFromNamespaceCollisionShadowingSummary(
    const Objc3NamespaceCollisionShadowingSummary
        &namespace_collision_shadowing_summary) {
  Objc3PublicPrivateApiPartitionSummary summary;
  summary.public_private_api_partition_sites =
      namespace_collision_shadowing_summary
          .namespace_collision_shadowing_sites;
  summary.namespace_segment_sites =
      namespace_collision_shadowing_summary.namespace_segment_sites;
  summary.import_edge_candidate_sites =
      namespace_collision_shadowing_summary.import_edge_candidate_sites;
  summary.object_pointer_type_sites =
      namespace_collision_shadowing_summary.object_pointer_type_sites;
  summary.pointer_declarator_sites =
      namespace_collision_shadowing_summary.pointer_declarator_sites;
  summary.normalized_sites = namespace_collision_shadowing_summary.normalized_sites;
  summary.contract_violation_sites =
      namespace_collision_shadowing_summary.contract_violation_sites;
  summary.deterministic =
      namespace_collision_shadowing_summary.deterministic &&
      summary.namespace_segment_sites <= summary.public_private_api_partition_sites &&
      summary.import_edge_candidate_sites <= summary.public_private_api_partition_sites &&
      summary.normalized_sites <= summary.public_private_api_partition_sites &&
      summary.contract_violation_sites <= summary.public_private_api_partition_sites;
  return summary;
}

static Objc3IncrementalModuleCacheInvalidationSummary
BuildIncrementalModuleCacheInvalidationSummaryFromPublicPrivateApiPartitionSummary(
    const Objc3PublicPrivateApiPartitionSummary
        &public_private_api_partition_summary) {
  Objc3IncrementalModuleCacheInvalidationSummary summary;
  summary.incremental_module_cache_invalidation_sites =
      public_private_api_partition_summary.public_private_api_partition_sites;
  summary.namespace_segment_sites =
      public_private_api_partition_summary.namespace_segment_sites;
  summary.import_edge_candidate_sites =
      public_private_api_partition_summary.import_edge_candidate_sites;
  summary.object_pointer_type_sites =
      public_private_api_partition_summary.object_pointer_type_sites;
  summary.pointer_declarator_sites =
      public_private_api_partition_summary.pointer_declarator_sites;
  summary.normalized_sites =
      std::min(public_private_api_partition_summary.normalized_sites,
               summary.incremental_module_cache_invalidation_sites);
  summary.cache_invalidation_candidate_sites =
      summary.incremental_module_cache_invalidation_sites -
      summary.normalized_sites;
  summary.contract_violation_sites =
      public_private_api_partition_summary.contract_violation_sites;
  summary.deterministic =
      public_private_api_partition_summary.deterministic &&
      summary.namespace_segment_sites <=
          summary.incremental_module_cache_invalidation_sites &&
      summary.import_edge_candidate_sites <=
          summary.incremental_module_cache_invalidation_sites &&
      summary.normalized_sites <=
          summary.incremental_module_cache_invalidation_sites &&
      summary.cache_invalidation_candidate_sites <=
          summary.incremental_module_cache_invalidation_sites &&
      summary.contract_violation_sites <=
          summary.incremental_module_cache_invalidation_sites;
  return summary;
}

static Objc3CrossModuleConformanceSummary
BuildCrossModuleConformanceSummaryFromIncrementalModuleCacheInvalidationSummary(
    const Objc3IncrementalModuleCacheInvalidationSummary
        &incremental_module_cache_invalidation_summary) {
  Objc3CrossModuleConformanceSummary summary;
  summary.cross_module_conformance_sites =
      incremental_module_cache_invalidation_summary
          .incremental_module_cache_invalidation_sites;
  summary.namespace_segment_sites =
      incremental_module_cache_invalidation_summary.namespace_segment_sites;
  summary.import_edge_candidate_sites =
      incremental_module_cache_invalidation_summary.import_edge_candidate_sites;
  summary.object_pointer_type_sites =
      incremental_module_cache_invalidation_summary.object_pointer_type_sites;
  summary.pointer_declarator_sites =
      incremental_module_cache_invalidation_summary.pointer_declarator_sites;
  summary.normalized_sites =
      incremental_module_cache_invalidation_summary.normalized_sites;
  summary.cache_invalidation_candidate_sites =
      incremental_module_cache_invalidation_summary
          .cache_invalidation_candidate_sites;
  summary.contract_violation_sites =
      incremental_module_cache_invalidation_summary.contract_violation_sites;
  summary.deterministic =
      incremental_module_cache_invalidation_summary.deterministic &&
      summary.namespace_segment_sites <= summary.cross_module_conformance_sites &&
      summary.import_edge_candidate_sites <= summary.cross_module_conformance_sites &&
      summary.normalized_sites <= summary.cross_module_conformance_sites &&
      summary.cache_invalidation_candidate_sites <=
          summary.cross_module_conformance_sites &&
      summary.normalized_sites + summary.cache_invalidation_candidate_sites ==
          summary.cross_module_conformance_sites &&
      summary.contract_violation_sites <= summary.cross_module_conformance_sites;
  return summary;
}

static Objc3ThrowsPropagationSummary
BuildThrowsPropagationSummaryFromCrossModuleConformanceSummary(
    const Objc3CrossModuleConformanceSummary
        &cross_module_conformance_summary) {
  Objc3ThrowsPropagationSummary summary;
  summary.throws_propagation_sites =
      cross_module_conformance_summary.cross_module_conformance_sites;
  summary.namespace_segment_sites =
      cross_module_conformance_summary.namespace_segment_sites;
  summary.import_edge_candidate_sites =
      cross_module_conformance_summary.import_edge_candidate_sites;
  summary.object_pointer_type_sites =
      cross_module_conformance_summary.object_pointer_type_sites;
  summary.pointer_declarator_sites =
      cross_module_conformance_summary.pointer_declarator_sites;
  summary.normalized_sites = cross_module_conformance_summary.normalized_sites;
  summary.cache_invalidation_candidate_sites =
      cross_module_conformance_summary.cache_invalidation_candidate_sites;
  summary.contract_violation_sites =
      cross_module_conformance_summary.contract_violation_sites;
  summary.deterministic =
      cross_module_conformance_summary.deterministic &&
      summary.namespace_segment_sites <= summary.throws_propagation_sites &&
      summary.import_edge_candidate_sites <= summary.throws_propagation_sites &&
      summary.normalized_sites <= summary.throws_propagation_sites &&
      summary.cache_invalidation_candidate_sites <= summary.throws_propagation_sites &&
      summary.normalized_sites + summary.cache_invalidation_candidate_sites ==
          summary.throws_propagation_sites &&
      summary.contract_violation_sites <= summary.throws_propagation_sites;
  return summary;
}
