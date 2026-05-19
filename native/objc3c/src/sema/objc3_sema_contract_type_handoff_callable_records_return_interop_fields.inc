  ValueType return_type = ValueType::I32;
  Objc3SemanticCanonicalType return_canonical_type;
  bool return_is_vector = false;
  std::string return_vector_base_spelling;
  unsigned return_vector_lane_count = 1;
  bool return_has_protocol_composition = false;
  std::vector<std::string> return_protocol_composition_lexicographic;
  bool return_has_invalid_protocol_composition = false;
