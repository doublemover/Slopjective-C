#include "artifacts/objc3_frontend_type_system_contract_artifacts.h"

#include <string>

#include "lower/contracts/optional_keypath_lowering_contracts.h"
#include "sema/objc3_sema_contract_core.h"
#include "sema/objc3_sema_contract_type_handoff.h"
#include "sema/objc3_sema_pass_manager_contract_flow.h"

namespace objc3::artifacts::frontend {
namespace {

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

}  // namespace

Objc3FrontendTypeSystemSemanticModelRecord
BuildFrontendTypeSystemSemanticModelRecord(
    const Objc3TypeSystemTypeSemanticModelSummary &summary) {
  Objc3FrontendTypeSystemSemanticModelRecord record;
  record.contract_id = summary.contract_id;
  record.optional_binding_sites = summary.optional_binding_sites;
  record.optional_binding_clause_sites = summary.optional_binding_clause_sites;
  record.optional_send_sites = summary.optional_send_sites;
  record.nil_coalescing_sites = summary.nil_coalescing_sites;
  record.typed_keypath_literal_sites = summary.typed_keypath_literal_sites;
  record.typed_keypath_self_root_sites = summary.typed_keypath_self_root_sites;
  record.typed_keypath_class_root_sites =
      summary.typed_keypath_class_root_sites;
  record.canonical_type_entries = summary.canonical_type_entries;
  record.canonical_object_type_entries =
      summary.canonical_object_type_entries;
  record.canonical_nullable_entries = summary.canonical_nullable_entries;
  record.canonical_nonnull_entries = summary.canonical_nonnull_entries;
  record.canonical_implicitly_unwrapped_entries =
      summary.canonical_implicitly_unwrapped_entries;
  record.canonical_null_resettable_entries =
      summary.canonical_null_resettable_entries;
  record.canonical_unspecified_nullability_entries =
      summary.canonical_unspecified_nullability_entries;
  record.canonical_invalid_type_entries =
      summary.canonical_invalid_type_entries;
  record.optional_binding_contract_violation_sites =
      summary.optional_binding_contract_violation_sites;
  record.optional_send_contract_violation_sites =
      summary.optional_send_contract_violation_sites;
  record.optional_flow_contract_violation_sites =
      summary.optional_flow_contract_violation_sites;
  record.deterministic = summary.deterministic;
  record.ready_for_lowering_and_runtime =
      summary.ready_for_lowering_and_runtime;
  record.replay_key = summary.replay_key;
  return record;
}

Objc3FrontendTypeSystemParitySurfaceRecord
BuildFrontendTypeSystemParitySurfaceRecord(
    const Objc3SemaParityContractSurface &surface) {
  Objc3FrontendTypeSystemParitySurfaceRecord record;
  record.lightweight_generic_constraint_sites_total =
      surface.lightweight_generic_constraint_sites_total;
  record.lightweight_generic_constraint_generic_suffix_sites_total =
      surface.lightweight_generic_constraint_generic_suffix_sites_total;
  record.lightweight_generic_constraint_object_pointer_type_sites_total =
      surface.lightweight_generic_constraint_object_pointer_type_sites_total;
  record.lightweight_generic_constraint_terminated_generic_suffix_sites_total =
      surface.lightweight_generic_constraint_terminated_generic_suffix_sites_total;
  record.lightweight_generic_constraint_pointer_declarator_sites_total =
      surface.lightweight_generic_constraint_pointer_declarator_sites_total;
  record.lightweight_generic_constraint_normalized_sites_total =
      surface.lightweight_generic_constraint_normalized_sites_total;
  record.lightweight_generic_constraint_contract_violation_sites_total =
      surface.lightweight_generic_constraint_contract_violation_sites_total;
  record.lightweight_generic_constraint_deterministic =
      surface.lightweight_generic_constraint_summary.deterministic;
  record.deterministic_lightweight_generic_constraint_handoff =
      surface.deterministic_lightweight_generic_constraint_handoff;
  record.nullability_flow_sites_total =
      surface.nullability_flow_sites_total;
  record.nullability_flow_object_pointer_type_sites_total =
      surface.nullability_flow_object_pointer_type_sites_total;
  record.nullability_flow_nullability_suffix_sites_total =
      surface.nullability_flow_nullability_suffix_sites_total;
  record.nullability_flow_nullable_suffix_sites_total =
      surface.nullability_flow_nullable_suffix_sites_total;
  record.nullability_flow_nonnull_suffix_sites_total =
      surface.nullability_flow_nonnull_suffix_sites_total;
  record.nullability_flow_normalized_sites_total =
      surface.nullability_flow_normalized_sites_total;
  record.nullability_flow_contract_violation_sites_total =
      surface.nullability_flow_contract_violation_sites_total;
  record.nullability_flow_warning_precision_deterministic =
      surface.nullability_flow_warning_precision_summary.deterministic;
  record.deterministic_nullability_flow_warning_precision_handoff =
      surface.deterministic_nullability_flow_warning_precision_handoff;
  record.protocol_qualified_object_type_sites_total =
      surface.protocol_qualified_object_type_sites_total;
  record.protocol_qualified_object_type_protocol_composition_sites_total =
      surface.protocol_qualified_object_type_protocol_composition_sites_total;
  record.protocol_qualified_object_type_object_pointer_type_sites_total =
      surface.protocol_qualified_object_type_object_pointer_type_sites_total;
  record.protocol_qualified_object_type_terminated_protocol_composition_sites_total =
      surface.protocol_qualified_object_type_terminated_protocol_composition_sites_total;
  record.protocol_qualified_object_type_pointer_declarator_sites_total =
      surface.protocol_qualified_object_type_pointer_declarator_sites_total;
  record.protocol_qualified_object_type_normalized_protocol_composition_sites_total =
      surface.protocol_qualified_object_type_normalized_protocol_composition_sites_total;
  record.protocol_qualified_object_type_contract_violation_sites_total =
      surface.protocol_qualified_object_type_contract_violation_sites_total;
  record.protocol_qualified_object_type_deterministic =
      surface.protocol_qualified_object_type_summary.deterministic;
  record.deterministic_protocol_qualified_object_type_handoff =
      surface.deterministic_protocol_qualified_object_type_handoff;
  record.variance_bridge_cast_sites_total =
      surface.variance_bridge_cast_sites_total;
  record.variance_bridge_cast_protocol_composition_sites_total =
      surface.variance_bridge_cast_protocol_composition_sites_total;
  record.variance_bridge_cast_ownership_qualifier_sites_total =
      surface.variance_bridge_cast_ownership_qualifier_sites_total;
  record.variance_bridge_cast_object_pointer_type_sites_total =
      surface.variance_bridge_cast_object_pointer_type_sites_total;
  record.variance_bridge_cast_pointer_declarator_sites_total =
      surface.variance_bridge_cast_pointer_declarator_sites_total;
  record.variance_bridge_cast_normalized_sites_total =
      surface.variance_bridge_cast_normalized_sites_total;
  record.variance_bridge_cast_contract_violation_sites_total =
      surface.variance_bridge_cast_contract_violation_sites_total;
  record.variance_bridge_cast_deterministic =
      surface.variance_bridge_cast_summary.deterministic;
  record.deterministic_variance_bridge_cast_handoff =
      surface.deterministic_variance_bridge_cast_handoff;
  record.generic_metadata_abi_sites_total =
      surface.generic_metadata_abi_sites_total;
  record.generic_metadata_abi_generic_suffix_sites_total =
      surface.generic_metadata_abi_generic_suffix_sites_total;
  record.generic_metadata_abi_protocol_composition_sites_total =
      surface.generic_metadata_abi_protocol_composition_sites_total;
  record.generic_metadata_abi_ownership_qualifier_sites_total =
      surface.generic_metadata_abi_ownership_qualifier_sites_total;
  record.generic_metadata_abi_object_pointer_type_sites_total =
      surface.generic_metadata_abi_object_pointer_type_sites_total;
  record.generic_metadata_abi_pointer_declarator_sites_total =
      surface.generic_metadata_abi_pointer_declarator_sites_total;
  record.generic_metadata_abi_normalized_sites_total =
      surface.generic_metadata_abi_normalized_sites_total;
  record.generic_metadata_abi_contract_violation_sites_total =
      surface.generic_metadata_abi_contract_violation_sites_total;
  record.generic_metadata_abi_deterministic =
      surface.generic_metadata_abi_summary.deterministic;
  record.deterministic_generic_metadata_abi_handoff =
      surface.deterministic_generic_metadata_abi_handoff;
  return record;
}

Objc3FrontendTypeSystemSemanticMetadataRecord
BuildFrontendTypeSystemSemanticMetadataRecord(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3FrontendTypeSystemSemanticMetadataRecord record;
  record.deterministic = IsDeterministicSemanticTypeMetadataHandoff(handoff);
  record.functions_lexicographic.reserve(handoff.functions_lexicographic.size());
  for (const Objc3SemanticFunctionTypeMetadata &function :
       handoff.functions_lexicographic) {
    record.functions_lexicographic.push_back(ToFrontendFunctionRecord(function));
  }
  record.interfaces_lexicographic.reserve(
      handoff.interfaces_lexicographic.size());
  for (const Objc3SemanticInterfaceTypeMetadata &interface_metadata :
       handoff.interfaces_lexicographic) {
    record.interfaces_lexicographic.push_back(
        ToFrontendInterfaceRecord(interface_metadata));
  }
  record.implementations_lexicographic.reserve(
      handoff.implementations_lexicographic.size());
  for (const Objc3SemanticImplementationTypeMetadata &implementation :
       handoff.implementations_lexicographic) {
    record.implementations_lexicographic.push_back(
        ToFrontendImplementationRecord(implementation));
  }
  return record;
}

Objc3TypeSystemOptionalKeypathLoweringContract
BuildTypeSystemOptionalKeypathLoweringContract(
    const Objc3TypeSystemTypeSemanticModelSummary &summary) {
  return ToLowerOptionalKeypathLoweringContract(
      BuildFrontendTypeSystemOptionalKeypathLoweringContract(
          BuildFrontendTypeSystemSemanticModelRecord(summary)));
}

Objc3FrontendLightweightGenericsConstraintLoweringContractRecord
BuildLightweightGenericsConstraintLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  return BuildLightweightGenericsConstraintLoweringContract(
      BuildFrontendTypeSystemParitySurfaceRecord(sema_parity_surface));
}

Objc3FrontendNullabilityFlowWarningPrecisionLoweringContractRecord
BuildNullabilityFlowWarningPrecisionLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  return BuildNullabilityFlowWarningPrecisionLoweringContract(
      BuildFrontendTypeSystemParitySurfaceRecord(sema_parity_surface));
}

Objc3FrontendProtocolQualifiedObjectTypeLoweringContractRecord
BuildProtocolQualifiedObjectTypeLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  return BuildProtocolQualifiedObjectTypeLoweringContract(
      BuildFrontendTypeSystemParitySurfaceRecord(sema_parity_surface));
}

Objc3FrontendVarianceBridgeCastLoweringContractRecord
BuildVarianceBridgeCastLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  return BuildVarianceBridgeCastLoweringContract(
      BuildFrontendTypeSystemParitySurfaceRecord(sema_parity_surface));
}

Objc3FrontendGenericMetadataAbiLoweringContractRecord
BuildGenericMetadataAbiLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  return BuildGenericMetadataAbiLoweringContract(
      BuildFrontendTypeSystemParitySurfaceRecord(sema_parity_surface));
}

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

}  // namespace objc3::artifacts::frontend
