#include "artifacts/objc3_frontend_type_system_contract_artifacts.h"

#include <algorithm>
#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

#include "ast/objc3_ast_contracts.h"
#include "io/objc3_json.h"
#include "runtime/metadata/runtime_metadata_model.h"
#include "sema/objc3_sema_contract_core.h"
#include "sema/objc3_sema_contract_type_handoff.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

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

bool IsProtocolQualifiedGenericArgumentSpelling(const std::string &argument) {
  return argument.find("id<") != std::string::npos ||
         argument.find("Class<") != std::string::npos;
}

void AccumulateGenericContractInventory(
    const Objc3SemanticCanonicalType &type,
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
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3FrontendTypeSystemGenericContractInventory inventory;
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
    const Objc3FrontendTypeSystemGenericContractInventory &inventory,
    const Objc3TypeSystemTypeSemanticModelSummary &semantic_summary) {
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
    const Objc3TypeSystemTypeSemanticModelSummary &semantic_summary) {
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

}  // namespace

std::string BuildTypeSystemGenericContractPreservationJson(
    const Objc3SemanticTypeMetadataHandoff &handoff,
    const Objc3TypeSystemTypeSemanticModelSummary &semantic_summary) {
  const Objc3FrontendTypeSystemGenericContractInventory inventory =
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
      << EscapeJsonString(
             kObjc3FrontendTypeSystemGenericContractPreservationContractId)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(kObjc3FrontendTypeSystemTypeSemanticModelContractId)
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

}  // namespace objc3::artifacts::frontend
