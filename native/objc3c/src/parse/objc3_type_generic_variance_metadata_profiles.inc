std::string BuildVarianceBridgeCastProfile(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &ownership_qualifier_spelling) {
  const std::size_t covariant_markers =
      CountMarkerOccurrences(generic_suffix_text, "__covariant");
  const std::size_t contravariant_markers =
      CountMarkerOccurrences(generic_suffix_text, "__contravariant");
  const std::size_t invariant_markers =
      CountMarkerOccurrences(generic_suffix_text, "__invariant");
  const std::size_t bridge_transfer_markers =
      CountMarkerOccurrences(generic_suffix_text, "__bridge_transfer");
  const std::size_t bridge_retained_markers =
      CountMarkerOccurrences(generic_suffix_text, "__bridge_retained");
  const std::size_t bridge_markers =
      CountMarkerOccurrences(generic_suffix_text, "__bridge") +
      CountMarkerOccurrences(ownership_qualifier_spelling, "__bridge");
  const std::size_t bridge_transfer_total =
      bridge_transfer_markers +
      CountMarkerOccurrences(ownership_qualifier_spelling, "__bridge_transfer");
  const std::size_t bridge_retained_total =
      bridge_retained_markers +
      CountMarkerOccurrences(ownership_qualifier_spelling, "__bridge_retained");
  const bool variance_marked =
      covariant_markers + contravariant_markers + invariant_markers > 0;
  const bool bridge_marked =
      bridge_markers + bridge_transfer_total + bridge_retained_total > 0;
  const bool variance_safe =
      (covariant_markers == 0 || contravariant_markers == 0) &&
      (covariant_markers + contravariant_markers <= 1);
  const bool bridge_cast_valid =
      bridge_transfer_total <= 1 && bridge_retained_total <= 1;
  const bool object_pointer_required_for_markers =
      !variance_marked && !bridge_marked ? true : object_pointer_type_spelling;

  std::ostringstream out;
  out << "variance-bridge-cast:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";has-generic-suffix=" << (has_generic_suffix ? "true" : "false")
      << ";terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";covariant-markers=" << covariant_markers
      << ";contravariant-markers=" << contravariant_markers
      << ";invariant-markers=" << invariant_markers
      << ";bridge-markers=" << bridge_markers
      << ";bridge-transfer-markers=" << bridge_transfer_total
      << ";bridge-retained-markers=" << bridge_retained_total
      << ";variance-safe=" << (variance_safe ? "true" : "false")
      << ";bridge-cast-valid=" << (bridge_cast_valid ? "true" : "false")
      << ";marker-object-pointer-valid="
      << (object_pointer_required_for_markers ? "true" : "false");
  return out.str();
}

bool IsVarianceBridgeCastProfileNormalized(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, const std::string &generic_suffix_text,
    const std::string &ownership_qualifier_spelling) {
  const std::size_t covariant_markers =
      CountMarkerOccurrences(generic_suffix_text, "__covariant");
  const std::size_t contravariant_markers =
      CountMarkerOccurrences(generic_suffix_text, "__contravariant");
  const std::size_t invariant_markers =
      CountMarkerOccurrences(generic_suffix_text, "__invariant");
  const std::size_t bridge_transfer_markers =
      CountMarkerOccurrences(generic_suffix_text, "__bridge_transfer") +
      CountMarkerOccurrences(ownership_qualifier_spelling, "__bridge_transfer");
  const std::size_t bridge_retained_markers =
      CountMarkerOccurrences(generic_suffix_text, "__bridge_retained") +
      CountMarkerOccurrences(ownership_qualifier_spelling, "__bridge_retained");
  const bool variance_marked =
      covariant_markers + contravariant_markers + invariant_markers > 0;
  const bool bridge_marked =
      CountMarkerOccurrences(generic_suffix_text, "__bridge") +
          CountMarkerOccurrences(ownership_qualifier_spelling, "__bridge") +
          bridge_transfer_markers + bridge_retained_markers >
      0;
  const bool variance_safe =
      (covariant_markers == 0 || contravariant_markers == 0) &&
      (covariant_markers + contravariant_markers <= 1);
  const bool bridge_cast_valid =
      bridge_transfer_markers <= 1 && bridge_retained_markers <= 1;
  if (variance_marked && (!has_generic_suffix || !generic_suffix_terminated)) {
    return false;
  }
  if ((variance_marked || bridge_marked) && !object_pointer_type_spelling) {
    return false;
  }
  return variance_safe && bridge_cast_valid;
}

std::string BuildGenericMetadataAbiProfile(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &ownership_qualifier_spelling) {
  const std::size_t generic_argument_slots =
      has_generic_suffix ? CountTopLevelGenericArgumentSlots(generic_suffix_text) : 0;
  const std::size_t variance_markers =
      CountMarkerOccurrences(generic_suffix_text, "__covariant") +
      CountMarkerOccurrences(generic_suffix_text, "__contravariant") +
      CountMarkerOccurrences(generic_suffix_text, "__invariant");
  const std::size_t bridge_markers =
      CountMarkerOccurrences(generic_suffix_text, "__bridge") +
      CountMarkerOccurrences(ownership_qualifier_spelling, "__bridge");
  const bool metadata_emission_ready =
      has_generic_suffix && generic_suffix_terminated &&
      object_pointer_type_spelling && generic_argument_slots > 0;
  const bool abi_layout_stable =
      metadata_emission_ready &&
      (!has_pointer_declarator || object_pointer_type_spelling);

  std::ostringstream out;
  out << "generic-metadata-abi:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";has-generic-suffix=" << (has_generic_suffix ? "true" : "false")
      << ";terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";generic-argument-slots=" << generic_argument_slots
      << ";variance-markers=" << variance_markers
      << ";bridge-markers=" << bridge_markers
      << ";metadata-emission-ready="
      << (metadata_emission_ready ? "true" : "false")
      << ";abi-layout-stable=" << (abi_layout_stable ? "true" : "false");
  return out.str();
}

bool IsGenericMetadataAbiProfileNormalized(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text) {
  if (!has_generic_suffix) {
    return true;
  }

  const std::size_t generic_argument_slots =
      CountTopLevelGenericArgumentSlots(generic_suffix_text);
  if (!generic_suffix_terminated || !object_pointer_type_spelling ||
      generic_argument_slots == 0) {
    return false;
  }

  if (has_pointer_declarator && !object_pointer_type_spelling) {
    return false;
  }
  return true;
}
