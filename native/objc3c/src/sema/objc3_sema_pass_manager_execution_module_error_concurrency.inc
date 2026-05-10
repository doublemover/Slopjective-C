  result.module_import_graph_summary = result.integration_surface.module_import_graph_summary;
  result.deterministic_module_import_graph_handoff =
      result.type_metadata_handoff.module_import_graph_summary.deterministic &&
      result.integration_surface.module_import_graph_summary.deterministic &&
      IsEquivalentModuleImportGraphSummary(result.integration_surface.module_import_graph_summary,
                                           result.type_metadata_handoff.module_import_graph_summary) &&
      result.type_metadata_handoff.module_import_graph_summary.import_edge_candidate_sites <=
          result.type_metadata_handoff.module_import_graph_summary.module_import_graph_sites &&
      result.type_metadata_handoff.module_import_graph_summary.namespace_segment_sites <=
          result.type_metadata_handoff.module_import_graph_summary.module_import_graph_sites &&
      result.type_metadata_handoff.module_import_graph_summary.normalized_sites <=
          result.type_metadata_handoff.module_import_graph_summary.module_import_graph_sites &&
      result.type_metadata_handoff.module_import_graph_summary.contract_violation_sites <=
          result.type_metadata_handoff.module_import_graph_summary.module_import_graph_sites;
  result.namespace_collision_shadowing_summary =
      result.integration_surface.namespace_collision_shadowing_summary;
  result.deterministic_namespace_collision_shadowing_handoff =
      result.type_metadata_handoff.namespace_collision_shadowing_summary.deterministic &&
      result.integration_surface.namespace_collision_shadowing_summary.deterministic &&
      IsEquivalentNamespaceCollisionShadowingSummary(
          result.integration_surface.namespace_collision_shadowing_summary,
          result.type_metadata_handoff.namespace_collision_shadowing_summary) &&
      result.type_metadata_handoff.namespace_collision_shadowing_summary
              .namespace_segment_sites <=
          result.type_metadata_handoff.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      result.type_metadata_handoff.namespace_collision_shadowing_summary
              .import_edge_candidate_sites <=
          result.type_metadata_handoff.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      result.type_metadata_handoff.namespace_collision_shadowing_summary
              .normalized_sites <=
          result.type_metadata_handoff.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites &&
      result.type_metadata_handoff.namespace_collision_shadowing_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.namespace_collision_shadowing_summary
              .namespace_collision_shadowing_sites;
  result.public_private_api_partition_summary =
      result.integration_surface.public_private_api_partition_summary;
  result.deterministic_public_private_api_partition_handoff =
      result.type_metadata_handoff.public_private_api_partition_summary
              .deterministic &&
      result.integration_surface.public_private_api_partition_summary
              .deterministic &&
      IsEquivalentPublicPrivateApiPartitionSummary(
          result.integration_surface.public_private_api_partition_summary,
          result.type_metadata_handoff.public_private_api_partition_summary) &&
      result.type_metadata_handoff.public_private_api_partition_summary
              .namespace_segment_sites <=
          result.type_metadata_handoff.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      result.type_metadata_handoff.public_private_api_partition_summary
              .import_edge_candidate_sites <=
          result.type_metadata_handoff.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      result.type_metadata_handoff.public_private_api_partition_summary
              .normalized_sites <=
          result.type_metadata_handoff.public_private_api_partition_summary
              .public_private_api_partition_sites &&
      result.type_metadata_handoff.public_private_api_partition_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.public_private_api_partition_summary
              .public_private_api_partition_sites;
  result.incremental_module_cache_invalidation_summary =
      result.integration_surface.incremental_module_cache_invalidation_summary;
  result.deterministic_incremental_module_cache_invalidation_handoff =
      result.type_metadata_handoff.incremental_module_cache_invalidation_summary
              .deterministic &&
      result.integration_surface.incremental_module_cache_invalidation_summary
              .deterministic &&
      IsEquivalentIncrementalModuleCacheInvalidationSummary(
          result.integration_surface.incremental_module_cache_invalidation_summary,
          result.type_metadata_handoff
              .incremental_module_cache_invalidation_summary) &&
      result.type_metadata_handoff.incremental_module_cache_invalidation_summary
              .namespace_segment_sites <=
          result.type_metadata_handoff
              .incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      result.type_metadata_handoff.incremental_module_cache_invalidation_summary
              .import_edge_candidate_sites <=
          result.type_metadata_handoff
              .incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      result.type_metadata_handoff.incremental_module_cache_invalidation_summary
              .normalized_sites <=
          result.type_metadata_handoff
              .incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      result.type_metadata_handoff.incremental_module_cache_invalidation_summary
              .cache_invalidation_candidate_sites <=
          result.type_metadata_handoff
              .incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites &&
      result.type_metadata_handoff.incremental_module_cache_invalidation_summary
              .contract_violation_sites <=
          result.type_metadata_handoff
              .incremental_module_cache_invalidation_summary
              .incremental_module_cache_invalidation_sites;
  result.cross_module_conformance_summary =
      result.integration_surface.cross_module_conformance_summary;
  result.deterministic_cross_module_conformance_handoff =
      result.type_metadata_handoff.cross_module_conformance_summary
              .deterministic &&
      result.integration_surface.cross_module_conformance_summary.deterministic &&
      IsEquivalentCrossModuleConformanceSummary(
          result.integration_surface.cross_module_conformance_summary,
          result.type_metadata_handoff.cross_module_conformance_summary) &&
      result.type_metadata_handoff.cross_module_conformance_summary.namespace_segment_sites <=
          result.type_metadata_handoff.cross_module_conformance_summary
              .cross_module_conformance_sites &&
      result.type_metadata_handoff.cross_module_conformance_summary.import_edge_candidate_sites <=
          result.type_metadata_handoff.cross_module_conformance_summary
              .cross_module_conformance_sites &&
      result.type_metadata_handoff.cross_module_conformance_summary.normalized_sites <=
          result.type_metadata_handoff.cross_module_conformance_summary
              .cross_module_conformance_sites &&
      result.type_metadata_handoff.cross_module_conformance_summary.cache_invalidation_candidate_sites <=
          result.type_metadata_handoff.cross_module_conformance_summary
              .cross_module_conformance_sites &&
      result.type_metadata_handoff.cross_module_conformance_summary
              .normalized_sites +
              result.type_metadata_handoff.cross_module_conformance_summary
                  .cache_invalidation_candidate_sites ==
          result.type_metadata_handoff.cross_module_conformance_summary
              .cross_module_conformance_sites &&
      result.type_metadata_handoff.cross_module_conformance_summary.contract_violation_sites <=
          result.type_metadata_handoff.cross_module_conformance_summary
              .cross_module_conformance_sites;
  result.throws_propagation_summary =
      result.integration_surface.throws_propagation_summary;
  result.deterministic_throws_propagation_handoff =
      result.type_metadata_handoff.throws_propagation_summary.deterministic &&
      result.integration_surface.throws_propagation_summary.deterministic &&
      IsEquivalentThrowsPropagationSummary(
          result.integration_surface.throws_propagation_summary,
          result.type_metadata_handoff.throws_propagation_summary) &&
      result.type_metadata_handoff.throws_propagation_summary.namespace_segment_sites <=
          result.type_metadata_handoff.throws_propagation_summary
              .throws_propagation_sites &&
      result.type_metadata_handoff.throws_propagation_summary.import_edge_candidate_sites <=
          result.type_metadata_handoff.throws_propagation_summary
              .throws_propagation_sites &&
      result.type_metadata_handoff.throws_propagation_summary.normalized_sites <=
          result.type_metadata_handoff.throws_propagation_summary
              .throws_propagation_sites &&
      result.type_metadata_handoff.throws_propagation_summary.cache_invalidation_candidate_sites <=
          result.type_metadata_handoff.throws_propagation_summary
              .throws_propagation_sites &&
      result.type_metadata_handoff.throws_propagation_summary.normalized_sites +
              result.type_metadata_handoff.throws_propagation_summary
                  .cache_invalidation_candidate_sites ==
          result.type_metadata_handoff.throws_propagation_summary
              .throws_propagation_sites &&
      result.type_metadata_handoff.throws_propagation_summary.contract_violation_sites <=
          result.type_metadata_handoff.throws_propagation_summary
              .throws_propagation_sites;
  result.actor_isolation_sendability_summary =
      result.integration_surface.actor_isolation_sendability_summary;
  result.deterministic_actor_isolation_sendability_handoff =
      result.type_metadata_handoff.actor_isolation_sendability_summary
              .deterministic &&
      result.integration_surface.actor_isolation_sendability_summary
          .deterministic &&
      IsEquivalentActorIsolationSendabilitySummary(
          result.integration_surface.actor_isolation_sendability_summary,
          result.type_metadata_handoff.actor_isolation_sendability_summary) &&
      result.type_metadata_handoff.actor_isolation_sendability_summary
              .actor_isolation_decl_sites <=
          result.type_metadata_handoff.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      result.type_metadata_handoff.actor_isolation_sendability_summary
              .actor_hop_sites <=
          result.type_metadata_handoff.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      result.type_metadata_handoff.actor_isolation_sendability_summary
              .sendable_annotation_sites <=
          result.type_metadata_handoff.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      result.type_metadata_handoff.actor_isolation_sendability_summary
              .non_sendable_crossing_sites <=
          result.type_metadata_handoff.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      result.type_metadata_handoff.actor_isolation_sendability_summary
              .isolation_boundary_sites <=
          result.type_metadata_handoff.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      result.type_metadata_handoff.actor_isolation_sendability_summary
              .normalized_sites <=
          result.type_metadata_handoff.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      result.type_metadata_handoff.actor_isolation_sendability_summary
              .gate_blocked_sites <=
          result.type_metadata_handoff.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      result.type_metadata_handoff.actor_isolation_sendability_summary
              .gate_blocked_sites <=
          result.type_metadata_handoff.actor_isolation_sendability_summary
              .non_sendable_crossing_sites &&
      result.type_metadata_handoff.actor_isolation_sendability_summary
              .contract_violation_sites <=
          result.type_metadata_handoff.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      result.type_metadata_handoff.actor_isolation_sendability_summary
              .normalized_sites +
              result.type_metadata_handoff.actor_isolation_sendability_summary
                  .gate_blocked_sites ==
          result.type_metadata_handoff.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites;
  result.task_runtime_cancellation_summary =
      result.integration_surface.task_runtime_cancellation_summary;
  result.deterministic_task_runtime_cancellation_handoff =
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .deterministic &&
      result.integration_surface.task_runtime_cancellation_summary
          .deterministic &&
      IsEquivalentTaskRuntimeCancellationSummary(
          result.integration_surface.task_runtime_cancellation_summary,
          result.type_metadata_handoff.task_runtime_cancellation_summary) &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
              .runtime_hook_sites <=
          result.type_metadata_handoff.task_runtime_cancellation_summary
              .task_runtime_interop_sites &&
      result.type_metadata_handoff.task_runtime_cancellation_summary
