bool IsValidObjc3TypeSystemOptionalKeypathLoweringContract(
    const Objc3TypeSystemOptionalKeypathLoweringContract &contract) {
  if (contract.optional_binding_clause_sites > contract.optional_binding_sites ||
      contract.typed_keypath_self_root_sites >
          contract.typed_keypath_literal_sites ||
      contract.typed_keypath_class_root_sites >
          contract.typed_keypath_literal_sites) {
    return false;
  }
  if (contract.live_optional_lowering_sites !=
      contract.optional_binding_sites + contract.optional_send_sites +
          contract.nil_coalescing_sites) {
    return false;
  }
  if (contract.single_evaluation_nil_short_circuit_sites !=
      contract.optional_binding_clause_sites + contract.optional_send_sites +
          contract.nil_coalescing_sites) {
    return false;
  }
  if (contract.live_typed_keypath_artifact_sites +
          contract.deferred_typed_keypath_sites !=
      contract.typed_keypath_literal_sites) {
    return false;
  }
  if (contract.contract_violation_sites >
      contract.live_optional_lowering_sites +
          contract.live_typed_keypath_artifact_sites +
          contract.deferred_typed_keypath_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3TypeSystemOptionalKeypathLoweringReplayKey(
    const Objc3TypeSystemOptionalKeypathLoweringContract &contract) {
  return std::string("optional_binding_sites=") +
         std::to_string(contract.optional_binding_sites) +
         ";optional_binding_clause_sites=" +
         std::to_string(contract.optional_binding_clause_sites) +
         ";optional_send_sites=" +
         std::to_string(contract.optional_send_sites) +
         ";nil_coalescing_sites=" +
         std::to_string(contract.nil_coalescing_sites) +
         ";typed_keypath_literal_sites=" +
         std::to_string(contract.typed_keypath_literal_sites) +
         ";typed_keypath_self_root_sites=" +
         std::to_string(contract.typed_keypath_self_root_sites) +
         ";typed_keypath_class_root_sites=" +
         std::to_string(contract.typed_keypath_class_root_sites) +
         ";live_optional_lowering_sites=" +
         std::to_string(contract.live_optional_lowering_sites) +
         ";single_evaluation_nil_short_circuit_sites=" +
         std::to_string(contract.single_evaluation_nil_short_circuit_sites) +
         ";live_typed_keypath_artifact_sites=" +
         std::to_string(contract.live_typed_keypath_artifact_sites) +
         ";deferred_typed_keypath_sites=" +
         std::to_string(contract.deferred_typed_keypath_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3TypeSystemOptionalKeypathLoweringLaneContract;
}

std::string Objc3TypeSystemOptionalKeypathLoweringSummary() {
  std::ostringstream out;
  // optional chaining lowering anchor: `?.member` now desugars onto
  // the same optional-send ABI and nil-short-circuit path already used by
  // bracketed optional sends, so the live lowering packet truthfully covers
  // optional-member access. The later lowering step now widens the same packet
  // to cover validated typed key-path descriptor emission and stable runtime
  // handles without claiming full key-path application/runtime evaluation yet.
  out << "contract_id=" << kObjc3TypeSystemOptionalKeypathLoweringContractId
      << ";optional_model="
      << kObjc3TypeSystemOptionalKeypathLoweringOptionalModel
      << ";typed_keypath_model="
      << kObjc3TypeSystemOptionalKeypathLoweringTypedKeypathModel
      << ";authority_model="
      << kObjc3TypeSystemOptionalKeypathLoweringAuthorityModel
      << ";fail_closed_model="
      << kObjc3TypeSystemOptionalKeypathLoweringFailClosedModel
      << ";lane_contract="
      << kObjc3TypeSystemOptionalKeypathLoweringLaneContract;
  return out.str();
}

std::string Objc3TypeSystemOptionalKeypathRuntimeHelperContractSummary() {
  std::ostringstream out;
  // live-optional-send-and-keypath-runtime-support anchor: optional
  // sends stay on the public selector lookup/dispatch ABI while validated
  // single-component typed key-path handles now feed the private runtime
  // registry/testing helper surface without falsely claiming full
  // multi-component key-path evaluation.
  out << "contract_id=" << kObjc3TypeSystemOptionalKeypathRuntimeHelperContractId
      << ";surface_path=" << kObjc3TypeSystemOptionalKeypathRuntimeHelperSurfacePath
      << ";optional_model="
      << kObjc3TypeSystemOptionalKeypathRuntimeHelperOptionalModel
      << ";typed_keypath_model="
      << kObjc3TypeSystemOptionalKeypathRuntimeHelperTypedKeypathModel
      << ";diagnostic_model="
      << kObjc3TypeSystemOptionalKeypathRuntimeHelperDiagnosticModel
      << ";lookup_selector_symbol="
      << kObjc3RuntimeSupportLibraryLookupSelectorSymbol
      << ";dispatch_i32_symbol="
      << kObjc3RuntimeSupportLibraryDispatchI32Symbol
      << ";keypath_descriptor_section="
      << kObjc3RuntimeKeypathDescriptorLogicalSection
      << ";keypath_descriptor_aggregate=__objc3_sec_keypath_descriptors"
      << ";typed_keypath_runtime_execution_helper_landed=true";
  return out.str();
}
