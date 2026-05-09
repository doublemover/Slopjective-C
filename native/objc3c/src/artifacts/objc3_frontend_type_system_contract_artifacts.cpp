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
namespace {

using objc3::io::EscapeJsonString;

inline constexpr const char
    *kObjc3TypeSystemOptionalKeypathLoweringSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_type_system_optional_keypath_lowering_contract";

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

std::string BuildTypeSystemNullabilityContractPreservationReplayKey(
    const Objc3TypeSystemTypeSemanticModelSummary &summary) {
  std::ostringstream out;
  out << kObjc3TypeSystemNullabilityContractPreservationContractId
      << ";source_contract=" << kObjc3TypeSystemTypeSemanticModelContractId
      << ";type_semantic_replay=" << summary.replay_key
      << ";canonical_types=" << summary.canonical_type_entries
      << ";object_types=" << summary.canonical_object_type_entries
      << ";nullable=" << summary.canonical_nullable_entries
      << ";nonnull=" << summary.canonical_nonnull_entries
      << ";iuo=" << summary.canonical_implicitly_unwrapped_entries
      << ";null_resettable=" << summary.canonical_null_resettable_entries
      << ";unspecified=" << summary.canonical_unspecified_nullability_entries
      << ";invalid=" << summary.canonical_invalid_type_entries;
  return out.str();
}

std::string BuildTypeSystemNullabilityContractPreservationJson(
    const Objc3TypeSystemTypeSemanticModelSummary &summary) {
  const std::string replay_key =
      BuildTypeSystemNullabilityContractPreservationReplayKey(summary);
  const bool nullability_count_consistent =
      summary.canonical_type_entries ==
      summary.canonical_nullable_entries + summary.canonical_nonnull_entries +
          summary.canonical_implicitly_unwrapped_entries +
          summary.canonical_null_resettable_entries +
          summary.canonical_unspecified_nullability_entries;
  const bool ready = summary.ready_for_lowering_and_runtime &&
                     summary.deterministic &&
                     nullability_count_consistent &&
                     !summary.replay_key.empty();
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(
             kObjc3TypeSystemNullabilityContractPreservationContractId)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(kObjc3TypeSystemTypeSemanticModelContractId)
      << "\",\"preservation_model\":\"runtime-import-surface-preserves-canonical-nullability-counts-and-type-semantic-replay-boundary\""
      << ",\"canonical_type_count\":" << summary.canonical_type_entries
      << ",\"object_type_count\":" << summary.canonical_object_type_entries
      << ",\"nullable_entry_count\":" << summary.canonical_nullable_entries
      << ",\"nonnull_entry_count\":" << summary.canonical_nonnull_entries
      << ",\"implicitly_unwrapped_entry_count\":"
      << summary.canonical_implicitly_unwrapped_entries
      << ",\"null_resettable_entry_count\":"
      << summary.canonical_null_resettable_entries
      << ",\"unspecified_nullability_entry_count\":"
      << summary.canonical_unspecified_nullability_entries
      << ",\"invalid_nullability_entry_count\":"
      << summary.canonical_invalid_type_entries
      << ",\"ready\":" << (ready ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"type_semantic_replay_key\":\""
      << EscapeJsonString(summary.replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key) << "\"}";
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

std::string BuildTypeSystemOptionalKeypathLoweringContractJson(
    const Objc3TypeSystemOptionalKeypathLoweringContract &contract,
    const Objc3TypeSystemTypeSemanticModelSummary &semantic_summary,
    const std::string &semantic_summary_replay_key,
    const std::string &message_send_selector_lowering_replay_key,
    const std::string &dispatch_abi_marshalling_replay_key,
    const std::string &nil_receiver_semantics_foldability_replay_key,
    const std::string &replay_key) {
  std::ostringstream out;
  const bool source_semantic_model_ready = semantic_summary.deterministic;
  const bool typed_keypath_artifact_emission_deferred =
      contract.deferred_typed_keypath_sites != 0;
  const bool ready_for_native_optional_lowering =
      contract.deterministic && contract.contract_violation_sites == 0;
  const bool ready_for_typed_keypath_artifact_emission =
      contract.deterministic && contract.contract_violation_sites == 0 &&
      contract.live_typed_keypath_artifact_sites ==
          contract.typed_keypath_literal_sites;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathLoweringContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathLoweringSurfacePath)
      << "\",\"source_semantic_contract_id\":\""
      << EscapeJsonString(semantic_summary.contract_id)
      << "\",\"optional_model\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathLoweringOptionalModel)
      << "\",\"typed_keypath_model\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathLoweringTypedKeypathModel)
      << "\",\"authority_model\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathLoweringAuthorityModel)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathLoweringFailClosedModel)
      << "\",\"optional_binding_sites\":"
      << contract.optional_binding_sites
      << ",\"optional_binding_clause_sites\":"
      << contract.optional_binding_clause_sites
      << ",\"optional_send_sites\":" << contract.optional_send_sites
      << ",\"nil_coalescing_sites\":" << contract.nil_coalescing_sites
      << ",\"typed_keypath_literal_sites\":"
      << contract.typed_keypath_literal_sites
      << ",\"typed_keypath_self_root_sites\":"
      << contract.typed_keypath_self_root_sites
      << ",\"typed_keypath_class_root_sites\":"
      << contract.typed_keypath_class_root_sites
      << ",\"live_optional_lowering_sites\":"
      << contract.live_optional_lowering_sites
      << ",\"single_evaluation_nil_short_circuit_sites\":"
      << contract.single_evaluation_nil_short_circuit_sites
      << ",\"live_typed_keypath_artifact_sites\":"
      << contract.live_typed_keypath_artifact_sites
      << ",\"deferred_typed_keypath_sites\":"
      << contract.deferred_typed_keypath_sites
      << ",\"contract_violation_sites\":"
      << contract.contract_violation_sites
      << ",\"deterministic\":"
      << (contract.deterministic ? "true" : "false")
      << ",\"fail_closed\":true"
      << ",\"source_semantic_model_ready\":"
      << (source_semantic_model_ready ? "true" : "false")
      << ",\"message_send_selector_lowering_ready\":true"
      << ",\"dispatch_abi_marshalling_ready\":true"
      << ",\"nil_receiver_semantics_foldability_ready\":true"
      << ",\"live_optional_binding_lowering_landed\":true"
      << ",\"live_optional_send_lowering_landed\":true"
      << ",\"live_nil_coalescing_lowering_landed\":true"
      << ",\"single_evaluation_nil_short_circuit_landed\":true"
      << ",\"typed_keypath_artifact_emission_deferred\":"
      << (typed_keypath_artifact_emission_deferred ? "true" : "false")
      << ",\"ready_for_native_optional_lowering\":"
      << (ready_for_native_optional_lowering ? "true" : "false")
      << ",\"ready_for_typed_keypath_artifact_emission\":"
      << (ready_for_typed_keypath_artifact_emission ? "true" : "false")
      << ",\"semantic_summary_replay_key\":\""
      << EscapeJsonString(semantic_summary_replay_key)
      << "\",\"message_send_selector_lowering_replay_key\":\""
      << EscapeJsonString(message_send_selector_lowering_replay_key)
      << "\",\"dispatch_abi_marshalling_replay_key\":\""
      << EscapeJsonString(dispatch_abi_marshalling_replay_key)
      << "\",\"nil_receiver_semantics_foldability_replay_key\":\""
      << EscapeJsonString(nil_receiver_semantics_foldability_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key) << "\"}";
  return out.str();
}

std::string BuildTypeSystemOptionalKeypathRuntimeHelperContractJson(
    const Objc3TypeSystemOptionalKeypathLoweringContract &contract,
    const Objc3RuntimeSupportLibraryLinkWiringSummary &runtime_link_wiring,
    const std::string &lowering_replay_key) {
  const bool optional_send_runtime_ready =
      runtime_link_wiring.ready_for_runtime_library_consumption &&
      contract.live_optional_lowering_sites >= contract.optional_send_sites;
  const bool typed_keypath_descriptor_handles_ready =
      contract.live_typed_keypath_artifact_sites ==
          contract.typed_keypath_literal_sites &&
      contract.deferred_typed_keypath_sites == 0;
  const bool typed_keypath_runtime_execution_helper_landed = true;
  const bool diagnostic_fail_closed_ready = true;
  std::ostringstream replay_key;
  replay_key << "contract="
             << kObjc3TypeSystemOptionalKeypathRuntimeHelperContractId
             << ";lowering_replay=" << lowering_replay_key
             << ";runtime_link_ready="
             << (runtime_link_wiring.ready_for_runtime_library_consumption
                     ? "true"
                     : "false")
             << ";optional_send_sites=" << contract.optional_send_sites
             << ";live_optional_lowering_sites="
             << contract.live_optional_lowering_sites
             << ";typed_keypath_literal_sites="
             << contract.typed_keypath_literal_sites
             << ";live_typed_keypath_artifact_sites="
             << contract.live_typed_keypath_artifact_sites;
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathRuntimeHelperContractId)
      << "\",\"source_lowering_contract_id\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathLoweringContractId)
      << "\",\"runtime_link_wiring_contract_id\":\""
      << EscapeJsonString(kObjc3RuntimeSupportLibraryLinkWiringContractId)
      << "\",\"frontend_surface_path\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathRuntimeHelperSurfacePath)
      << "\",\"optional_send_helper_model\":\""
      << EscapeJsonString(kObjc3TypeSystemOptionalKeypathRuntimeHelperOptionalModel)
      << "\",\"typed_keypath_helper_model\":\""
      << EscapeJsonString(
             kObjc3TypeSystemOptionalKeypathRuntimeHelperTypedKeypathModel)
      << "\",\"diagnostic_fail_closed_model\":\""
      << EscapeJsonString(
             kObjc3TypeSystemOptionalKeypathRuntimeHelperDiagnosticModel)
      << "\",\"public_lookup_selector_symbol\":\""
      << EscapeJsonString(kObjc3RuntimeSupportLibraryLookupSelectorSymbol)
      << "\",\"public_dispatch_i32_symbol\":\""
      << EscapeJsonString(kObjc3RuntimeSupportLibraryDispatchI32Symbol)
      << "\",\"keypath_descriptor_section\":\""
      << EscapeJsonString(kObjc3RuntimeKeypathDescriptorLogicalSection)
      << "\",\"keypath_descriptor_aggregate_symbol\":\"__objc3_sec_keypath_descriptors\""
      << ",\"runtime_library_archive_available\":"
      << (runtime_link_wiring.runtime_library_archive_available ? "true"
                                                                : "false")
      << ",\"runtime_library_consumption_ready\":"
      << (runtime_link_wiring.ready_for_runtime_library_consumption ? "true"
                                                                    : "false")
      << ",\"optional_send_runtime_ready\":"
      << (optional_send_runtime_ready ? "true" : "false")
      << ",\"typed_keypath_descriptor_handles_ready\":"
      << (typed_keypath_descriptor_handles_ready ? "true" : "false")
      << ",\"typed_keypath_runtime_execution_helper_landed\":"
      << (typed_keypath_runtime_execution_helper_landed ? "true" : "false")
      << ",\"diagnostic_fail_closed_ready\":"
      << (diagnostic_fail_closed_ready ? "true" : "false")
      << ",\"fail_closed\":true"
      << ",\"replay_key\":\"" << EscapeJsonString(replay_key.str()) << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
