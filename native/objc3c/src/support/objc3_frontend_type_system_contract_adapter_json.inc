std::string BuildTypeSystemGenericContractPreservationJson(
    const Objc3SemanticTypeMetadataHandoff &handoff,
    const Objc3TypeSystemTypeSemanticModelSummary &semantic_summary) {
  return RenderTypeSystemGenericContractPreservationJson(
      BuildFrontendTypeSystemSemanticMetadataRecord(handoff),
      BuildFrontendTypeSystemSemanticModelRecord(semantic_summary));
}

std::string BuildTypeSystemNullabilityContractPreservationJson(
    const Objc3TypeSystemTypeSemanticModelSummary &summary) {
  return RenderTypeSystemNullabilityContractPreservationJson(
      BuildFrontendTypeSystemSemanticModelRecord(summary));
}

std::string BuildTypeSystemProtocolContractPreservationJson(
    const Objc3Program &program,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_records,
    const Objc3TypeSystemTypeSemanticModelSummary &semantic_summary) {
  return RenderTypeSystemProtocolContractPreservationJson(
      program, runtime_records,
      BuildFrontendTypeSystemSemanticModelRecord(semantic_summary));
}

std::string BuildTypeSystemOptionalKeypathLoweringContractJson(
    const Objc3TypeSystemOptionalKeypathLoweringContract &contract,
    const Objc3TypeSystemTypeSemanticModelSummary &semantic_summary,
    const std::string &semantic_summary_replay_key,
    const std::string &message_send_selector_lowering_replay_key,
    const std::string &dispatch_abi_marshalling_replay_key,
    const std::string &nil_receiver_semantics_foldability_replay_key,
    const std::string &replay_key) {
  return RenderTypeSystemOptionalKeypathLoweringContractJson(
      ToFrontendOptionalKeypathLoweringRecord(contract),
      BuildFrontendTypeSystemSemanticModelRecord(semantic_summary),
      semantic_summary_replay_key, message_send_selector_lowering_replay_key,
      dispatch_abi_marshalling_replay_key,
      nil_receiver_semantics_foldability_replay_key, replay_key);
}

std::string BuildTypeSystemOptionalKeypathRuntimeHelperContractJson(
    const Objc3TypeSystemOptionalKeypathLoweringContract &contract,
    const Objc3RuntimeSupportLibraryLinkWiringSummary &runtime_link_wiring,
    const std::string &lowering_replay_key) {
  return RenderTypeSystemOptionalKeypathRuntimeHelperContractJson(
      ToFrontendOptionalKeypathLoweringRecord(contract), runtime_link_wiring,
      lowering_replay_key);
}
