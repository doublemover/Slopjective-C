namespace {

struct VarianceBridgeCastProfileEvidence {
  std::size_t covariant_markers = 0;
  std::size_t contravariant_markers = 0;
  std::size_t invariant_markers = 0;
  std::size_t bridge_markers = 0;
  std::size_t bridge_transfer_markers = 0;
  std::size_t bridge_retained_markers = 0;
};

VarianceBridgeCastProfileEvidence CollectVarianceBridgeCastProfileEvidence(
    const std::string &generic_suffix_text,
    const std::string &ownership_qualifier_spelling) {
  VarianceBridgeCastProfileEvidence evidence;
  evidence.covariant_markers =
      CountMarkerOccurrences(generic_suffix_text, "__covariant");
  evidence.contravariant_markers =
      CountMarkerOccurrences(generic_suffix_text, "__contravariant");
  evidence.invariant_markers =
      CountMarkerOccurrences(generic_suffix_text, "__invariant");
  evidence.bridge_markers =
      CountMarkerOccurrences(generic_suffix_text, "__bridge") +
      CountMarkerOccurrences(ownership_qualifier_spelling, "__bridge");
  evidence.bridge_transfer_markers =
      CountMarkerOccurrences(generic_suffix_text, "__bridge_transfer") +
      CountMarkerOccurrences(ownership_qualifier_spelling, "__bridge_transfer");
  evidence.bridge_retained_markers =
      CountMarkerOccurrences(generic_suffix_text, "__bridge_retained") +
      CountMarkerOccurrences(ownership_qualifier_spelling, "__bridge_retained");
  return evidence;
}

struct GenericMetadataAbiProfileEvidence {
  std::size_t generic_argument_slots = 0;
  std::size_t variance_markers = 0;
  std::size_t bridge_markers = 0;
};

std::size_t CountGenericMetadataAbiArgumentSlots(
    bool has_generic_suffix, const std::string &generic_suffix_text) {
  return has_generic_suffix ? CountTopLevelGenericArgumentSlots(generic_suffix_text)
                            : 0;
}

GenericMetadataAbiProfileEvidence CollectGenericMetadataAbiProfileEvidence(
    bool has_generic_suffix, const std::string &generic_suffix_text,
    const std::string &ownership_qualifier_spelling) {
  GenericMetadataAbiProfileEvidence evidence;
  evidence.generic_argument_slots =
      CountGenericMetadataAbiArgumentSlots(has_generic_suffix,
                                           generic_suffix_text);
  evidence.variance_markers =
      CountMarkerOccurrences(generic_suffix_text, "__covariant") +
      CountMarkerOccurrences(generic_suffix_text, "__contravariant") +
      CountMarkerOccurrences(generic_suffix_text, "__invariant");
  evidence.bridge_markers =
      CountMarkerOccurrences(generic_suffix_text, "__bridge") +
      CountMarkerOccurrences(ownership_qualifier_spelling, "__bridge");
  return evidence;
}

}  // namespace
