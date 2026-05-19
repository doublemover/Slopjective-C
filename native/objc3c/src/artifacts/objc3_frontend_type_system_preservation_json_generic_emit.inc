std::string RenderTypeSystemGenericContractPreservationJson(
    const Objc3FrontendTypeSystemSemanticMetadataRecord &handoff,
    const Objc3FrontendTypeSystemSemanticModelRecord &semantic_summary) {
  const Objc3FrontendTypeSystemGenericContractInventory inventory =
      BuildTypeSystemGenericContractInventory(handoff);
  const bool deterministic = handoff.deterministic;
  const bool ready =
      deterministic && semantic_summary.ready_for_lowering_and_runtime &&
      semantic_summary.deterministic && !semantic_summary.replay_key.empty();
  const std::string replay_key =
      BuildTypeSystemGenericContractPreservationReplayKey(inventory,
                                                          semantic_summary);
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(
             kObjc3FrontendTypeSystemGenericContractPreservationContractId)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(kObjc3FrontendTypeSystemTypeSemanticModelContractId)
      << "\",\"preservation_model\":\"runtime-import-surface-preserves-interface-generic-parameter-variance-adoption-and-specialization-reference-facts\""
      << ",\"interface_count\":" << inventory.interface_count
      << ",\"generic_interface_count\":" << inventory.generic_interface_count
      << ",\"generic_parameter_count\":" << inventory.generic_parameter_count
      << ",\"generic_variance_annotation_count\":"
      << inventory.generic_variance_annotation_count
      << ",\"generic_argument_reference_count\":"
      << inventory.generic_argument_reference_count
      << ",\"protocol_qualified_generic_argument_count\":"
      << inventory.protocol_qualified_generic_argument_count
      << ",\"ready\":" << (ready ? "true" : "false")
      << ",\"deterministic\":" << (deterministic ? "true" : "false")
      << ",\"type_semantic_replay_key\":\""
      << EscapeJsonString(semantic_summary.replay_key)
      << "\",\"generic_contract_interfaces\":[";
  for (std::size_t i = 0; i < handoff.interfaces_lexicographic.size(); ++i) {
    const Objc3FrontendTypeSystemInterfaceRecord &interface_metadata =
        handoff.interfaces_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"name\":\"" << EscapeJsonString(interface_metadata.name)
        << "\",\"super_name\":\""
        << EscapeJsonString(interface_metadata.super_name)
        << "\",\"generic_parameter_names_source_order\":"
        << BuildStringArrayJson(
               interface_metadata.generic_parameter_names_source_order)
        << ",\"generic_parameter_variance_source_order\":"
        << BuildStringArrayJson(
               interface_metadata.generic_parameter_variance_source_order)
        << ",\"adopted_protocols_lexicographic\":"
        << BuildStringArrayJson(
               interface_metadata.adopted_protocols_lexicographic)
        << "}";
  }
  out << "],\"replay_key\":\"" << EscapeJsonString(replay_key) << "\"}";
  return out.str();
}
