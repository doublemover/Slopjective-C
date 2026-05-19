Objc3FrontendTypeSystemOptionalKeypathLoweringContractRecord
ToFrontendOptionalKeypathLoweringRecord(
    const Objc3TypeSystemOptionalKeypathLoweringContract &contract) {
  Objc3FrontendTypeSystemOptionalKeypathLoweringContractRecord record;
  record.optional_binding_sites = contract.optional_binding_sites;
  record.optional_binding_clause_sites = contract.optional_binding_clause_sites;
  record.optional_send_sites = contract.optional_send_sites;
  record.nil_coalescing_sites = contract.nil_coalescing_sites;
  record.typed_keypath_literal_sites = contract.typed_keypath_literal_sites;
  record.typed_keypath_self_root_sites = contract.typed_keypath_self_root_sites;
  record.typed_keypath_class_root_sites =
      contract.typed_keypath_class_root_sites;
  record.live_optional_lowering_sites = contract.live_optional_lowering_sites;
  record.single_evaluation_nil_short_circuit_sites =
      contract.single_evaluation_nil_short_circuit_sites;
  record.live_typed_keypath_artifact_sites =
      contract.live_typed_keypath_artifact_sites;
  record.deferred_typed_keypath_sites = contract.deferred_typed_keypath_sites;
  record.contract_violation_sites = contract.contract_violation_sites;
  record.deterministic = contract.deterministic;
  return record;
}

Objc3TypeSystemOptionalKeypathLoweringContract
ToLowerOptionalKeypathLoweringContract(
    const Objc3FrontendTypeSystemOptionalKeypathLoweringContractRecord
        &record) {
  Objc3TypeSystemOptionalKeypathLoweringContract contract;
  contract.optional_binding_sites = record.optional_binding_sites;
  contract.optional_binding_clause_sites = record.optional_binding_clause_sites;
  contract.optional_send_sites = record.optional_send_sites;
  contract.nil_coalescing_sites = record.nil_coalescing_sites;
  contract.typed_keypath_literal_sites = record.typed_keypath_literal_sites;
  contract.typed_keypath_self_root_sites = record.typed_keypath_self_root_sites;
  contract.typed_keypath_class_root_sites =
      record.typed_keypath_class_root_sites;
  contract.live_optional_lowering_sites = record.live_optional_lowering_sites;
  contract.single_evaluation_nil_short_circuit_sites =
      record.single_evaluation_nil_short_circuit_sites;
  contract.live_typed_keypath_artifact_sites =
      record.live_typed_keypath_artifact_sites;
  contract.deferred_typed_keypath_sites = record.deferred_typed_keypath_sites;
  contract.contract_violation_sites = record.contract_violation_sites;
  contract.deterministic = record.deterministic;
  return contract;
}

Objc3FrontendTypeSystemCanonicalTypeRecord ToFrontendCanonicalTypeRecord(
    const Objc3SemanticCanonicalType &type) {
  Objc3FrontendTypeSystemCanonicalTypeRecord record;
  record.generic_arguments_source_order =
      type.generic_arguments_source_order;
  return record;
}

Objc3FrontendTypeSystemPropertyRecord ToFrontendPropertyRecord(
    const Objc3SemanticPropertyTypeMetadata &property) {
  Objc3FrontendTypeSystemPropertyRecord record;
  record.canonical_type = ToFrontendCanonicalTypeRecord(property.canonical_type);
  return record;
}

Objc3FrontendTypeSystemMethodRecord ToFrontendMethodRecord(
    const Objc3SemanticMethodTypeMetadata &method) {
  Objc3FrontendTypeSystemMethodRecord record;
  record.return_canonical_type =
      ToFrontendCanonicalTypeRecord(method.return_canonical_type);
  record.param_canonical_types.reserve(method.param_canonical_types.size());
  for (const Objc3SemanticCanonicalType &param :
       method.param_canonical_types) {
    record.param_canonical_types.push_back(ToFrontendCanonicalTypeRecord(param));
  }
  return record;
}

Objc3FrontendTypeSystemFunctionRecord ToFrontendFunctionRecord(
    const Objc3SemanticFunctionTypeMetadata &function) {
  Objc3FrontendTypeSystemFunctionRecord record;
  record.return_canonical_type =
      ToFrontendCanonicalTypeRecord(function.return_canonical_type);
  record.param_canonical_types.reserve(function.param_canonical_types.size());
  for (const Objc3SemanticCanonicalType &param :
       function.param_canonical_types) {
    record.param_canonical_types.push_back(ToFrontendCanonicalTypeRecord(param));
  }
  return record;
}

Objc3FrontendTypeSystemInterfaceRecord ToFrontendInterfaceRecord(
    const Objc3SemanticInterfaceTypeMetadata &interface_metadata) {
  Objc3FrontendTypeSystemInterfaceRecord record;
  record.name = interface_metadata.name;
  record.super_name = interface_metadata.super_name;
  record.generic_parameter_names_source_order =
      interface_metadata.generic_parameter_names_source_order;
  record.generic_parameter_variance_source_order =
      interface_metadata.generic_parameter_variance_source_order;
  record.adopted_protocols_lexicographic =
      interface_metadata.adopted_protocols_lexicographic;
  record.properties_lexicographic.reserve(
      interface_metadata.properties_lexicographic.size());
  for (const Objc3SemanticPropertyTypeMetadata &property :
       interface_metadata.properties_lexicographic) {
    record.properties_lexicographic.push_back(ToFrontendPropertyRecord(property));
  }
  record.methods_lexicographic.reserve(
      interface_metadata.methods_lexicographic.size());
  for (const Objc3SemanticMethodTypeMetadata &method :
       interface_metadata.methods_lexicographic) {
    record.methods_lexicographic.push_back(ToFrontendMethodRecord(method));
  }
  return record;
}

Objc3FrontendTypeSystemImplementationRecord ToFrontendImplementationRecord(
    const Objc3SemanticImplementationTypeMetadata &implementation) {
  Objc3FrontendTypeSystemImplementationRecord record;
  record.properties_lexicographic.reserve(
      implementation.properties_lexicographic.size());
  for (const Objc3SemanticPropertyTypeMetadata &property :
       implementation.properties_lexicographic) {
    record.properties_lexicographic.push_back(ToFrontendPropertyRecord(property));
  }
  record.methods_lexicographic.reserve(
      implementation.methods_lexicographic.size());
  for (const Objc3SemanticMethodTypeMetadata &method :
       implementation.methods_lexicographic) {
    record.methods_lexicographic.push_back(ToFrontendMethodRecord(method));
  }
  return record;
}
