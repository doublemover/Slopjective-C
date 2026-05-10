std::string BuildVarianceBridgeCastProfile(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &ownership_qualifier_spelling) {
  const VarianceBridgeCastProfileEvidence evidence =
      CollectVarianceBridgeCastProfileEvidence(generic_suffix_text,
                                               ownership_qualifier_spelling);

  std::ostringstream out;
  out << "variance-bridge-cast:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";has-generic-suffix=" << (has_generic_suffix ? "true" : "false")
      << ";terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";covariant-markers=" << evidence.covariant_markers
      << ";contravariant-markers=" << evidence.contravariant_markers
      << ";invariant-markers=" << evidence.invariant_markers
      << ";bridge-markers=" << evidence.bridge_markers
      << ";bridge-transfer-markers=" << evidence.bridge_transfer_markers
      << ";bridge-retained-markers=" << evidence.bridge_retained_markers
      << ";variance-safe="
      << (IsVarianceMarkerSetSafe(evidence) ? "true" : "false")
      << ";bridge-cast-valid="
      << (IsBridgeCastMarkerSetValid(evidence) ? "true" : "false")
      << ";marker-object-pointer-valid="
      << (IsMarkerObjectPointerRequirementSatisfied(
              object_pointer_type_spelling, evidence)
              ? "true"
              : "false");
  return out.str();
}

std::string BuildGenericMetadataAbiProfile(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated, bool has_pointer_declarator,
    const std::string &generic_suffix_text,
    const std::string &ownership_qualifier_spelling) {
  const GenericMetadataAbiProfileEvidence evidence =
      CollectGenericMetadataAbiProfileEvidence(
          has_generic_suffix, generic_suffix_text, ownership_qualifier_spelling);
  const bool metadata_emission_ready = IsGenericMetadataEmissionReady(
      object_pointer_type_spelling, has_generic_suffix, generic_suffix_terminated,
      evidence);
  const bool abi_layout_stable = IsGenericMetadataAbiLayoutStable(
      object_pointer_type_spelling, has_pointer_declarator,
      metadata_emission_ready);

  std::ostringstream out;
  out << "generic-metadata-abi:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";has-generic-suffix=" << (has_generic_suffix ? "true" : "false")
      << ";terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";generic-argument-slots=" << evidence.generic_argument_slots
      << ";variance-markers=" << evidence.variance_markers
      << ";bridge-markers=" << evidence.bridge_markers
      << ";metadata-emission-ready="
      << (metadata_emission_ready ? "true" : "false")
      << ";abi-layout-stable=" << (abi_layout_stable ? "true" : "false");
  return out.str();
}
