std::string BuildExecutableMetadataRuntimeIngestBinaryEnvelope(
    const Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary
        &packaging_contract,
    const Objc3ExecutableMetadataTypedLoweringHandoff &typed_lowering_handoff,
    const Objc3ExecutableMetadataDebugProjectionSummary &debug_projection) {
  if (!IsReadyObjc3ExecutableMetadataRuntimeIngestPackagingContractSummary(
          packaging_contract) ||
      !IsReadyObjc3ExecutableMetadataTypedLoweringHandoff(
          typed_lowering_handoff) ||
      !IsReadyObjc3ExecutableMetadataDebugProjectionSummary(debug_projection)) {
    return {};
  }

  const std::string packaging_json =
      BuildExecutableMetadataRuntimeIngestPackagingContractSummaryJson(
          packaging_contract);
  const std::string typed_handoff_json =
      BuildExecutableMetadataTypedLoweringHandoffJson(typed_lowering_handoff);
  const std::string debug_projection_json =
      BuildExecutableMetadataDebugProjectionSummaryJson(debug_projection);

  return BuildExecutableMetadataRuntimeIngestBinaryEnvelopePayload(
      packaging_json, typed_handoff_json, debug_projection_json);
}
