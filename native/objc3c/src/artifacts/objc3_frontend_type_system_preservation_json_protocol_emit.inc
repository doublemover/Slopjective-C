std::string RenderTypeSystemProtocolContractPreservationJson(
    const Objc3Program &program,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_records,
    const Objc3FrontendTypeSystemSemanticModelRecord &semantic_summary) {
  const Objc3FrontendTypeSystemProtocolContractInventory inventory =
      BuildTypeSystemProtocolContractInventory(program, runtime_records);
  const bool deterministic = runtime_records.deterministic;
  const bool ready =
      deterministic && semantic_summary.ready_for_lowering_and_runtime &&
      semantic_summary.deterministic && !semantic_summary.replay_key.empty() &&
      inventory.protocol_decl_count >=
          inventory.protocol_forward_declaration_count;
  const std::string replay_key =
      BuildTypeSystemProtocolContractPreservationReplayKey(inventory,
                                                           semantic_summary);
  std::vector<const Objc3ProtocolDecl *> protocols;
  protocols.reserve(program.protocols.size());
  for (const Objc3ProtocolDecl &protocol : program.protocols) {
    protocols.push_back(&protocol);
  }
  std::sort(protocols.begin(), protocols.end(),
            [](const Objc3ProtocolDecl *lhs, const Objc3ProtocolDecl *rhs) {
              return lhs->name < rhs->name;
            });

  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(
             kObjc3FrontendTypeSystemProtocolContractPreservationContractId)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(kObjc3FrontendTypeSystemTypeSemanticModelContractId)
      << "\",\"preservation_model\":\"runtime-import-surface-preserves-protocol-requirement-partitions-inheritance-and-conformance-edges\""
      << ",\"protocol_decl_count\":" << inventory.protocol_decl_count
      << ",\"protocol_forward_declaration_count\":"
      << inventory.protocol_forward_declaration_count
      << ",\"protocol_inheritance_edge_count\":"
      << inventory.protocol_inheritance_edge_count
      << ",\"protocol_required_method_count\":"
      << inventory.protocol_required_method_count
      << ",\"protocol_optional_method_count\":"
      << inventory.protocol_optional_method_count
      << ",\"protocol_required_property_count\":"
      << inventory.protocol_required_property_count
      << ",\"protocol_optional_property_count\":"
      << inventory.protocol_optional_property_count
      << ",\"class_protocol_adoption_count\":"
      << inventory.class_protocol_adoption_count
      << ",\"category_protocol_adoption_count\":"
      << inventory.category_protocol_adoption_count
      << ",\"ready\":" << (ready ? "true" : "false")
      << ",\"deterministic\":" << (deterministic ? "true" : "false")
      << ",\"type_semantic_replay_key\":\""
      << EscapeJsonString(semantic_summary.replay_key)
      << "\",\"protocol_contract_protocols\":[";
  for (std::size_t i = 0; i < protocols.size(); ++i) {
    const Objc3ProtocolDecl &protocol = *protocols[i];
    if (i != 0u) {
      out << ",";
    }
    std::size_t required_methods = 0;
    std::size_t optional_methods = 0;
    for (const Objc3MethodDecl &method : protocol.methods) {
      if (method.protocol_requirement_kind ==
          Objc3ProtocolRequirementKind::Optional) {
        ++optional_methods;
      } else {
        ++required_methods;
      }
    }
    std::size_t required_properties = 0;
    std::size_t optional_properties = 0;
    for (const Objc3PropertyDecl &property : protocol.properties) {
      if (property.protocol_requirement_kind ==
          Objc3ProtocolRequirementKind::Optional) {
        ++optional_properties;
      } else {
        ++required_properties;
      }
    }
    out << "{\"name\":\"" << EscapeJsonString(protocol.name)
        << "\",\"is_forward_declaration\":"
        << (protocol.is_forward_declaration ? "true" : "false")
        << ",\"inherited_protocols_lexicographic\":"
        << BuildStringArrayJson(protocol.inherited_protocols_lexicographic)
        << ",\"required_method_count\":" << required_methods
        << ",\"optional_method_count\":" << optional_methods
        << ",\"required_property_count\":" << required_properties
        << ",\"optional_property_count\":" << optional_properties
        << ",\"methods\":[";
    for (std::size_t method_index = 0; method_index < protocol.methods.size();
         ++method_index) {
      const Objc3MethodDecl &method = protocol.methods[method_index];
      if (method_index != 0u) {
        out << ",";
      }
      out << "{\"selector\":\"" << EscapeJsonString(method.selector)
          << "\",\"requirement_kind\":\""
          << Objc3ProtocolRequirementKindName(method.protocol_requirement_kind)
          << "\",\"is_class_method\":"
          << (method.is_class_method ? "true" : "false") << "}";
    }
    out << "],\"properties\":[";
    for (std::size_t property_index = 0;
         property_index < protocol.properties.size(); ++property_index) {
      const Objc3PropertyDecl &property = protocol.properties[property_index];
      if (property_index != 0u) {
        out << ",";
      }
      out << "{\"name\":\"" << EscapeJsonString(property.name)
          << "\",\"requirement_kind\":\""
          << Objc3ProtocolRequirementKindName(
                 property.protocol_requirement_kind)
          << "\"}";
    }
    out << "]}";
  }
  out << "],\"replay_key\":\"" << EscapeJsonString(replay_key) << "\"}";
  return out.str();
}
