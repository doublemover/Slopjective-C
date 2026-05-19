  result.core_semantic_parity_publication_readiness_record =
      BuildObjc3SemaCoreSemanticParityPublicationReadinessRecord(
          input,
          result.parity_surface,
          result.deterministic_semantic_diagnostics,
          result.deterministic_type_metadata_handoff,
          result.deterministic_interface_implementation_handoff,
          result.deterministic_protocol_category_composition_handoff,
          result.deterministic_class_protocol_category_linking_handoff,
          result.deterministic_selector_normalization_handoff,
          result.deterministic_property_attribute_handoff,
          result.deterministic_type_annotation_surface_handoff,
          result.deterministic_lightweight_generic_constraint_handoff,
          result.deterministic_nullability_flow_warning_precision_handoff,
          result.deterministic_protocol_qualified_object_type_handoff,
          result.deterministic_variance_bridge_cast_handoff);
  result.deterministic_core_semantic_parity_publication_readiness_record =
      IsReadyObjc3SemaCoreSemanticParityPublicationReadinessRecord(
          result.core_semantic_parity_publication_readiness_record);
  result.parity_surface.core_semantic_parity_publication_readiness_record =
      result.core_semantic_parity_publication_readiness_record;
  result.parity_surface
      .deterministic_core_semantic_parity_publication_readiness_record =
      result.deterministic_core_semantic_parity_publication_readiness_record;
  result.parity_surface.deterministic_semantic_diagnostics =
      result.parity_surface.core_semantic_parity_publication_readiness_record
          .semantic_diagnostics_ready;
  result.parity_surface.deterministic_type_metadata_handoff =
      result.parity_surface.core_semantic_parity_publication_readiness_record
          .type_metadata_handoff_ready;
  result.parity_surface.deterministic_interface_implementation_handoff =
      result.parity_surface.core_semantic_parity_publication_readiness_record
          .interface_implementation_ready;
  result.parity_surface.deterministic_protocol_category_composition_handoff =
      result.parity_surface.core_semantic_parity_publication_readiness_record
          .protocol_category_composition_ready;
  result.parity_surface.deterministic_class_protocol_category_linking_handoff =
      result.parity_surface.core_semantic_parity_publication_readiness_record
          .class_protocol_category_linking_ready;
  result.parity_surface.deterministic_selector_normalization_handoff =
      result.parity_surface.core_semantic_parity_publication_readiness_record
          .selector_normalization_ready;
  result.parity_surface.deterministic_property_attribute_handoff =
      result.parity_surface.core_semantic_parity_publication_readiness_record
          .property_attribute_ready;
  result.parity_surface.deterministic_type_annotation_surface_handoff =
      result.parity_surface.core_semantic_parity_publication_readiness_record
          .type_annotation_surface_ready;
  result.parity_surface.deterministic_lightweight_generic_constraint_handoff =
      result.parity_surface.core_semantic_parity_publication_readiness_record
          .lightweight_generic_constraint_ready;
  result.parity_surface.deterministic_nullability_flow_warning_precision_handoff =
      result.parity_surface.core_semantic_parity_publication_readiness_record
          .nullability_flow_warning_precision_ready;
  result.parity_surface.deterministic_protocol_qualified_object_type_handoff =
      result.parity_surface.core_semantic_parity_publication_readiness_record
          .protocol_qualified_object_type_ready;
  result.parity_surface.deterministic_variance_bridge_cast_handoff =
      result.parity_surface.core_semantic_parity_publication_readiness_record
          .variance_bridge_cast_ready;
  result.module_semantic_parity_publication_readiness_record =
      BuildObjc3SemaModuleSemanticParityPublicationReadinessRecord(
          input,
          result.parity_surface,
          result.deterministic_generic_metadata_abi_handoff,
          result.deterministic_module_import_graph_handoff,
          result.deterministic_namespace_collision_shadowing_handoff,
          result.deterministic_public_private_api_partition_handoff,
          result.deterministic_incremental_module_cache_invalidation_handoff);
  result.deterministic_module_semantic_parity_publication_readiness_record =
      IsReadyObjc3SemaModuleSemanticParityPublicationReadinessRecord(
          result.module_semantic_parity_publication_readiness_record);
  result.parity_surface.module_semantic_parity_publication_readiness_record =
      result.module_semantic_parity_publication_readiness_record;
  result.parity_surface
      .deterministic_module_semantic_parity_publication_readiness_record =
      result.deterministic_module_semantic_parity_publication_readiness_record;
  result.parity_surface.deterministic_generic_metadata_abi_handoff =
      result.parity_surface
          .module_semantic_parity_publication_readiness_record
          .generic_metadata_abi_ready;
  result.parity_surface.deterministic_module_import_graph_handoff =
      result.parity_surface
          .module_semantic_parity_publication_readiness_record
          .module_import_graph_ready;
  result.parity_surface.deterministic_namespace_collision_shadowing_handoff =
      result.parity_surface
          .module_semantic_parity_publication_readiness_record
          .namespace_collision_shadowing_ready;
  result.parity_surface.deterministic_public_private_api_partition_handoff =
      result.parity_surface
          .module_semantic_parity_publication_readiness_record
          .public_private_api_partition_ready;
  result.parity_surface.deterministic_incremental_module_cache_invalidation_handoff =
      result.parity_surface
          .module_semantic_parity_publication_readiness_record
          .incremental_module_cache_invalidation_ready;
  result.intermodule_flow_parity_publication_readiness_record =
      BuildObjc3SemaIntermoduleFlowParityPublicationReadinessRecord(
          input,
          result.parity_surface,
          result.deterministic_cross_module_conformance_handoff,
          result.deterministic_throws_propagation_handoff);
  result.deterministic_intermodule_flow_parity_publication_readiness_record =
      IsReadyObjc3SemaIntermoduleFlowParityPublicationReadinessRecord(
          result.intermodule_flow_parity_publication_readiness_record);
  result.parity_surface.intermodule_flow_parity_publication_readiness_record =
      result.intermodule_flow_parity_publication_readiness_record;
  result.parity_surface
      .deterministic_intermodule_flow_parity_publication_readiness_record =
      result.deterministic_intermodule_flow_parity_publication_readiness_record;
  result.parity_surface.deterministic_cross_module_conformance_handoff =
      result.parity_surface
          .intermodule_flow_parity_publication_readiness_record
          .cross_module_conformance_ready;
  result.parity_surface.deterministic_throws_propagation_handoff =
      result.parity_surface
          .intermodule_flow_parity_publication_readiness_record
          .throws_propagation_ready;
  result.concurrency_parity_publication_readiness_record =
      BuildObjc3SemaConcurrencyParityPublicationReadinessRecord(
          input,
          result.parity_surface,
          result.deterministic_actor_isolation_sendability_handoff,
          result.deterministic_task_runtime_cancellation_handoff,
          result.deterministic_concurrency_replay_race_guard_handoff);
  result.deterministic_concurrency_parity_publication_readiness_record =
      IsReadyObjc3SemaConcurrencyParityPublicationReadinessRecord(
          result.concurrency_parity_publication_readiness_record);
  result.parity_surface.concurrency_parity_publication_readiness_record =
      result.concurrency_parity_publication_readiness_record;
  result.parity_surface
      .deterministic_concurrency_parity_publication_readiness_record =
      result.deterministic_concurrency_parity_publication_readiness_record;
  result.parity_surface.deterministic_actor_isolation_sendability_handoff =
      result.parity_surface
          .concurrency_parity_publication_readiness_record
          .actor_isolation_sendability_ready;
  result.parity_surface.deterministic_task_runtime_cancellation_handoff =
      result.parity_surface
          .concurrency_parity_publication_readiness_record
          .task_runtime_cancellation_ready;
  result.parity_surface.deterministic_concurrency_replay_race_guard_handoff =
      result.parity_surface
          .concurrency_parity_publication_readiness_record
          .concurrency_replay_race_guard_ready &&
      result.parity_surface.concurrency_replay_race_guard_summary
              .task_handoff_sites <=
          result.parity_surface.concurrency_replay_race_guard_summary
