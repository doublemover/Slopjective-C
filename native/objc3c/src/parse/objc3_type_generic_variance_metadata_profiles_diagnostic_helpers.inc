namespace {

bool HasVarianceMarkers(const VarianceBridgeCastProfileEvidence &evidence) {
  return evidence.covariant_markers + evidence.contravariant_markers +
             evidence.invariant_markers >
         0;
}

bool HasBridgeMarkers(const VarianceBridgeCastProfileEvidence &evidence) {
  return evidence.bridge_markers + evidence.bridge_transfer_markers +
             evidence.bridge_retained_markers >
         0;
}

bool IsVarianceMarkerSetSafe(
    const VarianceBridgeCastProfileEvidence &evidence) {
  return (evidence.covariant_markers == 0 ||
          evidence.contravariant_markers == 0) &&
         (evidence.covariant_markers + evidence.contravariant_markers <= 1);
}

bool IsBridgeCastMarkerSetValid(
    const VarianceBridgeCastProfileEvidence &evidence) {
  return evidence.bridge_transfer_markers <= 1 &&
         evidence.bridge_retained_markers <= 1;
}

bool IsMarkerObjectPointerRequirementSatisfied(
    bool object_pointer_type_spelling,
    const VarianceBridgeCastProfileEvidence &evidence) {
  return !HasVarianceMarkers(evidence) && !HasBridgeMarkers(evidence)
             ? true
             : object_pointer_type_spelling;
}

bool IsGenericMetadataEmissionReady(
    bool object_pointer_type_spelling, bool has_generic_suffix,
    bool generic_suffix_terminated,
    const GenericMetadataAbiProfileEvidence &evidence) {
  return has_generic_suffix && generic_suffix_terminated &&
         object_pointer_type_spelling && evidence.generic_argument_slots > 0;
}

bool IsGenericMetadataAbiLayoutStable(
    bool object_pointer_type_spelling, bool has_pointer_declarator,
    bool metadata_emission_ready) {
  return metadata_emission_ready &&
         (!has_pointer_declarator || object_pointer_type_spelling);
}

}  // namespace
