struct Objc3FrontendTypeSystemSemanticModelRecord {
  std::string contract_id = kObjc3FrontendTypeSystemTypeSemanticModelContractId;
  std::size_t optional_binding_sites = 0;
  std::size_t optional_binding_clause_sites = 0;
  std::size_t optional_send_sites = 0;
  std::size_t nil_coalescing_sites = 0;
  std::size_t typed_keypath_literal_sites = 0;
  std::size_t typed_keypath_self_root_sites = 0;
  std::size_t typed_keypath_class_root_sites = 0;
  std::size_t canonical_type_entries = 0;
  std::size_t canonical_object_type_entries = 0;
  std::size_t canonical_nullable_entries = 0;
  std::size_t canonical_nonnull_entries = 0;
  std::size_t canonical_implicitly_unwrapped_entries = 0;
  std::size_t canonical_null_resettable_entries = 0;
  std::size_t canonical_unspecified_nullability_entries = 0;
  std::size_t canonical_invalid_type_entries = 0;
  std::size_t optional_binding_contract_violation_sites = 0;
  std::size_t optional_send_contract_violation_sites = 0;
  std::size_t optional_flow_contract_violation_sites = 0;
  bool deterministic = true;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
};

struct Objc3FrontendTypeSystemOptionalKeypathLoweringContractRecord {
  std::size_t optional_binding_sites = 0;
  std::size_t optional_binding_clause_sites = 0;
  std::size_t optional_send_sites = 0;
  std::size_t nil_coalescing_sites = 0;
  std::size_t typed_keypath_literal_sites = 0;
  std::size_t typed_keypath_self_root_sites = 0;
  std::size_t typed_keypath_class_root_sites = 0;
  std::size_t live_optional_lowering_sites = 0;
  std::size_t single_evaluation_nil_short_circuit_sites = 0;
  std::size_t live_typed_keypath_artifact_sites = 0;
  std::size_t deferred_typed_keypath_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3FrontendLightweightGenericsConstraintLoweringContractRecord {
  std::size_t generic_constraint_sites = 0;
  std::size_t generic_suffix_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t terminated_generic_suffix_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_constraint_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3FrontendNullabilityFlowWarningPrecisionLoweringContractRecord {
  std::size_t nullability_flow_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t nullability_suffix_sites = 0;
  std::size_t nullable_suffix_sites = 0;
  std::size_t nonnull_suffix_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3FrontendProtocolQualifiedObjectTypeLoweringContractRecord {
  std::size_t protocol_qualified_object_type_sites = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t terminated_protocol_composition_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_protocol_composition_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3FrontendVarianceBridgeCastLoweringContractRecord {
  std::size_t variance_bridge_cast_sites = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t ownership_qualifier_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3FrontendGenericMetadataAbiLoweringContractRecord {
  std::size_t generic_metadata_abi_sites = 0;
  std::size_t generic_suffix_sites = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t ownership_qualifier_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};
