bool TryParseVectorTypeSpelling(const Objc3LexToken &type_token,
                                ValueType &vector_type,
                                std::string &vector_base_spelling,
                                unsigned &vector_lane_count) {
  const std::string &text = type_token.text;
  const bool is_i32_vector = text.rfind("i32x", 0) == 0;
  const bool is_bool_vector = text.rfind("boolx", 0) == 0;
  if (!is_i32_vector && !is_bool_vector) {
    return false;
  }

  const std::size_t prefix_length = is_i32_vector ? 4u : 5u;
  if (text.size() <= prefix_length) {
    return false;
  }

  unsigned lane_count = 0;
  for (std::size_t i = prefix_length; i < text.size(); ++i) {
    const char c = text[i];
    if (!std::isdigit(static_cast<unsigned char>(c))) {
      return false;
    }
    lane_count = (lane_count * 10u) + static_cast<unsigned>(c - '0');
    if (lane_count > 1024u) {
      return false;
    }
  }

  if (lane_count != 2u && lane_count != 4u && lane_count != 8u &&
      lane_count != 16u) {
    return false;
  }

  vector_type = is_i32_vector ? ValueType::I32 : ValueType::Bool;
  vector_base_spelling = is_i32_vector ? "i32" : "bool";
  vector_lane_count = lane_count;
  return true;
}
