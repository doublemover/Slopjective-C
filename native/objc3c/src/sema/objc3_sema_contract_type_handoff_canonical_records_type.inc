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
  bool object_type_facts_authoritative = false;
  bool is_objc_object_reference = false;
  bool is_objc_id_object = false;
  bool is_objc_class_object = false;
  bool is_objc_metaclass_object = false;
  bool is_objc_protocol_object = false;
  bool is_objc_instancetype_object = false;
  bool is_objc_named_object_pointer = false;
  bool has_protocol_composition = false;
  std::vector<std::string> protocol_composition_lexicographic;
  bool protocol_composition_facts_authoritative = false;
  bool has_invalid_protocol_composition = false;
  bool has_generic_suffix = false;
  std::vector<std::string> generic_arguments_source_order;
  std::vector<std::string> generic_arguments_lexicographic;
  bool generic_arguments_facts_authoritative = false;
  bool has_invalid_generic_suffix = false;
  bool is_value_optional = false;
  std::string value_optional_payload_type_spelling;
  ValueType value_optional_payload_value_type = ValueType::Unknown;
  bool value_optional_semantic_value_model_supported = false;
  bool value_optional_stable_abi_layout_contract_supported = false;
  bool value_optional_binding_narrowing_supported = false;
  bool value_optional_interface_roundtrip_supported = false;
  bool value_optional_unwrap_requires_presence_check = true;
  bool value_optional_runtime_execution_supported = false;
  bool value_optional_lowering_supported = false;
  bool has_explicit_nullability = false;
  bool nullability_facts_authoritative = false;
  bool has_invalid_nullability_suffix = false;
  bool has_invalid_type_suffix = false;
  bool deterministic = true;
  std::string canonical_spelling;
  std::string replay_key;
};
