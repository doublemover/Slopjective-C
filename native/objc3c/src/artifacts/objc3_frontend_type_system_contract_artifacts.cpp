#include "artifacts/objc3_frontend_type_system_contract_artifacts.h"

#include <algorithm>
#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

#include "ast/objc3_ast_contracts.h"
#include "io/objc3_json.h"
#include "lower/contracts/runtime_metadata_source_record_contracts.h"
#include "runtime/metadata/property_metadata.h"
#include "sema/model/frontend_type_source_closure.h"
#include "sema/objc3_semantic_type_metadata_handoff.h"

namespace objc3::artifacts::frontend {

Objc3TypeSystemOptionalKeypathLoweringContract
BuildTypeSystemOptionalKeypathLoweringContract(
    const Objc3TypeSystemTypeSemanticModelSummary &summary) {
  Objc3TypeSystemOptionalKeypathLoweringContract contract;
  contract.optional_binding_sites = summary.optional_binding_sites;
  contract.optional_binding_clause_sites =
      summary.optional_binding_clause_sites;
  contract.optional_send_sites = summary.optional_send_sites;
  contract.nil_coalescing_sites = summary.nil_coalescing_sites;
  contract.typed_keypath_literal_sites = summary.typed_keypath_literal_sites;
  contract.typed_keypath_self_root_sites =
      summary.typed_keypath_self_root_sites;
  contract.typed_keypath_class_root_sites =
      summary.typed_keypath_class_root_sites;
  contract.live_optional_lowering_sites =
      summary.optional_binding_sites + summary.optional_send_sites +
      summary.nil_coalescing_sites;
  contract.single_evaluation_nil_short_circuit_sites =
      summary.optional_binding_clause_sites + summary.optional_send_sites +
      summary.nil_coalescing_sites;
  contract.live_typed_keypath_artifact_sites =
      summary.typed_keypath_literal_sites;
  contract.deferred_typed_keypath_sites = 0;
  contract.contract_violation_sites =
      summary.optional_binding_contract_violation_sites +
      summary.optional_send_contract_violation_sites +
      summary.optional_flow_contract_violation_sites;
  contract.deterministic =
      summary.deterministic && contract.contract_violation_sites == 0 &&
      contract.live_typed_keypath_artifact_sites +
              contract.deferred_typed_keypath_sites ==
          contract.typed_keypath_literal_sites;
  return contract;
}

Objc3LightweightGenericsConstraintLoweringContract
BuildLightweightGenericsConstraintLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3LightweightGenericsConstraintLoweringContract contract;
  const std::size_t raw_sites =
      sema_parity_surface.lightweight_generic_constraint_sites_total;
  const std::size_t raw_generic_suffix_sites =
      sema_parity_surface
          .lightweight_generic_constraint_generic_suffix_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface
          .lightweight_generic_constraint_object_pointer_type_sites_total;
  const std::size_t raw_terminated_generic_suffix_sites =
      sema_parity_surface
          .lightweight_generic_constraint_terminated_generic_suffix_sites_total;
  const std::size_t raw_pointer_declarator_sites =
      sema_parity_surface
          .lightweight_generic_constraint_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.lightweight_generic_constraint_normalized_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface
          .lightweight_generic_constraint_contract_violation_sites_total;

  contract.generic_constraint_sites =
      std::max({raw_sites, raw_generic_suffix_sites, raw_object_pointer_sites,
                raw_pointer_declarator_sites, raw_normalized_sites,
                raw_violation_sites});
  contract.generic_suffix_sites =
      std::min(raw_generic_suffix_sites, contract.generic_constraint_sites);
  contract.object_pointer_type_sites =
      std::min(raw_object_pointer_sites, contract.generic_constraint_sites);
  contract.terminated_generic_suffix_sites = std::min(
      raw_terminated_generic_suffix_sites, contract.generic_suffix_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_declarator_sites, contract.generic_constraint_sites);
  contract.normalized_constraint_sites =
      std::min(raw_normalized_sites, contract.generic_constraint_sites);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.generic_constraint_sites);

  contract.deterministic =
      sema_parity_surface.lightweight_generic_constraint_summary
          .deterministic &&
      sema_parity_surface.deterministic_lightweight_generic_constraint_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_constraint_sites ==
          contract.generic_constraint_sites;
  return contract;
}

Objc3NullabilityFlowWarningPrecisionLoweringContract
BuildNullabilityFlowWarningPrecisionLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3NullabilityFlowWarningPrecisionLoweringContract contract;
  contract.nullability_flow_sites =
      sema_parity_surface.nullability_flow_sites_total;
  contract.object_pointer_type_sites = std::max(
      sema_parity_surface.nullability_flow_object_pointer_type_sites_total,
      sema_parity_surface.nullability_flow_nullability_suffix_sites_total);
  contract.nullability_suffix_sites =
      sema_parity_surface.nullability_flow_nullability_suffix_sites_total;
  contract.nullable_suffix_sites =
      sema_parity_surface.nullability_flow_nullable_suffix_sites_total;
  contract.nonnull_suffix_sites =
      sema_parity_surface.nullability_flow_nonnull_suffix_sites_total;
  contract.normalized_sites =
      sema_parity_surface.nullability_flow_normalized_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.nullability_flow_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.nullability_flow_warning_precision_summary
          .deterministic &&
      sema_parity_surface
          .deterministic_nullability_flow_warning_precision_handoff;
  return contract;
}

Objc3ProtocolQualifiedObjectTypeLoweringContract
BuildProtocolQualifiedObjectTypeLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3ProtocolQualifiedObjectTypeLoweringContract contract;
  const std::size_t raw_protocol_sites =
      sema_parity_surface.protocol_qualified_object_type_sites_total;
  const std::size_t raw_protocol_composition_sites =
      sema_parity_surface
          .protocol_qualified_object_type_protocol_composition_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface
          .protocol_qualified_object_type_object_pointer_type_sites_total;
  const std::size_t raw_terminated_sites =
      sema_parity_surface
          .protocol_qualified_object_type_terminated_protocol_composition_sites_total;
  const std::size_t raw_pointer_sites =
      sema_parity_surface
          .protocol_qualified_object_type_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface
          .protocol_qualified_object_type_normalized_protocol_composition_sites_total;
  const std::size_t raw_contract_violation_sites =
      sema_parity_surface
          .protocol_qualified_object_type_contract_violation_sites_total;

  contract.protocol_qualified_object_type_sites =
      std::max({raw_protocol_sites, raw_protocol_composition_sites,
                raw_pointer_sites, raw_normalized_sites,
                raw_contract_violation_sites});
  contract.protocol_composition_sites = std::min(
      raw_protocol_composition_sites,
      contract.protocol_qualified_object_type_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.protocol_composition_sites);
  contract.terminated_protocol_composition_sites =
      std::min(raw_terminated_sites, contract.protocol_composition_sites);
  contract.pointer_declarator_sites = std::min(
      raw_pointer_sites, contract.protocol_qualified_object_type_sites);
  contract.normalized_protocol_composition_sites =
      std::min(raw_normalized_sites,
               contract.protocol_qualified_object_type_sites);
  contract.contract_violation_sites =
      std::min(raw_contract_violation_sites,
               contract.protocol_qualified_object_type_sites);

  const bool strict_deterministic =
      sema_parity_surface.protocol_qualified_object_type_summary
          .deterministic &&
      sema_parity_surface.deterministic_protocol_qualified_object_type_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_protocol_composition_sites ==
          contract.protocol_qualified_object_type_sites;
  contract.deterministic = strict_deterministic;
  return contract;
}

Objc3VarianceBridgeCastLoweringContract BuildVarianceBridgeCastLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3VarianceBridgeCastLoweringContract contract;
  const std::size_t raw_sites =
      sema_parity_surface.variance_bridge_cast_sites_total;
  const std::size_t raw_protocol_sites =
      sema_parity_surface
          .variance_bridge_cast_protocol_composition_sites_total;
  const std::size_t raw_ownership_sites =
      sema_parity_surface.variance_bridge_cast_ownership_qualifier_sites_total;
  const std::size_t raw_object_sites =
      sema_parity_surface.variance_bridge_cast_object_pointer_type_sites_total;
  const std::size_t raw_pointer_sites =
      sema_parity_surface.variance_bridge_cast_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.variance_bridge_cast_normalized_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.variance_bridge_cast_contract_violation_sites_total;

  contract.variance_bridge_cast_sites =
      std::max({raw_sites, raw_protocol_sites, raw_ownership_sites,
                raw_pointer_sites, raw_normalized_sites, raw_violation_sites});
  contract.protocol_composition_sites =
      std::min(raw_protocol_sites, contract.variance_bridge_cast_sites);
  contract.ownership_qualifier_sites =
      std::min(raw_ownership_sites, contract.variance_bridge_cast_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_sites, contract.protocol_composition_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_sites, contract.variance_bridge_cast_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.variance_bridge_cast_sites);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.variance_bridge_cast_sites);

  contract.deterministic =
      sema_parity_surface.variance_bridge_cast_summary.deterministic &&
      sema_parity_surface.deterministic_variance_bridge_cast_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_sites == contract.variance_bridge_cast_sites;
  return contract;
}

Objc3GenericMetadataAbiLoweringContract BuildGenericMetadataAbiLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3GenericMetadataAbiLoweringContract contract;
  const std::size_t raw_sites =
      sema_parity_surface.generic_metadata_abi_sites_total;
  const std::size_t raw_generic_suffix_sites =
      sema_parity_surface.generic_metadata_abi_generic_suffix_sites_total;
  const std::size_t raw_protocol_sites =
      sema_parity_surface.generic_metadata_abi_protocol_composition_sites_total;
  const std::size_t raw_ownership_sites =
      sema_parity_surface.generic_metadata_abi_ownership_qualifier_sites_total;
  const std::size_t raw_object_sites =
      sema_parity_surface.generic_metadata_abi_object_pointer_type_sites_total;
  const std::size_t raw_pointer_sites =
      sema_parity_surface.generic_metadata_abi_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.generic_metadata_abi_normalized_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.generic_metadata_abi_contract_violation_sites_total;

  contract.generic_metadata_abi_sites =
      std::max({raw_sites, raw_generic_suffix_sites, raw_protocol_sites,
                raw_ownership_sites, raw_pointer_sites, raw_normalized_sites,
                raw_violation_sites});
  contract.generic_suffix_sites =
      std::min(raw_generic_suffix_sites, contract.generic_metadata_abi_sites);
  contract.protocol_composition_sites =
      std::min(raw_protocol_sites, contract.generic_metadata_abi_sites);
  contract.ownership_qualifier_sites =
      std::min(raw_ownership_sites, contract.generic_metadata_abi_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_sites, contract.protocol_composition_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_sites, contract.generic_metadata_abi_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.generic_metadata_abi_sites);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.generic_metadata_abi_sites);

  contract.deterministic =
      sema_parity_surface.generic_metadata_abi_summary.deterministic &&
      sema_parity_surface.deterministic_generic_metadata_abi_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_sites == contract.generic_metadata_abi_sites;
  return contract;
}

namespace {

std::string BuildStringArrayJson(const std::vector<std::string> &values) {
  std::ostringstream out;
  out << "[";
  for (std::size_t i = 0; i < values.size(); ++i) {
    if (i != 0u) {
      out << ",";
    }
    out << "\"" << EscapeJsonString(values[i]) << "\"";
  }
  out << "]";
  return out.str();
}

struct Objc3TypeSystemGenericContractInventory {
  std::size_t interface_count = 0;
  std::size_t generic_interface_count = 0;
  std::size_t generic_parameter_count = 0;
  std::size_t generic_variance_annotation_count = 0;
  std::size_t generic_argument_reference_count = 0;
  std::size_t protocol_qualified_generic_argument_count = 0;
};

bool IsProtocolQualifiedGenericArgumentSpelling(const std::string &argument) {
  return argument.find("id<") != std::string::npos ||
         argument.find("Class<") != std::string::npos;
}

void AccumulateGenericContractInventory(
    const Objc3SemanticCanonicalType &type,
    Objc3TypeSystemGenericContractInventory &inventory) {
  inventory.generic_argument_reference_count +=
      type.generic_arguments_source_order.size();
  for (const std::string &argument : type.generic_arguments_source_order) {
    if (IsProtocolQualifiedGenericArgumentSpelling(argument)) {
      ++inventory.protocol_qualified_generic_argument_count;
    }
  }
}

Objc3TypeSystemGenericContractInventory BuildTypeSystemGenericContractInventory(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3TypeSystemGenericContractInventory inventory;
  inventory.interface_count = handoff.interfaces_lexicographic.size();
  for (const Objc3SemanticInterfaceTypeMetadata &interface_metadata :
       handoff.interfaces_lexicographic) {
    if (!interface_metadata.generic_parameter_names_source_order.empty()) {
      ++inventory.generic_interface_count;
    }
    inventory.generic_parameter_count +=
        interface_metadata.generic_parameter_names_source_order.size();
    inventory.generic_variance_annotation_count +=
        interface_metadata.generic_parameter_variance_source_order.size();
    for (const Objc3SemanticPropertyTypeMetadata &property :
         interface_metadata.properties_lexicographic) {
      AccumulateGenericContractInventory(property.canonical_type, inventory);
    }
    for (const Objc3SemanticMethodTypeMetadata &method :
         interface_metadata.methods_lexicographic) {
      AccumulateGenericContractInventory(method.return_canonical_type,
                                         inventory);
      for (const Objc3SemanticCanonicalType &param :
           method.param_canonical_types) {
        AccumulateGenericContractInventory(param, inventory);
      }
    }
  }
  for (const Objc3SemanticFunctionTypeMetadata &function :
       handoff.functions_lexicographic) {
    AccumulateGenericContractInventory(function.return_canonical_type,
                                       inventory);
    for (const Objc3SemanticCanonicalType &param :
         function.param_canonical_types) {
      AccumulateGenericContractInventory(param, inventory);
    }
  }
  for (const Objc3SemanticImplementationTypeMetadata &implementation :
       handoff.implementations_lexicographic) {
    for (const Objc3SemanticPropertyTypeMetadata &property :
         implementation.properties_lexicographic) {
      AccumulateGenericContractInventory(property.canonical_type, inventory);
    }
    for (const Objc3SemanticMethodTypeMetadata &method :
         implementation.methods_lexicographic) {
      AccumulateGenericContractInventory(method.return_canonical_type,
                                         inventory);
      for (const Objc3SemanticCanonicalType &param :
           method.param_canonical_types) {
        AccumulateGenericContractInventory(param, inventory);
      }
    }
  }
  return inventory;
}

std::string BuildTypeSystemGenericContractPreservationReplayKey(
    const Objc3TypeSystemGenericContractInventory &inventory,
    const Objc3TypeSystemTypeSemanticModelSummary &semantic_summary) {
  std::ostringstream out;
  out << kObjc3TypeSystemGenericContractPreservationContractId
      << ";source_contract=" << kObjc3TypeSystemTypeSemanticModelContractId
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

struct Objc3TypeSystemProtocolContractInventory {
  std::size_t protocol_decl_count = 0;
  std::size_t protocol_forward_declaration_count = 0;
  std::size_t protocol_inheritance_edge_count = 0;
  std::size_t protocol_required_method_count = 0;
  std::size_t protocol_optional_method_count = 0;
  std::size_t protocol_required_property_count = 0;
  std::size_t protocol_optional_property_count = 0;
  std::size_t class_protocol_adoption_count = 0;
  std::size_t category_protocol_adoption_count = 0;
};

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

Objc3TypeSystemProtocolContractInventory
BuildTypeSystemProtocolContractInventory(
    const Objc3Program &program,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_records) {
  Objc3TypeSystemProtocolContractInventory inventory;
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
    const Objc3TypeSystemProtocolContractInventory &inventory,
    const Objc3TypeSystemTypeSemanticModelSummary &semantic_summary) {
  std::ostringstream out;
  out << kObjc3TypeSystemProtocolContractPreservationContractId
      << ";source_contract=" << kObjc3TypeSystemTypeSemanticModelContractId
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

}  // namespace

std::string BuildTypeSystemGenericContractPreservationJson(
    const Objc3SemanticTypeMetadataHandoff &handoff,
    const Objc3TypeSystemTypeSemanticModelSummary &semantic_summary) {
  const Objc3TypeSystemGenericContractInventory inventory =
      BuildTypeSystemGenericContractInventory(handoff);
  const bool deterministic = IsDeterministicSemanticTypeMetadataHandoff(handoff);
  const bool ready =
      deterministic && semantic_summary.ready_for_lowering_and_runtime &&
      semantic_summary.deterministic && !semantic_summary.replay_key.empty();
  const std::string replay_key =
      BuildTypeSystemGenericContractPreservationReplayKey(inventory,
                                                          semantic_summary);
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3TypeSystemGenericContractPreservationContractId)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(kObjc3TypeSystemTypeSemanticModelContractId)
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
    const Objc3SemanticInterfaceTypeMetadata &interface_metadata =
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

std::string BuildTypeSystemProtocolContractPreservationJson(
    const Objc3Program &program,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_records,
    const Objc3TypeSystemTypeSemanticModelSummary &semantic_summary) {
  const Objc3TypeSystemProtocolContractInventory inventory =
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
             kObjc3TypeSystemProtocolContractPreservationContractId)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(kObjc3TypeSystemTypeSemanticModelContractId)
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

}  // namespace objc3::artifacts::frontend
