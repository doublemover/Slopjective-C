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
