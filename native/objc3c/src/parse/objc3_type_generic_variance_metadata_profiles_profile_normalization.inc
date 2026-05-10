bool IsVarianceBridgeCastProfileNormalized(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, const std::string &generic_suffix_text,
    const std::string &ownership_qualifier_spelling) {
  const VarianceBridgeCastProfileEvidence evidence =
      CollectVarianceBridgeCastProfileEvidence(generic_suffix_text,
                                               ownership_qualifier_spelling);
  if (HasVarianceMarkers(evidence) &&
      (!has_generic_suffix || !generic_suffix_terminated)) {
    return false;
  }
  if ((HasVarianceMarkers(evidence) || HasBridgeMarkers(evidence)) &&
      !object_pointer_type_spelling) {
    return false;
  }
  return IsVarianceMarkerSetSafe(evidence) &&
         IsBridgeCastMarkerSetValid(evidence);
}

bool IsGenericMetadataAbiProfileNormalized(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text) {
  if (!has_generic_suffix) {
    return true;
  }

  const std::size_t generic_argument_slots =
      CountGenericMetadataAbiArgumentSlots(has_generic_suffix,
                                           generic_suffix_text);
  if (!generic_suffix_terminated || !object_pointer_type_spelling ||
      generic_argument_slots == 0) {
    return false;
  }

  if (has_pointer_declarator && !object_pointer_type_spelling) {
    return false;
  }
  return true;
}
