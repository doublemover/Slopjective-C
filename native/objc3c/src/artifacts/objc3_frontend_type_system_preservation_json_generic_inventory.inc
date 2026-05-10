bool IsProtocolQualifiedGenericArgumentSpelling(const std::string &argument) {
  return argument.find("id<") != std::string::npos ||
         argument.find("Class<") != std::string::npos;
}

void AccumulateGenericContractInventory(
    const Objc3FrontendTypeSystemCanonicalTypeRecord &type,
    Objc3FrontendTypeSystemGenericContractInventory &inventory) {
  inventory.generic_argument_reference_count +=
      type.generic_arguments_source_order.size();
  for (const std::string &argument : type.generic_arguments_source_order) {
    if (IsProtocolQualifiedGenericArgumentSpelling(argument)) {
      ++inventory.protocol_qualified_generic_argument_count;
    }
  }
}

Objc3FrontendTypeSystemGenericContractInventory
BuildTypeSystemGenericContractInventory(
    const Objc3FrontendTypeSystemSemanticMetadataRecord &handoff) {
  Objc3FrontendTypeSystemGenericContractInventory inventory;
  inventory.interface_count = handoff.interfaces_lexicographic.size();
  for (const Objc3FrontendTypeSystemInterfaceRecord &interface_metadata :
       handoff.interfaces_lexicographic) {
    if (!interface_metadata.generic_parameter_names_source_order.empty()) {
      ++inventory.generic_interface_count;
    }
    inventory.generic_parameter_count +=
        interface_metadata.generic_parameter_names_source_order.size();
    inventory.generic_variance_annotation_count +=
        interface_metadata.generic_parameter_variance_source_order.size();
    for (const Objc3FrontendTypeSystemPropertyRecord &property :
         interface_metadata.properties_lexicographic) {
      AccumulateGenericContractInventory(property.canonical_type, inventory);
    }
    for (const Objc3FrontendTypeSystemMethodRecord &method :
         interface_metadata.methods_lexicographic) {
      AccumulateGenericContractInventory(method.return_canonical_type,
                                         inventory);
      for (const Objc3FrontendTypeSystemCanonicalTypeRecord &param :
           method.param_canonical_types) {
        AccumulateGenericContractInventory(param, inventory);
      }
    }
  }
  for (const Objc3FrontendTypeSystemFunctionRecord &function :
       handoff.functions_lexicographic) {
    AccumulateGenericContractInventory(function.return_canonical_type,
                                       inventory);
    for (const Objc3FrontendTypeSystemCanonicalTypeRecord &param :
         function.param_canonical_types) {
      AccumulateGenericContractInventory(param, inventory);
    }
  }
  for (const Objc3FrontendTypeSystemImplementationRecord &implementation :
       handoff.implementations_lexicographic) {
    for (const Objc3FrontendTypeSystemPropertyRecord &property :
         implementation.properties_lexicographic) {
      AccumulateGenericContractInventory(property.canonical_type, inventory);
    }
    for (const Objc3FrontendTypeSystemMethodRecord &method :
         implementation.methods_lexicographic) {
      AccumulateGenericContractInventory(method.return_canonical_type,
                                         inventory);
      for (const Objc3FrontendTypeSystemCanonicalTypeRecord &param :
           method.param_canonical_types) {
        AccumulateGenericContractInventory(param, inventory);
      }
    }
  }
  return inventory;
}

std::string BuildTypeSystemGenericContractPreservationReplayKey(
    const Objc3FrontendTypeSystemGenericContractInventory &inventory,
    const Objc3FrontendTypeSystemSemanticModelRecord &semantic_summary) {
  std::ostringstream out;
  out << kObjc3FrontendTypeSystemGenericContractPreservationContractId
      << ";source_contract="
      << kObjc3FrontendTypeSystemTypeSemanticModelContractId
      << ";type_semantic_replay=" << semantic_summary.replay_key
      << ";interfaces=" << inventory.interface_count
      << ";generic_interfaces=" << inventory.generic_interface_count
      << ";generic_parameters=" << inventory.generic_parameter_count
      << ";variance_annotations="
      << inventory.generic_variance_annotation_count
      << ";generic_argument_refs="
      << inventory.generic_argument_reference_count
      << ";protocol_qualified_generic_arguments="
      << inventory.protocol_qualified_generic_argument_count;
  return out.str();
}
