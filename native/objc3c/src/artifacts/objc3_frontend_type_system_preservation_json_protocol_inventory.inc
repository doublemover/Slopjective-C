std::string Objc3ProtocolRequirementKindName(
    Objc3ProtocolRequirementKind kind) {
  switch (kind) {
    case Objc3ProtocolRequirementKind::Required:
      return "required";
    case Objc3ProtocolRequirementKind::Optional:
      return "optional";
    case Objc3ProtocolRequirementKind::NotApplicable:
      break;
  }
  return "not-applicable";
}

Objc3FrontendTypeSystemProtocolContractInventory
BuildTypeSystemProtocolContractInventory(
    const Objc3Program &program,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_records) {
  Objc3FrontendTypeSystemProtocolContractInventory inventory;
  inventory.protocol_decl_count = program.protocols.size();
  for (const Objc3ProtocolDecl &protocol : program.protocols) {
    if (protocol.is_forward_declaration) {
      ++inventory.protocol_forward_declaration_count;
    }
    inventory.protocol_inheritance_edge_count +=
        protocol.inherited_protocols_lexicographic.size();
    for (const Objc3MethodDecl &method : protocol.methods) {
      if (method.protocol_requirement_kind ==
          Objc3ProtocolRequirementKind::Optional) {
        ++inventory.protocol_optional_method_count;
      } else {
        ++inventory.protocol_required_method_count;
      }
    }
    for (const Objc3PropertyDecl &property : protocol.properties) {
      if (property.protocol_requirement_kind ==
          Objc3ProtocolRequirementKind::Optional) {
        ++inventory.protocol_optional_property_count;
      } else {
        ++inventory.protocol_required_property_count;
      }
    }
  }
  for (const Objc3RuntimeMetadataClassSourceRecord &class_record :
       runtime_records.classes_lexicographic) {
    inventory.class_protocol_adoption_count +=
        class_record.adopted_protocols_lexicographic.size();
  }
  for (const Objc3RuntimeMetadataCategorySourceRecord &category_record :
       runtime_records.categories_lexicographic) {
    inventory.category_protocol_adoption_count +=
        category_record.adopted_protocols_lexicographic.size();
  }
  return inventory;
}

std::string BuildTypeSystemProtocolContractPreservationReplayKey(
    const Objc3FrontendTypeSystemProtocolContractInventory &inventory,
    const Objc3FrontendTypeSystemSemanticModelRecord &semantic_summary) {
  std::ostringstream out;
  out << kObjc3FrontendTypeSystemProtocolContractPreservationContractId
      << ";source_contract="
      << kObjc3FrontendTypeSystemTypeSemanticModelContractId
      << ";type_semantic_replay=" << semantic_summary.replay_key
      << ";protocols=" << inventory.protocol_decl_count
      << ";forward_declarations="
      << inventory.protocol_forward_declaration_count
      << ";inherited_edges=" << inventory.protocol_inheritance_edge_count
      << ";required_methods=" << inventory.protocol_required_method_count
      << ";optional_methods=" << inventory.protocol_optional_method_count
      << ";required_properties=" << inventory.protocol_required_property_count
      << ";optional_properties=" << inventory.protocol_optional_property_count
      << ";class_adoptions=" << inventory.class_protocol_adoption_count
      << ";category_adoptions=" << inventory.category_protocol_adoption_count;
  return out.str();
}
