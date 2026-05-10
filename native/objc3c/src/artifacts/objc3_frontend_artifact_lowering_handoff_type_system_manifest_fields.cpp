#include "artifacts/objc3_frontend_artifact_lowering_handoff_manifest_field_modules.h"

#include <string>

namespace objc3::artifacts::frontend {

void AppendObjc3FrontendArtifactLoweringHandoffTypeSystemManifestFields(
    std::ostringstream &manifest,
    const Objc3FrontendArtifactTypeSystemLoweringPlan
        &type_system_lowering_plan) {
  const auto &lightweight_generic_constraint_lowering_contract =
      type_system_lowering_plan.lightweight_generic_constraint_lowering_contract;
  const std::string &lightweight_generic_constraint_lowering_replay_key =
      type_system_lowering_plan
          .lightweight_generic_constraint_lowering_replay_key;
  const auto &nullability_flow_warning_precision_lowering_contract =
      type_system_lowering_plan
          .nullability_flow_warning_precision_lowering_contract;
  const std::string &nullability_flow_warning_precision_lowering_replay_key =
      type_system_lowering_plan
          .nullability_flow_warning_precision_lowering_replay_key;
  const auto &protocol_qualified_object_type_lowering_contract =
      type_system_lowering_plan
          .protocol_qualified_object_type_lowering_contract;
  const std::string &protocol_qualified_object_type_lowering_replay_key =
      type_system_lowering_plan
          .protocol_qualified_object_type_lowering_replay_key;
  const auto &variance_bridge_cast_lowering_contract =
      type_system_lowering_plan.variance_bridge_cast_lowering_contract;
  const std::string &variance_bridge_cast_lowering_replay_key =
      type_system_lowering_plan.variance_bridge_cast_lowering_replay_key;
  const auto &generic_metadata_abi_lowering_contract =
      type_system_lowering_plan.generic_metadata_abi_lowering_contract;
  const std::string &generic_metadata_abi_lowering_replay_key =
      type_system_lowering_plan.generic_metadata_abi_lowering_replay_key;

  manifest << ",\"deterministic_lightweight_generic_constraint_lowering_handoff\":"
           << (lightweight_generic_constraint_lowering_contract.deterministic ? "true" : "false")
           << ",\"lightweight_generic_constraint_lowering_sites\":"
           << lightweight_generic_constraint_lowering_contract.generic_constraint_sites
           << ",\"lightweight_generic_constraint_lowering_generic_suffix_sites\":"
           << lightweight_generic_constraint_lowering_contract.generic_suffix_sites
           << ",\"lightweight_generic_constraint_lowering_object_pointer_type_sites\":"
           << lightweight_generic_constraint_lowering_contract.object_pointer_type_sites
           << ",\"lightweight_generic_constraint_lowering_terminated_generic_suffix_sites\":"
           << lightweight_generic_constraint_lowering_contract.terminated_generic_suffix_sites
           << ",\"lightweight_generic_constraint_lowering_pointer_declarator_sites\":"
           << lightweight_generic_constraint_lowering_contract.pointer_declarator_sites
           << ",\"lightweight_generic_constraint_lowering_normalized_sites\":"
           << lightweight_generic_constraint_lowering_contract.normalized_constraint_sites
           << ",\"lightweight_generic_constraint_lowering_contract_violation_sites\":"
           << lightweight_generic_constraint_lowering_contract.contract_violation_sites
           << ",\"lowering_lightweight_generic_constraint_replay_key\":\""
           << lightweight_generic_constraint_lowering_replay_key
           << "\""
           << ",\"deterministic_nullability_flow_warning_precision_lowering_handoff\":"
           << (nullability_flow_warning_precision_lowering_contract.deterministic ? "true" : "false")
           << ",\"nullability_flow_warning_precision_lowering_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nullability_flow_sites
           << ",\"nullability_flow_warning_precision_lowering_object_pointer_type_sites\":"
           << nullability_flow_warning_precision_lowering_contract.object_pointer_type_sites
           << ",\"nullability_flow_warning_precision_lowering_nullability_suffix_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nullability_suffix_sites
           << ",\"nullability_flow_warning_precision_lowering_nullable_suffix_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nullable_suffix_sites
           << ",\"nullability_flow_warning_precision_lowering_nonnull_suffix_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nonnull_suffix_sites
           << ",\"nullability_flow_warning_precision_lowering_normalized_sites\":"
           << nullability_flow_warning_precision_lowering_contract.normalized_sites
           << ",\"nullability_flow_warning_precision_lowering_contract_violation_sites\":"
           << nullability_flow_warning_precision_lowering_contract.contract_violation_sites
           << ",\"lowering_nullability_flow_warning_precision_replay_key\":\""
           << nullability_flow_warning_precision_lowering_replay_key
           << "\""
           << ",\"deterministic_protocol_qualified_object_type_lowering_handoff\":"
           << (protocol_qualified_object_type_lowering_contract.deterministic ? "true" : "false")
           << ",\"protocol_qualified_object_type_lowering_sites\":"
           << protocol_qualified_object_type_lowering_contract.protocol_qualified_object_type_sites
           << ",\"protocol_qualified_object_type_lowering_protocol_composition_sites\":"
           << protocol_qualified_object_type_lowering_contract.protocol_composition_sites
           << ",\"protocol_qualified_object_type_lowering_object_pointer_type_sites\":"
           << protocol_qualified_object_type_lowering_contract.object_pointer_type_sites
           << ",\"protocol_qualified_object_type_lowering_terminated_protocol_composition_sites\":"
           << protocol_qualified_object_type_lowering_contract.terminated_protocol_composition_sites
           << ",\"protocol_qualified_object_type_lowering_pointer_declarator_sites\":"
           << protocol_qualified_object_type_lowering_contract.pointer_declarator_sites
           << ",\"protocol_qualified_object_type_lowering_normalized_protocol_composition_sites\":"
           << protocol_qualified_object_type_lowering_contract.normalized_protocol_composition_sites
           << ",\"protocol_qualified_object_type_lowering_contract_violation_sites\":"
           << protocol_qualified_object_type_lowering_contract.contract_violation_sites
           << ",\"lowering_protocol_qualified_object_type_replay_key\":\""
           << protocol_qualified_object_type_lowering_replay_key
           << "\""
           << ",\"deterministic_variance_bridge_cast_lowering_handoff\":"
           << (variance_bridge_cast_lowering_contract.deterministic ? "true" : "false")
           << ",\"variance_bridge_cast_lowering_sites\":"
           << variance_bridge_cast_lowering_contract.variance_bridge_cast_sites
           << ",\"variance_bridge_cast_lowering_protocol_composition_sites\":"
           << variance_bridge_cast_lowering_contract.protocol_composition_sites
           << ",\"variance_bridge_cast_lowering_ownership_qualifier_sites\":"
           << variance_bridge_cast_lowering_contract.ownership_qualifier_sites
           << ",\"variance_bridge_cast_lowering_object_pointer_type_sites\":"
           << variance_bridge_cast_lowering_contract.object_pointer_type_sites
           << ",\"variance_bridge_cast_lowering_pointer_declarator_sites\":"
           << variance_bridge_cast_lowering_contract.pointer_declarator_sites
           << ",\"variance_bridge_cast_lowering_normalized_sites\":"
           << variance_bridge_cast_lowering_contract.normalized_sites
           << ",\"variance_bridge_cast_lowering_contract_violation_sites\":"
           << variance_bridge_cast_lowering_contract.contract_violation_sites
           << ",\"lowering_variance_bridge_cast_replay_key\":\""
           << variance_bridge_cast_lowering_replay_key
           << "\""
           << ",\"deterministic_generic_metadata_abi_lowering_handoff\":"
           << (generic_metadata_abi_lowering_contract.deterministic ? "true" : "false")
           << ",\"generic_metadata_abi_lowering_sites\":"
           << generic_metadata_abi_lowering_contract.generic_metadata_abi_sites
           << ",\"generic_metadata_abi_lowering_generic_suffix_sites\":"
           << generic_metadata_abi_lowering_contract.generic_suffix_sites
           << ",\"generic_metadata_abi_lowering_protocol_composition_sites\":"
           << generic_metadata_abi_lowering_contract.protocol_composition_sites
           << ",\"generic_metadata_abi_lowering_ownership_qualifier_sites\":"
           << generic_metadata_abi_lowering_contract.ownership_qualifier_sites
           << ",\"generic_metadata_abi_lowering_object_pointer_type_sites\":"
           << generic_metadata_abi_lowering_contract.object_pointer_type_sites
           << ",\"generic_metadata_abi_lowering_pointer_declarator_sites\":"
           << generic_metadata_abi_lowering_contract.pointer_declarator_sites
           << ",\"generic_metadata_abi_lowering_normalized_sites\":"
           << generic_metadata_abi_lowering_contract.normalized_sites
           << ",\"generic_metadata_abi_lowering_contract_violation_sites\":"
           << generic_metadata_abi_lowering_contract.contract_violation_sites
           << ",\"lowering_generic_metadata_abi_replay_key\":\""
           << generic_metadata_abi_lowering_replay_key
           << "\"";
}

}  // namespace objc3::artifacts::frontend
