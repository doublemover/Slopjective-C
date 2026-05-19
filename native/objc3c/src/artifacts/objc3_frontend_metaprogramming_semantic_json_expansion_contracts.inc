std::string BuildMetaprogrammingExpansionLoweringContractJson(
    const Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary
        &property_source_summary,
    const Objc3MetaprogrammingDeriveExpansionInventorySummary &derive_summary,
    const Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary &macro_summary,
    const Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary
        &property_legality_summary,
    const Objc3MetaprogrammingExpansionLoweringContract &contract,
    const std::string &replay_key) {
  const bool ready_for_ir_emission =
      contract.deterministic &&
      IsValidObjc3MetaprogrammingExpansionLoweringContract(contract);
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3MetaprogrammingExpansionLoweringContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3MetaprogrammingExpansionLoweringSurfacePath)
      << "\",\"property_source_contract_id\":\""
      << EscapeJsonString(property_source_summary.contract_id)
      << "\",\"derive_contract_id\":\""
      << EscapeJsonString(derive_summary.contract_id)
      << "\",\"macro_contract_id\":\""
      << EscapeJsonString(macro_summary.contract_id)
      << "\",\"property_legality_contract_id\":\""
      << EscapeJsonString(property_legality_summary.contract_id)
      << "\",\"lane_contract_id\":\""
      << EscapeJsonString(kObjc3MetaprogrammingExpansionLoweringLaneContract)
      << "\",\"lowering_model\":\""
      << EscapeJsonString(kObjc3MetaprogrammingExpansionLoweringModel)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(kObjc3MetaprogrammingExpansionLoweringDeferredModel)
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key)
      << "\",\"derive_inventory_sites\":" << contract.derive_inventory_sites
      << ",\"derived_selector_artifact_sites\":"
      << contract.derived_selector_artifact_sites
      << ",\"macro_replay_visible_sites\":"
      << contract.macro_replay_visible_sites
      << ",\"property_behavior_sites\":" << contract.property_behavior_sites
      << ",\"synthesized_binding_sites\":"
      << contract.synthesized_binding_sites
      << ",\"synthesized_getter_sites\":"
      << contract.synthesized_getter_sites
      << ",\"synthesized_setter_sites\":"
      << contract.synthesized_setter_sites
      << ",\"replay_visible_metadata_sites\":"
      << contract.replay_visible_metadata_sites
      << ",\"guard_blocked_sites\":" << contract.guard_blocked_sites
      << ",\"contract_violation_sites\":"
      << contract.contract_violation_sites
      << ",\"deterministic_handoff\":"
      << (contract.deterministic ? "true" : "false")
      << ",\"ready_for_ir_emission\":"
      << (ready_for_ir_emission ? "true" : "false")
      << "}";
  return out.str();
}

std::string BuildMetaprogrammingSynthesizedArtifactEmissionContractJson(
    const Objc3MetaprogrammingExpansionLoweringContract &dependency_contract,
    const Objc3MetaprogrammingSynthesizedArtifactEmissionContract &contract,
    const std::string &replay_key) {
  const bool ready_for_ir_emission =
      contract.deterministic &&
      IsValidObjc3MetaprogrammingSynthesizedArtifactEmissionContract(contract);
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(
             kObjc3MetaprogrammingSynthesizedArtifactEmissionContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(
             kObjc3MetaprogrammingSynthesizedArtifactEmissionSurfacePath)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(kObjc3MetaprogrammingExpansionLoweringContractId)
      << "\",\"lane_contract_id\":\""
      << EscapeJsonString(
             kObjc3MetaprogrammingSynthesizedArtifactEmissionLaneContract)
      << "\",\"emission_model\":\""
      << EscapeJsonString(kObjc3MetaprogrammingSynthesizedArtifactEmissionModel)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(
             kObjc3MetaprogrammingSynthesizedArtifactEmissionDeferredModel)
      << "\",\"dependency_replay_key\":\""
      << EscapeJsonString(Objc3MetaprogrammingExpansionLoweringReplayKey(
             dependency_contract))
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key)
      << "\",\"derive_inventory_sites\":" << contract.derive_inventory_sites
      << ",\"emitted_derive_method_sites\":"
      << contract.emitted_derive_method_sites
      << ",\"emitted_macro_artifact_sites\":"
      << contract.emitted_macro_artifact_sites
      << ",\"emitted_property_behavior_artifact_sites\":"
      << contract.emitted_property_behavior_artifact_sites
      << ",\"emitted_global_artifact_sites\":"
      << contract.emitted_global_artifact_sites
      << ",\"emitted_runtime_method_list_sites\":"
      << contract.emitted_runtime_method_list_sites
      << ",\"guard_blocked_sites\":" << contract.guard_blocked_sites
      << ",\"contract_violation_sites\":"
      << contract.contract_violation_sites
      << ",\"deterministic_handoff\":"
      << (contract.deterministic ? "true" : "false")
      << ",\"ready_for_ir_emission\":"
      << (ready_for_ir_emission ? "true" : "false")
      << "}";
  return out.str();
}
