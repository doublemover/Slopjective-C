struct Objc3AutoreleasePoolScopeSiteMetadata {
  std::string scope_symbol;
  unsigned scope_depth = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3AutoreleasePoolScopeSummary {
  std::size_t scope_sites = 0;
  std::size_t scope_symbolized_sites = 0;
  std::size_t contract_violation_sites = 0;
  unsigned max_scope_depth = 0;
  bool deterministic = true;
};

enum class Objc3SemanticCanonicalTypeKind : std::uint8_t {
  Unknown = 0,
  Scalar = 1,
  Function = 2,
  Block = 3,
  Object = 4,
  ClassObject = 5,
  Selector = 6,
  ProtocolObject = 7,
  Instancetype = 8,
  ObjectPointer = 9,
  ForeignObject = 10,
  Vector = 11,
};

enum class Objc3SemanticCanonicalNullability : std::uint8_t {
  Unspecified = 0,
  Nullable = 1,
  Nonnull = 2,
  ImplicitlyUnwrapped = 3,
  NullResettable = 4,
  Inherited = 5,
};

enum class Objc3SemanticCanonicalOwnership : std::uint8_t {
  Unspecified = 0,
  Strong = 1,
  Copy = 2,
  Retain = 3,
  Weak = 4,
  Unowned = 5,
  UnsafeUnretained = 6,
  Assign = 7,
};

struct Objc3SemanticCanonicalType {
  ValueType value_type = ValueType::Unknown;
  Objc3SemanticCanonicalTypeKind kind =
      Objc3SemanticCanonicalTypeKind::Unknown;
  Objc3SemanticCanonicalNullability nullability =
      Objc3SemanticCanonicalNullability::Unspecified;
  Objc3SemanticCanonicalOwnership ownership =
      Objc3SemanticCanonicalOwnership::Unspecified;
  bool is_vector = false;
  std::string vector_base_spelling;
  unsigned vector_lane_count = 1;
  bool has_pointer_declarator = false;
  unsigned pointer_declarator_depth = 0;
  std::string object_pointer_type_name;
  bool has_protocol_composition = false;
  std::vector<std::string> protocol_composition_lexicographic;
  bool has_generic_suffix = false;
  std::vector<std::string> generic_arguments_source_order;
  std::vector<std::string> generic_arguments_lexicographic;
  bool has_invalid_type_suffix = false;
  bool deterministic = true;
  std::string canonical_spelling;
  std::string replay_key;
};

struct FunctionInfo {
  std::size_t arity = 0;
  std::vector<ValueType> param_types;
  std::vector<Objc3SemanticCanonicalType> param_canonical_types;
  std::vector<bool> param_is_vector;
  std::vector<std::string> param_vector_base_spelling;
  std::vector<unsigned> param_vector_lane_count;
  std::vector<bool> param_has_generic_suffix;
  std::vector<bool> param_has_pointer_declarator;
  std::vector<bool> param_has_nullability_suffix;
  std::vector<bool> param_has_ownership_qualifier;
  std::vector<bool> param_id_spelling;
  std::vector<bool> param_class_spelling;
  std::vector<bool> param_instancetype_spelling;
  std::vector<bool> param_object_pointer_type_spelling;
  std::vector<bool> param_has_invalid_generic_suffix;
  std::vector<bool> param_has_invalid_pointer_declarator;
  std::vector<bool> param_has_invalid_nullability_suffix;
  std::vector<bool> param_has_invalid_ownership_qualifier;
  std::vector<bool> param_has_invalid_type_suffix;
  std::vector<bool> param_ownership_insert_retain;
  std::vector<bool> param_ownership_insert_release;
  std::vector<bool> param_ownership_insert_autorelease;
  std::vector<bool> param_ownership_is_weak_reference;
  std::vector<bool> param_ownership_is_unowned_reference;
  std::vector<bool> param_ownership_is_unowned_safe_reference;
  std::vector<bool> param_ownership_arc_diagnostic_candidate;
  std::vector<bool> param_ownership_arc_fixit_available;
  std::vector<std::string> param_ownership_arc_diagnostic_profile;
  std::vector<std::string> param_ownership_arc_fixit_hint;
  std::vector<bool> param_has_protocol_composition;
  std::vector<std::vector<std::string>> param_protocol_composition_lexicographic;
  std::vector<bool> param_has_invalid_protocol_composition;
  bool return_has_generic_suffix = false;
  bool return_has_pointer_declarator = false;
  bool return_has_nullability_suffix = false;
  bool return_has_ownership_qualifier = false;
  bool return_id_spelling = false;
  bool return_class_spelling = false;
  bool return_instancetype_spelling = false;
  bool return_object_pointer_type_spelling = false;
  bool return_has_invalid_generic_suffix = false;
  bool return_has_invalid_pointer_declarator = false;
  bool return_has_invalid_nullability_suffix = false;
  bool return_has_invalid_ownership_qualifier = false;
  bool return_has_invalid_type_suffix = false;
  bool return_ownership_insert_retain = false;
  bool return_ownership_insert_release = false;
  bool return_ownership_insert_autorelease = false;
  bool return_ownership_is_weak_reference = false;
  bool return_ownership_is_unowned_reference = false;
  bool return_ownership_is_unowned_safe_reference = false;
  bool return_ownership_arc_diagnostic_candidate = false;
  bool return_ownership_arc_fixit_available = false;
  std::string return_ownership_arc_diagnostic_profile;
  std::string return_ownership_arc_fixit_hint;
  ValueType return_type = ValueType::I32;
  Objc3SemanticCanonicalType return_canonical_type;
  bool return_is_vector = false;
  std::string return_vector_base_spelling;
  unsigned return_vector_lane_count = 1;
  bool return_has_protocol_composition = false;
  std::vector<std::string> return_protocol_composition_lexicographic;
  bool return_has_invalid_protocol_composition = false;
  bool async_continuation_profile_is_normalized = false;
  bool deterministic_async_continuation_handoff = false;
  std::size_t async_continuation_sites = 0;
  std::size_t async_keyword_sites = 0;
  std::size_t async_function_sites = 0;
  std::size_t continuation_allocation_sites = 0;
  std::size_t continuation_resume_sites = 0;
  std::size_t continuation_suspend_sites = 0;
  std::size_t async_state_machine_sites = 0;
  std::size_t async_continuation_normalized_sites = 0;
  std::size_t async_continuation_gate_blocked_sites = 0;
  std::size_t async_continuation_contract_violation_sites = 0;
  bool actor_isolation_sendability_profile_is_normalized = false;
  bool deterministic_actor_isolation_sendability_handoff = false;
  std::size_t actor_isolation_sendability_sites = 0;
  std::size_t actor_isolation_decl_sites = 0;
  std::size_t actor_hop_sites = 0;
  std::size_t sendable_annotation_sites = 0;
  std::size_t non_sendable_crossing_sites = 0;
  std::size_t isolation_boundary_sites = 0;
  std::size_t actor_isolation_sendability_normalized_sites = 0;
  std::size_t actor_isolation_sendability_gate_blocked_sites = 0;
  std::size_t actor_isolation_sendability_contract_violation_sites = 0;
  bool task_runtime_cancellation_profile_is_normalized = false;
  bool deterministic_task_runtime_cancellation_handoff = false;
  std::size_t task_runtime_interop_sites = 0;
  std::size_t runtime_hook_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  std::size_t suspension_point_sites = 0;
  std::size_t cancellation_propagation_sites = 0;
  std::size_t task_runtime_cancellation_normalized_sites = 0;
  std::size_t task_runtime_cancellation_gate_blocked_sites = 0;
  std::size_t task_runtime_cancellation_contract_violation_sites = 0;
  bool concurrency_replay_race_guard_profile_is_normalized = false;
  bool deterministic_concurrency_replay_race_guard_handoff = false;
  std::size_t concurrency_replay_race_guard_sites = 0;
  std::size_t concurrency_replay_sites = 0;
  std::size_t replay_proof_sites = 0;
  std::size_t race_guard_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t actor_isolation_sites = 0;
  std::size_t deterministic_schedule_sites = 0;
  std::size_t concurrency_replay_guard_blocked_sites = 0;
  std::size_t concurrency_replay_contract_violation_sites = 0;
  bool ns_error_bridging_profile_is_normalized = false;
  bool deterministic_ns_error_bridging_lowering_handoff = false;
  std::size_t ns_error_bridging_sites = 0;
  std::size_t ns_error_parameter_sites = 0;
  std::size_t ns_error_out_parameter_sites = 0;
  std::size_t ns_error_bridge_path_sites = 0;
  std::size_t failable_call_sites = 0;
  std::size_t ns_error_bridging_normalized_sites = 0;
  std::size_t ns_error_bridge_boundary_sites = 0;
  std::size_t ns_error_bridging_contract_violation_sites = 0;
  bool has_definition = false;
  bool is_pure_annotation = false;
};

struct Objc3MethodInfo {
  std::string selector_normalized;
  std::size_t selector_piece_count = 0;
  std::size_t selector_parameter_piece_count = 0;
  bool selector_contract_normalized = false;
  bool selector_had_pieceless_form = false;
  bool selector_has_spelling_mismatch = false;
  bool selector_has_arity_mismatch = false;
  bool selector_has_parameter_linkage_mismatch = false;
  bool selector_has_normalization_flag_mismatch = false;
  bool selector_has_missing_piece_keyword = false;
  std::size_t arity = 0;
  std::vector<ValueType> param_types;
  std::vector<Objc3SemanticCanonicalType> param_canonical_types;
  std::vector<bool> param_is_vector;
  std::vector<std::string> param_vector_base_spelling;
  std::vector<unsigned> param_vector_lane_count;
  std::vector<bool> param_has_generic_suffix;
  std::vector<bool> param_has_pointer_declarator;
  std::vector<bool> param_has_nullability_suffix;
  std::vector<bool> param_has_ownership_qualifier;
  std::vector<bool> param_id_spelling;
  std::vector<bool> param_class_spelling;
  std::vector<bool> param_instancetype_spelling;
  std::vector<bool> param_object_pointer_type_spelling;
  std::vector<bool> param_has_invalid_generic_suffix;
  std::vector<bool> param_has_invalid_pointer_declarator;
  std::vector<bool> param_has_invalid_nullability_suffix;
  std::vector<bool> param_has_invalid_ownership_qualifier;
  std::vector<bool> param_has_invalid_type_suffix;
  std::vector<bool> param_ownership_insert_retain;
  std::vector<bool> param_ownership_insert_release;
  std::vector<bool> param_ownership_insert_autorelease;
  std::vector<bool> param_ownership_is_weak_reference;
  std::vector<bool> param_ownership_is_unowned_reference;
  std::vector<bool> param_ownership_is_unowned_safe_reference;
  std::vector<bool> param_ownership_arc_diagnostic_candidate;
  std::vector<bool> param_ownership_arc_fixit_available;
  std::vector<std::string> param_ownership_arc_diagnostic_profile;
  std::vector<std::string> param_ownership_arc_fixit_hint;
  std::vector<bool> param_has_protocol_composition;
  std::vector<std::vector<std::string>> param_protocol_composition_lexicographic;
  std::vector<bool> param_has_invalid_protocol_composition;
  bool return_has_generic_suffix = false;
  bool return_has_pointer_declarator = false;
  bool return_has_nullability_suffix = false;
  bool return_has_ownership_qualifier = false;
  bool return_id_spelling = false;
  bool return_class_spelling = false;
  bool return_instancetype_spelling = false;
  bool return_object_pointer_type_spelling = false;
  bool return_has_invalid_generic_suffix = false;
  bool return_has_invalid_pointer_declarator = false;
  bool return_has_invalid_nullability_suffix = false;
  bool return_has_invalid_ownership_qualifier = false;
  bool return_has_invalid_type_suffix = false;
  bool return_ownership_insert_retain = false;
  bool return_ownership_insert_release = false;
  bool return_ownership_insert_autorelease = false;
  bool return_ownership_is_weak_reference = false;
  bool return_ownership_is_unowned_reference = false;
  bool return_ownership_is_unowned_safe_reference = false;
  bool return_ownership_arc_diagnostic_candidate = false;
  bool return_ownership_arc_fixit_available = false;
  std::string return_ownership_arc_diagnostic_profile;
  std::string return_ownership_arc_fixit_hint;
  ValueType return_type = ValueType::I32;
  Objc3SemanticCanonicalType return_canonical_type;
  bool return_is_vector = false;
  std::string return_vector_base_spelling;
  unsigned return_vector_lane_count = 1;
  bool return_has_protocol_composition = false;
  std::vector<std::string> return_protocol_composition_lexicographic;
  bool return_has_invalid_protocol_composition = false;
  bool async_continuation_profile_is_normalized = false;
  bool deterministic_async_continuation_handoff = false;
  std::size_t async_continuation_sites = 0;
  std::size_t async_keyword_sites = 0;
  std::size_t async_function_sites = 0;
  std::size_t continuation_allocation_sites = 0;
  std::size_t continuation_resume_sites = 0;
  std::size_t continuation_suspend_sites = 0;
  std::size_t async_state_machine_sites = 0;
  std::size_t async_continuation_normalized_sites = 0;
  std::size_t async_continuation_gate_blocked_sites = 0;
  std::size_t async_continuation_contract_violation_sites = 0;
  bool actor_isolation_sendability_profile_is_normalized = false;
  bool deterministic_actor_isolation_sendability_handoff = false;
  std::size_t actor_isolation_sendability_sites = 0;
  std::size_t actor_isolation_decl_sites = 0;
  std::size_t actor_hop_sites = 0;
  std::size_t sendable_annotation_sites = 0;
  std::size_t non_sendable_crossing_sites = 0;
  std::size_t isolation_boundary_sites = 0;
  std::size_t actor_isolation_sendability_normalized_sites = 0;
  std::size_t actor_isolation_sendability_gate_blocked_sites = 0;
  std::size_t actor_isolation_sendability_contract_violation_sites = 0;
  bool task_runtime_cancellation_profile_is_normalized = false;
  bool deterministic_task_runtime_cancellation_handoff = false;
  std::size_t task_runtime_interop_sites = 0;
  std::size_t runtime_hook_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  std::size_t suspension_point_sites = 0;
  std::size_t cancellation_propagation_sites = 0;
  std::size_t task_runtime_cancellation_normalized_sites = 0;
  std::size_t task_runtime_cancellation_gate_blocked_sites = 0;
  std::size_t task_runtime_cancellation_contract_violation_sites = 0;
  bool concurrency_replay_race_guard_profile_is_normalized = false;
  bool deterministic_concurrency_replay_race_guard_handoff = false;
  std::size_t concurrency_replay_race_guard_sites = 0;
  std::size_t concurrency_replay_sites = 0;
  std::size_t replay_proof_sites = 0;
  std::size_t race_guard_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t actor_isolation_sites = 0;
  std::size_t deterministic_schedule_sites = 0;
  std::size_t concurrency_replay_guard_blocked_sites = 0;
  std::size_t concurrency_replay_contract_violation_sites = 0;
  bool ns_error_bridging_profile_is_normalized = false;
  bool deterministic_ns_error_bridging_lowering_handoff = false;
  std::size_t ns_error_bridging_sites = 0;
  std::size_t ns_error_parameter_sites = 0;
  std::size_t ns_error_out_parameter_sites = 0;
  std::size_t ns_error_bridge_path_sites = 0;
  std::size_t failable_call_sites = 0;
  std::size_t ns_error_bridging_normalized_sites = 0;
  std::size_t ns_error_bridge_boundary_sites = 0;
  std::size_t ns_error_bridging_contract_violation_sites = 0;
  bool objc_direct_declared = false;
  bool objc_final_declared = false;
  bool objc_dynamic_declared = false;
  bool effective_direct_dispatch = false;
  bool is_class_method = false;
  bool has_definition = false;
};

struct Objc3PropertyInfo {
  ValueType type = ValueType::Unknown;
  Objc3SemanticCanonicalType canonical_type;
  bool is_vector = false;
  std::string vector_base_spelling;
  unsigned vector_lane_count = 1;
  bool id_spelling = false;
  bool class_spelling = false;
  bool instancetype_spelling = false;
  bool object_pointer_type_spelling = false;
  bool has_generic_suffix = false;
  bool has_pointer_declarator = false;
  bool has_nullability_suffix = false;
  bool has_ownership_qualifier = false;
  bool has_invalid_generic_suffix = false;
  bool has_invalid_pointer_declarator = false;
  bool has_invalid_nullability_suffix = false;
  bool has_invalid_ownership_qualifier = false;
  bool has_invalid_type_suffix = false;
  bool ownership_insert_retain = false;
  bool ownership_insert_release = false;
  bool ownership_insert_autorelease = false;
  bool ownership_is_weak_reference = false;
  bool ownership_is_unowned_reference = false;
  bool ownership_is_unowned_safe_reference = false;
  bool ownership_arc_diagnostic_candidate = false;
  bool ownership_arc_fixit_available = false;
  std::string ownership_arc_diagnostic_profile;
  std::string ownership_arc_fixit_hint;
  std::string ownership_lifetime_profile;
  std::string ownership_runtime_hook_profile;
  std::size_t attribute_entries = 0;
  std::vector<std::string> attribute_names_lexicographic;
  bool is_readonly = false;
  bool is_readwrite = false;
  bool is_atomic = false;
  bool is_nonatomic = false;
  bool is_copy = false;
  bool is_retain = false;
  bool is_strong = false;
  bool is_weak = false;
  bool is_unowned = false;
  bool is_unsafe_unretained = false;
  bool is_assign = false;
  bool is_nullable = false;
  bool is_nonnull = false;
  bool is_null_resettable = false;
  bool is_class = false;
  bool is_direct = false;
  bool has_getter = false;
  bool has_setter = false;
  std::string getter_selector;
  std::string setter_selector;
  std::string ivar_binding_symbol;
  std::string executable_synthesized_binding_kind;
  std::string executable_synthesized_binding_symbol;
  std::string property_attribute_profile;
  std::string effective_getter_selector;
  bool effective_setter_available = false;
  std::string effective_setter_selector;
  std::string accessor_ownership_profile;
  std::string executable_ivar_layout_symbol;
  std::size_t executable_ivar_layout_slot_index = 0;
  std::size_t executable_ivar_layout_size_bytes = 0;
  std::size_t executable_ivar_layout_alignment_bytes = 0;
  std::size_t executable_ivar_layout_offset_bytes = 0;
  std::size_t executable_ivar_layout_padding_bytes = 0;
  std::size_t executable_ivar_layout_inherited_slot_count = 0;
  std::size_t executable_ivar_layout_inherited_size_bytes = 0;
  std::size_t executable_ivar_layout_owner_size_bytes = 0;
  std::size_t executable_ivar_init_order_index = 0;
  std::size_t executable_ivar_destroy_order_index = 0;
  bool executable_ivar_layout_valid = false;
  std::string executable_ivar_layout_replay_key;
  unsigned line = 1;
  unsigned column = 1;
  std::size_t invalid_attribute_entries = 0;
  std::size_t property_contract_violations = 0;
  bool has_unknown_attribute = false;
  bool has_duplicate_attribute = false;
  bool has_readwrite_conflict = false;
  bool has_atomicity_conflict = false;
  bool has_ownership_conflict = false;
  bool has_weak_unowned_conflict = false;
  bool has_accessor_selector_contract_violation = false;
  bool has_invalid_attribute_contract = false;
};

struct Objc3InterfaceInfo {
  std::string super_name;
  std::vector<std::string> generic_parameter_names_source_order;
  std::vector<std::string> generic_parameter_variance_source_order;
  std::vector<std::string> adopted_protocols_lexicographic;
  bool objc_direct_members_declared = false;
  bool objc_final_declared = false;
  bool objc_sealed_declared = false;
  std::unordered_map<std::string, Objc3PropertyInfo> properties;
  std::unordered_map<std::string, Objc3MethodInfo> methods;
};

struct Objc3ImplementationInfo {
  bool has_matching_interface = false;
  std::unordered_map<std::string, Objc3PropertyInfo> properties;
  std::unordered_map<std::string, Objc3MethodInfo> methods;
};

struct Objc3CategoryMergeInfo {
  std::vector<std::string> category_owner_identities_in_merge_order;
  std::unordered_map<std::string, Objc3PropertyInfo> merged_properties;
  std::unordered_map<std::string, std::string>
      merged_property_owner_identities;
  std::unordered_map<std::string, Objc3MethodInfo> merged_methods;
  std::unordered_map<std::string, std::string> merged_method_owner_identities;
  bool deterministic = true;
};

struct Objc3InterfaceImplementationSummary {
  std::size_t declared_interfaces = 0;
  std::size_t resolved_interfaces = 0;
  std::size_t declared_implementations = 0;
  std::size_t resolved_implementations = 0;
  std::size_t interface_method_symbols = 0;
  std::size_t implementation_method_symbols = 0;
  std::size_t linked_implementation_symbols = 0;
  bool deterministic = true;
};

#include "sema/objc3_sema_contract_bootstrap_compatibility.h"

struct Objc3SemanticIntegrationSurface {
  std::unordered_map<std::string, ValueType> globals;
  std::unordered_map<std::string, FunctionInfo> functions;
  // diagnostic precision anchor: these maps model class containers
  // only. Category containers are validated separately so valid
  // class-plus-category programs do not collapse into duplicate class-owner
  // diagnostics before runtime metadata conflict analysis runs.
  // binary-boundary anchor: the executable metadata binary envelope
  // must consume the canonical sema-owned class/category split without
  // rebuilding container ownership from manifest-only heuristics.
  std::unordered_map<std::string, Objc3InterfaceInfo> interfaces;
  std::unordered_map<std::string, Objc3ImplementationInfo> implementations;
  std::unordered_map<std::string, Objc3InterfaceInfo> category_interfaces;
  std::unordered_map<std::string, Objc3ImplementationInfo>
      category_implementations;
  std::unordered_map<std::string, Objc3CategoryMergeInfo>
      category_merge_surfaces;
  Objc3InterfaceImplementationSummary interface_implementation_summary;
  Objc3BootstrapLegalityFailureContractSummary
      bootstrap_legality_failure_contract_summary;
  Objc3BootstrapLegalitySemanticsSummary
      bootstrap_legality_semantics_summary;
  Objc3BootstrapFailureRestartSemanticsSummary
      bootstrap_failure_restart_semantics_summary;
  Objc3CompatibilityStrictnessClaimSemanticsSummary
      compatibility_strictness_claim_semantics_summary;
  Objc3ProtocolCategoryCompositionSummary protocol_category_composition_summary;
  Objc3ClassProtocolCategoryLinkingSummary class_protocol_category_linking_summary;
  Objc3SelectorNormalizationSummary selector_normalization_summary;
  Objc3PropertyAttributeSummary property_attribute_summary;
  Objc3TypeAnnotationSurfaceSummary type_annotation_surface_summary;
  Objc3LightweightGenericConstraintSummary lightweight_generic_constraint_summary;
  Objc3NullabilityFlowWarningPrecisionSummary nullability_flow_warning_precision_summary;
  Objc3ProtocolQualifiedObjectTypeSummary protocol_qualified_object_type_summary;
  Objc3VarianceBridgeCastSummary variance_bridge_cast_summary;
  Objc3GenericMetadataAbiSummary generic_metadata_abi_summary;
  Objc3ModuleImportGraphSummary module_import_graph_summary;
  Objc3NamespaceCollisionShadowingSummary namespace_collision_shadowing_summary;
  Objc3PublicPrivateApiPartitionSummary public_private_api_partition_summary;
  Objc3IncrementalModuleCacheInvalidationSummary incremental_module_cache_invalidation_summary;
  Objc3CrossModuleConformanceSummary cross_module_conformance_summary;
  Objc3ThrowsPropagationSummary throws_propagation_summary;
  Objc3UnwindCleanupSummary unwind_cleanup_summary;
  Objc3AsyncContinuationSummary async_continuation_summary;
  Objc3AwaitLoweringSuspensionStateSummary await_lowering_suspension_state_lowering_summary;
  Objc3ActorIsolationSendabilitySummary actor_isolation_sendability_summary;
  Objc3TaskRuntimeCancellationSummary task_runtime_cancellation_summary;
  Objc3ConcurrencyReplayRaceGuardSummary concurrency_replay_race_guard_summary;
  Objc3UnsafePointerExtensionSummary unsafe_pointer_extension_summary;
  Objc3InlineAsmIntrinsicGovernanceSummary inline_asm_intrinsic_governance_summary;
  Objc3NSErrorBridgingSummary ns_error_bridging_summary;
  Objc3ResultLikeLoweringSummary result_like_lowering_summary;
  Objc3ErrorDiagnosticsRecoverySummary error_diagnostics_recovery_summary;
  Objc3SymbolGraphScopeResolutionSummary symbol_graph_scope_resolution_summary;
  Objc3MethodLookupOverrideConflictSummary method_lookup_override_conflict_summary;
  Objc3PropertySynthesisIvarBindingSummary property_synthesis_ivar_binding_summary;
  Objc3IdClassSelObjectPointerTypeCheckingSummary id_class_sel_object_pointer_type_checking_summary;
  std::vector<Objc3BlockLiteralCaptureSiteMetadata> block_literal_capture_sites_lexicographic;
  Objc3BlockLiteralCaptureSemanticsSummary block_literal_capture_semantics_summary;
  std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> block_abi_invoke_trampoline_sites_lexicographic;
  Objc3BlockAbiInvokeTrampolineSemanticsSummary block_abi_invoke_trampoline_semantics_summary;
  std::vector<Objc3BlockStorageEscapeSiteMetadata> block_storage_escape_sites_lexicographic;
  Objc3BlockStorageEscapeSemanticsSummary block_storage_escape_semantics_summary;
  std::vector<Objc3BlockCopyDisposeSiteMetadata> block_copy_dispose_sites_lexicographic;
  Objc3BlockCopyDisposeSemanticsSummary block_copy_dispose_semantics_summary;
  std::vector<Objc3BlockDeterminismPerfBaselineSiteMetadata> block_determinism_perf_baseline_sites_lexicographic;
  Objc3BlockDeterminismPerfBaselineSummary block_determinism_perf_baseline_summary;
  std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> message_send_selector_lowering_sites_lexicographic;
  Objc3MessageSendSelectorLoweringSummary message_send_selector_lowering_summary;
  Objc3DispatchAbiMarshallingSummary dispatch_abi_marshalling_summary;
  Objc3NilReceiverSemanticsFoldabilitySummary nil_receiver_semantics_foldability_summary;
  Objc3SuperDispatchMethodFamilySummary super_dispatch_method_family_summary;
  Objc3RuntimeLinkHostLinkSummary runtime_link_host_link_summary;
  Objc3RetainReleaseOperationSummary retain_release_operation_summary;
  Objc3WeakUnownedSemanticsSummary weak_unowned_semantics_summary;
  Objc3ArcDiagnosticsFixitSummary arc_diagnostics_fixit_summary;
  std::vector<Objc3AutoreleasePoolScopeSiteMetadata> autoreleasepool_scope_sites_lexicographic;
  Objc3AutoreleasePoolScopeSummary autoreleasepool_scope_summary;
  bool built = false;
};

struct Objc3SemanticFunctionTypeMetadata {
  std::string name;
  std::size_t arity = 0;
  std::vector<ValueType> param_types;
  std::vector<Objc3SemanticCanonicalType> param_canonical_types;
  std::vector<bool> param_is_vector;
  std::vector<std::string> param_vector_base_spelling;
  std::vector<unsigned> param_vector_lane_count;
  std::vector<bool> param_has_generic_suffix;
  std::vector<bool> param_has_pointer_declarator;
  std::vector<bool> param_has_nullability_suffix;
  std::vector<bool> param_has_ownership_qualifier;
  std::vector<bool> param_id_spelling;
  std::vector<bool> param_class_spelling;
  std::vector<bool> param_instancetype_spelling;
  std::vector<bool> param_object_pointer_type_spelling;
  std::vector<bool> param_has_invalid_generic_suffix;
  std::vector<bool> param_has_invalid_pointer_declarator;
  std::vector<bool> param_has_invalid_nullability_suffix;
  std::vector<bool> param_has_invalid_ownership_qualifier;
  std::vector<bool> param_has_invalid_type_suffix;
  std::vector<bool> param_ownership_insert_retain;
  std::vector<bool> param_ownership_insert_release;
  std::vector<bool> param_ownership_insert_autorelease;
  std::vector<bool> param_ownership_is_weak_reference;
  std::vector<bool> param_ownership_is_unowned_reference;
  std::vector<bool> param_ownership_is_unowned_safe_reference;
  std::vector<bool> param_ownership_arc_diagnostic_candidate;
  std::vector<bool> param_ownership_arc_fixit_available;
  std::vector<std::string> param_ownership_arc_diagnostic_profile;
  std::vector<std::string> param_ownership_arc_fixit_hint;
  std::vector<bool> param_has_protocol_composition;
  std::vector<std::vector<std::string>> param_protocol_composition_lexicographic;
  std::vector<bool> param_has_invalid_protocol_composition;
  bool return_has_generic_suffix = false;
  bool return_has_pointer_declarator = false;
  bool return_has_nullability_suffix = false;
  bool return_has_ownership_qualifier = false;
  bool return_id_spelling = false;
  bool return_class_spelling = false;
  bool return_instancetype_spelling = false;
  bool return_object_pointer_type_spelling = false;
  bool return_has_invalid_generic_suffix = false;
  bool return_has_invalid_pointer_declarator = false;
  bool return_has_invalid_nullability_suffix = false;
  bool return_has_invalid_ownership_qualifier = false;
  bool return_has_invalid_type_suffix = false;
  bool return_ownership_insert_retain = false;
  bool return_ownership_insert_release = false;
  bool return_ownership_insert_autorelease = false;
  bool return_ownership_is_weak_reference = false;
  bool return_ownership_is_unowned_reference = false;
  bool return_ownership_is_unowned_safe_reference = false;
  bool return_ownership_arc_diagnostic_candidate = false;
  bool return_ownership_arc_fixit_available = false;
  std::string return_ownership_arc_diagnostic_profile;
  std::string return_ownership_arc_fixit_hint;
  ValueType return_type = ValueType::I32;
  Objc3SemanticCanonicalType return_canonical_type;
  bool return_is_vector = false;
  std::string return_vector_base_spelling;
  unsigned return_vector_lane_count = 1;
  bool return_has_protocol_composition = false;
  std::vector<std::string> return_protocol_composition_lexicographic;
  bool return_has_invalid_protocol_composition = false;
  bool async_continuation_profile_is_normalized = false;
  bool deterministic_async_continuation_handoff = false;
  std::size_t async_continuation_sites = 0;
  std::size_t async_keyword_sites = 0;
  std::size_t async_function_sites = 0;
  std::size_t continuation_allocation_sites = 0;
  std::size_t continuation_resume_sites = 0;
  std::size_t continuation_suspend_sites = 0;
  std::size_t async_state_machine_sites = 0;
  std::size_t async_continuation_normalized_sites = 0;
  std::size_t async_continuation_gate_blocked_sites = 0;
  std::size_t async_continuation_contract_violation_sites = 0;
  bool actor_isolation_sendability_profile_is_normalized = false;
  bool deterministic_actor_isolation_sendability_handoff = false;
  std::size_t actor_isolation_sendability_sites = 0;
  std::size_t actor_isolation_decl_sites = 0;
  std::size_t actor_hop_sites = 0;
  std::size_t sendable_annotation_sites = 0;
  std::size_t non_sendable_crossing_sites = 0;
  std::size_t isolation_boundary_sites = 0;
  std::size_t actor_isolation_sendability_normalized_sites = 0;
  std::size_t actor_isolation_sendability_gate_blocked_sites = 0;
  std::size_t actor_isolation_sendability_contract_violation_sites = 0;
  bool task_runtime_cancellation_profile_is_normalized = false;
  bool deterministic_task_runtime_cancellation_handoff = false;
  std::size_t task_runtime_interop_sites = 0;
  std::size_t runtime_hook_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  std::size_t suspension_point_sites = 0;
  std::size_t cancellation_propagation_sites = 0;
  std::size_t task_runtime_cancellation_normalized_sites = 0;
  std::size_t task_runtime_cancellation_gate_blocked_sites = 0;
  std::size_t task_runtime_cancellation_contract_violation_sites = 0;
  bool concurrency_replay_race_guard_profile_is_normalized = false;
  bool deterministic_concurrency_replay_race_guard_handoff = false;
  std::size_t concurrency_replay_race_guard_sites = 0;
  std::size_t concurrency_replay_sites = 0;
  std::size_t replay_proof_sites = 0;
  std::size_t race_guard_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t actor_isolation_sites = 0;
  std::size_t deterministic_schedule_sites = 0;
  std::size_t concurrency_replay_guard_blocked_sites = 0;
  std::size_t concurrency_replay_contract_violation_sites = 0;
  bool ns_error_bridging_profile_is_normalized = false;
  bool deterministic_ns_error_bridging_lowering_handoff = false;
  std::size_t ns_error_bridging_sites = 0;
  std::size_t ns_error_parameter_sites = 0;
  std::size_t ns_error_out_parameter_sites = 0;
  std::size_t ns_error_bridge_path_sites = 0;
  std::size_t failable_call_sites = 0;
  std::size_t ns_error_bridging_normalized_sites = 0;
  std::size_t ns_error_bridge_boundary_sites = 0;
  std::size_t ns_error_bridging_contract_violation_sites = 0;
  bool has_definition = false;
  bool is_pure_annotation = false;
};

struct Objc3SemanticMethodTypeMetadata {
  std::string selector;
  std::string selector_normalized;
  std::size_t selector_piece_count = 0;
  std::size_t selector_parameter_piece_count = 0;
  bool selector_contract_normalized = false;
  bool selector_had_pieceless_form = false;
  bool selector_has_spelling_mismatch = false;
  bool selector_has_arity_mismatch = false;
  bool selector_has_parameter_linkage_mismatch = false;
  bool selector_has_normalization_flag_mismatch = false;
  bool selector_has_missing_piece_keyword = false;
  std::size_t arity = 0;
  std::vector<ValueType> param_types;
  std::vector<Objc3SemanticCanonicalType> param_canonical_types;
  std::vector<bool> param_is_vector;
  std::vector<std::string> param_vector_base_spelling;
  std::vector<unsigned> param_vector_lane_count;
  std::vector<bool> param_has_generic_suffix;
  std::vector<bool> param_has_pointer_declarator;
  std::vector<bool> param_has_nullability_suffix;
  std::vector<bool> param_has_ownership_qualifier;
  std::vector<bool> param_id_spelling;
  std::vector<bool> param_class_spelling;
  std::vector<bool> param_instancetype_spelling;
  std::vector<bool> param_object_pointer_type_spelling;
  std::vector<bool> param_has_invalid_generic_suffix;
  std::vector<bool> param_has_invalid_pointer_declarator;
  std::vector<bool> param_has_invalid_nullability_suffix;
  std::vector<bool> param_has_invalid_ownership_qualifier;
  std::vector<bool> param_has_invalid_type_suffix;
  std::vector<bool> param_ownership_insert_retain;
  std::vector<bool> param_ownership_insert_release;
  std::vector<bool> param_ownership_insert_autorelease;
  std::vector<bool> param_ownership_is_weak_reference;
  std::vector<bool> param_ownership_is_unowned_reference;
  std::vector<bool> param_ownership_is_unowned_safe_reference;
  std::vector<bool> param_ownership_arc_diagnostic_candidate;
  std::vector<bool> param_ownership_arc_fixit_available;
  std::vector<std::string> param_ownership_arc_diagnostic_profile;
  std::vector<std::string> param_ownership_arc_fixit_hint;
  std::vector<bool> param_has_protocol_composition;
  std::vector<std::vector<std::string>> param_protocol_composition_lexicographic;
  std::vector<bool> param_has_invalid_protocol_composition;
  bool return_has_generic_suffix = false;
  bool return_has_pointer_declarator = false;
  bool return_has_nullability_suffix = false;
  bool return_has_ownership_qualifier = false;
  bool return_id_spelling = false;
  bool return_class_spelling = false;
  bool return_instancetype_spelling = false;
  bool return_object_pointer_type_spelling = false;
  bool return_has_invalid_generic_suffix = false;
  bool return_has_invalid_pointer_declarator = false;
  bool return_has_invalid_nullability_suffix = false;
  bool return_has_invalid_ownership_qualifier = false;
  bool return_has_invalid_type_suffix = false;
  bool return_ownership_insert_retain = false;
  bool return_ownership_insert_release = false;
  bool return_ownership_insert_autorelease = false;
  bool return_ownership_is_weak_reference = false;
  bool return_ownership_is_unowned_reference = false;
  bool return_ownership_is_unowned_safe_reference = false;
  bool return_ownership_arc_diagnostic_candidate = false;
  bool return_ownership_arc_fixit_available = false;
  std::string return_ownership_arc_diagnostic_profile;
  std::string return_ownership_arc_fixit_hint;
  ValueType return_type = ValueType::I32;
  Objc3SemanticCanonicalType return_canonical_type;
  bool return_is_vector = false;
  std::string return_vector_base_spelling;
  unsigned return_vector_lane_count = 1;
  bool return_has_protocol_composition = false;
  std::vector<std::string> return_protocol_composition_lexicographic;
  bool return_has_invalid_protocol_composition = false;
  bool async_continuation_profile_is_normalized = false;
  bool deterministic_async_continuation_handoff = false;
  std::size_t async_continuation_sites = 0;
  std::size_t async_keyword_sites = 0;
  std::size_t async_function_sites = 0;
  std::size_t continuation_allocation_sites = 0;
  std::size_t continuation_resume_sites = 0;
  std::size_t continuation_suspend_sites = 0;
  std::size_t async_state_machine_sites = 0;
  std::size_t async_continuation_normalized_sites = 0;
  std::size_t async_continuation_gate_blocked_sites = 0;
  std::size_t async_continuation_contract_violation_sites = 0;
  bool actor_isolation_sendability_profile_is_normalized = false;
  bool deterministic_actor_isolation_sendability_handoff = false;
  std::size_t actor_isolation_sendability_sites = 0;
  std::size_t actor_isolation_decl_sites = 0;
  std::size_t actor_hop_sites = 0;
  std::size_t sendable_annotation_sites = 0;
  std::size_t non_sendable_crossing_sites = 0;
  std::size_t isolation_boundary_sites = 0;
  std::size_t actor_isolation_sendability_normalized_sites = 0;
  std::size_t actor_isolation_sendability_gate_blocked_sites = 0;
  std::size_t actor_isolation_sendability_contract_violation_sites = 0;
  bool task_runtime_cancellation_profile_is_normalized = false;
  bool deterministic_task_runtime_cancellation_handoff = false;
  std::size_t task_runtime_interop_sites = 0;
  std::size_t runtime_hook_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  std::size_t suspension_point_sites = 0;
  std::size_t cancellation_propagation_sites = 0;
  std::size_t task_runtime_cancellation_normalized_sites = 0;
  std::size_t task_runtime_cancellation_gate_blocked_sites = 0;
  std::size_t task_runtime_cancellation_contract_violation_sites = 0;
  bool concurrency_replay_race_guard_profile_is_normalized = false;
  bool deterministic_concurrency_replay_race_guard_handoff = false;
  std::size_t concurrency_replay_race_guard_sites = 0;
  std::size_t concurrency_replay_sites = 0;
  std::size_t replay_proof_sites = 0;
  std::size_t race_guard_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t actor_isolation_sites = 0;
  std::size_t deterministic_schedule_sites = 0;
  std::size_t concurrency_replay_guard_blocked_sites = 0;
  std::size_t concurrency_replay_contract_violation_sites = 0;
  bool ns_error_bridging_profile_is_normalized = false;
  bool deterministic_ns_error_bridging_lowering_handoff = false;
  std::size_t ns_error_bridging_sites = 0;
  std::size_t ns_error_parameter_sites = 0;
  std::size_t ns_error_out_parameter_sites = 0;
  std::size_t ns_error_bridge_path_sites = 0;
  std::size_t failable_call_sites = 0;
  std::size_t ns_error_bridging_normalized_sites = 0;
  std::size_t ns_error_bridge_boundary_sites = 0;
  std::size_t ns_error_bridging_contract_violation_sites = 0;
  bool is_class_method = false;
  bool has_definition = false;
};

struct Objc3SemanticPropertyTypeMetadata {
  std::string name;
  ValueType type = ValueType::Unknown;
  Objc3SemanticCanonicalType canonical_type;
  bool is_vector = false;
  std::string vector_base_spelling;
  unsigned vector_lane_count = 1;
  bool id_spelling = false;
  bool class_spelling = false;
  bool instancetype_spelling = false;
  bool object_pointer_type_spelling = false;
  bool has_generic_suffix = false;
  bool has_pointer_declarator = false;
  bool has_nullability_suffix = false;
  bool has_ownership_qualifier = false;
  bool has_invalid_generic_suffix = false;
  bool has_invalid_pointer_declarator = false;
  bool has_invalid_nullability_suffix = false;
  bool has_invalid_ownership_qualifier = false;
  bool has_invalid_type_suffix = false;
  bool ownership_insert_retain = false;
  bool ownership_insert_release = false;
  bool ownership_insert_autorelease = false;
  bool ownership_is_weak_reference = false;
  bool ownership_is_unowned_reference = false;
  bool ownership_is_unowned_safe_reference = false;
  bool ownership_arc_diagnostic_candidate = false;
  bool ownership_arc_fixit_available = false;
  std::string ownership_arc_diagnostic_profile;
  std::string ownership_arc_fixit_hint;
  std::string ownership_lifetime_profile;
  std::string ownership_runtime_hook_profile;
  std::size_t attribute_entries = 0;
  std::vector<std::string> attribute_names_lexicographic;
  bool is_readonly = false;
  bool is_readwrite = false;
  bool is_atomic = false;
  bool is_nonatomic = false;
  bool is_copy = false;
  bool is_retain = false;
  bool is_strong = false;
  bool is_weak = false;
  bool is_unowned = false;
  bool is_unsafe_unretained = false;
  bool is_assign = false;
  bool is_nullable = false;
  bool is_nonnull = false;
  bool is_null_resettable = false;
  bool is_class = false;
  bool is_direct = false;
  bool has_getter = false;
  bool has_setter = false;
  std::string getter_selector;
  std::string setter_selector;
  std::string ivar_binding_symbol;
  std::string executable_synthesized_binding_kind;
  std::string executable_synthesized_binding_symbol;
  std::string property_attribute_profile;
  std::string effective_getter_selector;
  bool effective_setter_available = false;
  std::string effective_setter_selector;
  std::string accessor_ownership_profile;
  std::string executable_ivar_layout_symbol;
  std::size_t executable_ivar_layout_slot_index = 0;
  std::size_t executable_ivar_layout_size_bytes = 0;
  std::size_t executable_ivar_layout_alignment_bytes = 0;
  std::size_t executable_ivar_layout_offset_bytes = 0;
  std::size_t executable_ivar_layout_padding_bytes = 0;
  std::size_t executable_ivar_layout_inherited_slot_count = 0;
  std::size_t executable_ivar_layout_inherited_size_bytes = 0;
  std::size_t executable_ivar_layout_owner_size_bytes = 0;
  std::size_t executable_ivar_init_order_index = 0;
  std::size_t executable_ivar_destroy_order_index = 0;
  bool executable_ivar_layout_valid = false;
  std::string executable_ivar_layout_replay_key;
  std::size_t invalid_attribute_entries = 0;
  std::size_t property_contract_violations = 0;
  bool has_unknown_attribute = false;
  bool has_duplicate_attribute = false;
  bool has_readwrite_conflict = false;
  bool has_atomicity_conflict = false;
  bool has_ownership_conflict = false;
  bool has_weak_unowned_conflict = false;
  bool has_accessor_selector_contract_violation = false;
  bool has_invalid_attribute_contract = false;
};

struct Objc3SemanticInterfaceTypeMetadata {
  std::string name;
  std::string super_name;
  std::vector<std::string> generic_parameter_names_source_order;
  std::vector<std::string> generic_parameter_variance_source_order;
  std::vector<std::string> adopted_protocols_lexicographic;
  std::vector<Objc3SemanticPropertyTypeMetadata> properties_lexicographic;
  std::vector<Objc3SemanticMethodTypeMetadata> methods_lexicographic;
};

struct Objc3SemanticImplementationTypeMetadata {
  std::string name;
  bool has_matching_interface = false;
  std::vector<Objc3SemanticPropertyTypeMetadata> properties_lexicographic;
  std::vector<Objc3SemanticMethodTypeMetadata> methods_lexicographic;
};

// lowering-handoff anchor: this typed metadata handoff is the
// canonical sema-to-lowering schema input for executable metadata graph
// lowering freeze packets and must remain deterministic and replayable.
// typed-lowering anchor: the concrete lowering-ready metadata graph
// packet consumes this same deterministic sema metadata surface so the typed
// handoff stays schema-stable between manifest publication and later lowering.
// debug-projection anchor: the manifest/IR inspection matrix replays
// this same typed sema surface so operators inspect one deterministic schema
// before runtime section emission lands.
// runtime-ingest packaging anchor: the manifest packaging boundary
// must carry this same typed sema surface forward verbatim, so runtime ingest
// packaging never invents a second schema between lane-C publication and
// later section emission/startup registration.
struct Objc3SemanticTypeMetadataHandoff {
  std::vector<std::string> global_names_lexicographic;
  std::vector<Objc3SemanticFunctionTypeMetadata> functions_lexicographic;
  std::vector<Objc3SemanticInterfaceTypeMetadata> interfaces_lexicographic;
  std::vector<Objc3SemanticImplementationTypeMetadata> implementations_lexicographic;
  Objc3InterfaceImplementationSummary interface_implementation_summary;
  Objc3ProtocolCategoryCompositionSummary protocol_category_composition_summary;
  Objc3ClassProtocolCategoryLinkingSummary class_protocol_category_linking_summary;
  Objc3SelectorNormalizationSummary selector_normalization_summary;
  Objc3PropertyAttributeSummary property_attribute_summary;
  Objc3TypeAnnotationSurfaceSummary type_annotation_surface_summary;
  Objc3LightweightGenericConstraintSummary lightweight_generic_constraint_summary;
  Objc3NullabilityFlowWarningPrecisionSummary nullability_flow_warning_precision_summary;
  Objc3ProtocolQualifiedObjectTypeSummary protocol_qualified_object_type_summary;
  Objc3VarianceBridgeCastSummary variance_bridge_cast_summary;
  Objc3GenericMetadataAbiSummary generic_metadata_abi_summary;
  Objc3ModuleImportGraphSummary module_import_graph_summary;
  Objc3NamespaceCollisionShadowingSummary namespace_collision_shadowing_summary;
  Objc3PublicPrivateApiPartitionSummary public_private_api_partition_summary;
  Objc3IncrementalModuleCacheInvalidationSummary incremental_module_cache_invalidation_summary;
  Objc3CrossModuleConformanceSummary cross_module_conformance_summary;
  Objc3ThrowsPropagationSummary throws_propagation_summary;
  Objc3UnwindCleanupSummary unwind_cleanup_summary;
  Objc3AsyncContinuationSummary async_continuation_summary;
  Objc3AwaitLoweringSuspensionStateSummary await_lowering_suspension_state_lowering_summary;
  Objc3ActorIsolationSendabilitySummary actor_isolation_sendability_summary;
  Objc3TaskRuntimeCancellationSummary task_runtime_cancellation_summary;
  Objc3ConcurrencyReplayRaceGuardSummary concurrency_replay_race_guard_summary;
  Objc3UnsafePointerExtensionSummary unsafe_pointer_extension_summary;
  Objc3InlineAsmIntrinsicGovernanceSummary inline_asm_intrinsic_governance_summary;
  Objc3NSErrorBridgingSummary ns_error_bridging_summary;
  Objc3ResultLikeLoweringSummary result_like_lowering_summary;
  Objc3ErrorDiagnosticsRecoverySummary error_diagnostics_recovery_summary;
  Objc3SymbolGraphScopeResolutionSummary symbol_graph_scope_resolution_summary;
  Objc3MethodLookupOverrideConflictSummary method_lookup_override_conflict_summary;
  Objc3PropertySynthesisIvarBindingSummary property_synthesis_ivar_binding_summary;
  Objc3IdClassSelObjectPointerTypeCheckingSummary id_class_sel_object_pointer_type_checking_summary;
  std::vector<Objc3BlockLiteralCaptureSiteMetadata> block_literal_capture_sites_lexicographic;
  Objc3BlockLiteralCaptureSemanticsSummary block_literal_capture_semantics_summary;
  std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> block_abi_invoke_trampoline_sites_lexicographic;
  Objc3BlockAbiInvokeTrampolineSemanticsSummary block_abi_invoke_trampoline_semantics_summary;
  std::vector<Objc3BlockStorageEscapeSiteMetadata> block_storage_escape_sites_lexicographic;
  Objc3BlockStorageEscapeSemanticsSummary block_storage_escape_semantics_summary;
  std::vector<Objc3BlockCopyDisposeSiteMetadata> block_copy_dispose_sites_lexicographic;
  Objc3BlockCopyDisposeSemanticsSummary block_copy_dispose_semantics_summary;
  std::vector<Objc3BlockDeterminismPerfBaselineSiteMetadata> block_determinism_perf_baseline_sites_lexicographic;
  Objc3BlockDeterminismPerfBaselineSummary block_determinism_perf_baseline_summary;
  std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> message_send_selector_lowering_sites_lexicographic;
  Objc3MessageSendSelectorLoweringSummary message_send_selector_lowering_summary;
  Objc3DispatchAbiMarshallingSummary dispatch_abi_marshalling_summary;
  Objc3NilReceiverSemanticsFoldabilitySummary nil_receiver_semantics_foldability_summary;
  Objc3SuperDispatchMethodFamilySummary super_dispatch_method_family_summary;
  Objc3RuntimeLinkHostLinkSummary runtime_link_host_link_summary;
  Objc3RetainReleaseOperationSummary retain_release_operation_summary;
  Objc3WeakUnownedSemanticsSummary weak_unowned_semantics_summary;
  Objc3ArcDiagnosticsFixitSummary arc_diagnostics_fixit_summary;
  std::vector<Objc3AutoreleasePoolScopeSiteMetadata> autoreleasepool_scope_sites_lexicographic;
  Objc3AutoreleasePoolScopeSummary autoreleasepool_scope_summary;
};

struct Objc3SemanticValidationOptions {
  std::size_t max_message_send_args = 4;
  bool allow_source_only_block_literals = false;
  bool allow_source_only_defer_statements = false;
  bool allow_source_only_error_runtime_surface = false;
  bool arc_mode_enabled = false;
};

Objc3SemanticTypeMetadataHandoff BuildSemanticTypeMetadataHandoff(const Objc3SemanticIntegrationSurface &surface);
bool IsDeterministicSemanticTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff);
