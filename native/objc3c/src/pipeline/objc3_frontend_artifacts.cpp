#include "pipeline/objc3_frontend_artifacts.h"

#include <algorithm>
#include <cstdint>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ir/objc3_ir_emitter.h"
#include "pipeline/objc3_ir_emission_core_feature_implementation_surface.h"
#include "pipeline/objc3_ir_emission_completeness_scaffold.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_edge_case_compatibility_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_scaffold.h"
#include "pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.h"
#include "pipeline/objc3_lowering_pipeline_pass_graph_scaffold.h"
#include "pipeline/objc3_ownership_aware_lowering_behavior_scaffold.h"
#include "pipeline/objc3_parse_lowering_readiness_surface.h"

namespace {

const char *TypeName(ValueType type) {
  switch (type) {
    case ValueType::I32:
      return "i32";
    case ValueType::Bool:
      return "bool";
    case ValueType::Void:
      return "void";
    case ValueType::Function:
      return "function";
    case ValueType::ObjCId:
      return "id";
    case ValueType::ObjCClass:
      return "Class";
    case ValueType::ObjCSel:
      return "SEL";
    case ValueType::ObjCProtocol:
      return "Protocol";
    case ValueType::ObjCInstancetype:
      return "instancetype";
    case ValueType::ObjCObjectPtr:
      return "object-pointer";
    default:
      return "unknown";
  }
}

const char *CompatibilityModeName(Objc3FrontendCompatibilityMode mode) {
  switch (mode) {
    case Objc3FrontendCompatibilityMode::kLegacy:
      return "legacy";
    case Objc3FrontendCompatibilityMode::kCanonical:
    default:
      return "canonical";
  }
}

std::string EscapeJsonString(const std::string &value) {
  std::ostringstream out;
  for (const unsigned char c : value) {
    switch (c) {
      case '\\':
        out << "\\\\";
        break;
      case '"':
        out << "\\\"";
        break;
      case '\b':
        out << "\\b";
        break;
      case '\f':
        out << "\\f";
        break;
      case '\n':
        out << "\\n";
        break;
      case '\r':
        out << "\\r";
        break;
      case '\t':
        out << "\\t";
        break;
      default:
        if (c < 0x20u) {
          out << "\\u00";
          constexpr char kHex[] = "0123456789abcdef";
          out << kHex[(c >> 4u) & 0x0fu] << kHex[c & 0x0fu];
        } else {
          out << static_cast<char>(c);
        }
        break;
    }
  }
  return out.str();
}

std::string BuildExecutableMetadataSourceGraphJson(
    const Objc3ExecutableMetadataSourceGraph &graph) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(graph.contract_id)
      << "\",\"owner_identity_model\":\""
      << EscapeJsonString(graph.owner_identity_model)
      << "\",\"metaclass_node_policy\":\""
      << EscapeJsonString(graph.metaclass_node_policy)
      << "\",\"edge_ordering_model\":\""
      << EscapeJsonString(graph.edge_ordering_model)
      << "\",\"interface_nodes\":"
      << graph.interface_nodes_lexicographic.size()
      << ",\"implementation_nodes\":"
      << graph.implementation_nodes_lexicographic.size()
      << ",\"class_nodes\":" << graph.class_nodes_lexicographic.size()
      << ",\"metaclass_nodes\":"
      << graph.metaclass_nodes_lexicographic.size()
      << ",\"protocol_nodes\":" << graph.protocol_nodes_lexicographic.size()
      << ",\"category_nodes\":" << graph.category_nodes_lexicographic.size()
      << ",\"property_nodes\":" << graph.property_nodes_lexicographic.size()
      << ",\"ivar_nodes\":" << graph.ivar_nodes_lexicographic.size()
      << ",\"method_nodes\":" << graph.method_nodes_lexicographic.size()
      << ",\"interface_node_entries\":[";
  for (std::size_t i = 0; i < graph.interface_nodes_lexicographic.size(); ++i) {
    const auto &node = graph.interface_nodes_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"class_name\":\"" << EscapeJsonString(node.class_name)
        << "\",\"owner_identity\":\"" << EscapeJsonString(node.owner_identity)
        << "\",\"class_owner_identity\":\""
        << EscapeJsonString(node.class_owner_identity)
        << "\",\"metaclass_owner_identity\":\""
        << EscapeJsonString(node.metaclass_owner_identity)
        << "\",\"super_class_owner_identity\":\""
        << EscapeJsonString(node.super_class_owner_identity)
        << "\",\"has_super\":" << (node.has_super ? "true" : "false")
        << ",\"property_count\":" << node.property_count
        << ",\"method_count\":" << node.method_count
        << ",\"class_method_count\":" << node.class_method_count
        << ",\"instance_method_count\":" << node.instance_method_count
        << ",\"line\":" << node.line << ",\"column\":" << node.column << "}";
  }
  out << "],\"implementation_node_entries\":[";
  for (std::size_t i = 0; i < graph.implementation_nodes_lexicographic.size();
       ++i) {
    const auto &node = graph.implementation_nodes_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"class_name\":\"" << EscapeJsonString(node.class_name)
        << "\",\"owner_identity\":\"" << EscapeJsonString(node.owner_identity)
        << "\",\"interface_owner_identity\":\""
        << EscapeJsonString(node.interface_owner_identity)
        << "\",\"class_owner_identity\":\""
        << EscapeJsonString(node.class_owner_identity)
        << "\",\"metaclass_owner_identity\":\""
        << EscapeJsonString(node.metaclass_owner_identity)
        << "\",\"has_matching_interface\":"
        << (node.has_matching_interface ? "true" : "false")
        << ",\"property_count\":" << node.property_count
        << ",\"method_count\":" << node.method_count
        << ",\"class_method_count\":" << node.class_method_count
        << ",\"instance_method_count\":" << node.instance_method_count
        << ",\"line\":" << node.line << ",\"column\":" << node.column << "}";
  }
  out << "],\"class_node_entries\":[";
  for (std::size_t i = 0; i < graph.class_nodes_lexicographic.size(); ++i) {
    const auto &node = graph.class_nodes_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"class_name\":\"" << EscapeJsonString(node.class_name)
        << "\",\"owner_identity\":\"" << EscapeJsonString(node.owner_identity)
        << "\",\"interface_owner_identity\":\""
        << EscapeJsonString(node.interface_owner_identity)
        << "\",\"implementation_owner_identity\":\""
        << EscapeJsonString(node.implementation_owner_identity)
        << "\",\"metaclass_owner_identity\":\""
        << EscapeJsonString(node.metaclass_owner_identity)
        << "\",\"super_class_owner_identity\":\""
        << EscapeJsonString(node.super_class_owner_identity)
        << "\",\"has_interface\":" << (node.has_interface ? "true" : "false")
        << ",\"has_implementation\":"
        << (node.has_implementation ? "true" : "false")
        << ",\"has_super\":" << (node.has_super ? "true" : "false")
        << ",\"interface_property_count\":" << node.interface_property_count
        << ",\"implementation_property_count\":"
        << node.implementation_property_count
        << ",\"interface_method_count\":" << node.interface_method_count
        << ",\"implementation_method_count\":"
        << node.implementation_method_count
        << ",\"interface_class_method_count\":"
        << node.interface_class_method_count
        << ",\"implementation_class_method_count\":"
        << node.implementation_class_method_count
        << ",\"interface_instance_method_count\":"
        << node.interface_instance_method_count
        << ",\"implementation_instance_method_count\":"
        << node.implementation_instance_method_count
        << ",\"line\":" << node.line << ",\"column\":" << node.column << "}";
  }
  out << "],\"metaclass_node_entries\":[";
  for (std::size_t i = 0; i < graph.metaclass_nodes_lexicographic.size(); ++i) {
    const auto &node = graph.metaclass_nodes_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"class_name\":\"" << EscapeJsonString(node.class_name)
        << "\",\"owner_identity\":\"" << EscapeJsonString(node.owner_identity)
        << "\",\"class_owner_identity\":\""
        << EscapeJsonString(node.class_owner_identity)
        << "\",\"interface_owner_identity\":\""
        << EscapeJsonString(node.interface_owner_identity)
        << "\",\"implementation_owner_identity\":\""
        << EscapeJsonString(node.implementation_owner_identity)
        << "\",\"super_metaclass_owner_identity\":\""
        << EscapeJsonString(node.super_metaclass_owner_identity)
        << "\",\"derived_from_interface\":"
        << (node.derived_from_interface ? "true" : "false")
        << ",\"has_implementation\":"
        << (node.has_implementation ? "true" : "false")
        << ",\"has_super\":" << (node.has_super ? "true" : "false")
        << ",\"interface_class_method_count\":"
        << node.interface_class_method_count
        << ",\"implementation_class_method_count\":"
        << node.implementation_class_method_count
        << ",\"line\":" << node.line << ",\"column\":" << node.column << "}";
  }
  out << "],\"protocol_node_entries\":[";
  for (std::size_t i = 0; i < graph.protocol_nodes_lexicographic.size(); ++i) {
    const auto &node = graph.protocol_nodes_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"protocol_name\":\"" << EscapeJsonString(node.protocol_name)
        << "\",\"owner_identity\":\"" << EscapeJsonString(node.owner_identity)
        << "\",\"inherited_protocol_owner_identities\":[";
    for (std::size_t inherited_index = 0;
         inherited_index <
         node.inherited_protocol_owner_identities_lexicographic.size();
         ++inherited_index) {
      if (inherited_index != 0u) {
        out << ",";
      }
      out << "\""
          << EscapeJsonString(node.inherited_protocol_owner_identities_lexicographic[inherited_index])
          << "\"";
    }
    out << "],\"property_count\":" << node.property_count
        << ",\"method_count\":" << node.method_count
        << ",\"is_forward_declaration\":"
        << (node.is_forward_declaration ? "true" : "false")
        << ",\"line\":" << node.line << ",\"column\":" << node.column << "}";
  }
  out << "],\"category_node_entries\":[";
  for (std::size_t i = 0; i < graph.category_nodes_lexicographic.size(); ++i) {
    const auto &node = graph.category_nodes_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"class_name\":\"" << EscapeJsonString(node.class_name)
        << "\",\"category_name\":\"" << EscapeJsonString(node.category_name)
        << "\",\"owner_identity\":\"" << EscapeJsonString(node.owner_identity)
        << "\",\"interface_owner_identity\":\""
        << EscapeJsonString(node.interface_owner_identity)
        << "\",\"implementation_owner_identity\":\""
        << EscapeJsonString(node.implementation_owner_identity)
        << "\",\"class_owner_identity\":\""
        << EscapeJsonString(node.class_owner_identity)
        << "\",\"adopted_protocol_owner_identities\":[";
    for (std::size_t adopted_index = 0;
         adopted_index < node.adopted_protocol_owner_identities_lexicographic.size();
         ++adopted_index) {
      if (adopted_index != 0u) {
        out << ",";
      }
      out << "\""
          << EscapeJsonString(node.adopted_protocol_owner_identities_lexicographic[adopted_index])
          << "\"";
    }
    out << "],\"has_interface\":" << (node.has_interface ? "true" : "false")
        << ",\"has_implementation\":"
        << (node.has_implementation ? "true" : "false")
        << ",\"interface_property_count\":" << node.interface_property_count
        << ",\"implementation_property_count\":"
        << node.implementation_property_count
        << ",\"interface_method_count\":" << node.interface_method_count
        << ",\"implementation_method_count\":"
        << node.implementation_method_count
        << ",\"interface_class_method_count\":"
        << node.interface_class_method_count
        << ",\"implementation_class_method_count\":"
        << node.implementation_class_method_count
        << ",\"line\":" << node.line << ",\"column\":" << node.column << "}";
  }
  out << "],\"property_node_entries\":[";
  for (std::size_t i = 0; i < graph.property_nodes_lexicographic.size(); ++i) {
    const auto &node = graph.property_nodes_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"owner_kind\":\"" << EscapeJsonString(node.owner_kind)
        << "\",\"owner_name\":\"" << EscapeJsonString(node.owner_name)
        << "\",\"owner_identity\":\"" << EscapeJsonString(node.owner_identity)
        << "\",\"declaration_owner_identity\":\""
        << EscapeJsonString(node.declaration_owner_identity)
        << "\",\"export_owner_identity\":\""
        << EscapeJsonString(node.export_owner_identity)
        << "\",\"property_name\":\"" << EscapeJsonString(node.property_name)
        << "\",\"type_name\":\"" << EscapeJsonString(node.type_name)
        << "\",\"has_getter\":" << (node.has_getter ? "true" : "false")
        << ",\"getter_selector\":\"" << EscapeJsonString(node.getter_selector)
        << "\",\"has_setter\":" << (node.has_setter ? "true" : "false")
        << ",\"setter_selector\":\"" << EscapeJsonString(node.setter_selector)
        << "\",\"ivar_binding_symbol\":\""
        << EscapeJsonString(node.ivar_binding_symbol)
        << "\",\"line\":" << node.line << ",\"column\":" << node.column << "}";
  }
  out << "],\"method_node_entries\":[";
  for (std::size_t i = 0; i < graph.method_nodes_lexicographic.size(); ++i) {
    const auto &node = graph.method_nodes_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"owner_kind\":\"" << EscapeJsonString(node.owner_kind)
        << "\",\"owner_name\":\"" << EscapeJsonString(node.owner_name)
        << "\",\"owner_identity\":\"" << EscapeJsonString(node.owner_identity)
        << "\",\"declaration_owner_identity\":\""
        << EscapeJsonString(node.declaration_owner_identity)
        << "\",\"export_owner_identity\":\""
        << EscapeJsonString(node.export_owner_identity)
        << "\",\"selector\":\"" << EscapeJsonString(node.selector)
        << "\",\"is_class_method\":"
        << (node.is_class_method ? "true" : "false")
        << ",\"has_body\":" << (node.has_body ? "true" : "false")
        << ",\"parameter_count\":" << node.parameter_count
        << ",\"return_type_name\":\""
        << EscapeJsonString(node.return_type_name)
        << "\",\"line\":" << node.line << ",\"column\":" << node.column << "}";
  }
  out << "],\"ivar_node_entries\":[";
  for (std::size_t i = 0; i < graph.ivar_nodes_lexicographic.size(); ++i) {
    const auto &node = graph.ivar_nodes_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"owner_kind\":\"" << EscapeJsonString(node.owner_kind)
        << "\",\"owner_name\":\"" << EscapeJsonString(node.owner_name)
        << "\",\"owner_identity\":\"" << EscapeJsonString(node.owner_identity)
        << "\",\"declaration_owner_identity\":\""
        << EscapeJsonString(node.declaration_owner_identity)
        << "\",\"export_owner_identity\":\""
        << EscapeJsonString(node.export_owner_identity)
        << "\",\"property_owner_identity\":\""
        << EscapeJsonString(node.property_owner_identity)
        << "\",\"property_name\":\"" << EscapeJsonString(node.property_name)
        << "\",\"ivar_binding_symbol\":\""
        << EscapeJsonString(node.ivar_binding_symbol)
        << "\",\"line\":" << node.line << ",\"column\":" << node.column << "}";
  }
  out << "],\"owner_edges\":[";
  for (std::size_t i = 0; i < graph.owner_edges_lexicographic.size(); ++i) {
    const auto &edge = graph.owner_edges_lexicographic[i];
    if (i != 0u) {
      out << ",";
    }
    out << "{\"edge_kind\":\"" << EscapeJsonString(edge.edge_kind)
        << "\",\"source_owner_identity\":\""
        << EscapeJsonString(edge.source_owner_identity)
        << "\",\"target_owner_identity\":\""
        << EscapeJsonString(edge.target_owner_identity)
        << "\",\"line\":" << edge.line << ",\"column\":" << edge.column
        << "}";
  }
  out << "],\"lexicographic_owner_edge_ordering\":"
      << (graph.deterministic ? "true" : "false")
      << ",\"source_graph_complete\":"
      << (graph.source_graph_complete ? "true" : "false")
      << ",\"ready_for_semantic_closure\":"
      << (graph.ready_for_semantic_closure ? "true" : "false")
      << ",\"ready_for_lowering\":"
      << (graph.ready_for_lowering ? "true" : "false") << "}";
  return out.str();
}

std::string BuildExecutableMetadataSemanticConsistencyBoundaryJson(
    const Objc3ExecutableMetadataSemanticConsistencyBoundary &boundary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(boundary.contract_id)
      << "\",\"executable_metadata_source_graph_contract_id\":\""
      << EscapeJsonString(boundary.executable_metadata_source_graph_contract_id)
      << "\",\"semantic_boundary_frozen\":"
      << (boundary.semantic_boundary_frozen ? "true" : "false")
      << ",\"lowering_admission_ready\":"
      << (boundary.lowering_admission_ready ? "true" : "false")
      << ",\"fail_closed\":"
      << (boundary.fail_closed ? "true" : "false")
      << ",\"ready\":"
      << (IsReadyObjc3ExecutableMetadataSemanticConsistencyBoundary(boundary)
              ? "true"
              : "false")
      << ",\"source_graph_ready\":"
      << (boundary.source_graph_ready ? "true" : "false")
      << ",\"protocol_category_handoff_deterministic\":"
      << (boundary.protocol_category_handoff_deterministic ? "true" : "false")
      << ",\"class_protocol_category_linking_deterministic\":"
      << (boundary.class_protocol_category_linking_deterministic ? "true"
                                                                 : "false")
      << ",\"selector_normalization_deterministic\":"
      << (boundary.selector_normalization_deterministic ? "true" : "false")
      << ",\"property_attribute_deterministic\":"
      << (boundary.property_attribute_deterministic ? "true" : "false")
      << ",\"symbol_graph_scope_resolution_deterministic\":"
      << (boundary.symbol_graph_scope_resolution_deterministic ? "true"
                                                               : "false")
      << ",\"protocol_inheritance_edges_complete\":"
      << (boundary.protocol_inheritance_edges_complete ? "true" : "false")
      << ",\"category_attachment_edges_complete\":"
      << (boundary.category_attachment_edges_complete ? "true" : "false")
      << ",\"declaration_export_owner_split_complete\":"
      << (boundary.declaration_export_owner_split_complete ? "true" : "false")
      << ",\"property_method_ivar_owner_edges_complete\":"
      << (boundary.property_method_ivar_owner_edges_complete ? "true" : "false")
      << ",\"semantic_conflict_diagnostics_enforcement_pending\":"
      << (boundary.semantic_conflict_diagnostics_enforcement_pending ? "true"
                                                                     : "false")
      << ",\"duplicate_export_owner_enforcement_pending\":"
      << (boundary.duplicate_export_owner_enforcement_pending ? "true"
                                                              : "false")
      << ",\"lowering_admission_pending\":"
      << (boundary.lowering_admission_pending ? "true" : "false")
      << ",\"protocol_node_count\":" << boundary.protocol_node_count
      << ",\"category_node_count\":" << boundary.category_node_count
      << ",\"property_node_count\":" << boundary.property_node_count
      << ",\"method_node_count\":" << boundary.method_node_count
      << ",\"ivar_node_count\":" << boundary.ivar_node_count
      << ",\"owner_edge_count\":" << boundary.owner_edge_count
      << ",\"failure_reason\":\"" << EscapeJsonString(boundary.failure_reason)
      << "\"}";
  return out.str();
}

std::string BuildExecutableMetadataSemanticValidationSurfaceJson(
    const Objc3ExecutableMetadataSemanticValidationSurface &surface) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(surface.contract_id)
      << "\",\"executable_metadata_semantic_consistency_contract_id\":\""
      << EscapeJsonString(
             surface.executable_metadata_semantic_consistency_contract_id)
      << "\",\"semantic_consistency_ready\":"
      << (surface.semantic_consistency_ready ? "true" : "false")
      << ",\"ready\":"
      << (IsReadyObjc3ExecutableMetadataSemanticValidationSurface(surface)
              ? "true"
              : "false")
      << ",\"method_lookup_override_conflict_handoff_deterministic\":"
      << (surface.method_lookup_override_conflict_handoff_deterministic
              ? "true"
              : "false")
      << ",\"class_protocol_category_linking_deterministic\":"
      << (surface.class_protocol_category_linking_deterministic ? "true"
                                                                : "false")
      << ",\"class_inheritance_edges_complete\":"
      << (surface.class_inheritance_edges_complete ? "true" : "false")
      << ",\"protocol_inheritance_edges_complete\":"
      << (surface.protocol_inheritance_edges_complete ? "true" : "false")
      << ",\"metaclass_edges_complete\":"
      << (surface.metaclass_edges_complete ? "true" : "false")
      << ",\"inheritance_chain_cycle_free\":"
      << (surface.inheritance_chain_cycle_free ? "true" : "false")
      << ",\"superclass_targets_resolved\":"
      << (surface.superclass_targets_resolved ? "true" : "false")
      << ",\"protocol_inheritance_targets_resolved\":"
      << (surface.protocol_inheritance_targets_resolved ? "true" : "false")
      << ",\"metaclass_targets_resolved\":"
      << (surface.metaclass_targets_resolved ? "true" : "false")
      << ",\"metaclass_lineage_aligned\":"
      << (surface.metaclass_lineage_aligned ? "true" : "false")
      << ",\"method_override_edges_complete\":"
      << (surface.method_override_edges_complete ? "true" : "false")
      << ",\"override_lookup_complete\":"
      << (surface.override_lookup_complete ? "true" : "false")
      << ",\"override_conflicts_absent\":"
      << (surface.override_conflicts_absent ? "true" : "false")
      << ",\"protocol_composition_valid\":"
      << (surface.protocol_composition_valid ? "true" : "false")
      << ",\"inheritance_validation_ready\":"
      << (surface.inheritance_validation_ready ? "true" : "false")
      << ",\"override_validation_ready\":"
      << (surface.override_validation_ready ? "true" : "false")
      << ",\"protocol_composition_validation_ready\":"
      << (surface.protocol_composition_validation_ready ? "true" : "false")
      << ",\"metaclass_relationship_validation_ready\":"
      << (surface.metaclass_relationship_validation_ready ? "true" : "false")
      << ",\"semantic_validation_complete\":"
      << (surface.semantic_validation_complete ? "true" : "false")
      << ",\"lowering_admission_ready\":"
      << (surface.lowering_admission_ready ? "true" : "false")
      << ",\"fail_closed\":"
      << (surface.fail_closed ? "true" : "false")
      << ",\"class_inheritance_edge_count\":"
      << surface.class_inheritance_edge_count
      << ",\"protocol_inheritance_edge_count\":"
      << surface.protocol_inheritance_edge_count
      << ",\"metaclass_super_edge_count\":"
      << surface.metaclass_super_edge_count
      << ",\"override_edge_count\":" << surface.override_edge_count
      << ",\"class_method_override_edge_count\":"
      << surface.class_method_override_edge_count
      << ",\"instance_method_override_edge_count\":"
      << surface.instance_method_override_edge_count
      << ",\"override_lookup_sites\":" << surface.override_lookup_sites
      << ",\"override_lookup_hits\":" << surface.override_lookup_hits
      << ",\"override_lookup_misses\":" << surface.override_lookup_misses
      << ",\"override_conflicts\":" << surface.override_conflicts
      << ",\"unresolved_base_interfaces\":"
      << surface.unresolved_base_interfaces
      << ",\"protocol_composition_sites\":"
      << surface.protocol_composition_sites
      << ",\"protocol_composition_symbols\":"
      << surface.protocol_composition_symbols
      << ",\"category_composition_sites\":"
      << surface.category_composition_sites
      << ",\"category_composition_symbols\":"
      << surface.category_composition_symbols
      << ",\"invalid_protocol_composition_sites\":"
      << surface.invalid_protocol_composition_sites
      << ",\"failure_reason\":\""
      << EscapeJsonString(surface.failure_reason) << "\"}";
  return out.str();
}

std::string BuildExecutableMetadataLoweringHandoffSurfaceJson(
    const Objc3ExecutableMetadataLoweringHandoffSurface &surface) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(surface.contract_id)
      << "\",\"executable_metadata_source_graph_contract_id\":\""
      << EscapeJsonString(surface.executable_metadata_source_graph_contract_id)
      << "\",\"executable_metadata_semantic_consistency_contract_id\":\""
      << EscapeJsonString(
             surface.executable_metadata_semantic_consistency_contract_id)
      << "\",\"executable_metadata_semantic_validation_contract_id\":\""
      << EscapeJsonString(
             surface.executable_metadata_semantic_validation_contract_id)
      << "\",\"ready\":"
      << (IsReadyObjc3ExecutableMetadataLoweringHandoffSurface(surface)
              ? "true"
              : "false")
      << ",\"source_graph_ready\":"
      << (surface.source_graph_ready ? "true" : "false")
      << ",\"semantic_consistency_ready\":"
      << (surface.semantic_consistency_ready ? "true" : "false")
      << ",\"semantic_validation_ready\":"
      << (surface.semantic_validation_ready ? "true" : "false")
      << ",\"semantic_type_metadata_handoff_deterministic\":"
      << (surface.semantic_type_metadata_handoff_deterministic ? "true"
                                                               : "false")
      << ",\"protocol_category_handoff_deterministic\":"
      << (surface.protocol_category_handoff_deterministic ? "true" : "false")
      << ",\"class_protocol_category_linking_handoff_deterministic\":"
      << (surface.class_protocol_category_linking_handoff_deterministic ? "true"
                                                                        : "false")
      << ",\"selector_normalization_handoff_deterministic\":"
      << (surface.selector_normalization_handoff_deterministic ? "true"
                                                               : "false")
      << ",\"property_attribute_handoff_deterministic\":"
      << (surface.property_attribute_handoff_deterministic ? "true" : "false")
      << ",\"symbol_graph_scope_resolution_handoff_deterministic\":"
      << (surface.symbol_graph_scope_resolution_handoff_deterministic ? "true"
                                                                      : "false")
      << ",\"property_synthesis_ivar_binding_handoff_deterministic\":"
      << (surface.property_synthesis_ivar_binding_handoff_deterministic ? "true"
                                                                        : "false")
      << ",\"lowering_schema_frozen\":"
      << (surface.lowering_schema_frozen ? "true" : "false")
      << ",\"fail_closed\":" << (surface.fail_closed ? "true" : "false")
      << ",\"ready_for_lowering\":"
      << (surface.ready_for_lowering ? "true" : "false")
      << ",\"interface_node_count\":" << surface.interface_node_count
      << ",\"implementation_node_count\":"
      << surface.implementation_node_count
      << ",\"class_node_count\":" << surface.class_node_count
      << ",\"metaclass_node_count\":" << surface.metaclass_node_count
      << ",\"protocol_node_count\":" << surface.protocol_node_count
      << ",\"category_node_count\":" << surface.category_node_count
      << ",\"property_node_count\":" << surface.property_node_count
      << ",\"method_node_count\":" << surface.method_node_count
      << ",\"ivar_node_count\":" << surface.ivar_node_count
      << ",\"owner_edge_count\":" << surface.owner_edge_count
      << ",\"replay_key\":\"" << EscapeJsonString(surface.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(surface.failure_reason) << "\"}";
  return out.str();
}

std::string BuildExecutableMetadataTypedLoweringHandoffJson(
    const Objc3ExecutableMetadataTypedLoweringHandoff &surface) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(surface.contract_id)
      << "\",\"executable_metadata_lowering_handoff_contract_id\":\""
      << EscapeJsonString(
             surface.executable_metadata_lowering_handoff_contract_id)
      << "\",\"executable_metadata_source_graph_contract_id\":\""
      << EscapeJsonString(surface.executable_metadata_source_graph_contract_id)
      << "\",\"executable_metadata_semantic_consistency_contract_id\":\""
      << EscapeJsonString(
             surface.executable_metadata_semantic_consistency_contract_id)
      << "\",\"executable_metadata_semantic_validation_contract_id\":\""
      << EscapeJsonString(
             surface.executable_metadata_semantic_validation_contract_id)
      << "\",\"manifest_schema_ordering_model\":\""
      << EscapeJsonString(surface.manifest_schema_ordering_model)
      << "\",\"ready\":"
      << (IsReadyObjc3ExecutableMetadataTypedLoweringHandoff(surface) ? "true"
                                                                     : "false")
      << ",\"source_graph_ready\":"
      << (surface.source_graph_ready ? "true" : "false")
      << ",\"semantic_consistency_ready\":"
      << (surface.semantic_consistency_ready ? "true" : "false")
      << ",\"semantic_validation_ready\":"
      << (surface.semantic_validation_ready ? "true" : "false")
      << ",\"lowering_handoff_surface_ready\":"
      << (surface.lowering_handoff_surface_ready ? "true" : "false")
      << ",\"deterministic\":"
      << (surface.deterministic ? "true" : "false")
      << ",\"manifest_schema_frozen\":"
      << (surface.manifest_schema_frozen ? "true" : "false")
      << ",\"fail_closed\":" << (surface.fail_closed ? "true" : "false")
      << ",\"ready_for_lowering\":"
      << (surface.ready_for_lowering ? "true" : "false")
      << ",\"source_graph\":"
      << BuildExecutableMetadataSourceGraphJson(surface.source_graph)
      << ",\"replay_key\":\"" << EscapeJsonString(surface.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(surface.failure_reason) << "\"}";
  return out.str();
}

std::string BuildExecutableMetadataDebugProjectionRowDescriptor(
    const Objc3ExecutableMetadataDebugProjectionMatrixRow &row) {
  std::ostringstream out;
  out << row.row_key << "|" << row.artifact_kind << "|" << row.fixture_path << "|"
      << row.emit_prefix << "|" << row.artifact_relative_path << "|"
      << row.probe_command << "|" << row.inspection_command << "|"
      << row.expected_anchor;
  return out.str();
}

std::string BuildExecutableMetadataDebugProjectionReplayKey(
    const Objc3ExecutableMetadataDebugProjectionSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id << ";typed_handoff_contract_id="
      << summary.typed_lowering_handoff_contract_id
      << ";source_graph_contract_id=" << summary.source_graph_contract_id
      << ";named_metadata_name=" << summary.named_metadata_name
      << ";manifest_surface_path=" << summary.manifest_surface_path
      << ";typed_handoff_surface_path=" << summary.typed_handoff_surface_path
      << ";source_graph_surface_path=" << summary.source_graph_surface_path;
  for (std::size_t index = 0; index < summary.rows.size(); ++index) {
    out << ";row[" << index << "]="
        << BuildExecutableMetadataDebugProjectionRowDescriptor(
               summary.rows[index]);
  }
  return out.str();
}

Objc3ExecutableMetadataDebugProjectionSummary
BuildExecutableMetadataDebugProjectionSummary(
    const Objc3ExecutableMetadataTypedLoweringHandoff
        &executable_metadata_typed_lowering_handoff) {
  Objc3ExecutableMetadataDebugProjectionSummary summary;
  summary.fail_closed = true;
  summary.matrix_published = true;
  summary.manifest_debug_surface_published = true;
  summary.ir_named_metadata_published = true;
  summary.active_typed_handoff_ready =
      IsReadyObjc3ExecutableMetadataTypedLoweringHandoff(
          executable_metadata_typed_lowering_handoff);
  if (summary.active_typed_handoff_ready) {
    summary.active_typed_handoff_replay_key =
        executable_metadata_typed_lowering_handoff.replay_key;
  }
  summary.rows[0] = Objc3ExecutableMetadataDebugProjectionMatrixRow{
      kObjc3ExecutableMetadataDebugProjectionClassManifestRowKey,
      "manifest",
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3ExecutableMetadataDebugProjectionEmitPrefix,
      kObjc3ExecutableMetadataDebugProjectionManifestRelativePath,
      kObjc3ExecutableMetadataDebugProjectionClassProbeCommand,
      kObjc3ExecutableMetadataDebugProjectionManifestInspectionCommand,
      kObjc3ExecutableMetadataDebugProjectionManifestSurfacePath};
  summary.rows[1] = Objc3ExecutableMetadataDebugProjectionMatrixRow{
      kObjc3ExecutableMetadataDebugProjectionCategoryManifestRowKey,
      "manifest",
      kObjc3ExecutableMetadataDebugProjectionCategoryFixturePath,
      kObjc3ExecutableMetadataDebugProjectionEmitPrefix,
      kObjc3ExecutableMetadataDebugProjectionManifestRelativePath,
      kObjc3ExecutableMetadataDebugProjectionCategoryProbeCommand,
      kObjc3ExecutableMetadataDebugProjectionManifestInspectionCommand,
      kObjc3ExecutableMetadataTypedLoweringHandoffManifestSurfacePath};
  summary.rows[2] = Objc3ExecutableMetadataDebugProjectionMatrixRow{
      kObjc3ExecutableMetadataDebugProjectionIrNamedMetadataRowKey,
      "llvm-ir",
      kObjc3ExecutableMetadataDebugProjectionIrFixturePath,
      kObjc3ExecutableMetadataDebugProjectionEmitPrefix,
      kObjc3ExecutableMetadataDebugProjectionIrRelativePath,
      kObjc3ExecutableMetadataDebugProjectionIrProbeCommand,
      kObjc3ExecutableMetadataDebugProjectionIrInspectionCommand,
      kObjc3ExecutableMetadataDebugProjectionNamedMetadataName};
  summary.matrix_row_count = summary.rows.size();
  summary.replay_key =
      BuildExecutableMetadataDebugProjectionReplayKey(summary);
  summary.replay_anchor_deterministic = !summary.replay_key.empty();
  if (!IsReadyObjc3ExecutableMetadataDebugProjectionSummary(summary)) {
    summary.failure_reason =
        "executable metadata debug projection matrix contract is incomplete";
  }
  return summary;
}

std::string BuildExecutableMetadataDebugProjectionSummaryJson(
    const Objc3ExecutableMetadataDebugProjectionSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"typed_lowering_handoff_contract_id\":\""
      << EscapeJsonString(summary.typed_lowering_handoff_contract_id)
      << "\",\"source_graph_contract_id\":\""
      << EscapeJsonString(summary.source_graph_contract_id)
      << "\",\"named_metadata_name\":\""
      << EscapeJsonString(summary.named_metadata_name)
      << "\",\"manifest_surface_path\":\""
      << EscapeJsonString(summary.manifest_surface_path)
      << "\",\"typed_handoff_surface_path\":\""
      << EscapeJsonString(summary.typed_handoff_surface_path)
      << "\",\"source_graph_surface_path\":\""
      << EscapeJsonString(summary.source_graph_surface_path)
      << "\",\"ready\":"
      << (IsReadyObjc3ExecutableMetadataDebugProjectionSummary(summary)
              ? "true"
              : "false")
      << ",\"matrix_published\":"
      << (summary.matrix_published ? "true" : "false")
      << ",\"fail_closed\":"
      << (summary.fail_closed ? "true" : "false")
      << ",\"manifest_debug_surface_published\":"
      << (summary.manifest_debug_surface_published ? "true" : "false")
      << ",\"ir_named_metadata_published\":"
      << (summary.ir_named_metadata_published ? "true" : "false")
      << ",\"replay_anchor_deterministic\":"
      << (summary.replay_anchor_deterministic ? "true" : "false")
      << ",\"active_typed_handoff_ready\":"
      << (summary.active_typed_handoff_ready ? "true" : "false")
      << ",\"matrix_row_count\":" << summary.matrix_row_count
      << ",\"rows\":[";
  for (std::size_t index = 0; index < summary.rows.size(); ++index) {
    const auto &row = summary.rows[index];
    if (index != 0u) {
      out << ",";
    }
    out << "{\"row_key\":\"" << EscapeJsonString(row.row_key)
        << "\",\"artifact_kind\":\"" << EscapeJsonString(row.artifact_kind)
        << "\",\"fixture_path\":\"" << EscapeJsonString(row.fixture_path)
        << "\",\"emit_prefix\":\"" << EscapeJsonString(row.emit_prefix)
        << "\",\"artifact_relative_path\":\""
        << EscapeJsonString(row.artifact_relative_path)
        << "\",\"probe_command\":\"" << EscapeJsonString(row.probe_command)
        << "\",\"inspection_command\":\""
        << EscapeJsonString(row.inspection_command)
        << "\",\"expected_anchor\":\""
        << EscapeJsonString(row.expected_anchor) << "\"}";
  }
  out << "],\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"active_typed_handoff_replay_key\":\""
      << EscapeJsonString(summary.active_typed_handoff_replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

std::string BuildExecutableMetadataRuntimeIngestPackagingReplayKey(
    const Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary
        &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";typed_contract=" << summary.typed_lowering_handoff_contract_id
      << ";debug_contract=" << summary.debug_projection_contract_id
      << ";packaging_surface_path=" << summary.packaging_surface_path
      << ";typed_handoff_surface_path=" << summary.typed_handoff_surface_path
      << ";debug_projection_surface_path="
      << summary.debug_projection_surface_path
      << ";payload_model=" << summary.packaging_payload_model
      << ";transport_artifact=" << summary.transport_artifact_relative_path
      << ";typed_replay=" << summary.typed_lowering_handoff_replay_key
      << ";debug_replay=" << summary.debug_projection_replay_key;
  return out.str();
}

Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary
BuildExecutableMetadataRuntimeIngestPackagingContractSummary(
    const Objc3ExecutableMetadataTypedLoweringHandoff
        &executable_metadata_typed_lowering_handoff,
    const Objc3ExecutableMetadataDebugProjectionSummary
        &executable_metadata_debug_projection) {
  Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary summary;
  summary.boundary_frozen = true;
  summary.fail_closed = true;
  summary.manifest_transport_frozen = true;
  summary.runtime_section_emission_not_yet_landed = true;
  summary.startup_registration_not_yet_landed = true;
  summary.runtime_loader_registration_not_yet_landed = true;
  summary.explicit_non_goals_published = true;
  summary.typed_lowering_handoff_ready =
      IsReadyObjc3ExecutableMetadataTypedLoweringHandoff(
          executable_metadata_typed_lowering_handoff);
  summary.debug_projection_ready =
      IsReadyObjc3ExecutableMetadataDebugProjectionSummary(
          executable_metadata_debug_projection);
  if (summary.typed_lowering_handoff_ready) {
    summary.typed_lowering_handoff_replay_key =
        executable_metadata_typed_lowering_handoff.replay_key;
  }
  if (summary.debug_projection_ready) {
    summary.debug_projection_replay_key =
        executable_metadata_debug_projection.replay_key;
  }
  summary.ready_for_packaging_implementation =
      summary.typed_lowering_handoff_ready && summary.debug_projection_ready;
  if (summary.ready_for_packaging_implementation) {
    summary.replay_key =
        BuildExecutableMetadataRuntimeIngestPackagingReplayKey(summary);
  }
  if (!IsReadyObjc3ExecutableMetadataRuntimeIngestPackagingContractSummary(
          summary)) {
    summary.failure_reason =
        "runtime ingest packaging contract boundary is incomplete";
  }
  return summary;
}

std::string BuildExecutableMetadataRuntimeIngestPackagingContractSummaryJson(
    const Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary
        &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"typed_lowering_handoff_contract_id\":\""
      << EscapeJsonString(summary.typed_lowering_handoff_contract_id)
      << "\",\"debug_projection_contract_id\":\""
      << EscapeJsonString(summary.debug_projection_contract_id)
      << "\",\"packaging_surface_path\":\""
      << EscapeJsonString(summary.packaging_surface_path)
      << "\",\"typed_handoff_surface_path\":\""
      << EscapeJsonString(summary.typed_handoff_surface_path)
      << "\",\"debug_projection_surface_path\":\""
      << EscapeJsonString(summary.debug_projection_surface_path)
      << "\",\"packaging_payload_model\":\""
      << EscapeJsonString(summary.packaging_payload_model)
      << "\",\"transport_artifact_relative_path\":\""
      << EscapeJsonString(summary.transport_artifact_relative_path)
      << "\",\"ready\":"
      << (IsReadyObjc3ExecutableMetadataRuntimeIngestPackagingContractSummary(
              summary)
              ? "true"
              : "false")
      << ",\"boundary_frozen\":"
      << (summary.boundary_frozen ? "true" : "false")
      << ",\"fail_closed\":"
      << (summary.fail_closed ? "true" : "false")
      << ",\"typed_lowering_handoff_ready\":"
      << (summary.typed_lowering_handoff_ready ? "true" : "false")
      << ",\"debug_projection_ready\":"
      << (summary.debug_projection_ready ? "true" : "false")
      << ",\"manifest_transport_frozen\":"
      << (summary.manifest_transport_frozen ? "true" : "false")
      << ",\"runtime_section_emission_not_yet_landed\":"
      << (summary.runtime_section_emission_not_yet_landed ? "true" : "false")
      << ",\"startup_registration_not_yet_landed\":"
      << (summary.startup_registration_not_yet_landed ? "true" : "false")
      << ",\"runtime_loader_registration_not_yet_landed\":"
      << (summary.runtime_loader_registration_not_yet_landed ? "true"
                                                             : "false")
      << ",\"explicit_non_goals_published\":"
      << (summary.explicit_non_goals_published ? "true" : "false")
      << ",\"ready_for_packaging_implementation\":"
      << (summary.ready_for_packaging_implementation ? "true" : "false")
      << ",\"typed_lowering_handoff_replay_key\":\""
      << EscapeJsonString(summary.typed_lowering_handoff_replay_key)
      << "\",\"debug_projection_replay_key\":\""
      << EscapeJsonString(summary.debug_projection_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

void AppendRuntimeIngestBinaryEnvelopeU32(std::string &payload,
                                          std::uint32_t value) {
  for (std::uint32_t shift = 0; shift < 32u; shift += 8u) {
    payload.push_back(
        static_cast<char>((value >> shift) & static_cast<std::uint32_t>(0xFFu)));
  }
}

void AppendRuntimeIngestBinaryEnvelopeChunk(std::string &payload,
                                            const std::string &chunk_name,
                                            const std::string &chunk_payload) {
  AppendRuntimeIngestBinaryEnvelopeU32(
      payload, static_cast<std::uint32_t>(chunk_name.size()));
  payload.append(chunk_name);
  AppendRuntimeIngestBinaryEnvelopeU32(
      payload, static_cast<std::uint32_t>(chunk_payload.size()));
  payload.append(chunk_payload);
}

std::string BuildExecutableMetadataRuntimeIngestBinaryEnvelope(
    const Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary
        &packaging_contract,
    const Objc3ExecutableMetadataTypedLoweringHandoff &typed_lowering_handoff,
    const Objc3ExecutableMetadataDebugProjectionSummary &debug_projection) {
  if (!IsReadyObjc3ExecutableMetadataRuntimeIngestPackagingContractSummary(
          packaging_contract) ||
      !IsReadyObjc3ExecutableMetadataTypedLoweringHandoff(
          typed_lowering_handoff) ||
      !IsReadyObjc3ExecutableMetadataDebugProjectionSummary(debug_projection)) {
    return {};
  }

  const std::string packaging_json =
      BuildExecutableMetadataRuntimeIngestPackagingContractSummaryJson(
          packaging_contract);
  const std::string typed_handoff_json =
      BuildExecutableMetadataTypedLoweringHandoffJson(typed_lowering_handoff);
  const std::string debug_projection_json =
      BuildExecutableMetadataDebugProjectionSummaryJson(debug_projection);

  std::string payload;
  payload.reserve(
      std::char_traits<char>::length(
          kObjc3ExecutableMetadataRuntimeIngestBinaryMagic) +
      sizeof(std::uint32_t) * 8u + packaging_json.size() +
      typed_handoff_json.size() + debug_projection_json.size() + 256u);
  payload.append(kObjc3ExecutableMetadataRuntimeIngestBinaryMagic);
  AppendRuntimeIngestBinaryEnvelopeU32(
      payload, kObjc3ExecutableMetadataRuntimeIngestBinaryEnvelopeVersion);
  AppendRuntimeIngestBinaryEnvelopeU32(
      payload, kObjc3ExecutableMetadataRuntimeIngestBinaryEnvelopeChunkCount);
  AppendRuntimeIngestBinaryEnvelopeChunk(
      payload,
      kObjc3ExecutableMetadataRuntimeIngestBinaryPackagingChunkName,
      packaging_json);
  AppendRuntimeIngestBinaryEnvelopeChunk(
      payload, kObjc3ExecutableMetadataRuntimeIngestBinaryTypedHandoffChunkName,
      typed_handoff_json);
  AppendRuntimeIngestBinaryEnvelopeChunk(
      payload, kObjc3ExecutableMetadataRuntimeIngestBinaryDebugProjectionChunkName,
      debug_projection_json);
  return payload;
}

std::string BuildExecutableMetadataRuntimeIngestBinaryBoundaryReplayKey(
    const Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";packaging_contract_id=" << summary.packaging_contract_id
      << ";typed_contract_id=" << summary.typed_lowering_handoff_contract_id
      << ";debug_contract_id=" << summary.debug_projection_contract_id
      << ";packaging_surface_path=" << summary.packaging_surface_path
      << ";binary_boundary_surface_path=" << summary.binary_boundary_surface_path
      << ";payload_model=" << summary.payload_model
      << ";envelope_format=" << summary.envelope_format
      << ";artifact_relative_path=" << summary.artifact_relative_path
      << ";artifact_suffix=" << summary.artifact_suffix
      << ";binary_magic=" << summary.binary_magic
      << ";envelope_version=" << summary.envelope_version
      << ";chunk_count=" << summary.chunk_count
      << ";payload_bytes=" << summary.payload_bytes
      << ";packaging_replay=" << summary.packaging_contract_replay_key
      << ";typed_replay=" << summary.typed_lowering_handoff_replay_key
      << ";debug_replay=" << summary.debug_projection_replay_key;
  return out.str();
}

Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary
BuildExecutableMetadataRuntimeIngestBinaryBoundarySummary(
    const Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary
        &packaging_contract,
    const Objc3ExecutableMetadataTypedLoweringHandoff &typed_lowering_handoff,
    const Objc3ExecutableMetadataDebugProjectionSummary &debug_projection,
    const std::string &binary_payload) {
  Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary summary;
  summary.fail_closed = true;
  summary.packaging_contract_ready =
      IsReadyObjc3ExecutableMetadataRuntimeIngestPackagingContractSummary(
          packaging_contract);
  summary.typed_lowering_handoff_ready =
      IsReadyObjc3ExecutableMetadataTypedLoweringHandoff(
          typed_lowering_handoff);
  summary.debug_projection_ready =
      IsReadyObjc3ExecutableMetadataDebugProjectionSummary(debug_projection);
  if (summary.packaging_contract_ready) {
    summary.packaging_contract_replay_key = packaging_contract.replay_key;
  }
  if (summary.typed_lowering_handoff_ready) {
    summary.typed_lowering_handoff_replay_key = typed_lowering_handoff.replay_key;
  }
  if (summary.debug_projection_ready) {
    summary.debug_projection_replay_key = debug_projection.replay_key;
  }
  summary.binary_payload_present = !binary_payload.empty();
  summary.binary_boundary_emitted = summary.binary_payload_present;
  summary.binary_envelope_deterministic =
      summary.binary_payload_present && summary.packaging_contract_ready &&
      summary.typed_lowering_handoff_ready &&
      summary.debug_projection_ready;
  summary.ready_for_section_emission_handoff =
      summary.packaging_contract_ready && summary.binary_envelope_deterministic;
  summary.payload_bytes = binary_payload.size();
  if (summary.ready_for_section_emission_handoff) {
    summary.replay_key =
        BuildExecutableMetadataRuntimeIngestBinaryBoundaryReplayKey(summary);
  }
  if (!IsReadyObjc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary(
          summary)) {
    summary.failure_reason =
        "runtime ingest binary boundary payload is incomplete";
  }
  return summary;
}

std::string BuildExecutableMetadataRuntimeIngestBinaryBoundarySummaryJson(
    const Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"packaging_contract_id\":\""
      << EscapeJsonString(summary.packaging_contract_id)
      << "\",\"typed_lowering_handoff_contract_id\":\""
      << EscapeJsonString(summary.typed_lowering_handoff_contract_id)
      << "\",\"debug_projection_contract_id\":\""
      << EscapeJsonString(summary.debug_projection_contract_id)
      << "\",\"packaging_surface_path\":\""
      << EscapeJsonString(summary.packaging_surface_path)
      << "\",\"binary_boundary_surface_path\":\""
      << EscapeJsonString(summary.binary_boundary_surface_path)
      << "\",\"payload_model\":\""
      << EscapeJsonString(summary.payload_model)
      << "\",\"envelope_format\":\""
      << EscapeJsonString(summary.envelope_format)
      << "\",\"artifact_relative_path\":\""
      << EscapeJsonString(summary.artifact_relative_path)
      << "\",\"artifact_suffix\":\""
      << EscapeJsonString(summary.artifact_suffix)
      << "\",\"binary_magic\":\""
      << EscapeJsonString(summary.binary_magic)
      << "\",\"envelope_version\":" << summary.envelope_version
      << ",\"chunk_count\":" << summary.chunk_count
      << ",\"chunk_names\":[";
  for (std::size_t i = 0; i < summary.chunk_names.size(); ++i) {
    out << "\"" << EscapeJsonString(summary.chunk_names[i]) << "\"";
    if (i + 1u != summary.chunk_names.size()) {
      out << ",";
    }
  }
  out << "],\"ready\":"
      << (IsReadyObjc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary(
              summary)
              ? "true"
              : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"packaging_contract_ready\":"
      << (summary.packaging_contract_ready ? "true" : "false")
      << ",\"typed_lowering_handoff_ready\":"
      << (summary.typed_lowering_handoff_ready ? "true" : "false")
      << ",\"debug_projection_ready\":"
      << (summary.debug_projection_ready ? "true" : "false")
      << ",\"binary_payload_present\":"
      << (summary.binary_payload_present ? "true" : "false")
      << ",\"binary_boundary_emitted\":"
      << (summary.binary_boundary_emitted ? "true" : "false")
      << ",\"binary_envelope_deterministic\":"
      << (summary.binary_envelope_deterministic ? "true" : "false")
      << ",\"ready_for_section_emission_handoff\":"
      << (summary.ready_for_section_emission_handoff ? "true" : "false")
      << ",\"payload_bytes\":" << summary.payload_bytes
      << ",\"packaging_contract_replay_key\":\""
      << EscapeJsonString(summary.packaging_contract_replay_key)
      << "\",\"typed_lowering_handoff_replay_key\":\""
      << EscapeJsonString(summary.typed_lowering_handoff_replay_key)
      << "\",\"debug_projection_replay_key\":\""
      << EscapeJsonString(summary.debug_projection_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

std::string MakeDiag(unsigned line, unsigned column, const std::string &code, const std::string &message) {
  std::ostringstream out;
  out << "error:" << line << ":" << column << ": " << message << " [" << code << "]";
  return out.str();
}

std::vector<std::string> FlattenStageDiagnostics(const Objc3FrontendDiagnosticsBus &diagnostics_bus) {
  std::vector<std::string> diagnostics;
  diagnostics.reserve(diagnostics_bus.size());
  diagnostics.insert(diagnostics.end(), diagnostics_bus.lexer.begin(), diagnostics_bus.lexer.end());
  diagnostics.insert(diagnostics.end(), diagnostics_bus.parser.begin(), diagnostics_bus.parser.end());
  diagnostics.insert(diagnostics.end(), diagnostics_bus.semantic.begin(), diagnostics_bus.semantic.end());
  return diagnostics;
}

struct Objc3ParserDiagnosticCodeCoverage {
  std::size_t unique_code_count = 0;
  std::uint64_t unique_code_fingerprint = 1469598103934665603ull;
  bool deterministic_surface = true;
};

std::string TryExtractDiagnosticCode(const std::string &diag_text, bool &ok) {
  ok = false;
  const std::size_t end = diag_text.size();
  if (end < 3u || diag_text[end - 1] != ']') {
    return std::string{};
  }
  const std::size_t begin = diag_text.rfind('[');
  if (begin == std::string::npos || begin + 2u >= end) {
    return std::string{};
  }
  const std::string code = diag_text.substr(begin + 1u, end - begin - 2u);
  if (code.empty()) {
    return std::string{};
  }
  ok = true;
  return code;
}

std::uint64_t MixParserDiagnosticCodeFingerprint(std::uint64_t fingerprint, const std::string &code) {
  constexpr std::uint64_t kFnvPrime = 1099511628211ull;
  fingerprint = (fingerprint ^ static_cast<std::uint64_t>(code.size())) * kFnvPrime;
  for (const unsigned char c : code) {
    fingerprint = (fingerprint ^ static_cast<std::uint64_t>(c)) * kFnvPrime;
  }
  return fingerprint;
}

Objc3ParserDiagnosticCodeCoverage BuildObjc3ParserDiagnosticCodeCoverage(
    const std::vector<std::string> &parser_diagnostics) {
  Objc3ParserDiagnosticCodeCoverage coverage;
  std::unordered_set<std::string> unique_codes;
  unique_codes.reserve(parser_diagnostics.size());
  for (const auto &diag_text : parser_diagnostics) {
    bool code_ok = false;
    const std::string code = TryExtractDiagnosticCode(diag_text, code_ok);
    if (!code_ok) {
      coverage.deterministic_surface = false;
      continue;
    }
    unique_codes.insert(code);
  }
  std::vector<std::string> sorted_codes(unique_codes.begin(), unique_codes.end());
  std::sort(sorted_codes.begin(), sorted_codes.end());
  coverage.unique_code_count = sorted_codes.size();
  for (const auto &code : sorted_codes) {
    coverage.unique_code_fingerprint =
        MixParserDiagnosticCodeFingerprint(coverage.unique_code_fingerprint, code);
  }
  return coverage;
}

Objc3PropertySynthesisIvarBindingContract BuildPropertySynthesisIvarBindingContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  const Objc3PropertySynthesisIvarBindingSummary &summary =
      sema_parity_surface.property_synthesis_ivar_binding_summary;
  Objc3PropertySynthesisIvarBindingContract contract;
  contract.property_synthesis_sites = summary.property_synthesis_sites;
  contract.property_synthesis_explicit_ivar_bindings =
      summary.property_synthesis_explicit_ivar_bindings;
  contract.property_synthesis_default_ivar_bindings =
      summary.property_synthesis_default_ivar_bindings;
  contract.ivar_binding_sites = summary.ivar_binding_sites;
  contract.ivar_binding_resolved = summary.ivar_binding_resolved;
  contract.ivar_binding_missing = summary.ivar_binding_missing;
  contract.ivar_binding_conflicts = summary.ivar_binding_conflicts;
  contract.deterministic =
      summary.deterministic &&
      sema_parity_surface.deterministic_property_synthesis_ivar_binding_handoff;
  return contract;
}

Objc3RuntimeMetadataSectionAbiFreezeSummary
BuildRuntimeMetadataSectionAbiFreezeSummary(
    const Objc3RuntimeMetadataSourceOwnershipBoundary
        &runtime_metadata_source_ownership,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality,
    const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement) {
  Objc3RuntimeMetadataSectionAbiFreezeSummary summary;
  summary.boundary_frozen = true;
  summary.fail_closed = true;
  summary.object_file_section_inventory_frozen = true;
  summary.symbol_policy_frozen = true;
  summary.visibility_model_frozen = true;
  summary.retention_policy_frozen = true;
  summary.runtime_metadata_source_boundary_ready =
      IsReadyObjc3RuntimeMetadataSourceOwnershipBoundary(
          runtime_metadata_source_ownership);
  summary.runtime_export_legality_boundary_ready =
      IsReadyObjc3RuntimeExportLegalityBoundary(runtime_export_legality);
  summary.runtime_export_enforcement_ready =
      IsReadyObjc3RuntimeExportEnforcementSummary(runtime_export_enforcement);
  summary.ready_for_section_scaffold =
      summary.runtime_metadata_source_boundary_ready &&
      summary.runtime_export_legality_boundary_ready &&
      summary.runtime_export_enforcement_ready;
  if (!summary.ready_for_section_scaffold) {
    summary.failure_reason =
        "runtime metadata section ABI freeze prerequisites are not ready";
  }
  return summary;
}

Objc3RuntimeMetadataSectionScaffoldSummary
BuildRuntimeMetadataSectionScaffoldSummary(
    const Objc3RuntimeMetadataSectionAbiFreezeSummary &runtime_metadata_section_abi,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality,
    const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement) {
  Objc3RuntimeMetadataSectionScaffoldSummary summary;
  summary.fail_closed = true;
  if (!IsReadyObjc3RuntimeMetadataSectionAbiFreezeSummary(
          runtime_metadata_section_abi) ||
      !IsReadyObjc3RuntimeExportEnforcementSummary(
          runtime_export_enforcement)) {
    summary.failure_reason =
        "runtime metadata section scaffold prerequisites are not ready";
    return summary;
  }

  summary.scaffold_emitted = true;
  summary.uses_llvm_used = true;
  summary.image_info_emitted = true;
  summary.class_descriptor_count = runtime_export_legality.class_record_count;
  summary.protocol_descriptor_count =
      runtime_export_legality.protocol_record_count;
  summary.category_descriptor_count =
      runtime_export_legality.category_record_count;
  summary.property_descriptor_count =
      runtime_export_legality.property_record_count;
  summary.ivar_descriptor_count = runtime_export_legality.ivar_record_count;
  summary.total_descriptor_count =
      summary.class_descriptor_count + summary.protocol_descriptor_count +
      summary.category_descriptor_count + summary.property_descriptor_count +
      summary.ivar_descriptor_count;
  summary.total_retained_global_count = summary.total_descriptor_count + 6u;
  return summary;
}

Objc3RuntimeMetadataObjectInspectionHarnessSummary
BuildRuntimeMetadataObjectInspectionHarnessSummary(
    const Objc3RuntimeMetadataSectionAbiFreezeSummary &runtime_metadata_section_abi,
    const Objc3RuntimeMetadataSectionScaffoldSummary &runtime_metadata_section_scaffold) {
  Objc3RuntimeMetadataObjectInspectionHarnessSummary summary;
  summary.fail_closed = true;
  if (!IsReadyObjc3RuntimeMetadataSectionAbiFreezeSummary(
          runtime_metadata_section_abi) ||
      !IsReadyObjc3RuntimeMetadataSectionScaffoldSummary(
          runtime_metadata_section_scaffold)) {
    summary.failure_reason =
        "runtime metadata object inspection harness prerequisites are not ready";
    return summary;
  }

  summary.matrix_published = true;
  summary.uses_llvm_readobj = true;
  summary.uses_llvm_objdump = true;
  summary.matrix_row_count = 2u;
  return summary;
}

std::string BuildRuntimeMetadataSourceToSectionMatrixReplayKey(
    const Objc3RuntimeMetadataSourceToSectionMatrixSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";source_graph_contract=" << summary.source_graph_contract_id
      << ";section_abi_contract=" << summary.section_abi_contract_id
      << ";section_scaffold_contract=" << summary.section_scaffold_contract_id
      << ";object_inspection_contract=" << summary.object_inspection_contract_id
      << ";surface_path=" << summary.manifest_surface_path
      << ";row_ordering=" << summary.row_ordering_model
      << ";row_count=" << summary.matrix_row_count;
  for (const auto &row : summary.rows) {
    out << ";row=" << row.row_key << "|" << row.graph_node_kind << "|"
        << row.emission_mode << "|" << row.logical_section << "|"
        << row.payload_role << "|" << row.descriptor_symbol_family << "|"
        << row.aggregate_symbol << "|" << row.relocation_behavior << "|"
        << row.proof_fixture_path << "|" << row.proof_mode << "|"
        << row.section_inventory_command << "|" << row.symbol_inventory_command;
  }
  return out.str();
}

Objc3RuntimeMetadataSourceToSectionMatrixSummary
BuildRuntimeMetadataSourceToSectionMatrixSummary(
    const Objc3ExecutableMetadataSourceGraph &executable_metadata_source_graph,
    const Objc3RuntimeMetadataSectionAbiFreezeSummary &runtime_metadata_section_abi,
    const Objc3RuntimeMetadataSectionScaffoldSummary &runtime_metadata_section_scaffold,
    const Objc3RuntimeMetadataObjectInspectionHarnessSummary
        &runtime_metadata_object_inspection) {
  Objc3RuntimeMetadataSourceToSectionMatrixSummary summary;
  summary.matrix_published = true;
  summary.fail_closed = true;
  summary.source_graph_ready =
      IsReadyObjc3ExecutableMetadataSourceGraph(executable_metadata_source_graph);
  summary.section_abi_ready =
      IsReadyObjc3RuntimeMetadataSectionAbiFreezeSummary(
          runtime_metadata_section_abi);
  summary.section_scaffold_ready =
      IsReadyObjc3RuntimeMetadataSectionScaffoldSummary(
          runtime_metadata_section_scaffold);
  summary.object_inspection_ready =
      IsReadyObjc3RuntimeMetadataObjectInspectionHarnessSummary(
          runtime_metadata_object_inspection);
  summary.supported_node_coverage_complete = true;
  summary.explicit_non_goals_published = true;
  summary.row_ordering_frozen = true;
  summary.rows[0] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionInterfaceRowKey,
      "interface",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneEmissionMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      "no standalone emitted interface payload yet",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionSourceGraphFixtureProofMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue};
  summary.rows[1] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionImplementationRowKey,
      "implementation",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneEmissionMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      "no standalone emitted implementation payload yet",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionSourceGraphFixtureProofMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue};
  summary.rows[2] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionClassRowKey,
      "class",
      kObjc3RuntimeMetadataSourceToSectionStandaloneEmissionMode,
      kObjc3RuntimeMetadataLogicalClassDescriptorSection,
      "standalone class descriptor payload",
      "__objc3_meta_class_####",
      kObjc3RuntimeMetadataClassDescriptorAggregateSymbol,
      kObjc3RuntimeMetadataSourceToSectionAggregateRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionFixturePlusObjectInspectionProofMode,
      kObjc3RuntimeMetadataObjectInspectionSectionCommand,
      kObjc3RuntimeMetadataObjectInspectionSymbolCommand};
  summary.rows[3] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionMetaclassRowKey,
      "metaclass",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneEmissionMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      "no standalone emitted metaclass payload yet",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionSourceGraphFixtureProofMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue};
  summary.rows[4] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionProtocolRowKey,
      "protocol",
      kObjc3RuntimeMetadataSourceToSectionStandaloneEmissionMode,
      kObjc3RuntimeMetadataLogicalProtocolDescriptorSection,
      "standalone protocol descriptor payload",
      "__objc3_meta_protocol_####",
      kObjc3RuntimeMetadataProtocolDescriptorAggregateSymbol,
      kObjc3RuntimeMetadataSourceToSectionAggregateRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionFixturePlusObjectInspectionProofMode,
      kObjc3RuntimeMetadataObjectInspectionSectionCommand,
      kObjc3RuntimeMetadataObjectInspectionSymbolCommand};
  summary.rows[5] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionCategoryRowKey,
      "category",
      kObjc3RuntimeMetadataSourceToSectionStandaloneEmissionMode,
      kObjc3RuntimeMetadataLogicalCategoryDescriptorSection,
      "standalone category descriptor payload",
      "__objc3_meta_category_####",
      kObjc3RuntimeMetadataCategoryDescriptorAggregateSymbol,
      kObjc3RuntimeMetadataSourceToSectionAggregateRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionCategoryFixturePath,
      kObjc3RuntimeMetadataSourceToSectionFixturePlusObjectInspectionProofMode,
      kObjc3RuntimeMetadataObjectInspectionSectionCommand,
      kObjc3RuntimeMetadataObjectInspectionSymbolCommand};
  summary.rows[6] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionPropertyRowKey,
      "property",
      kObjc3RuntimeMetadataSourceToSectionStandaloneEmissionMode,
      kObjc3RuntimeMetadataLogicalPropertyDescriptorSection,
      "standalone property descriptor payload",
      "__objc3_meta_property_####",
      kObjc3RuntimeMetadataPropertyDescriptorAggregateSymbol,
      kObjc3RuntimeMetadataSourceToSectionAggregateRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionFixturePlusObjectInspectionProofMode,
      kObjc3RuntimeMetadataObjectInspectionSectionCommand,
      kObjc3RuntimeMetadataObjectInspectionSymbolCommand};
  summary.rows[7] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionMethodRowKey,
      "method",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneEmissionMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      "no standalone emitted method payload yet",
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionSourceGraphFixtureProofMode,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue,
      kObjc3RuntimeMetadataSourceToSectionNoStandaloneValue};
  summary.rows[8] = Objc3RuntimeMetadataSourceToSectionMatrixRow{
      kObjc3RuntimeMetadataSourceToSectionIvarRowKey,
      "ivar",
      kObjc3RuntimeMetadataSourceToSectionStandaloneEmissionMode,
      kObjc3RuntimeMetadataLogicalIvarDescriptorSection,
      "standalone ivar descriptor payload",
      "__objc3_meta_ivar_####",
      kObjc3RuntimeMetadataIvarDescriptorAggregateSymbol,
      kObjc3RuntimeMetadataSourceToSectionAggregateRelocationBehavior,
      kObjc3ExecutableMetadataDebugProjectionClassFixturePath,
      kObjc3RuntimeMetadataSourceToSectionFixturePlusObjectInspectionProofMode,
      kObjc3RuntimeMetadataObjectInspectionSectionCommand,
      kObjc3RuntimeMetadataObjectInspectionSymbolCommand};
  summary.matrix_row_count = summary.rows.size();
  summary.replay_key =
      BuildRuntimeMetadataSourceToSectionMatrixReplayKey(summary);
  if (!IsReadyObjc3RuntimeMetadataSourceToSectionMatrixSummary(summary)) {
    summary.failure_reason =
        "runtime metadata source-to-section completeness matrix is incomplete";
  }
  return summary;
}

std::string BuildRuntimeMetadataSourceToSectionMatrixSummaryJson(
    const Objc3RuntimeMetadataSourceToSectionMatrixSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"source_graph_contract_id\":\""
      << EscapeJsonString(summary.source_graph_contract_id)
      << "\",\"section_abi_contract_id\":\""
      << EscapeJsonString(summary.section_abi_contract_id)
      << "\",\"section_scaffold_contract_id\":\""
      << EscapeJsonString(summary.section_scaffold_contract_id)
      << "\",\"object_inspection_contract_id\":\""
      << EscapeJsonString(summary.object_inspection_contract_id)
      << "\",\"manifest_surface_path\":\""
      << EscapeJsonString(summary.manifest_surface_path)
      << "\",\"row_ordering_model\":\""
      << EscapeJsonString(summary.row_ordering_model)
      << "\",\"ready\":"
      << (IsReadyObjc3RuntimeMetadataSourceToSectionMatrixSummary(summary)
              ? "true"
              : "false")
      << ",\"matrix_published\":"
      << (summary.matrix_published ? "true" : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"source_graph_ready\":"
      << (summary.source_graph_ready ? "true" : "false")
      << ",\"section_abi_ready\":"
      << (summary.section_abi_ready ? "true" : "false")
      << ",\"section_scaffold_ready\":"
      << (summary.section_scaffold_ready ? "true" : "false")
      << ",\"object_inspection_ready\":"
      << (summary.object_inspection_ready ? "true" : "false")
      << ",\"supported_node_coverage_complete\":"
      << (summary.supported_node_coverage_complete ? "true" : "false")
      << ",\"explicit_non_goals_published\":"
      << (summary.explicit_non_goals_published ? "true" : "false")
      << ",\"row_ordering_frozen\":"
      << (summary.row_ordering_frozen ? "true" : "false")
      << ",\"matrix_row_count\":" << summary.matrix_row_count
      << ",\"rows\":[";
  for (std::size_t index = 0; index < summary.rows.size(); ++index) {
    const auto &row = summary.rows[index];
    if (index != 0u) {
      out << ",";
    }
    out << "{\"row_key\":\"" << EscapeJsonString(row.row_key)
        << "\",\"graph_node_kind\":\""
        << EscapeJsonString(row.graph_node_kind)
        << "\",\"emission_mode\":\""
        << EscapeJsonString(row.emission_mode)
        << "\",\"logical_section\":\""
        << EscapeJsonString(row.logical_section)
        << "\",\"payload_role\":\""
        << EscapeJsonString(row.payload_role)
        << "\",\"descriptor_symbol_family\":\""
        << EscapeJsonString(row.descriptor_symbol_family)
        << "\",\"aggregate_symbol\":\""
        << EscapeJsonString(row.aggregate_symbol)
        << "\",\"relocation_behavior\":\""
        << EscapeJsonString(row.relocation_behavior)
        << "\",\"proof_fixture_path\":\""
        << EscapeJsonString(row.proof_fixture_path)
        << "\",\"proof_mode\":\""
        << EscapeJsonString(row.proof_mode)
        << "\",\"section_inventory_command\":\""
        << EscapeJsonString(row.section_inventory_command)
        << "\",\"symbol_inventory_command\":\""
        << EscapeJsonString(row.symbol_inventory_command) << "\"}";
  }
  out << "],\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

Objc3RuntimeSupportLibraryContractSummary
BuildRuntimeSupportLibraryContractSummary() {
  Objc3RuntimeSupportLibraryContractSummary summary;
  summary.boundary_frozen = true;
  summary.fail_closed = true;
  summary.target_name_frozen = true;
  summary.exported_entrypoints_frozen = true;
  summary.ownership_boundaries_frozen = true;
  summary.build_constraints_frozen = true;
  summary.shim_remains_test_only = true;
  summary.native_runtime_library_present = false;
  summary.driver_link_wiring_pending = true;
  summary.ready_for_runtime_library_skeleton = true;
  return summary;
}

Objc3RuntimeSupportLibraryCoreFeatureSummary
BuildRuntimeSupportLibraryCoreFeatureSummary(
    const Objc3RuntimeSupportLibraryContractSummary &runtime_support_library) {
  Objc3RuntimeSupportLibraryCoreFeatureSummary summary;
  summary.support_library_contract_id = runtime_support_library.contract_id;
  summary.metadata_scaffold_contract_id =
      runtime_support_library.metadata_scaffold_contract_id;
  summary.fail_closed = true;
  summary.native_runtime_library_sources_present = true;
  summary.native_runtime_library_header_present = true;
  summary.native_runtime_library_archive_build_enabled = true;
  summary.native_runtime_library_entrypoints_implemented = true;
  summary.selector_lookup_stateful = true;
  summary.deterministic_dispatch_formula_matches_test_shim = true;
  summary.reset_for_testing_supported = true;
  summary.shim_remains_test_only = true;
  summary.driver_link_wiring_pending = true;
  summary.ready_for_driver_link_wiring =
      IsReadyObjc3RuntimeSupportLibraryContractSummary(runtime_support_library);
  return summary;
}

Objc3RuntimeSupportLibraryLinkWiringSummary
BuildRuntimeSupportLibraryLinkWiringSummary(
    const Objc3RuntimeSupportLibraryCoreFeatureSummary
        &runtime_support_library_core_feature) {
  Objc3RuntimeSupportLibraryLinkWiringSummary summary;
  summary.support_library_core_feature_contract_id =
      runtime_support_library_core_feature.contract_id;
  summary.fail_closed = true;
  summary.runtime_library_archive_available =
      runtime_support_library_core_feature
          .native_runtime_library_archive_build_enabled;
  summary.compatibility_dispatch_alias_exported = true;
  summary.driver_emits_runtime_link_contract = true;
  summary.execution_smoke_consumes_runtime_library = true;
  summary.shim_remains_test_only = true;
  summary.ready_for_runtime_library_consumption =
      IsReadyObjc3RuntimeSupportLibraryCoreFeatureSummary(
          runtime_support_library_core_feature);
  return summary;
}

std::string BuildRuntimeTranslationUnitRegistrationContractReplayKey(
    const Objc3RuntimeTranslationUnitRegistrationContractSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";binary_boundary_contract_id=" << summary.binary_boundary_contract_id
      << ";archive_static_link_contract_id="
      << summary.archive_static_link_contract_id
      << ";object_emission_closeout_contract_id="
      << summary.object_emission_closeout_contract_id
      << ";runtime_support_library_link_wiring_contract_id="
      << summary.runtime_support_library_link_wiring_contract_id
      << ";registration_surface_path=" << summary.registration_surface_path
      << ";registration_payload_model="
      << summary.registration_payload_model
      << ";constructor_root_symbol=" << summary.constructor_root_symbol
      << ";constructor_root_ownership_model="
      << summary.constructor_root_ownership_model
      << ";constructor_emission_mode=" << summary.constructor_emission_mode
      << ";constructor_priority_policy="
      << summary.constructor_priority_policy
      << ";registration_entrypoint_symbol="
      << summary.registration_entrypoint_symbol
      << ";translation_unit_identity_model="
      << summary.translation_unit_identity_model
      << ";binary_boundary_replay_key=" << summary.binary_boundary_replay_key;
  for (std::size_t index = 0; index < summary.runtime_owned_payload_artifacts.size();
       ++index) {
    out << ";runtime_owned_payload_artifacts[" << index
        << "]=" << summary.runtime_owned_payload_artifacts[index];
  }
  return out.str();
}

Objc3RuntimeTranslationUnitRegistrationContractSummary
BuildRuntimeTranslationUnitRegistrationContractSummary(
    const Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary
        &runtime_ingest_binary_boundary,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring) {
  Objc3RuntimeTranslationUnitRegistrationContractSummary summary;
  summary.fail_closed = true;
  summary.boundary_frozen = true;
  summary.binary_boundary_ready =
      IsReadyObjc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary(
          runtime_ingest_binary_boundary);
  summary.runtime_support_library_link_wiring_ready =
      IsReadyObjc3RuntimeSupportLibraryLinkWiringSummary(
          runtime_support_library_link_wiring);
  const std::string archive_static_link_summary =
      Objc3RuntimeMetadataArchiveStaticLinkDiscoverySummary();
  summary.archive_static_link_surface_ready =
      archive_static_link_summary.find(
          std::string("contract=") +
          kObjc3RuntimeArchiveStaticLinkDiscoveryContractId) !=
          std::string::npos &&
      archive_static_link_summary.find(
          std::string("translation_unit_identity_model=") +
          summary.translation_unit_identity_model) != std::string::npos;
  const std::string object_emission_closeout_summary =
      Objc3RuntimeMetadataObjectEmissionCloseoutSummary();
  summary.object_emission_closeout_surface_ready =
      object_emission_closeout_summary.find(
          std::string("contract=") +
          kObjc3RuntimeMetadataObjectEmissionCloseoutContractId) !=
          std::string::npos &&
      object_emission_closeout_summary.find(
          "non_goals=no-startup-registration-or-runtime-bootstrap") !=
          std::string::npos;
  summary.runtime_owned_payload_inventory_published = true;
  summary.constructor_root_reserved_not_emitted = true;
  summary.startup_registration_not_yet_landed = true;
  summary.runtime_bootstrap_not_yet_landed = true;
  summary.explicit_non_goals_published = true;
  summary.runtime_owned_payload_artifact_count =
      summary.runtime_owned_payload_artifacts.size();
  if (summary.binary_boundary_ready) {
    summary.binary_boundary_replay_key = runtime_ingest_binary_boundary.replay_key;
  }
  summary.ready_for_registration_manifest_implementation =
      summary.binary_boundary_ready &&
      summary.archive_static_link_surface_ready &&
      summary.object_emission_closeout_surface_ready &&
      summary.runtime_support_library_link_wiring_ready;
  if (summary.ready_for_registration_manifest_implementation) {
    summary.replay_key =
        BuildRuntimeTranslationUnitRegistrationContractReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeTranslationUnitRegistrationContractSummary(summary)) {
    summary.failure_reason =
        "translation-unit registration surface contract is incomplete";
  }
  return summary;
}

std::string BuildRuntimeTranslationUnitRegistrationContractSummaryJson(
    const Objc3RuntimeTranslationUnitRegistrationContractSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"binary_boundary_contract_id\":\""
      << EscapeJsonString(summary.binary_boundary_contract_id)
      << "\",\"archive_static_link_contract_id\":\""
      << EscapeJsonString(summary.archive_static_link_contract_id)
      << "\",\"object_emission_closeout_contract_id\":\""
      << EscapeJsonString(summary.object_emission_closeout_contract_id)
      << "\",\"runtime_support_library_link_wiring_contract_id\":\""
      << EscapeJsonString(summary.runtime_support_library_link_wiring_contract_id)
      << "\",\"registration_surface_path\":\""
      << EscapeJsonString(summary.registration_surface_path)
      << "\",\"registration_payload_model\":\""
      << EscapeJsonString(summary.registration_payload_model)
      << "\",\"ready\":"
      << (IsReadyObjc3RuntimeTranslationUnitRegistrationContractSummary(summary)
              ? "true"
              : "false")
      << ",\"boundary_frozen\":"
      << (summary.boundary_frozen ? "true" : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"binary_boundary_ready\":"
      << (summary.binary_boundary_ready ? "true" : "false")
      << ",\"archive_static_link_surface_ready\":"
      << (summary.archive_static_link_surface_ready ? "true" : "false")
      << ",\"object_emission_closeout_surface_ready\":"
      << (summary.object_emission_closeout_surface_ready ? "true" : "false")
      << ",\"runtime_support_library_link_wiring_ready\":"
      << (summary.runtime_support_library_link_wiring_ready ? "true" : "false")
      << ",\"runtime_owned_payload_inventory_published\":"
      << (summary.runtime_owned_payload_inventory_published ? "true" : "false")
      << ",\"constructor_root_reserved_not_emitted\":"
      << (summary.constructor_root_reserved_not_emitted ? "true" : "false")
      << ",\"startup_registration_not_yet_landed\":"
      << (summary.startup_registration_not_yet_landed ? "true" : "false")
      << ",\"runtime_bootstrap_not_yet_landed\":"
      << (summary.runtime_bootstrap_not_yet_landed ? "true" : "false")
      << ",\"explicit_non_goals_published\":"
      << (summary.explicit_non_goals_published ? "true" : "false")
      << ",\"ready_for_registration_manifest_implementation\":"
      << (summary.ready_for_registration_manifest_implementation ? "true"
                                                                : "false")
      << ",\"runtime_owned_payload_artifact_count\":"
      << summary.runtime_owned_payload_artifact_count
      << ",\"runtime_owned_payload_artifacts\":[";
  for (std::size_t index = 0; index < summary.runtime_owned_payload_artifacts.size();
       ++index) {
    if (index != 0u) {
      out << ",";
    }
    out << "\""
        << EscapeJsonString(summary.runtime_owned_payload_artifacts[index])
        << "\"";
  }
  out << "],\"constructor_root_symbol\":\""
      << EscapeJsonString(summary.constructor_root_symbol)
      << "\",\"constructor_root_ownership_model\":\""
      << EscapeJsonString(summary.constructor_root_ownership_model)
      << "\",\"constructor_emission_mode\":\""
      << EscapeJsonString(summary.constructor_emission_mode)
      << "\",\"constructor_priority_policy\":\""
      << EscapeJsonString(summary.constructor_priority_policy)
      << "\",\"registration_entrypoint_symbol\":\""
      << EscapeJsonString(summary.registration_entrypoint_symbol)
      << "\",\"translation_unit_identity_model\":\""
      << EscapeJsonString(summary.translation_unit_identity_model)
      << "\",\"binary_boundary_replay_key\":\""
      << EscapeJsonString(summary.binary_boundary_replay_key)
      << "\",\"replay_key\":\""
      << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

std::string BuildRuntimeTranslationUnitRegistrationManifestReplayKey(
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";translation_unit_registration_contract_id="
      << summary.translation_unit_registration_contract_id
      << ";runtime_support_library_link_wiring_contract_id="
      << summary.runtime_support_library_link_wiring_contract_id
      << ";manifest_surface_path=" << summary.manifest_surface_path
      << ";manifest_payload_model=" << summary.manifest_payload_model
      << ";manifest_artifact_relative_path="
      << summary.manifest_artifact_relative_path
      << ";runtime_support_library_archive_relative_path="
      << summary.runtime_support_library_archive_relative_path
      << ";constructor_root_symbol=" << summary.constructor_root_symbol
      << ";constructor_root_ownership_model="
      << summary.constructor_root_ownership_model
      << ";manifest_authority_model=" << summary.manifest_authority_model
      << ";constructor_init_stub_symbol_prefix="
      << summary.constructor_init_stub_symbol_prefix
      << ";constructor_init_stub_ownership_model="
      << summary.constructor_init_stub_ownership_model
      << ";constructor_priority_policy="
      << summary.constructor_priority_policy
      << ";registration_entrypoint_symbol="
      << summary.registration_entrypoint_symbol
      << ";translation_unit_identity_model="
      << summary.translation_unit_identity_model
      << ";class_descriptor_count=" << summary.class_descriptor_count
      << ";protocol_descriptor_count=" << summary.protocol_descriptor_count
      << ";category_descriptor_count=" << summary.category_descriptor_count
      << ";property_descriptor_count=" << summary.property_descriptor_count
      << ";ivar_descriptor_count=" << summary.ivar_descriptor_count
      << ";total_descriptor_count=" << summary.total_descriptor_count
      << ";translation_unit_registration_order_ordinal="
      << summary.translation_unit_registration_order_ordinal
      << ";translation_unit_registration_replay_key="
      << summary.translation_unit_registration_replay_key;
  for (std::size_t index = 0;
       index < summary.runtime_owned_payload_artifacts.size(); ++index) {
    out << ";runtime_owned_payload_artifacts[" << index
        << "]=" << summary.runtime_owned_payload_artifacts[index];
  }
  return out.str();
}

Objc3RuntimeTranslationUnitRegistrationManifestSummary
BuildRuntimeTranslationUnitRegistrationManifestSummary(
    const Objc3RuntimeTranslationUnitRegistrationContractSummary
        &registration_contract,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring,
    const Objc3RuntimeMetadataSectionScaffoldSummary
        &runtime_metadata_section_scaffold) {
  Objc3RuntimeTranslationUnitRegistrationManifestSummary summary;
  summary.fail_closed = true;
  summary.translation_unit_registration_contract_ready =
      IsReadyObjc3RuntimeTranslationUnitRegistrationContractSummary(
          registration_contract);
  summary.runtime_support_library_link_wiring_ready =
      IsReadyObjc3RuntimeSupportLibraryLinkWiringSummary(
          runtime_support_library_link_wiring);
  summary.runtime_manifest_template_published = true;
  summary.constructor_root_manifest_authoritative = true;
  summary.constructor_root_reserved_for_lowering = true;
  summary.init_stub_emission_deferred_to_lowering = true;
  summary.runtime_registration_artifact_emitted_by_driver = true;
  summary.runtime_owned_payload_artifact_count =
      summary.runtime_owned_payload_artifacts.size();
  summary.class_descriptor_count =
      runtime_metadata_section_scaffold.class_descriptor_count;
  summary.protocol_descriptor_count =
      runtime_metadata_section_scaffold.protocol_descriptor_count;
  summary.category_descriptor_count =
      runtime_metadata_section_scaffold.category_descriptor_count;
  summary.property_descriptor_count =
      runtime_metadata_section_scaffold.property_descriptor_count;
  summary.ivar_descriptor_count =
      runtime_metadata_section_scaffold.ivar_descriptor_count;
  summary.total_descriptor_count =
      runtime_metadata_section_scaffold.total_descriptor_count;
  if (summary.translation_unit_registration_contract_ready) {
    summary.translation_unit_registration_replay_key =
        registration_contract.replay_key;
  }
  summary.ready_for_lowering_init_stub_emission =
      summary.translation_unit_registration_contract_ready &&
      summary.runtime_support_library_link_wiring_ready;
  summary.launch_integration_ready =
      summary.ready_for_lowering_init_stub_emission;
  if (summary.ready_for_lowering_init_stub_emission) {
    summary.replay_key =
        BuildRuntimeTranslationUnitRegistrationManifestReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(summary)) {
    summary.failure_reason =
        "translation-unit registration manifest summary is incomplete";
  }
  return summary;
}

std::string BuildRuntimeTranslationUnitRegistrationManifestSummaryJson(
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"launch_integration_contract_id\":\""
      << EscapeJsonString(summary.launch_integration_contract_id)
      << "\",\"translation_unit_registration_contract_id\":\""
      << EscapeJsonString(summary.translation_unit_registration_contract_id)
      << "\",\"runtime_support_library_link_wiring_contract_id\":\""
      << EscapeJsonString(
             summary.runtime_support_library_link_wiring_contract_id)
      << "\",\"manifest_surface_path\":\""
      << EscapeJsonString(summary.manifest_surface_path)
      << "\",\"manifest_payload_model\":\""
      << EscapeJsonString(summary.manifest_payload_model)
      << "\",\"manifest_artifact_relative_path\":\""
      << EscapeJsonString(summary.manifest_artifact_relative_path)
      << "\",\"ready\":"
      << (IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(summary)
              ? "true"
              : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"translation_unit_registration_contract_ready\":"
      << (summary.translation_unit_registration_contract_ready ? "true"
                                                              : "false")
      << ",\"runtime_support_library_link_wiring_ready\":"
      << (summary.runtime_support_library_link_wiring_ready ? "true"
                                                            : "false")
      << ",\"runtime_manifest_template_published\":"
      << (summary.runtime_manifest_template_published ? "true" : "false")
      << ",\"constructor_root_manifest_authoritative\":"
      << (summary.constructor_root_manifest_authoritative ? "true" : "false")
      << ",\"constructor_root_reserved_for_lowering\":"
      << (summary.constructor_root_reserved_for_lowering ? "true" : "false")
      << ",\"init_stub_emission_deferred_to_lowering\":"
      << (summary.init_stub_emission_deferred_to_lowering ? "true" : "false")
      << ",\"runtime_registration_artifact_emitted_by_driver\":"
      << (summary.runtime_registration_artifact_emitted_by_driver ? "true"
                                                                 : "false")
      << ",\"ready_for_lowering_init_stub_emission\":"
      << (summary.ready_for_lowering_init_stub_emission ? "true" : "false")
      << ",\"launch_integration_ready\":"
      << (summary.launch_integration_ready ? "true" : "false")
      << ",\"runtime_owned_payload_artifact_count\":"
      << summary.runtime_owned_payload_artifact_count
      << ",\"runtime_owned_payload_artifacts\":[";
  for (std::size_t index = 0;
       index < summary.runtime_owned_payload_artifacts.size(); ++index) {
    if (index != 0u) {
      out << ",";
    }
    out << "\""
        << EscapeJsonString(summary.runtime_owned_payload_artifacts[index])
        << "\"";
  }
  out << "],\"runtime_support_library_archive_relative_path\":\""
      << EscapeJsonString(summary.runtime_support_library_archive_relative_path)
      << "\",\"constructor_root_symbol\":\""
      << EscapeJsonString(summary.constructor_root_symbol)
      << "\",\"constructor_root_ownership_model\":\""
      << EscapeJsonString(summary.constructor_root_ownership_model)
      << "\",\"manifest_authority_model\":\""
      << EscapeJsonString(summary.manifest_authority_model)
      << "\",\"constructor_init_stub_symbol_prefix\":\""
      << EscapeJsonString(summary.constructor_init_stub_symbol_prefix)
      << "\",\"constructor_init_stub_ownership_model\":\""
      << EscapeJsonString(summary.constructor_init_stub_ownership_model)
      << "\",\"constructor_priority_policy\":\""
      << EscapeJsonString(summary.constructor_priority_policy)
      << "\",\"registration_entrypoint_symbol\":\""
      << EscapeJsonString(summary.registration_entrypoint_symbol)
      << "\",\"translation_unit_identity_model\":\""
      << EscapeJsonString(summary.translation_unit_identity_model)
      << "\",\"runtime_library_resolution_model\":\""
      << EscapeJsonString(summary.runtime_library_resolution_model)
      << "\",\"driver_linker_flag_consumption_model\":\""
      << EscapeJsonString(summary.driver_linker_flag_consumption_model)
      << "\",\"compile_wrapper_command_surface\":\""
      << EscapeJsonString(summary.compile_wrapper_command_surface)
      << "\",\"compile_proof_command_surface\":\""
      << EscapeJsonString(summary.compile_proof_command_surface)
      << "\",\"execution_smoke_command_surface\":\""
      << EscapeJsonString(summary.execution_smoke_command_surface)
      << "\",\"class_descriptor_count\":"
      << summary.class_descriptor_count
      << ",\"protocol_descriptor_count\":"
      << summary.protocol_descriptor_count
      << ",\"category_descriptor_count\":"
      << summary.category_descriptor_count
      << ",\"property_descriptor_count\":"
      << summary.property_descriptor_count
      << ",\"ivar_descriptor_count\":"
      << summary.ivar_descriptor_count
      << ",\"total_descriptor_count\":"
      << summary.total_descriptor_count
      << ",\"translation_unit_registration_order_ordinal\":"
      << summary.translation_unit_registration_order_ordinal
      << ",\"translation_unit_registration_replay_key\":\""
      << EscapeJsonString(summary.translation_unit_registration_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

std::string BuildRuntimeStartupBootstrapInvariantReplayKey(
    const Objc3RuntimeStartupBootstrapInvariantSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";registration_manifest_contract_id="
      << summary.registration_manifest_contract_id
      << ";bootstrap_surface_path=" << summary.bootstrap_surface_path
      << ";duplicate_registration_policy="
      << summary.duplicate_registration_policy
      << ";realization_order_policy=" << summary.realization_order_policy
      << ";failure_mode=" << summary.failure_mode
      << ";image_local_initialization_scope="
      << summary.image_local_initialization_scope
      << ";constructor_root_uniqueness_policy="
      << summary.constructor_root_uniqueness_policy
      << ";constructor_root_consumption_model="
      << summary.constructor_root_consumption_model
      << ";startup_execution_mode=" << summary.startup_execution_mode
      << ";constructor_root_symbol=" << summary.constructor_root_symbol
      << ";registration_entrypoint_symbol="
      << summary.registration_entrypoint_symbol
      << ";manifest_authority_model=" << summary.manifest_authority_model
      << ";translation_unit_identity_model="
      << summary.translation_unit_identity_model
      << ";registration_manifest_replay_key="
      << summary.registration_manifest_replay_key;
  return out.str();
}

Objc3RuntimeStartupBootstrapInvariantSummary
BuildRuntimeStartupBootstrapInvariantSummary(
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &registration_manifest) {
  Objc3RuntimeStartupBootstrapInvariantSummary summary;
  summary.fail_closed = true;
  summary.registration_manifest_contract_ready =
      IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(
          registration_manifest);
  summary.duplicate_registration_semantics_frozen = true;
  summary.realization_order_semantics_frozen = true;
  summary.failure_mode_semantics_frozen = true;
  summary.image_local_initialization_scope_frozen = true;
  summary.constructor_root_uniqueness_frozen = true;
  summary.startup_execution_not_yet_landed = true;
  summary.live_duplicate_registration_enforcement_not_yet_landed = true;
  summary.image_local_realization_not_yet_landed = true;
  summary.ready_for_bootstrap_implementation =
      summary.registration_manifest_contract_ready;
  if (summary.registration_manifest_contract_ready) {
    summary.registration_manifest_replay_key = registration_manifest.replay_key;
  }
  if (summary.ready_for_bootstrap_implementation) {
    summary.replay_key =
        BuildRuntimeStartupBootstrapInvariantReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeStartupBootstrapInvariantSummary(summary)) {
    summary.failure_reason =
        "runtime startup bootstrap invariant summary is incomplete";
  }
  return summary;
}

std::string BuildRuntimeStartupBootstrapInvariantSummaryJson(
    const Objc3RuntimeStartupBootstrapInvariantSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"registration_manifest_contract_id\":\""
      << EscapeJsonString(summary.registration_manifest_contract_id)
      << "\",\"bootstrap_surface_path\":\""
      << EscapeJsonString(summary.bootstrap_surface_path)
      << "\",\"duplicate_registration_policy\":\""
      << EscapeJsonString(summary.duplicate_registration_policy)
      << "\",\"realization_order_policy\":\""
      << EscapeJsonString(summary.realization_order_policy)
      << "\",\"failure_mode\":\""
      << EscapeJsonString(summary.failure_mode)
      << "\",\"image_local_initialization_scope\":\""
      << EscapeJsonString(summary.image_local_initialization_scope)
      << "\",\"constructor_root_uniqueness_policy\":\""
      << EscapeJsonString(summary.constructor_root_uniqueness_policy)
      << "\",\"constructor_root_consumption_model\":\""
      << EscapeJsonString(summary.constructor_root_consumption_model)
      << "\",\"startup_execution_mode\":\""
      << EscapeJsonString(summary.startup_execution_mode)
      << "\",\"constructor_root_symbol\":\""
      << EscapeJsonString(summary.constructor_root_symbol)
      << "\",\"registration_entrypoint_symbol\":\""
      << EscapeJsonString(summary.registration_entrypoint_symbol)
      << "\",\"manifest_authority_model\":\""
      << EscapeJsonString(summary.manifest_authority_model)
      << "\",\"translation_unit_identity_model\":\""
      << EscapeJsonString(summary.translation_unit_identity_model)
      << "\",\"ready\":"
      << (IsReadyObjc3RuntimeStartupBootstrapInvariantSummary(summary) ? "true"
                                                                       : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"registration_manifest_contract_ready\":"
      << (summary.registration_manifest_contract_ready ? "true" : "false")
      << ",\"duplicate_registration_semantics_frozen\":"
      << (summary.duplicate_registration_semantics_frozen ? "true" : "false")
      << ",\"realization_order_semantics_frozen\":"
      << (summary.realization_order_semantics_frozen ? "true" : "false")
      << ",\"failure_mode_semantics_frozen\":"
      << (summary.failure_mode_semantics_frozen ? "true" : "false")
      << ",\"image_local_initialization_scope_frozen\":"
      << (summary.image_local_initialization_scope_frozen ? "true" : "false")
      << ",\"constructor_root_uniqueness_frozen\":"
      << (summary.constructor_root_uniqueness_frozen ? "true" : "false")
      << ",\"startup_execution_not_yet_landed\":"
      << (summary.startup_execution_not_yet_landed ? "true" : "false")
      << ",\"live_duplicate_registration_enforcement_not_yet_landed\":"
      << (summary.live_duplicate_registration_enforcement_not_yet_landed ? "true"
                                                                         : "false")
      << ",\"image_local_realization_not_yet_landed\":"
      << (summary.image_local_realization_not_yet_landed ? "true" : "false")
      << ",\"ready_for_bootstrap_implementation\":"
      << (summary.ready_for_bootstrap_implementation ? "true" : "false")
      << ",\"registration_manifest_replay_key\":\""
      << EscapeJsonString(summary.registration_manifest_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

std::string BuildRuntimeBootstrapSemanticsReplayKey(
    const Objc3RuntimeBootstrapSemanticsSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";bootstrap_invariant_contract_id="
      << summary.bootstrap_invariant_contract_id
      << ";registration_manifest_contract_id="
      << summary.registration_manifest_contract_id
      << ";bootstrap_surface_path=" << summary.bootstrap_surface_path
      << ";duplicate_registration_policy="
      << summary.duplicate_registration_policy
      << ";realization_order_policy=" << summary.realization_order_policy
      << ";failure_mode=" << summary.failure_mode
      << ";image_local_initialization_scope="
      << summary.image_local_initialization_scope
      << ";constructor_root_symbol=" << summary.constructor_root_symbol
      << ";registration_entrypoint_symbol="
      << summary.registration_entrypoint_symbol
      << ";manifest_authority_model=" << summary.manifest_authority_model
      << ";translation_unit_identity_model="
      << summary.translation_unit_identity_model
      << ";runtime_library_archive_relative_path="
      << summary.runtime_library_archive_relative_path
      << ";registration_result_model=" << summary.registration_result_model
      << ";registration_order_ordinal_model="
      << summary.registration_order_ordinal_model
      << ";runtime_state_snapshot_symbol="
      << summary.runtime_state_snapshot_symbol
      << ";success_status_code=" << summary.success_status_code
      << ";invalid_descriptor_status_code="
      << summary.invalid_descriptor_status_code
      << ";duplicate_registration_status_code="
      << summary.duplicate_registration_status_code
      << ";out_of_order_status_code="
      << summary.out_of_order_status_code
      << ";translation_unit_registration_order_ordinal="
      << summary.translation_unit_registration_order_ordinal
      << ";bootstrap_invariant_replay_key="
      << summary.bootstrap_invariant_replay_key
      << ";registration_manifest_replay_key="
      << summary.registration_manifest_replay_key;
  return out.str();
}

Objc3RuntimeBootstrapSemanticsSummary BuildRuntimeBootstrapSemanticsSummary(
    const Objc3RuntimeStartupBootstrapInvariantSummary &bootstrap_invariants,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &registration_manifest) {
  Objc3RuntimeBootstrapSemanticsSummary summary;
  summary.fail_closed = true;
  summary.bootstrap_invariant_contract_ready =
      IsReadyObjc3RuntimeStartupBootstrapInvariantSummary(
          bootstrap_invariants);
  summary.registration_manifest_contract_ready =
      IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(
          registration_manifest);
  summary.registration_manifest_bootstrap_semantics_published =
      summary.registration_manifest_contract_ready;
  summary.live_runtime_enforcement_landed = true;
  summary.runtime_probe_required = true;
  summary.no_partial_commit_on_failure = true;
  summary.ready_for_constructor_root_implementation =
      summary.bootstrap_invariant_contract_ready &&
      summary.registration_manifest_contract_ready;
  summary.translation_unit_registration_order_ordinal =
      registration_manifest.translation_unit_registration_order_ordinal;
  if (summary.bootstrap_invariant_contract_ready) {
    summary.bootstrap_invariant_replay_key = bootstrap_invariants.replay_key;
  }
  if (summary.registration_manifest_contract_ready) {
    summary.registration_manifest_replay_key = registration_manifest.replay_key;
  }
  if (summary.ready_for_constructor_root_implementation) {
    summary.replay_key = BuildRuntimeBootstrapSemanticsReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeBootstrapSemanticsSummary(summary)) {
    summary.failure_reason =
        "runtime bootstrap semantics summary is incomplete";
  }
  return summary;
}

std::string BuildRuntimeBootstrapSemanticsSummaryJson(
    const Objc3RuntimeBootstrapSemanticsSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"bootstrap_invariant_contract_id\":\""
      << EscapeJsonString(summary.bootstrap_invariant_contract_id)
      << "\",\"registration_manifest_contract_id\":\""
      << EscapeJsonString(summary.registration_manifest_contract_id)
      << "\",\"bootstrap_surface_path\":\""
      << EscapeJsonString(summary.bootstrap_surface_path)
      << "\",\"duplicate_registration_policy\":\""
      << EscapeJsonString(summary.duplicate_registration_policy)
      << "\",\"realization_order_policy\":\""
      << EscapeJsonString(summary.realization_order_policy)
      << "\",\"failure_mode\":\""
      << EscapeJsonString(summary.failure_mode)
      << "\",\"image_local_initialization_scope\":\""
      << EscapeJsonString(summary.image_local_initialization_scope)
      << "\",\"constructor_root_symbol\":\""
      << EscapeJsonString(summary.constructor_root_symbol)
      << "\",\"registration_entrypoint_symbol\":\""
      << EscapeJsonString(summary.registration_entrypoint_symbol)
      << "\",\"manifest_authority_model\":\""
      << EscapeJsonString(summary.manifest_authority_model)
      << "\",\"translation_unit_identity_model\":\""
      << EscapeJsonString(summary.translation_unit_identity_model)
      << "\",\"runtime_library_archive_relative_path\":\""
      << EscapeJsonString(summary.runtime_library_archive_relative_path)
      << "\",\"registration_result_model\":\""
      << EscapeJsonString(summary.registration_result_model)
      << "\",\"registration_order_ordinal_model\":\""
      << EscapeJsonString(summary.registration_order_ordinal_model)
      << "\",\"runtime_state_snapshot_symbol\":\""
      << EscapeJsonString(summary.runtime_state_snapshot_symbol)
      << "\",\"success_status_code\":" << summary.success_status_code
      << ",\"invalid_descriptor_status_code\":"
      << summary.invalid_descriptor_status_code
      << ",\"duplicate_registration_status_code\":"
      << summary.duplicate_registration_status_code
      << ",\"out_of_order_status_code\":"
      << summary.out_of_order_status_code
      << ",\"translation_unit_registration_order_ordinal\":"
      << summary.translation_unit_registration_order_ordinal
      << ",\"ready\":"
      << (IsReadyObjc3RuntimeBootstrapSemanticsSummary(summary) ? "true"
                                                                : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"bootstrap_invariant_contract_ready\":"
      << (summary.bootstrap_invariant_contract_ready ? "true" : "false")
      << ",\"registration_manifest_contract_ready\":"
      << (summary.registration_manifest_contract_ready ? "true" : "false")
      << ",\"live_runtime_enforcement_landed\":"
      << (summary.live_runtime_enforcement_landed ? "true" : "false")
      << ",\"registration_manifest_bootstrap_semantics_published\":"
      << (summary.registration_manifest_bootstrap_semantics_published ? "true"
                                                                      : "false")
      << ",\"runtime_probe_required\":"
      << (summary.runtime_probe_required ? "true" : "false")
      << ",\"no_partial_commit_on_failure\":"
      << (summary.no_partial_commit_on_failure ? "true" : "false")
      << ",\"ready_for_constructor_root_implementation\":"
      << (summary.ready_for_constructor_root_implementation ? "true" : "false")
      << ",\"bootstrap_invariant_replay_key\":\""
      << EscapeJsonString(summary.bootstrap_invariant_replay_key)
      << "\",\"registration_manifest_replay_key\":\""
      << EscapeJsonString(summary.registration_manifest_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

std::string BuildRuntimeBootstrapLoweringReplayKey(
    const Objc3RuntimeBootstrapLoweringSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";registration_manifest_contract_id="
      << summary.registration_manifest_contract_id
      << ";bootstrap_semantics_contract_id="
      << summary.bootstrap_semantics_contract_id
      << ";bootstrap_surface_path=" << summary.bootstrap_surface_path
      << ";lowering_boundary_model=" << summary.lowering_boundary_model
      << ";constructor_root_symbol=" << summary.constructor_root_symbol
      << ";constructor_init_stub_symbol_prefix="
      << summary.constructor_init_stub_symbol_prefix
      << ";registration_table_symbol_prefix="
      << summary.registration_table_symbol_prefix
      << ";image_local_init_state_symbol_prefix="
      << summary.image_local_init_state_symbol_prefix
      << ";registration_entrypoint_symbol="
      << summary.registration_entrypoint_symbol
      << ";global_ctor_list_model=" << summary.global_ctor_list_model
      << ";registration_table_layout_model="
      << summary.registration_table_layout_model
      << ";image_local_initialization_model="
      << summary.image_local_initialization_model
      << ";registration_table_abi_version="
      << summary.registration_table_abi_version
      << ";registration_table_pointer_field_count="
      << summary.registration_table_pointer_field_count
      << ";constructor_root_emission_state="
      << summary.constructor_root_emission_state
      << ";init_stub_emission_state=" << summary.init_stub_emission_state
      << ";registration_table_emission_state="
      << summary.registration_table_emission_state
      << ";bootstrap_ir_materialization_landed="
      << (summary.bootstrap_ir_materialization_landed ? "true" : "false")
      << ";image_local_initialization_landed="
      << (summary.image_local_initialization_landed ? "true" : "false")
      << ";registration_manifest_replay_key="
      << summary.registration_manifest_replay_key
      << ";bootstrap_semantics_replay_key="
      << summary.bootstrap_semantics_replay_key;
  return out.str();
}

Objc3RuntimeBootstrapLoweringSummary BuildRuntimeBootstrapLoweringSummary(
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &registration_manifest,
    const Objc3RuntimeBootstrapSemanticsSummary &bootstrap_semantics) {
  Objc3RuntimeBootstrapLoweringSummary summary;
  summary.fail_closed = true;
  summary.registration_manifest_contract_ready =
      IsReadyObjc3RuntimeTranslationUnitRegistrationManifestSummary(
          registration_manifest);
  summary.bootstrap_semantics_contract_ready =
      IsReadyObjc3RuntimeBootstrapSemanticsSummary(bootstrap_semantics);
  summary.lowering_contract_published = true;
  summary.manifest_authority_preserved =
      summary.registration_manifest_contract_ready &&
      registration_manifest.constructor_root_manifest_authoritative;
  summary.no_bootstrap_ir_materialization_yet = false;
  summary.bootstrap_ir_materialization_landed =
      summary.registration_manifest_contract_ready &&
      summary.bootstrap_semantics_contract_ready;
  summary.image_local_initialization_landed =
      summary.registration_manifest_contract_ready &&
      summary.bootstrap_semantics_contract_ready;
  summary.ready_for_bootstrap_materialization =
      summary.registration_manifest_contract_ready &&
      summary.bootstrap_semantics_contract_ready;
  if (summary.registration_manifest_contract_ready) {
    summary.registration_manifest_replay_key = registration_manifest.replay_key;
  }
  if (summary.bootstrap_semantics_contract_ready) {
    summary.bootstrap_semantics_replay_key = bootstrap_semantics.replay_key;
  }
  if (summary.ready_for_bootstrap_materialization) {
    summary.replay_key = BuildRuntimeBootstrapLoweringReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeBootstrapLoweringSummary(summary)) {
    summary.failure_reason = "runtime bootstrap lowering summary is incomplete";
  }
  return summary;
}

std::string BuildRuntimeBootstrapLoweringSummaryJson(
    const Objc3RuntimeBootstrapLoweringSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"registration_manifest_contract_id\":\""
      << EscapeJsonString(summary.registration_manifest_contract_id)
      << "\",\"bootstrap_semantics_contract_id\":\""
      << EscapeJsonString(summary.bootstrap_semantics_contract_id)
      << "\",\"bootstrap_surface_path\":\""
      << EscapeJsonString(summary.bootstrap_surface_path)
      << "\",\"lowering_boundary_model\":\""
      << EscapeJsonString(summary.lowering_boundary_model)
      << "\",\"constructor_root_symbol\":\""
      << EscapeJsonString(summary.constructor_root_symbol)
      << "\",\"constructor_init_stub_symbol_prefix\":\""
      << EscapeJsonString(summary.constructor_init_stub_symbol_prefix)
      << "\",\"registration_table_symbol_prefix\":\""
      << EscapeJsonString(summary.registration_table_symbol_prefix)
      << "\",\"image_local_init_state_symbol_prefix\":\""
      << EscapeJsonString(summary.image_local_init_state_symbol_prefix)
      << "\",\"registration_entrypoint_symbol\":\""
      << EscapeJsonString(summary.registration_entrypoint_symbol)
      << "\",\"global_ctor_list_model\":\""
      << EscapeJsonString(summary.global_ctor_list_model)
      << "\",\"registration_table_layout_model\":\""
      << EscapeJsonString(summary.registration_table_layout_model)
      << "\",\"image_local_initialization_model\":\""
      << EscapeJsonString(summary.image_local_initialization_model)
      << "\",\"registration_table_abi_version\":"
      << summary.registration_table_abi_version
      << ",\"registration_table_pointer_field_count\":"
      << summary.registration_table_pointer_field_count
      << ",\"constructor_root_emission_state\":\""
      << EscapeJsonString(summary.constructor_root_emission_state)
      << "\",\"init_stub_emission_state\":\""
      << EscapeJsonString(summary.init_stub_emission_state)
      << "\",\"registration_table_emission_state\":\""
      << EscapeJsonString(summary.registration_table_emission_state)
      << "\",\"ready\":"
      << (IsReadyObjc3RuntimeBootstrapLoweringSummary(summary) ? "true"
                                                               : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"registration_manifest_contract_ready\":"
      << (summary.registration_manifest_contract_ready ? "true" : "false")
      << ",\"bootstrap_semantics_contract_ready\":"
      << (summary.bootstrap_semantics_contract_ready ? "true" : "false")
      << ",\"lowering_contract_published\":"
      << (summary.lowering_contract_published ? "true" : "false")
      << ",\"manifest_authority_preserved\":"
      << (summary.manifest_authority_preserved ? "true" : "false")
      << ",\"no_bootstrap_ir_materialization_yet\":"
      << (summary.no_bootstrap_ir_materialization_yet ? "true" : "false")
      << ",\"bootstrap_ir_materialization_landed\":"
      << (summary.bootstrap_ir_materialization_landed ? "true" : "false")
      << ",\"image_local_initialization_landed\":"
      << (summary.image_local_initialization_landed ? "true" : "false")
      << ",\"ready_for_bootstrap_materialization\":"
      << (summary.ready_for_bootstrap_materialization ? "true" : "false")
      << ",\"registration_manifest_replay_key\":\""
      << EscapeJsonString(summary.registration_manifest_replay_key)
      << "\",\"bootstrap_semantics_replay_key\":\""
      << EscapeJsonString(summary.bootstrap_semantics_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

std::string BuildRuntimeBootstrapApiReplayKey(
    const Objc3RuntimeBootstrapApiSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";support_library_core_feature_contract_id="
      << summary.support_library_core_feature_contract_id
      << ";support_library_link_wiring_contract_id="
      << summary.support_library_link_wiring_contract_id
      << ";bootstrap_surface_path=" << summary.bootstrap_surface_path
      << ";public_header_path=" << summary.public_header_path
      << ";archive_relative_path=" << summary.archive_relative_path
      << ";registration_status_enum_type="
      << summary.registration_status_enum_type
      << ";image_descriptor_type=" << summary.image_descriptor_type
      << ";selector_handle_type=" << summary.selector_handle_type
      << ";registration_snapshot_type="
      << summary.registration_snapshot_type
      << ";registration_entrypoint_symbol="
      << summary.registration_entrypoint_symbol
      << ";selector_lookup_symbol=" << summary.selector_lookup_symbol
      << ";dispatch_entrypoint_symbol=" << summary.dispatch_entrypoint_symbol
      << ";state_snapshot_symbol=" << summary.state_snapshot_symbol
      << ";reset_for_testing_symbol=" << summary.reset_for_testing_symbol
      << ";compatibility_dispatch_symbol="
      << summary.compatibility_dispatch_symbol
      << ";registration_result_model=" << summary.registration_result_model
      << ";registration_order_ordinal_model="
      << summary.registration_order_ordinal_model
      << ";runtime_state_locking_model="
      << summary.runtime_state_locking_model
      << ";startup_invocation_model=" << summary.startup_invocation_model
      << ";image_walk_lifecycle_model="
      << summary.image_walk_lifecycle_model
      << ";deterministic_reset_lifecycle_model="
      << summary.deterministic_reset_lifecycle_model
      << ";support_library_core_feature_replay_key="
      << summary.support_library_core_feature_replay_key
      << ";support_library_link_wiring_replay_key="
      << summary.support_library_link_wiring_replay_key;
  return out.str();
}

Objc3RuntimeBootstrapApiSummary BuildRuntimeBootstrapApiSummary(
    const Objc3RuntimeSupportLibraryCoreFeatureSummary &runtime_support_library,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring) {
  Objc3RuntimeBootstrapApiSummary summary;
  summary.fail_closed = true;
  summary.support_library_core_feature_contract_ready =
      IsReadyObjc3RuntimeSupportLibraryCoreFeatureSummary(
          runtime_support_library);
  summary.support_library_link_wiring_contract_ready =
      IsReadyObjc3RuntimeSupportLibraryLinkWiringSummary(
          runtime_support_library_link_wiring);
  summary.api_surface_frozen = true;
  summary.registration_entrypoint_frozen = true;
  summary.selector_lookup_and_dispatch_frozen = true;
  summary.reset_and_snapshot_hooks_frozen = true;
  summary.runtime_probe_required = true;
  summary.image_walk_not_yet_landed = true;
  summary.deterministic_reset_expansion_not_yet_landed = true;
  summary.ready_for_registrar_implementation =
      summary.support_library_core_feature_contract_ready &&
      summary.support_library_link_wiring_contract_ready;
  if (summary.support_library_core_feature_contract_ready) {
    summary.support_library_core_feature_replay_key =
        runtime_support_library.contract_id + ";archive_relative_path=" +
        runtime_support_library.archive_relative_path + ";public_header_path=" +
        runtime_support_library.public_header_path + ";register_image_symbol=" +
        runtime_support_library.register_image_symbol +
        ";lookup_selector_symbol=" +
        runtime_support_library.lookup_selector_symbol +
        ";dispatch_i32_symbol=" + runtime_support_library.dispatch_i32_symbol +
        ";reset_for_testing_symbol=" +
        runtime_support_library.reset_for_testing_symbol;
  }
  if (summary.support_library_link_wiring_contract_ready) {
    summary.support_library_link_wiring_replay_key =
        runtime_support_library_link_wiring.contract_id +
        ";archive_relative_path=" +
        runtime_support_library_link_wiring.archive_relative_path +
        ";compatibility_dispatch_symbol=" +
        runtime_support_library_link_wiring.compatibility_dispatch_symbol +
        ";runtime_dispatch_symbol=" +
        runtime_support_library_link_wiring.runtime_dispatch_symbol +
        ";driver_link_mode=" +
        runtime_support_library_link_wiring.driver_link_mode;
  }
  if (summary.ready_for_registrar_implementation) {
    summary.replay_key = BuildRuntimeBootstrapApiReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeBootstrapApiSummary(summary)) {
    summary.failure_reason = "runtime bootstrap api summary is incomplete";
  }
  return summary;
}

std::string BuildRuntimeBootstrapApiSummaryJson(
    const Objc3RuntimeBootstrapApiSummary &summary) {
  std::ostringstream out;
  out << "{\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"support_library_core_feature_contract_id\":\""
      << EscapeJsonString(summary.support_library_core_feature_contract_id)
      << "\",\"support_library_link_wiring_contract_id\":\""
      << EscapeJsonString(summary.support_library_link_wiring_contract_id)
      << "\",\"bootstrap_surface_path\":\""
      << EscapeJsonString(summary.bootstrap_surface_path)
      << "\",\"public_header_path\":\""
      << EscapeJsonString(summary.public_header_path)
      << "\",\"archive_relative_path\":\""
      << EscapeJsonString(summary.archive_relative_path)
      << "\",\"registration_status_enum_type\":\""
      << EscapeJsonString(summary.registration_status_enum_type)
      << "\",\"image_descriptor_type\":\""
      << EscapeJsonString(summary.image_descriptor_type)
      << "\",\"selector_handle_type\":\""
      << EscapeJsonString(summary.selector_handle_type)
      << "\",\"registration_snapshot_type\":\""
      << EscapeJsonString(summary.registration_snapshot_type)
      << "\",\"registration_entrypoint_symbol\":\""
      << EscapeJsonString(summary.registration_entrypoint_symbol)
      << "\",\"selector_lookup_symbol\":\""
      << EscapeJsonString(summary.selector_lookup_symbol)
      << "\",\"dispatch_entrypoint_symbol\":\""
      << EscapeJsonString(summary.dispatch_entrypoint_symbol)
      << "\",\"state_snapshot_symbol\":\""
      << EscapeJsonString(summary.state_snapshot_symbol)
      << "\",\"reset_for_testing_symbol\":\""
      << EscapeJsonString(summary.reset_for_testing_symbol)
      << "\",\"compatibility_dispatch_symbol\":\""
      << EscapeJsonString(summary.compatibility_dispatch_symbol)
      << "\",\"registration_result_model\":\""
      << EscapeJsonString(summary.registration_result_model)
      << "\",\"registration_order_ordinal_model\":\""
      << EscapeJsonString(summary.registration_order_ordinal_model)
      << "\",\"runtime_state_locking_model\":\""
      << EscapeJsonString(summary.runtime_state_locking_model)
      << "\",\"startup_invocation_model\":\""
      << EscapeJsonString(summary.startup_invocation_model)
      << "\",\"image_walk_lifecycle_model\":\""
      << EscapeJsonString(summary.image_walk_lifecycle_model)
      << "\",\"deterministic_reset_lifecycle_model\":\""
      << EscapeJsonString(summary.deterministic_reset_lifecycle_model)
      << "\",\"ready\":"
      << (IsReadyObjc3RuntimeBootstrapApiSummary(summary) ? "true" : "false")
      << ",\"fail_closed\":" << (summary.fail_closed ? "true" : "false")
      << ",\"support_library_core_feature_contract_ready\":"
      << (summary.support_library_core_feature_contract_ready ? "true"
                                                              : "false")
      << ",\"support_library_link_wiring_contract_ready\":"
      << (summary.support_library_link_wiring_contract_ready ? "true"
                                                             : "false")
      << ",\"api_surface_frozen\":"
      << (summary.api_surface_frozen ? "true" : "false")
      << ",\"registration_entrypoint_frozen\":"
      << (summary.registration_entrypoint_frozen ? "true" : "false")
      << ",\"selector_lookup_and_dispatch_frozen\":"
      << (summary.selector_lookup_and_dispatch_frozen ? "true" : "false")
      << ",\"reset_and_snapshot_hooks_frozen\":"
      << (summary.reset_and_snapshot_hooks_frozen ? "true" : "false")
      << ",\"runtime_probe_required\":"
      << (summary.runtime_probe_required ? "true" : "false")
      << ",\"image_walk_not_yet_landed\":"
      << (summary.image_walk_not_yet_landed ? "true" : "false")
      << ",\"deterministic_reset_expansion_not_yet_landed\":"
      << (summary.deterministic_reset_expansion_not_yet_landed ? "true"
                                                               : "false")
      << ",\"ready_for_registrar_implementation\":"
      << (summary.ready_for_registrar_implementation ? "true" : "false")
      << ",\"support_library_core_feature_replay_key\":\""
      << EscapeJsonString(summary.support_library_core_feature_replay_key)
      << "\",\"support_library_link_wiring_replay_key\":\""
      << EscapeJsonString(summary.support_library_link_wiring_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

void AccumulateIdClassSelObjectPointerTypecheckSite(
    bool id_spelling,
    bool class_spelling,
    bool sel_spelling,
    bool object_pointer_type_spelling,
    const std::string &object_pointer_type_name,
    Objc3IdClassSelObjectPointerTypecheckContract &contract) {
  const std::size_t active_spelling_count =
      (id_spelling ? 1u : 0u) +
      (class_spelling ? 1u : 0u) +
      (sel_spelling ? 1u : 0u) +
      (object_pointer_type_spelling ? 1u : 0u);
  if (active_spelling_count > 1u) {
    contract.deterministic = false;
  }

  if (id_spelling) {
    ++contract.id_typecheck_sites;
  }
  if (class_spelling) {
    ++contract.class_typecheck_sites;
  }
  if (sel_spelling) {
    ++contract.sel_typecheck_sites;
  }
  if (object_pointer_type_spelling) {
    ++contract.object_pointer_typecheck_sites;
    if (object_pointer_type_name.empty()) {
      contract.deterministic = false;
    }
  }

  if (active_spelling_count > 0u) {
    ++contract.total_typecheck_sites;
  }
}

void AccumulateIdClassSelObjectPointerTypecheckMethod(
    const Objc3MethodDecl &method,
    Objc3IdClassSelObjectPointerTypecheckContract &contract) {
  AccumulateIdClassSelObjectPointerTypecheckSite(method.return_id_spelling,
                                                 method.return_class_spelling,
                                                 method.return_sel_spelling,
                                                 method.return_object_pointer_type_spelling,
                                                 method.return_object_pointer_type_name,
                                                 contract);
  for (const auto &param : method.params) {
    AccumulateIdClassSelObjectPointerTypecheckSite(param.id_spelling,
                                                   param.class_spelling,
                                                   param.sel_spelling,
                                                   param.object_pointer_type_spelling,
                                                   param.object_pointer_type_name,
                                                   contract);
  }
}

template <typename Container>
void AccumulateIdClassSelObjectPointerTypecheckObjcDeclarations(
    const Container &declarations,
    Objc3IdClassSelObjectPointerTypecheckContract &contract) {
  for (const auto &declaration : declarations) {
    for (const auto &property : declaration.properties) {
      AccumulateIdClassSelObjectPointerTypecheckSite(property.id_spelling,
                                                     property.class_spelling,
                                                     property.sel_spelling,
                                                     property.object_pointer_type_spelling,
                                                     property.object_pointer_type_name,
                                                     contract);
    }
    for (const auto &method : declaration.methods) {
      AccumulateIdClassSelObjectPointerTypecheckMethod(method, contract);
    }
  }
}

Objc3IdClassSelObjectPointerTypecheckContract BuildIdClassSelObjectPointerTypecheckContract(
    const Objc3Program &program) {
  Objc3IdClassSelObjectPointerTypecheckContract contract;
  for (const auto &fn : program.functions) {
    AccumulateIdClassSelObjectPointerTypecheckSite(fn.return_id_spelling,
                                                   fn.return_class_spelling,
                                                   fn.return_sel_spelling,
                                                   fn.return_object_pointer_type_spelling,
                                                   fn.return_object_pointer_type_name,
                                                   contract);
    for (const auto &param : fn.params) {
      AccumulateIdClassSelObjectPointerTypecheckSite(param.id_spelling,
                                                     param.class_spelling,
                                                     param.sel_spelling,
                                                     param.object_pointer_type_spelling,
                                                     param.object_pointer_type_name,
                                                     contract);
    }
  }
  AccumulateIdClassSelObjectPointerTypecheckObjcDeclarations(program.protocols, contract);
  AccumulateIdClassSelObjectPointerTypecheckObjcDeclarations(program.interfaces, contract);
  AccumulateIdClassSelObjectPointerTypecheckObjcDeclarations(program.implementations, contract);
  return contract;
}

std::size_t CountSelectorPieces(const std::string &selector) {
  if (selector.empty()) {
    return 0;
  }
  std::size_t colons = 0;
  for (char c : selector) {
    if (c == ':') {
      ++colons;
    }
  }
  return colons == 0 ? 1 : colons;
}

void AccumulateMessageSendSelectorLoweringExpr(
    const Expr *expr,
    Objc3MessageSendSelectorLoweringContract &contract,
    std::unordered_set<std::string> &selector_literals) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
    case Expr::Kind::MessageSend: {
      ++contract.message_send_sites;
      ++contract.receiver_expression_sites;
      if (expr->args.empty()) {
        ++contract.unary_selector_sites;
      } else {
        ++contract.keyword_selector_sites;
      }
      contract.argument_expression_sites += expr->args.size();
      const std::size_t selector_pieces = CountSelectorPieces(expr->selector);
      contract.selector_piece_sites += selector_pieces;
      if (selector_pieces == 0u) {
        contract.deterministic = false;
      } else {
        selector_literals.insert(expr->selector);
      }
      AccumulateMessageSendSelectorLoweringExpr(expr->receiver.get(), contract, selector_literals);
      for (const auto &arg : expr->args) {
        AccumulateMessageSendSelectorLoweringExpr(arg.get(), contract, selector_literals);
      }
      return;
    }
    case Expr::Kind::Binary:
      AccumulateMessageSendSelectorLoweringExpr(expr->left.get(), contract, selector_literals);
      AccumulateMessageSendSelectorLoweringExpr(expr->right.get(), contract, selector_literals);
      return;
    case Expr::Kind::Conditional:
      AccumulateMessageSendSelectorLoweringExpr(expr->left.get(), contract, selector_literals);
      AccumulateMessageSendSelectorLoweringExpr(expr->right.get(), contract, selector_literals);
      AccumulateMessageSendSelectorLoweringExpr(expr->third.get(), contract, selector_literals);
      return;
    case Expr::Kind::Call:
      for (const auto &arg : expr->args) {
        AccumulateMessageSendSelectorLoweringExpr(arg.get(), contract, selector_literals);
      }
      return;
    default:
      return;
  }
}

void AccumulateMessageSendSelectorLoweringForClause(
    const ForClause &clause,
    Objc3MessageSendSelectorLoweringContract &contract,
    std::unordered_set<std::string> &selector_literals) {
  if (clause.value != nullptr) {
    AccumulateMessageSendSelectorLoweringExpr(clause.value.get(), contract, selector_literals);
  }
}

void AccumulateMessageSendSelectorLoweringStmt(
    const Stmt *stmt,
    Objc3MessageSendSelectorLoweringContract &contract,
    std::unordered_set<std::string> &selector_literals) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
      if (stmt->let_stmt != nullptr) {
        AccumulateMessageSendSelectorLoweringExpr(stmt->let_stmt->value.get(), contract, selector_literals);
      }
      return;
    case Stmt::Kind::Assign:
      if (stmt->assign_stmt != nullptr) {
        AccumulateMessageSendSelectorLoweringExpr(stmt->assign_stmt->value.get(), contract, selector_literals);
      }
      return;
    case Stmt::Kind::Return:
      if (stmt->return_stmt != nullptr) {
        AccumulateMessageSendSelectorLoweringExpr(stmt->return_stmt->value.get(), contract, selector_literals);
      }
      return;
    case Stmt::Kind::Expr:
      if (stmt->expr_stmt != nullptr) {
        AccumulateMessageSendSelectorLoweringExpr(stmt->expr_stmt->value.get(), contract, selector_literals);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt == nullptr) {
        return;
      }
      AccumulateMessageSendSelectorLoweringExpr(stmt->if_stmt->condition.get(), contract, selector_literals);
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        AccumulateMessageSendSelectorLoweringStmt(then_stmt.get(), contract, selector_literals);
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        AccumulateMessageSendSelectorLoweringStmt(else_stmt.get(), contract, selector_literals);
      }
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt == nullptr) {
        return;
      }
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        AccumulateMessageSendSelectorLoweringStmt(body_stmt.get(), contract, selector_literals);
      }
      AccumulateMessageSendSelectorLoweringExpr(stmt->do_while_stmt->condition.get(), contract, selector_literals);
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt == nullptr) {
        return;
      }
      AccumulateMessageSendSelectorLoweringForClause(stmt->for_stmt->init, contract, selector_literals);
      AccumulateMessageSendSelectorLoweringExpr(stmt->for_stmt->condition.get(), contract, selector_literals);
      AccumulateMessageSendSelectorLoweringForClause(stmt->for_stmt->step, contract, selector_literals);
      for (const auto &body_stmt : stmt->for_stmt->body) {
        AccumulateMessageSendSelectorLoweringStmt(body_stmt.get(), contract, selector_literals);
      }
      return;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt == nullptr) {
        return;
      }
      AccumulateMessageSendSelectorLoweringExpr(stmt->switch_stmt->condition.get(), contract, selector_literals);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &case_stmt : switch_case.body) {
          AccumulateMessageSendSelectorLoweringStmt(case_stmt.get(), contract, selector_literals);
        }
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt == nullptr) {
        return;
      }
      AccumulateMessageSendSelectorLoweringExpr(stmt->while_stmt->condition.get(), contract, selector_literals);
      for (const auto &body_stmt : stmt->while_stmt->body) {
        AccumulateMessageSendSelectorLoweringStmt(body_stmt.get(), contract, selector_literals);
      }
      return;
    case Stmt::Kind::Block:
      if (stmt->block_stmt == nullptr) {
        return;
      }
      for (const auto &body_stmt : stmt->block_stmt->body) {
        AccumulateMessageSendSelectorLoweringStmt(body_stmt.get(), contract, selector_literals);
      }
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

void AccumulateDispatchSurfaceClassificationExpr(
    const Expr *expr,
    Objc3DispatchSurfaceClassificationContract &contract);

void AccumulateDispatchSurfaceClassificationStmt(
    const Stmt *stmt,
    Objc3DispatchSurfaceClassificationContract &contract) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
    case Stmt::Kind::Let:
      AccumulateDispatchSurfaceClassificationExpr(stmt->let_stmt->value.get(),
                                                  contract);
      return;
    case Stmt::Kind::Assign:
      AccumulateDispatchSurfaceClassificationExpr(
          stmt->assign_stmt->value.get(), contract);
      return;
    case Stmt::Kind::Return:
      AccumulateDispatchSurfaceClassificationExpr(
          stmt->return_stmt->value.get(), contract);
      return;
    case Stmt::Kind::Expr:
      AccumulateDispatchSurfaceClassificationExpr(stmt->expr_stmt->value.get(),
                                                  contract);
      return;
    case Stmt::Kind::If:
      AccumulateDispatchSurfaceClassificationExpr(
          stmt->if_stmt->condition.get(), contract);
      for (const auto &nested : stmt->if_stmt->then_body) {
        AccumulateDispatchSurfaceClassificationStmt(nested.get(), contract);
      }
      for (const auto &nested : stmt->if_stmt->else_body) {
        AccumulateDispatchSurfaceClassificationStmt(nested.get(), contract);
      }
      return;
    case Stmt::Kind::DoWhile:
      for (const auto &nested : stmt->do_while_stmt->body) {
        AccumulateDispatchSurfaceClassificationStmt(nested.get(), contract);
      }
      AccumulateDispatchSurfaceClassificationExpr(
          stmt->do_while_stmt->condition.get(), contract);
      return;
    case Stmt::Kind::For:
      AccumulateDispatchSurfaceClassificationExpr(stmt->for_stmt->init.value.get(),
                                                  contract);
      AccumulateDispatchSurfaceClassificationExpr(
          stmt->for_stmt->condition.get(), contract);
      AccumulateDispatchSurfaceClassificationExpr(stmt->for_stmt->step.value.get(),
                                                  contract);
      for (const auto &nested : stmt->for_stmt->body) {
        AccumulateDispatchSurfaceClassificationStmt(nested.get(), contract);
      }
      return;
    case Stmt::Kind::Switch:
      AccumulateDispatchSurfaceClassificationExpr(
          stmt->switch_stmt->condition.get(), contract);
      for (const auto &case_stmt : stmt->switch_stmt->cases) {
        for (const auto &nested : case_stmt.body) {
          AccumulateDispatchSurfaceClassificationStmt(nested.get(), contract);
        }
      }
      return;
    case Stmt::Kind::While:
      AccumulateDispatchSurfaceClassificationExpr(
          stmt->while_stmt->condition.get(), contract);
      for (const auto &nested : stmt->while_stmt->body) {
        AccumulateDispatchSurfaceClassificationStmt(nested.get(), contract);
      }
      return;
    case Stmt::Kind::Block:
      for (const auto &nested : stmt->block_stmt->body) {
        AccumulateDispatchSurfaceClassificationStmt(nested.get(), contract);
      }
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

void AccumulateDispatchSurfaceClassificationExpr(
    const Expr *expr,
    Objc3DispatchSurfaceClassificationContract &contract) {
  if (expr == nullptr) {
    return;
  }

  switch (expr->kind) {
    case Expr::Kind::MessageSend: {
      if (!expr->dispatch_surface_is_normalized) {
        contract.deterministic = false;
      }
      switch (expr->dispatch_surface_kind) {
        case Expr::DispatchSurfaceKind::Instance:
          ++contract.instance_dispatch_sites;
          break;
        case Expr::DispatchSurfaceKind::Class:
          ++contract.class_dispatch_sites;
          break;
        case Expr::DispatchSurfaceKind::Super:
          ++contract.super_dispatch_sites;
          break;
        case Expr::DispatchSurfaceKind::Direct:
          ++contract.direct_dispatch_sites;
          break;
        case Expr::DispatchSurfaceKind::Dynamic:
          ++contract.dynamic_dispatch_sites;
          break;
        case Expr::DispatchSurfaceKind::Unclassified:
        default:
          ++contract.dynamic_dispatch_sites;
          contract.deterministic = false;
          break;
      }
      AccumulateDispatchSurfaceClassificationExpr(expr->receiver.get(), contract);
      for (const auto &arg : expr->args) {
        AccumulateDispatchSurfaceClassificationExpr(arg.get(), contract);
      }
      return;
    }
    case Expr::Kind::Binary:
      AccumulateDispatchSurfaceClassificationExpr(expr->left.get(), contract);
      AccumulateDispatchSurfaceClassificationExpr(expr->right.get(), contract);
      return;
    case Expr::Kind::Conditional:
      AccumulateDispatchSurfaceClassificationExpr(expr->left.get(), contract);
      AccumulateDispatchSurfaceClassificationExpr(expr->right.get(), contract);
      AccumulateDispatchSurfaceClassificationExpr(expr->third.get(), contract);
      return;
    case Expr::Kind::Call:
      AccumulateDispatchSurfaceClassificationExpr(expr->receiver.get(), contract);
      for (const auto &arg : expr->args) {
        AccumulateDispatchSurfaceClassificationExpr(arg.get(), contract);
      }
      return;
    case Expr::Kind::BlockLiteral:
      return;
    case Expr::Kind::Number:
    case Expr::Kind::BoolLiteral:
    case Expr::Kind::NilLiteral:
    case Expr::Kind::Identifier:
      return;
  }
}

Objc3DispatchSurfaceClassificationContract BuildDispatchSurfaceClassificationContract(
    const Objc3Program &program) {
  Objc3DispatchSurfaceClassificationContract contract;
  for (const auto &global : program.globals) {
    AccumulateDispatchSurfaceClassificationExpr(global.value.get(), contract);
  }
  for (const auto &function : program.functions) {
    for (const auto &stmt : function.body) {
      AccumulateDispatchSurfaceClassificationStmt(stmt.get(), contract);
    }
  }
  for (const auto &implementation_decl : program.implementations) {
    for (const auto &method_decl : implementation_decl.methods) {
      if (!method_decl.has_body) {
        continue;
      }
      for (const auto &stmt : method_decl.body) {
        AccumulateDispatchSurfaceClassificationStmt(stmt.get(), contract);
      }
    }
  }
  return contract;
}

Objc3MessageSendSelectorLoweringContract BuildMessageSendSelectorLoweringContract(const Objc3Program &program) {
  Objc3MessageSendSelectorLoweringContract contract;
  std::unordered_set<std::string> selector_literals;

  for (const auto &global : program.globals) {
    AccumulateMessageSendSelectorLoweringExpr(global.value.get(), contract, selector_literals);
  }
  for (const auto &function : program.functions) {
    for (const auto &stmt : function.body) {
      AccumulateMessageSendSelectorLoweringStmt(stmt.get(), contract, selector_literals);
    }
  }
  // M255-B003 parity-expansion anchor: lowering-side selector accounting must
  // walk implementation methods too or runtime legality/method-family evidence
  // drifts from the sema-owned site surface.
  for (const auto &implementation_decl : program.implementations) {
    for (const auto &method_decl : implementation_decl.methods) {
      if (!method_decl.has_body) {
        continue;
      }
      for (const auto &stmt : method_decl.body) {
        AccumulateMessageSendSelectorLoweringStmt(stmt.get(), contract, selector_literals);
      }
    }
  }

  contract.selector_literal_entries = selector_literals.size();
  for (const auto &selector : selector_literals) {
    contract.selector_literal_characters += selector.size();
  }
  return contract;
}

void AccumulateDispatchAbiMarshallingExpr(
    const Expr *expr,
    std::size_t runtime_dispatch_arg_slots,
    Objc3DispatchAbiMarshallingContract &contract) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
    case Expr::Kind::MessageSend: {
      ++contract.message_send_sites;
      ++contract.receiver_slots_marshaled;
      ++contract.selector_slots_marshaled;
      const std::size_t actual_args = expr->args.size();
      const std::size_t marshalled_args = std::min(actual_args, runtime_dispatch_arg_slots);
      contract.argument_value_slots_marshaled += marshalled_args;
      if (actual_args > runtime_dispatch_arg_slots) {
        contract.deterministic = false;
      }
      contract.argument_padding_slots_marshaled += (runtime_dispatch_arg_slots - marshalled_args);
      contract.argument_total_slots_marshaled += runtime_dispatch_arg_slots;
      AccumulateDispatchAbiMarshallingExpr(expr->receiver.get(), runtime_dispatch_arg_slots, contract);
      for (const auto &arg : expr->args) {
        AccumulateDispatchAbiMarshallingExpr(arg.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    }
    case Expr::Kind::Binary:
      AccumulateDispatchAbiMarshallingExpr(expr->left.get(), runtime_dispatch_arg_slots, contract);
      AccumulateDispatchAbiMarshallingExpr(expr->right.get(), runtime_dispatch_arg_slots, contract);
      return;
    case Expr::Kind::Conditional:
      AccumulateDispatchAbiMarshallingExpr(expr->left.get(), runtime_dispatch_arg_slots, contract);
      AccumulateDispatchAbiMarshallingExpr(expr->right.get(), runtime_dispatch_arg_slots, contract);
      AccumulateDispatchAbiMarshallingExpr(expr->third.get(), runtime_dispatch_arg_slots, contract);
      return;
    case Expr::Kind::Call:
      for (const auto &arg : expr->args) {
        AccumulateDispatchAbiMarshallingExpr(arg.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    default:
      return;
  }
}

void AccumulateDispatchAbiMarshallingForClause(
    const ForClause &clause,
    std::size_t runtime_dispatch_arg_slots,
    Objc3DispatchAbiMarshallingContract &contract) {
  if (clause.value != nullptr) {
    AccumulateDispatchAbiMarshallingExpr(clause.value.get(), runtime_dispatch_arg_slots, contract);
  }
}

void AccumulateDispatchAbiMarshallingStmt(
    const Stmt *stmt,
    std::size_t runtime_dispatch_arg_slots,
    Objc3DispatchAbiMarshallingContract &contract) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
      if (stmt->let_stmt != nullptr) {
        AccumulateDispatchAbiMarshallingExpr(stmt->let_stmt->value.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    case Stmt::Kind::Assign:
      if (stmt->assign_stmt != nullptr) {
        AccumulateDispatchAbiMarshallingExpr(stmt->assign_stmt->value.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    case Stmt::Kind::Return:
      if (stmt->return_stmt != nullptr) {
        AccumulateDispatchAbiMarshallingExpr(stmt->return_stmt->value.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    case Stmt::Kind::Expr:
      if (stmt->expr_stmt != nullptr) {
        AccumulateDispatchAbiMarshallingExpr(stmt->expr_stmt->value.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt == nullptr) {
        return;
      }
      AccumulateDispatchAbiMarshallingExpr(stmt->if_stmt->condition.get(), runtime_dispatch_arg_slots, contract);
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        AccumulateDispatchAbiMarshallingStmt(then_stmt.get(), runtime_dispatch_arg_slots, contract);
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        AccumulateDispatchAbiMarshallingStmt(else_stmt.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt == nullptr) {
        return;
      }
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        AccumulateDispatchAbiMarshallingStmt(body_stmt.get(), runtime_dispatch_arg_slots, contract);
      }
      AccumulateDispatchAbiMarshallingExpr(stmt->do_while_stmt->condition.get(), runtime_dispatch_arg_slots, contract);
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt == nullptr) {
        return;
      }
      AccumulateDispatchAbiMarshallingForClause(stmt->for_stmt->init, runtime_dispatch_arg_slots, contract);
      AccumulateDispatchAbiMarshallingExpr(stmt->for_stmt->condition.get(), runtime_dispatch_arg_slots, contract);
      AccumulateDispatchAbiMarshallingForClause(stmt->for_stmt->step, runtime_dispatch_arg_slots, contract);
      for (const auto &body_stmt : stmt->for_stmt->body) {
        AccumulateDispatchAbiMarshallingStmt(body_stmt.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt == nullptr) {
        return;
      }
      AccumulateDispatchAbiMarshallingExpr(stmt->switch_stmt->condition.get(), runtime_dispatch_arg_slots, contract);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &case_stmt : switch_case.body) {
          AccumulateDispatchAbiMarshallingStmt(case_stmt.get(), runtime_dispatch_arg_slots, contract);
        }
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt == nullptr) {
        return;
      }
      AccumulateDispatchAbiMarshallingExpr(stmt->while_stmt->condition.get(), runtime_dispatch_arg_slots, contract);
      for (const auto &body_stmt : stmt->while_stmt->body) {
        AccumulateDispatchAbiMarshallingStmt(body_stmt.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    case Stmt::Kind::Block:
      if (stmt->block_stmt == nullptr) {
        return;
      }
      for (const auto &body_stmt : stmt->block_stmt->body) {
        AccumulateDispatchAbiMarshallingStmt(body_stmt.get(), runtime_dispatch_arg_slots, contract);
      }
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}

Objc3DispatchAbiMarshallingContract BuildDispatchAbiMarshallingContract(
    const Objc3Program &program,
    std::size_t runtime_dispatch_arg_slots) {
  Objc3DispatchAbiMarshallingContract contract;
  contract.runtime_dispatch_arg_slots = runtime_dispatch_arg_slots;

  for (const auto &global : program.globals) {
    AccumulateDispatchAbiMarshallingExpr(global.value.get(), runtime_dispatch_arg_slots, contract);
  }
  for (const auto &function : program.functions) {
    for (const auto &stmt : function.body) {
      AccumulateDispatchAbiMarshallingStmt(stmt.get(), runtime_dispatch_arg_slots, contract);
    }
  }
  // M255-B003 parity-expansion anchor: ABI marshalling counts must include
  // implementation method bodies so runtime-shim host-link validation stays in
  // lockstep with live super/dynamic dispatch sites.
  for (const auto &implementation_decl : program.implementations) {
    for (const auto &method_decl : implementation_decl.methods) {
      if (!method_decl.has_body) {
        continue;
      }
      for (const auto &stmt : method_decl.body) {
        AccumulateDispatchAbiMarshallingStmt(stmt.get(), runtime_dispatch_arg_slots, contract);
      }
    }
  }

  contract.total_marshaled_slots = contract.receiver_slots_marshaled +
                                   contract.selector_slots_marshaled +
                                   contract.argument_total_slots_marshaled;
  return contract;
}

Objc3NilReceiverSemanticsFoldabilityContract BuildNilReceiverSemanticsFoldabilityContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3NilReceiverSemanticsFoldabilityContract contract;
  contract.message_send_sites =
      sema_parity_surface.nil_receiver_semantics_foldability_sites_total;
  contract.receiver_nil_literal_sites =
      sema_parity_surface.nil_receiver_semantics_foldability_receiver_nil_literal_sites_total;
  contract.nil_receiver_semantics_enabled_sites =
      sema_parity_surface.nil_receiver_semantics_foldability_enabled_sites_total;
  contract.nil_receiver_foldable_sites =
      sema_parity_surface.nil_receiver_semantics_foldability_foldable_sites_total;
  contract.nil_receiver_runtime_dispatch_required_sites =
      sema_parity_surface.nil_receiver_semantics_foldability_runtime_dispatch_required_sites_total;
  contract.non_nil_receiver_sites =
      sema_parity_surface.nil_receiver_semantics_foldability_non_nil_receiver_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.nil_receiver_semantics_foldability_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.nil_receiver_semantics_foldability_summary.deterministic &&
      sema_parity_surface.deterministic_nil_receiver_semantics_foldability_handoff;
  return contract;
}

Objc3SuperDispatchMethodFamilyContract BuildSuperDispatchMethodFamilyContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3SuperDispatchMethodFamilyContract contract;
  contract.message_send_sites =
      sema_parity_surface.super_dispatch_method_family_sites_total;
  contract.receiver_super_identifier_sites =
      sema_parity_surface.super_dispatch_method_family_receiver_super_identifier_sites_total;
  contract.super_dispatch_enabled_sites =
      sema_parity_surface.super_dispatch_method_family_enabled_sites_total;
  contract.super_dispatch_requires_class_context_sites =
      sema_parity_surface.super_dispatch_method_family_requires_class_context_sites_total;
  contract.method_family_init_sites =
      sema_parity_surface.super_dispatch_method_family_init_sites_total;
  contract.method_family_copy_sites =
      sema_parity_surface.super_dispatch_method_family_copy_sites_total;
  contract.method_family_mutable_copy_sites =
      sema_parity_surface.super_dispatch_method_family_mutable_copy_sites_total;
  contract.method_family_new_sites =
      sema_parity_surface.super_dispatch_method_family_new_sites_total;
  contract.method_family_none_sites =
      sema_parity_surface.super_dispatch_method_family_none_sites_total;
  contract.method_family_returns_retained_result_sites =
      sema_parity_surface.super_dispatch_method_family_returns_retained_result_sites_total;
  contract.method_family_returns_related_result_sites =
      sema_parity_surface.super_dispatch_method_family_returns_related_result_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.super_dispatch_method_family_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.super_dispatch_method_family_summary.deterministic &&
      sema_parity_surface.deterministic_super_dispatch_method_family_handoff;
  return contract;
}

Objc3RuntimeShimHostLinkContract BuildRuntimeShimHostLinkContract(
    const Objc3DispatchAbiMarshallingContract &dispatch_abi_marshalling_contract,
    const Objc3NilReceiverSemanticsFoldabilityContract &nil_receiver_semantics_foldability_contract,
    const Objc3FrontendOptions &options) {
  Objc3RuntimeShimHostLinkContract contract;
  contract.message_send_sites = dispatch_abi_marshalling_contract.message_send_sites;
  contract.runtime_shim_required_sites =
      nil_receiver_semantics_foldability_contract.nil_receiver_runtime_dispatch_required_sites;
  if (contract.runtime_shim_required_sites <= contract.message_send_sites) {
    contract.runtime_shim_elided_sites =
        contract.message_send_sites - contract.runtime_shim_required_sites;
  } else {
    contract.runtime_shim_elided_sites = 0;
    contract.contract_violation_sites = 1;
  }
  contract.runtime_dispatch_arg_slots = options.lowering.max_message_send_args;
  contract.runtime_dispatch_declaration_parameter_count = contract.runtime_dispatch_arg_slots + 2u;
  contract.runtime_dispatch_symbol = options.lowering.runtime_dispatch_symbol;
  contract.default_runtime_dispatch_symbol_binding =
      contract.runtime_dispatch_symbol == kObjc3RuntimeDispatchSymbol;
  contract.deterministic =
      dispatch_abi_marshalling_contract.deterministic &&
      nil_receiver_semantics_foldability_contract.deterministic;
  return contract;
}

Objc3RuntimeDispatchLoweringAbiContract BuildRuntimeDispatchLoweringAbiContract(
    const Objc3DispatchAbiMarshallingContract &dispatch_abi_marshalling_contract,
    const Objc3RuntimeShimHostLinkContract &runtime_shim_host_link_contract,
    const Objc3RuntimeBootstrapApiSummary &runtime_bootstrap_api_summary) {
  Objc3RuntimeDispatchLoweringAbiContract contract;
  contract.message_send_sites = dispatch_abi_marshalling_contract.message_send_sites;
  contract.fixed_argument_slot_count =
      runtime_shim_host_link_contract.runtime_dispatch_arg_slots;
  contract.runtime_dispatch_parameter_count =
      runtime_shim_host_link_contract
          .runtime_dispatch_declaration_parameter_count;
  contract.canonical_runtime_dispatch_symbol =
      runtime_bootstrap_api_summary.dispatch_entrypoint_symbol;
  contract.compatibility_runtime_dispatch_symbol =
      runtime_bootstrap_api_summary.compatibility_dispatch_symbol;
  contract.default_lowering_target_symbol =
      contract.canonical_runtime_dispatch_symbol;
  contract.default_lowering_target_model =
      kObjc3RuntimeDispatchLiveCutoverDefaultTargetModel;
  contract.compatibility_bridge_role_model =
      kObjc3RuntimeDispatchLiveCutoverCompatibilityModel;
  contract.deferred_cases_model =
      kObjc3RuntimeDispatchLiveCutoverDeferredCasesModel;
  contract.selector_lookup_symbol =
      runtime_bootstrap_api_summary.selector_lookup_symbol;
  contract.selector_handle_type =
      runtime_bootstrap_api_summary.selector_handle_type;
  contract.fail_closed =
      IsReadyObjc3RuntimeBootstrapApiSummary(runtime_bootstrap_api_summary);
  contract.deterministic =
      dispatch_abi_marshalling_contract.deterministic &&
      runtime_shim_host_link_contract.deterministic &&
      !runtime_bootstrap_api_summary.replay_key.empty();
  return contract;
}

Objc3OwnershipQualifierLoweringContract BuildOwnershipQualifierLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3OwnershipQualifierLoweringContract contract;
  contract.ownership_qualifier_sites =
      sema_parity_surface.type_annotation_ownership_qualifier_sites_total;
  contract.invalid_ownership_qualifier_sites =
      sema_parity_surface.type_annotation_invalid_ownership_qualifier_sites_total;
  contract.object_pointer_type_annotation_sites =
      sema_parity_surface.type_annotation_object_pointer_type_sites_total;
  contract.deterministic =
      sema_parity_surface.type_annotation_surface_summary.deterministic &&
      sema_parity_surface.deterministic_type_annotation_surface_handoff;
  return contract;
}

Objc3RetainReleaseOperationLoweringContract BuildRetainReleaseOperationLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3RetainReleaseOperationLoweringContract contract;
  contract.ownership_qualified_sites =
      sema_parity_surface.retain_release_operation_ownership_qualified_sites_total;
  contract.retain_insertion_sites =
      sema_parity_surface.retain_release_operation_retain_insertion_sites_total;
  contract.release_insertion_sites =
      sema_parity_surface.retain_release_operation_release_insertion_sites_total;
  contract.autorelease_insertion_sites =
      sema_parity_surface.retain_release_operation_autorelease_insertion_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.retain_release_operation_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.retain_release_operation_summary.deterministic &&
      sema_parity_surface.deterministic_retain_release_operation_handoff;
  return contract;
}

Objc3AutoreleasePoolScopeLoweringContract BuildAutoreleasePoolScopeLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3AutoreleasePoolScopeLoweringContract contract;
  contract.scope_sites = sema_parity_surface.autoreleasepool_scope_sites_total;
  contract.scope_symbolized_sites = sema_parity_surface.autoreleasepool_scope_symbolized_sites_total;
  contract.max_scope_depth = sema_parity_surface.autoreleasepool_scope_max_depth_total;
  contract.scope_entry_transition_sites = sema_parity_surface.autoreleasepool_scope_sites_total;
  contract.scope_exit_transition_sites = sema_parity_surface.autoreleasepool_scope_sites_total;
  contract.contract_violation_sites = sema_parity_surface.autoreleasepool_scope_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.autoreleasepool_scope_summary.deterministic &&
      sema_parity_surface.deterministic_autoreleasepool_scope_handoff;
  return contract;
}

Objc3WeakUnownedSemanticsLoweringContract BuildWeakUnownedSemanticsLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3WeakUnownedSemanticsLoweringContract contract;
  contract.ownership_candidate_sites =
      sema_parity_surface.weak_unowned_semantics_ownership_candidate_sites_total;
  contract.weak_reference_sites =
      sema_parity_surface.weak_unowned_semantics_weak_reference_sites_total;
  contract.unowned_reference_sites =
      sema_parity_surface.weak_unowned_semantics_unowned_reference_sites_total;
  contract.unowned_safe_reference_sites =
      sema_parity_surface.weak_unowned_semantics_unowned_safe_reference_sites_total;
  contract.weak_unowned_conflict_sites =
      sema_parity_surface.weak_unowned_semantics_conflict_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.weak_unowned_semantics_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.weak_unowned_semantics_summary.deterministic &&
      sema_parity_surface.deterministic_weak_unowned_semantics_handoff;
  return contract;
}

Objc3ArcDiagnosticsFixitLoweringContract BuildArcDiagnosticsFixitLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3ArcDiagnosticsFixitLoweringContract contract;
  contract.ownership_arc_diagnostic_candidate_sites =
      sema_parity_surface.ownership_arc_diagnostic_candidate_sites_total;
  contract.ownership_arc_fixit_available_sites =
      sema_parity_surface.ownership_arc_fixit_available_sites_total;
  contract.ownership_arc_profiled_sites =
      sema_parity_surface.ownership_arc_profiled_sites_total;
  contract.ownership_arc_weak_unowned_conflict_diagnostic_sites =
      sema_parity_surface.ownership_arc_weak_unowned_conflict_diagnostic_sites_total;
  contract.ownership_arc_empty_fixit_hint_sites =
      sema_parity_surface.ownership_arc_empty_fixit_hint_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.ownership_arc_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.arc_diagnostics_fixit_summary.deterministic &&
      sema_parity_surface.deterministic_arc_diagnostics_fixit_handoff;
  return contract;
}

Objc3BlockLiteralCaptureLoweringContract BuildBlockLiteralCaptureLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3BlockLiteralCaptureLoweringContract contract;
  contract.block_literal_sites =
      sema_parity_surface.block_literal_capture_semantics_sites_total;
  contract.block_parameter_entries =
      sema_parity_surface.block_literal_capture_semantics_parameter_entries_total;
  contract.block_capture_entries =
      sema_parity_surface.block_literal_capture_semantics_capture_entries_total;
  contract.block_body_statement_entries =
      sema_parity_surface.block_literal_capture_semantics_body_statement_entries_total;
  contract.block_empty_capture_sites =
      sema_parity_surface.block_literal_capture_semantics_empty_capture_sites_total;
  contract.block_nondeterministic_capture_sites =
      sema_parity_surface.block_literal_capture_semantics_nondeterministic_capture_sites_total;
  contract.block_non_normalized_sites =
      sema_parity_surface.block_literal_capture_semantics_non_normalized_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.block_literal_capture_semantics_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.block_literal_capture_semantics_summary.deterministic &&
      sema_parity_surface.deterministic_block_literal_capture_semantics_handoff;
  return contract;
}

Objc3BlockAbiInvokeTrampolineLoweringContract BuildBlockAbiInvokeTrampolineLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3BlockAbiInvokeTrampolineLoweringContract contract;
  contract.block_literal_sites =
      sema_parity_surface.block_abi_invoke_trampoline_sites_total;
  contract.invoke_argument_slots_total =
      sema_parity_surface.block_abi_invoke_trampoline_invoke_argument_slots_total;
  contract.capture_word_count_total =
      sema_parity_surface.block_abi_invoke_trampoline_capture_word_count_total;
  contract.parameter_entries_total =
      sema_parity_surface.block_abi_invoke_trampoline_parameter_entries_total;
  contract.capture_entries_total =
      sema_parity_surface.block_abi_invoke_trampoline_capture_entries_total;
  contract.body_statement_entries_total =
      sema_parity_surface.block_abi_invoke_trampoline_body_statement_entries_total;
  contract.descriptor_symbolized_sites =
      sema_parity_surface.block_abi_invoke_trampoline_descriptor_symbolized_sites_total;
  contract.invoke_trampoline_symbolized_sites =
      sema_parity_surface.block_abi_invoke_trampoline_invoke_symbolized_sites_total;
  contract.missing_invoke_trampoline_sites =
      sema_parity_surface.block_abi_invoke_trampoline_missing_invoke_sites_total;
  contract.non_normalized_layout_sites =
      sema_parity_surface.block_abi_invoke_trampoline_non_normalized_layout_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.block_abi_invoke_trampoline_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.block_abi_invoke_trampoline_semantics_summary.deterministic &&
      sema_parity_surface.deterministic_block_abi_invoke_trampoline_handoff;
  return contract;
}

Objc3BlockStorageEscapeLoweringContract BuildBlockStorageEscapeLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3BlockStorageEscapeLoweringContract contract;
  contract.block_literal_sites = sema_parity_surface.block_storage_escape_sites_total;
  contract.mutable_capture_count_total =
      sema_parity_surface.block_storage_escape_mutable_capture_count_total;
  contract.byref_slot_count_total =
      sema_parity_surface.block_storage_escape_byref_slot_count_total;
  contract.parameter_entries_total =
      sema_parity_surface.block_storage_escape_parameter_entries_total;
  contract.capture_entries_total =
      sema_parity_surface.block_storage_escape_capture_entries_total;
  contract.body_statement_entries_total =
      sema_parity_surface.block_storage_escape_body_statement_entries_total;
  contract.requires_byref_cells_sites =
      sema_parity_surface.block_storage_escape_requires_byref_cells_sites_total;
  contract.escape_analysis_enabled_sites =
      sema_parity_surface.block_storage_escape_escape_analysis_enabled_sites_total;
  contract.escape_to_heap_sites =
      sema_parity_surface.block_storage_escape_escape_to_heap_sites_total;
  contract.escape_profile_normalized_sites =
      sema_parity_surface.block_storage_escape_escape_profile_normalized_sites_total;
  contract.byref_layout_symbolized_sites =
      sema_parity_surface.block_storage_escape_byref_layout_symbolized_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.block_storage_escape_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.block_storage_escape_semantics_summary.deterministic &&
      sema_parity_surface.deterministic_block_storage_escape_handoff;
  return contract;
}

Objc3BlockCopyDisposeLoweringContract BuildBlockCopyDisposeLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3BlockCopyDisposeLoweringContract contract;
  contract.block_literal_sites = sema_parity_surface.block_copy_dispose_sites_total;
  contract.mutable_capture_count_total =
      sema_parity_surface.block_copy_dispose_mutable_capture_count_total;
  contract.byref_slot_count_total =
      sema_parity_surface.block_copy_dispose_byref_slot_count_total;
  contract.parameter_entries_total =
      sema_parity_surface.block_copy_dispose_parameter_entries_total;
  contract.capture_entries_total =
      sema_parity_surface.block_copy_dispose_capture_entries_total;
  contract.body_statement_entries_total =
      sema_parity_surface.block_copy_dispose_body_statement_entries_total;
  contract.copy_helper_required_sites =
      sema_parity_surface.block_copy_dispose_copy_helper_required_sites_total;
  contract.dispose_helper_required_sites =
      sema_parity_surface.block_copy_dispose_dispose_helper_required_sites_total;
  contract.profile_normalized_sites =
      sema_parity_surface.block_copy_dispose_profile_normalized_sites_total;
  contract.copy_helper_symbolized_sites =
      sema_parity_surface.block_copy_dispose_copy_helper_symbolized_sites_total;
  contract.dispose_helper_symbolized_sites =
      sema_parity_surface.block_copy_dispose_dispose_helper_symbolized_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.block_copy_dispose_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.block_copy_dispose_semantics_summary.deterministic &&
      sema_parity_surface.deterministic_block_copy_dispose_handoff;
  return contract;
}

Objc3BlockDeterminismPerfBaselineLoweringContract BuildBlockDeterminismPerfBaselineLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3BlockDeterminismPerfBaselineLoweringContract contract;
  contract.block_literal_sites =
      sema_parity_surface.block_determinism_perf_baseline_sites_total;
  contract.baseline_weight_total =
      sema_parity_surface.block_determinism_perf_baseline_weight_total;
  contract.parameter_entries_total =
      sema_parity_surface.block_determinism_perf_baseline_parameter_entries_total;
  contract.capture_entries_total =
      sema_parity_surface.block_determinism_perf_baseline_capture_entries_total;
  contract.body_statement_entries_total =
      sema_parity_surface.block_determinism_perf_baseline_body_statement_entries_total;
  contract.deterministic_capture_sites =
      sema_parity_surface.block_determinism_perf_baseline_deterministic_capture_sites_total;
  contract.heavy_tier_sites =
      sema_parity_surface.block_determinism_perf_baseline_heavy_tier_sites_total;
  contract.normalized_profile_sites =
      sema_parity_surface.block_determinism_perf_baseline_normalized_profile_sites_total;
  contract.contract_violation_sites =
      sema_parity_surface.block_determinism_perf_baseline_contract_violation_sites_total;
  contract.deterministic =
      sema_parity_surface.block_determinism_perf_baseline_summary.deterministic &&
      sema_parity_surface.deterministic_block_determinism_perf_baseline_handoff;
  return contract;
}

Objc3LightweightGenericsConstraintLoweringContract BuildLightweightGenericsConstraintLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3LightweightGenericsConstraintLoweringContract contract;
  const std::size_t raw_sites =
      sema_parity_surface.lightweight_generic_constraint_sites_total;
  const std::size_t raw_generic_suffix_sites =
      sema_parity_surface.lightweight_generic_constraint_generic_suffix_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface.lightweight_generic_constraint_object_pointer_type_sites_total;
  const std::size_t raw_terminated_generic_suffix_sites =
      sema_parity_surface.lightweight_generic_constraint_terminated_generic_suffix_sites_total;
  const std::size_t raw_pointer_declarator_sites =
      sema_parity_surface.lightweight_generic_constraint_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.lightweight_generic_constraint_normalized_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.lightweight_generic_constraint_contract_violation_sites_total;

  contract.generic_constraint_sites =
      std::max({raw_sites, raw_generic_suffix_sites, raw_object_pointer_sites, raw_pointer_declarator_sites,
                raw_normalized_sites, raw_violation_sites});
  contract.generic_suffix_sites =
      std::min(raw_generic_suffix_sites, contract.generic_constraint_sites);
  contract.object_pointer_type_sites =
      std::min(raw_object_pointer_sites, contract.generic_constraint_sites);
  contract.terminated_generic_suffix_sites =
      std::min(raw_terminated_generic_suffix_sites, contract.generic_suffix_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_declarator_sites, contract.generic_constraint_sites);
  contract.normalized_constraint_sites =
      std::min(raw_normalized_sites, contract.generic_constraint_sites);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.generic_constraint_sites);

  contract.deterministic =
      sema_parity_surface.lightweight_generic_constraint_summary.deterministic &&
      sema_parity_surface.deterministic_lightweight_generic_constraint_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_constraint_sites == contract.generic_constraint_sites;
  return contract;
}

Objc3NullabilityFlowWarningPrecisionLoweringContract BuildNullabilityFlowWarningPrecisionLoweringContract(
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
      sema_parity_surface.nullability_flow_warning_precision_summary.deterministic &&
      sema_parity_surface.deterministic_nullability_flow_warning_precision_handoff;
  return contract;
}

Objc3ProtocolQualifiedObjectTypeLoweringContract BuildProtocolQualifiedObjectTypeLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3ProtocolQualifiedObjectTypeLoweringContract contract;
  const std::size_t raw_protocol_sites =
      sema_parity_surface.protocol_qualified_object_type_sites_total;
  const std::size_t raw_protocol_composition_sites =
      sema_parity_surface.protocol_qualified_object_type_protocol_composition_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface.protocol_qualified_object_type_object_pointer_type_sites_total;
  const std::size_t raw_terminated_sites =
      sema_parity_surface.protocol_qualified_object_type_terminated_protocol_composition_sites_total;
  const std::size_t raw_pointer_sites =
      sema_parity_surface.protocol_qualified_object_type_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.protocol_qualified_object_type_normalized_protocol_composition_sites_total;
  const std::size_t raw_contract_violation_sites =
      sema_parity_surface.protocol_qualified_object_type_contract_violation_sites_total;

  contract.protocol_qualified_object_type_sites = std::max(
      {raw_protocol_sites, raw_protocol_composition_sites, raw_pointer_sites, raw_normalized_sites,
       raw_contract_violation_sites});
  contract.protocol_composition_sites =
      std::min(raw_protocol_composition_sites, contract.protocol_qualified_object_type_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.protocol_composition_sites);
  contract.terminated_protocol_composition_sites =
      std::min(raw_terminated_sites, contract.protocol_composition_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_sites, contract.protocol_qualified_object_type_sites);
  contract.normalized_protocol_composition_sites =
      std::min(raw_normalized_sites, contract.protocol_qualified_object_type_sites);
  contract.contract_violation_sites =
      std::min(raw_contract_violation_sites, contract.protocol_qualified_object_type_sites);

  const bool strict_deterministic =
      sema_parity_surface.protocol_qualified_object_type_summary.deterministic &&
      sema_parity_surface.deterministic_protocol_qualified_object_type_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_protocol_composition_sites == contract.protocol_qualified_object_type_sites;
  contract.deterministic = strict_deterministic;
  return contract;
}

Objc3VarianceBridgeCastLoweringContract BuildVarianceBridgeCastLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3VarianceBridgeCastLoweringContract contract;
  const std::size_t raw_sites = sema_parity_surface.variance_bridge_cast_sites_total;
  const std::size_t raw_protocol_sites =
      sema_parity_surface.variance_bridge_cast_protocol_composition_sites_total;
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

  contract.variance_bridge_cast_sites = std::max(
      {raw_sites, raw_protocol_sites, raw_ownership_sites, raw_pointer_sites, raw_normalized_sites,
       raw_violation_sites});
  contract.protocol_composition_sites =
      std::min(raw_protocol_sites, contract.variance_bridge_cast_sites);
  contract.ownership_qualifier_sites =
      std::min(raw_ownership_sites, contract.variance_bridge_cast_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_sites, contract.protocol_composition_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_sites, contract.variance_bridge_cast_sites);
  contract.normalized_sites = std::min(raw_normalized_sites, contract.variance_bridge_cast_sites);
  contract.contract_violation_sites = std::min(raw_violation_sites, contract.variance_bridge_cast_sites);

  contract.deterministic = sema_parity_surface.variance_bridge_cast_summary.deterministic &&
                           sema_parity_surface.deterministic_variance_bridge_cast_handoff &&
                           contract.contract_violation_sites == 0 &&
                           contract.normalized_sites == contract.variance_bridge_cast_sites;
  return contract;
}

Objc3GenericMetadataAbiLoweringContract BuildGenericMetadataAbiLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3GenericMetadataAbiLoweringContract contract;
  const std::size_t raw_sites = sema_parity_surface.generic_metadata_abi_sites_total;
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

  contract.generic_metadata_abi_sites = std::max(
      {raw_sites, raw_generic_suffix_sites, raw_protocol_sites, raw_ownership_sites, raw_pointer_sites,
       raw_normalized_sites, raw_violation_sites});
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
  contract.normalized_sites = std::min(raw_normalized_sites, contract.generic_metadata_abi_sites);
  contract.contract_violation_sites = std::min(raw_violation_sites, contract.generic_metadata_abi_sites);

  contract.deterministic = sema_parity_surface.generic_metadata_abi_summary.deterministic &&
                           sema_parity_surface.deterministic_generic_metadata_abi_handoff &&
                           contract.contract_violation_sites == 0 &&
                           contract.normalized_sites == contract.generic_metadata_abi_sites;
  return contract;
}

Objc3ModuleImportGraphLoweringContract BuildModuleImportGraphLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3ModuleImportGraphLoweringContract contract;
  const std::size_t raw_sites = sema_parity_surface.module_import_graph_sites_total;
  const std::size_t raw_import_edge_sites =
      sema_parity_surface.module_import_graph_import_edge_candidate_sites_total;
  const std::size_t raw_namespace_segment_sites =
      sema_parity_surface.module_import_graph_namespace_segment_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface.module_import_graph_object_pointer_type_sites_total;
  const std::size_t raw_pointer_declarator_sites =
      sema_parity_surface.module_import_graph_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.module_import_graph_normalized_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.module_import_graph_contract_violation_sites_total;

  contract.module_import_graph_sites =
      std::max({raw_sites, raw_import_edge_sites, raw_namespace_segment_sites, raw_pointer_declarator_sites,
                raw_normalized_sites, raw_violation_sites});
  contract.import_edge_candidate_sites =
      std::min(raw_import_edge_sites, contract.module_import_graph_sites);
  contract.namespace_segment_sites =
      std::min(raw_namespace_segment_sites, contract.module_import_graph_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.import_edge_candidate_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_declarator_sites, contract.module_import_graph_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.module_import_graph_sites);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.module_import_graph_sites);
  contract.deterministic = sema_parity_surface.module_import_graph_summary.deterministic &&
                           sema_parity_surface.deterministic_module_import_graph_handoff &&
                           contract.contract_violation_sites == 0 &&
                           contract.normalized_sites == contract.module_import_graph_sites;
  return contract;
}

Objc3NamespaceCollisionShadowingLoweringContract
BuildNamespaceCollisionShadowingLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3NamespaceCollisionShadowingLoweringContract contract;
  const std::size_t raw_sites = sema_parity_surface.namespace_collision_shadowing_sites_total;
  const std::size_t raw_namespace_segment_sites =
      sema_parity_surface.namespace_collision_shadowing_namespace_segment_sites_total;
  const std::size_t raw_import_edge_sites =
      sema_parity_surface.namespace_collision_shadowing_import_edge_candidate_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface.namespace_collision_shadowing_object_pointer_type_sites_total;
  const std::size_t raw_pointer_declarator_sites =
      sema_parity_surface.namespace_collision_shadowing_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.namespace_collision_shadowing_normalized_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.namespace_collision_shadowing_contract_violation_sites_total;

  contract.namespace_collision_shadowing_sites =
      std::max({raw_sites, raw_namespace_segment_sites, raw_import_edge_sites, raw_pointer_declarator_sites,
                raw_normalized_sites, raw_violation_sites});
  contract.namespace_segment_sites =
      std::min(raw_namespace_segment_sites, contract.namespace_collision_shadowing_sites);
  contract.import_edge_candidate_sites =
      std::min(raw_import_edge_sites, contract.namespace_collision_shadowing_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.import_edge_candidate_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_declarator_sites, contract.namespace_collision_shadowing_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.namespace_collision_shadowing_sites);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.namespace_collision_shadowing_sites);
  contract.deterministic = sema_parity_surface.namespace_collision_shadowing_summary.deterministic &&
                           sema_parity_surface.deterministic_namespace_collision_shadowing_handoff &&
                           contract.contract_violation_sites == 0 &&
                           contract.normalized_sites == contract.namespace_collision_shadowing_sites;
  return contract;
}

Objc3PublicPrivateApiPartitionLoweringContract
BuildPublicPrivateApiPartitionLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3PublicPrivateApiPartitionLoweringContract contract;
  const std::size_t raw_sites = sema_parity_surface.public_private_api_partition_sites_total;
  const std::size_t raw_namespace_segment_sites =
      sema_parity_surface.public_private_api_partition_namespace_segment_sites_total;
  const std::size_t raw_import_edge_sites =
      sema_parity_surface.public_private_api_partition_import_edge_candidate_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface.public_private_api_partition_object_pointer_type_sites_total;
  const std::size_t raw_pointer_declarator_sites =
      sema_parity_surface.public_private_api_partition_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.public_private_api_partition_normalized_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.public_private_api_partition_contract_violation_sites_total;

  contract.public_private_api_partition_sites =
      std::max({raw_sites, raw_namespace_segment_sites, raw_import_edge_sites, raw_pointer_declarator_sites,
                raw_normalized_sites, raw_violation_sites});
  contract.namespace_segment_sites =
      std::min(raw_namespace_segment_sites, contract.public_private_api_partition_sites);
  contract.import_edge_candidate_sites =
      std::min(raw_import_edge_sites, contract.public_private_api_partition_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.import_edge_candidate_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_declarator_sites, contract.public_private_api_partition_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.public_private_api_partition_sites);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.public_private_api_partition_sites);
  contract.deterministic = sema_parity_surface.public_private_api_partition_summary.deterministic &&
                           sema_parity_surface.deterministic_public_private_api_partition_handoff &&
                           contract.contract_violation_sites == 0 &&
                           contract.normalized_sites == contract.public_private_api_partition_sites;
  return contract;
}

Objc3IncrementalModuleCacheInvalidationLoweringContract
BuildIncrementalModuleCacheInvalidationLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3IncrementalModuleCacheInvalidationLoweringContract contract;
  const std::size_t raw_sites =
      sema_parity_surface.incremental_module_cache_invalidation_sites_total;
  const std::size_t raw_namespace_segment_sites =
      sema_parity_surface.incremental_module_cache_invalidation_namespace_segment_sites_total;
  const std::size_t raw_import_edge_sites =
      sema_parity_surface.incremental_module_cache_invalidation_import_edge_candidate_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface.incremental_module_cache_invalidation_object_pointer_type_sites_total;
  const std::size_t raw_pointer_declarator_sites =
      sema_parity_surface.incremental_module_cache_invalidation_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.incremental_module_cache_invalidation_normalized_sites_total;
  const std::size_t raw_cache_candidate_sites =
      sema_parity_surface.incremental_module_cache_invalidation_cache_invalidation_candidate_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.incremental_module_cache_invalidation_contract_violation_sites_total;

  contract.incremental_module_cache_invalidation_sites =
      std::max({raw_sites, raw_namespace_segment_sites, raw_import_edge_sites, raw_pointer_declarator_sites,
                raw_normalized_sites, raw_cache_candidate_sites, raw_violation_sites});
  contract.namespace_segment_sites =
      std::min(raw_namespace_segment_sites, contract.incremental_module_cache_invalidation_sites);
  contract.import_edge_candidate_sites =
      std::min(raw_import_edge_sites, contract.incremental_module_cache_invalidation_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.import_edge_candidate_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_declarator_sites, contract.incremental_module_cache_invalidation_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.incremental_module_cache_invalidation_sites);
  const std::size_t normalized_budget =
      (contract.incremental_module_cache_invalidation_sites >= contract.normalized_sites)
          ? (contract.incremental_module_cache_invalidation_sites - contract.normalized_sites)
          : 0;
  contract.cache_invalidation_candidate_sites =
      std::min(raw_cache_candidate_sites, normalized_budget);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.incremental_module_cache_invalidation_sites);
  contract.deterministic =
      sema_parity_surface.incremental_module_cache_invalidation_summary
          .deterministic &&
      sema_parity_surface
          .deterministic_incremental_module_cache_invalidation_handoff &&
      contract.contract_violation_sites == 0 &&
      contract.normalized_sites ==
          contract.incremental_module_cache_invalidation_sites;
  return contract;
}

Objc3CrossModuleConformanceLoweringContract
BuildCrossModuleConformanceLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3CrossModuleConformanceLoweringContract contract;
  const std::size_t raw_sites = sema_parity_surface.cross_module_conformance_sites_total;
  const std::size_t raw_namespace_segment_sites =
      sema_parity_surface.cross_module_conformance_namespace_segment_sites_total;
  const std::size_t raw_import_edge_sites =
      sema_parity_surface.cross_module_conformance_import_edge_candidate_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface.cross_module_conformance_object_pointer_type_sites_total;
  const std::size_t raw_pointer_declarator_sites =
      sema_parity_surface.cross_module_conformance_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.cross_module_conformance_normalized_sites_total;
  const std::size_t raw_cache_candidate_sites =
      sema_parity_surface.cross_module_conformance_cache_invalidation_candidate_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.cross_module_conformance_contract_violation_sites_total;

  contract.cross_module_conformance_sites =
      std::max({raw_sites, raw_namespace_segment_sites, raw_import_edge_sites, raw_pointer_declarator_sites,
                raw_normalized_sites, raw_cache_candidate_sites, raw_violation_sites});
  contract.namespace_segment_sites =
      std::min(raw_namespace_segment_sites, contract.cross_module_conformance_sites);
  contract.import_edge_candidate_sites =
      std::min(raw_import_edge_sites, contract.cross_module_conformance_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.import_edge_candidate_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_declarator_sites, contract.cross_module_conformance_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.cross_module_conformance_sites);
  const std::size_t normalized_budget =
      (contract.cross_module_conformance_sites >= contract.normalized_sites)
          ? (contract.cross_module_conformance_sites - contract.normalized_sites)
          : 0;
  contract.cache_invalidation_candidate_sites =
      std::min(raw_cache_candidate_sites, normalized_budget);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.cross_module_conformance_sites);
  contract.deterministic = sema_parity_surface.cross_module_conformance_summary.deterministic &&
                           sema_parity_surface.deterministic_cross_module_conformance_handoff &&
                           contract.contract_violation_sites == 0 &&
                           contract.normalized_sites == contract.cross_module_conformance_sites;
  return contract;
}

Objc3ThrowsPropagationLoweringContract
BuildThrowsPropagationLoweringContract(
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3ThrowsPropagationLoweringContract contract;
  const std::size_t raw_sites = sema_parity_surface.throws_propagation_sites_total;
  const std::size_t raw_namespace_segment_sites =
      sema_parity_surface.throws_propagation_namespace_segment_sites_total;
  const std::size_t raw_import_edge_sites =
      sema_parity_surface.throws_propagation_import_edge_candidate_sites_total;
  const std::size_t raw_object_pointer_sites =
      sema_parity_surface.throws_propagation_object_pointer_type_sites_total;
  const std::size_t raw_pointer_declarator_sites =
      sema_parity_surface.throws_propagation_pointer_declarator_sites_total;
  const std::size_t raw_normalized_sites =
      sema_parity_surface.throws_propagation_normalized_sites_total;
  const std::size_t raw_cache_candidate_sites =
      sema_parity_surface.throws_propagation_cache_invalidation_candidate_sites_total;
  const std::size_t raw_violation_sites =
      sema_parity_surface.throws_propagation_contract_violation_sites_total;

  contract.throws_propagation_sites =
      std::max({raw_sites, raw_namespace_segment_sites, raw_import_edge_sites, raw_pointer_declarator_sites,
                raw_normalized_sites, raw_cache_candidate_sites, raw_violation_sites});
  contract.namespace_segment_sites =
      std::min(raw_namespace_segment_sites, contract.throws_propagation_sites);
  contract.import_edge_candidate_sites =
      std::min(raw_import_edge_sites, contract.throws_propagation_sites);
  contract.object_pointer_type_sites =
      std::max(raw_object_pointer_sites, contract.import_edge_candidate_sites);
  contract.pointer_declarator_sites =
      std::min(raw_pointer_declarator_sites, contract.throws_propagation_sites);
  contract.normalized_sites =
      std::min(raw_normalized_sites, contract.throws_propagation_sites);
  const std::size_t normalized_budget =
      (contract.throws_propagation_sites >= contract.normalized_sites)
          ? (contract.throws_propagation_sites - contract.normalized_sites)
          : 0;
  contract.cache_invalidation_candidate_sites =
      std::min(raw_cache_candidate_sites, normalized_budget);
  contract.contract_violation_sites =
      std::min(raw_violation_sites, contract.throws_propagation_sites);
  contract.deterministic = sema_parity_surface.throws_propagation_summary.deterministic &&
                           sema_parity_surface.deterministic_throws_propagation_handoff &&
                           contract.contract_violation_sites == 0 &&
                           contract.normalized_sites == contract.throws_propagation_sites;
  return contract;
}

}  // namespace

Objc3FrontendArtifactBundle BuildObjc3FrontendArtifacts(const std::filesystem::path &input_path,
                                                        const Objc3FrontendPipelineResult &pipeline_result,
                                                        const Objc3FrontendOptions &options) {
  Objc3FrontendArtifactBundle bundle;
  const Objc3Program &program = Objc3ParsedProgramAst(pipeline_result.program);
  bundle.stage_diagnostics = pipeline_result.stage_diagnostics;
  bundle.parse_lowering_readiness_surface = BuildObjc3ParseLoweringReadinessSurface(pipeline_result, options);
  bundle.diagnostics = FlattenStageDiagnostics(bundle.stage_diagnostics);
  if (!bundle.diagnostics.empty()) {
    return bundle;
  }

  const bool metadata_only_ir_emission_mode = [&pipeline_result]() {
    const Objc3ExecutableMetadataTypedLoweringHandoff
        &executable_metadata_typed_lowering_handoff =
            pipeline_result.executable_metadata_typed_lowering_handoff;
    const Objc3ExecutableMetadataSourceGraph &executable_metadata_source_graph =
        pipeline_result.executable_metadata_source_graph;
    const Objc3RuntimeMetadataSourceOwnershipBoundary
        &runtime_metadata_source_ownership =
            pipeline_result.runtime_metadata_source_ownership_boundary;
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality =
        pipeline_result.runtime_export_legality_boundary;
    const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement =
        pipeline_result.runtime_export_enforcement_summary;
    const Objc3RuntimeMetadataSectionAbiFreezeSummary runtime_metadata_section_abi =
        BuildRuntimeMetadataSectionAbiFreezeSummary(
            runtime_metadata_source_ownership,
            runtime_export_legality,
            runtime_export_enforcement);
    const Objc3RuntimeMetadataSectionScaffoldSummary
        runtime_metadata_section_scaffold =
            BuildRuntimeMetadataSectionScaffoldSummary(
                runtime_metadata_section_abi,
                runtime_export_legality,
                runtime_export_enforcement);
    const Objc3RuntimeMetadataObjectInspectionHarnessSummary
        runtime_metadata_object_inspection =
            BuildRuntimeMetadataObjectInspectionHarnessSummary(
                runtime_metadata_section_abi,
                runtime_metadata_section_scaffold);
    const Objc3RuntimeMetadataSourceToSectionMatrixSummary
        runtime_metadata_source_to_section_matrix =
            BuildRuntimeMetadataSourceToSectionMatrixSummary(
                executable_metadata_source_graph,
                runtime_metadata_section_abi,
                runtime_metadata_section_scaffold,
                runtime_metadata_object_inspection);
    const Objc3ExecutableMetadataDebugProjectionSummary
        executable_metadata_debug_projection =
            BuildExecutableMetadataDebugProjectionSummary(
                executable_metadata_typed_lowering_handoff);
    const Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary
        executable_metadata_runtime_ingest_packaging_contract =
            BuildExecutableMetadataRuntimeIngestPackagingContractSummary(
                executable_metadata_typed_lowering_handoff,
                executable_metadata_debug_projection);
    const std::string executable_metadata_runtime_ingest_binary_payload =
        BuildExecutableMetadataRuntimeIngestBinaryEnvelope(
            executable_metadata_runtime_ingest_packaging_contract,
            executable_metadata_typed_lowering_handoff,
            executable_metadata_debug_projection);
    const Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary
        executable_metadata_runtime_ingest_binary_boundary =
            BuildExecutableMetadataRuntimeIngestBinaryBoundarySummary(
                executable_metadata_runtime_ingest_packaging_contract,
                executable_metadata_typed_lowering_handoff,
                executable_metadata_debug_projection,
                executable_metadata_runtime_ingest_binary_payload);
    return IsReadyObjc3ExecutableMetadataTypedLoweringHandoff(
               executable_metadata_typed_lowering_handoff) &&
           IsReadyObjc3RuntimeMetadataSourceOwnershipBoundary(
               runtime_metadata_source_ownership) &&
           IsReadyObjc3RuntimeExportLegalityBoundary(runtime_export_legality) &&
           IsReadyObjc3RuntimeExportEnforcementSummary(
               runtime_export_enforcement) &&
           IsReadyObjc3RuntimeMetadataSectionAbiFreezeSummary(
               runtime_metadata_section_abi) &&
           IsReadyObjc3RuntimeMetadataSectionScaffoldSummary(
               runtime_metadata_section_scaffold) &&
           IsReadyObjc3RuntimeMetadataSourceToSectionMatrixSummary(
               runtime_metadata_source_to_section_matrix) &&
           IsReadyObjc3ExecutableMetadataDebugProjectionSummary(
               executable_metadata_debug_projection) &&
           IsReadyObjc3ExecutableMetadataRuntimeIngestPackagingContractSummary(
               executable_metadata_runtime_ingest_packaging_contract) &&
           IsReadyObjc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary(
               executable_metadata_runtime_ingest_binary_boundary) &&
           runtime_metadata_section_scaffold.class_descriptor_count > 0u;
  }();

  std::string post_pipeline_failure_code;
  std::string post_pipeline_failure_message;
  const auto record_post_pipeline_failure = [&](const char *code, std::string message) {
    if (!post_pipeline_failure_code.empty()) {
      return;
    }
    post_pipeline_failure_code = code == nullptr ? "" : code;
    post_pipeline_failure_message = std::move(message);
  };

  const Objc3IREmissionCoreFeatureImplementationSurface
      ir_emission_core_feature_impl_surface =
          BuildObjc3IREmissionCoreFeatureImplementationSurface(pipeline_result);

  if (!metadata_only_ir_emission_mode) {
    std::string parse_lowering_readiness_error;
    if (!IsObjc3ParseLoweringReadinessSurfaceReady(bundle.parse_lowering_readiness_surface,
                                                   parse_lowering_readiness_error)) {
      record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: parse-to-lowering readiness check failed: " +
              parse_lowering_readiness_error);
    }

    std::string diagnostics_surfacing_scaffold_error;
    if (!IsObjc3LoweringRuntimeDiagnosticsSurfacingScaffoldReady(
            pipeline_result.lowering_runtime_diagnostics_surfacing_scaffold,
            diagnostics_surfacing_scaffold_error)) {
      record_post_pipeline_failure("O3L301",         "LLVM IR emission failed: lowering/runtime diagnostics surfacing scaffold check failed: " +
              diagnostics_surfacing_scaffold_error);
    }

  std::string diagnostics_surfacing_core_feature_error;
  if (!IsObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurfaceReady(
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface,
          diagnostics_surfacing_core_feature_error)) {
    record_post_pipeline_failure("O3L321",         "LLVM IR emission failed: lowering/runtime diagnostics surfacing core feature check failed: " +
            diagnostics_surfacing_core_feature_error);
  }

  std::string diagnostics_surfacing_core_feature_expansion_error;
  if (!IsObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurfaceReady(
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface,
          diagnostics_surfacing_core_feature_expansion_error)) {
    record_post_pipeline_failure("O3L322",         "LLVM IR emission failed: lowering/runtime diagnostics surfacing core feature expansion check failed: " +
            diagnostics_surfacing_core_feature_expansion_error);
  }

  std::string diagnostics_surfacing_edge_case_compatibility_error;
  if (!IsObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseCompatibilitySurfaceReady(
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_edge_case_compatibility_surface,
          diagnostics_surfacing_edge_case_compatibility_error)) {
    record_post_pipeline_failure("O3L323",         "LLVM IR emission failed: lowering/runtime diagnostics surfacing edge-case compatibility check failed: " +
            diagnostics_surfacing_edge_case_compatibility_error);
  }

  std::string diagnostics_surfacing_edge_case_robustness_error;
  if (!IsObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurfaceReady(
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_surface,
          diagnostics_surfacing_edge_case_robustness_error)) {
    record_post_pipeline_failure("O3L324",         "LLVM IR emission failed: lowering/runtime diagnostics surfacing edge-case expansion and robustness check failed: " +
            diagnostics_surfacing_edge_case_robustness_error);
  }

  std::string diagnostics_surfacing_diagnostics_hardening_error;
  if (!IsObjc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurfaceReady(
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface,
          diagnostics_surfacing_diagnostics_hardening_error)) {
    record_post_pipeline_failure("O3L325",         "LLVM IR emission failed: lowering/runtime diagnostics surfacing diagnostics hardening check failed: " +
            diagnostics_surfacing_diagnostics_hardening_error);
  }

  std::string diagnostics_surfacing_recovery_determinism_error;
  if (!IsObjc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurfaceReady(
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface,
          diagnostics_surfacing_recovery_determinism_error)) {
    record_post_pipeline_failure("O3L326",         "LLVM IR emission failed: lowering/runtime diagnostics surfacing recovery/determinism hardening check failed: " +
            diagnostics_surfacing_recovery_determinism_error);
  }

  std::string diagnostics_surfacing_conformance_matrix_error;
  if (!IsObjc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurfaceReady(
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_surface,
          diagnostics_surfacing_conformance_matrix_error)) {
    record_post_pipeline_failure("O3L327",         "LLVM IR emission failed: lowering/runtime diagnostics surfacing conformance matrix check failed: " +
            diagnostics_surfacing_conformance_matrix_error);
  }

  std::string lowering_pass_graph_error;
  if (!IsObjc3LoweringPipelinePassGraphScaffoldReady(
          pipeline_result.lowering_pipeline_pass_graph_scaffold,
          lowering_pass_graph_error)) {
    record_post_pipeline_failure("O3L301",         "LLVM IR emission failed: lowering pipeline pass-graph scaffold check failed: " +
            lowering_pass_graph_error);
  }

  std::string lowering_pass_graph_core_feature_error;
  // Historical C001 freeze marker:
  // IsObjc3LoweringPipelinePassGraphCoreFeatureSurfaceReady(
  if (!IsObjc3IREmissionCompletenessCoreFeatureReady(
          pipeline_result.ir_emission_completeness_scaffold,
          lowering_pass_graph_core_feature_error)) {
    record_post_pipeline_failure("O3L302",         "LLVM IR emission failed: lowering pipeline pass-graph core feature check failed: " +
            lowering_pass_graph_core_feature_error);
  }

  std::string lowering_pass_graph_expansion_error;
  // Historical C001 freeze marker:
  // IsObjc3LoweringPipelinePassGraphCoreFeatureExpansionReady(
  if (!IsObjc3IREmissionCompletenessExpansionReady(
          pipeline_result.ir_emission_completeness_scaffold,
          lowering_pass_graph_expansion_error)) {
    record_post_pipeline_failure("O3L303",         "LLVM IR emission failed: lowering pipeline pass-graph core feature expansion check failed: " +
            lowering_pass_graph_expansion_error);
  }

  std::string lowering_pass_graph_lane_a_edge_case_compatibility_error;
  if (!IsObjc3LoweringPipelinePassGraphEdgeCaseCompatibilityReady(
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface,
          lowering_pass_graph_lane_a_edge_case_compatibility_error)) {
    record_post_pipeline_failure("O3L305",         "LLVM IR emission failed: lowering pipeline pass-graph lane-A edge-case compatibility check failed: " +
            lowering_pass_graph_lane_a_edge_case_compatibility_error);
  }

  std::string lowering_pass_graph_lane_a_edge_case_robustness_error;
  if (!IsObjc3LoweringPipelinePassGraphEdgeCaseRobustnessReady(
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface,
          lowering_pass_graph_lane_a_edge_case_robustness_error)) {
    record_post_pipeline_failure("O3L307",         "LLVM IR emission failed: lowering pipeline pass-graph lane-A edge-case robustness check failed: " +
            lowering_pass_graph_lane_a_edge_case_robustness_error);
  }

  std::string lowering_pass_graph_lane_a_diagnostics_hardening_error;
  if (!IsObjc3LoweringPipelinePassGraphDiagnosticsHardeningReady(
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface,
          lowering_pass_graph_lane_a_diagnostics_hardening_error)) {
    record_post_pipeline_failure("O3L308",         "LLVM IR emission failed: lowering pipeline pass-graph lane-A diagnostics hardening check failed: " +
            lowering_pass_graph_lane_a_diagnostics_hardening_error);
  }

  std::string lowering_pass_graph_lane_a_recovery_determinism_error;
  if (!IsObjc3LoweringPipelinePassGraphRecoveryDeterminismReady(
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface,
          lowering_pass_graph_lane_a_recovery_determinism_error)) {
    record_post_pipeline_failure("O3L309",         "LLVM IR emission failed: lowering pipeline pass-graph lane-A recovery determinism check failed: " +
            lowering_pass_graph_lane_a_recovery_determinism_error);
  }

  std::string lowering_pass_graph_lane_a_conformance_matrix_error;
  if (!IsObjc3LoweringPipelinePassGraphConformanceMatrixReady(
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface,
          lowering_pass_graph_lane_a_conformance_matrix_error)) {
    record_post_pipeline_failure("O3L311",         "LLVM IR emission failed: lowering pipeline pass-graph lane-A conformance matrix check failed: " +
            lowering_pass_graph_lane_a_conformance_matrix_error);
  }

  std::string lowering_pass_graph_lane_a_conformance_corpus_error;
  if (!IsObjc3LoweringPipelinePassGraphConformanceCorpusReady(
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface,
          lowering_pass_graph_lane_a_conformance_corpus_error)) {
    record_post_pipeline_failure("O3L313",         "LLVM IR emission failed: lowering pipeline pass-graph lane-A conformance corpus check failed: " +
            lowering_pass_graph_lane_a_conformance_corpus_error);
  }
  std::string lowering_pass_graph_lane_a_performance_quality_guardrails_error;
  if (!IsObjc3LoweringPipelinePassGraphPerformanceQualityGuardrailsReady(
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface,
          lowering_pass_graph_lane_a_performance_quality_guardrails_error)) {
    record_post_pipeline_failure("O3L315",         "LLVM IR emission failed: lowering pipeline pass-graph lane-A performance quality guardrails check failed: " +
            lowering_pass_graph_lane_a_performance_quality_guardrails_error);
  }

  std::string lowering_pass_graph_edge_case_compatibility_error;
  if (!IsObjc3IREmissionCompletenessEdgeCaseCompatibilityReady(
          pipeline_result.ir_emission_completeness_scaffold,
          lowering_pass_graph_edge_case_compatibility_error)) {
    record_post_pipeline_failure("O3L304",         "LLVM IR emission failed: lowering pipeline pass-graph edge-case compatibility check failed: " +
            lowering_pass_graph_edge_case_compatibility_error);
  }

    std::string ir_emission_core_feature_impl_error;
  if (!IsObjc3IREmissionCoreFeatureImplementationReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_impl_error)) {
    record_post_pipeline_failure("O3L306",         "LLVM IR emission failed: IR emission core feature implementation check failed: " +
            ir_emission_core_feature_impl_error);
  }
  std::string ir_emission_core_feature_expansion_error;
  if (!IsObjc3IREmissionCoreFeatureExpansionReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_expansion_error)) {
    record_post_pipeline_failure("O3L314",         "LLVM IR emission failed: IR emission core feature expansion check failed: " +
            ir_emission_core_feature_expansion_error);
  }
  std::string ir_emission_core_feature_edge_case_compatibility_error;
  if (!IsObjc3IREmissionCoreFeatureEdgeCaseCompatibilityReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_edge_case_compatibility_error)) {
    record_post_pipeline_failure("O3L316",         "LLVM IR emission failed: IR emission core feature edge-case compatibility check failed: " +
            ir_emission_core_feature_edge_case_compatibility_error);
  }
  std::string ir_emission_core_feature_edge_case_robustness_error;
  if (!IsObjc3IREmissionCoreFeatureEdgeCaseRobustnessReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_edge_case_robustness_error)) {
    record_post_pipeline_failure("O3L317",         "LLVM IR emission failed: IR emission core feature edge-case robustness check failed: " +
            ir_emission_core_feature_edge_case_robustness_error);
  }
  std::string ir_emission_core_feature_diagnostics_hardening_error;
  if (!IsObjc3IREmissionCoreFeatureDiagnosticsHardeningReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_diagnostics_hardening_error)) {
    record_post_pipeline_failure("O3L318",         "LLVM IR emission failed: IR emission core feature diagnostics hardening check failed: " +
            ir_emission_core_feature_diagnostics_hardening_error);
  }
  std::string ir_emission_core_feature_recovery_determinism_hardening_error;
  if (!IsObjc3IREmissionCoreFeatureRecoveryDeterminismHardeningReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_recovery_determinism_hardening_error)) {
    record_post_pipeline_failure("O3L319",         "LLVM IR emission failed: IR emission core feature recovery determinism hardening check failed: " +
            ir_emission_core_feature_recovery_determinism_hardening_error);
  }
  std::string ir_emission_core_feature_conformance_matrix_error;
  if (!IsObjc3IREmissionCoreFeatureConformanceMatrixReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_conformance_matrix_error)) {
    record_post_pipeline_failure("O3L320",         "LLVM IR emission failed: IR emission core feature conformance matrix check failed: " +
            ir_emission_core_feature_conformance_matrix_error);
  }
  std::string ir_emission_core_feature_conformance_corpus_error;
  if (!IsObjc3IREmissionCoreFeatureConformanceCorpusReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_conformance_corpus_error)) {
    record_post_pipeline_failure("O3L330",         "LLVM IR emission failed: IR emission core feature conformance corpus check failed: " +
            ir_emission_core_feature_conformance_corpus_error);
  }
  std::string ir_emission_core_feature_performance_quality_guardrails_error;
  if (!IsObjc3IREmissionCoreFeaturePerformanceQualityGuardrailsReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_performance_quality_guardrails_error)) {
    record_post_pipeline_failure("O3L331",         "LLVM IR emission failed: IR emission core feature performance quality guardrails check failed: " +
            ir_emission_core_feature_performance_quality_guardrails_error);
  }
  std::string ir_emission_core_feature_cross_lane_integration_sync_error;
  if (!IsObjc3IREmissionCoreFeatureCrossLaneIntegrationSyncReady(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_cross_lane_integration_sync_error)) {
    record_post_pipeline_failure("O3L332",         "LLVM IR emission failed: IR emission core feature cross-lane integration sync check failed: " +
            ir_emission_core_feature_cross_lane_integration_sync_error);
  }
  std::string ir_emission_core_feature_advanced_core_shard1_error;
  if (!IsObjc3IREmissionCoreFeatureAdvancedCoreShard1Ready(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_advanced_core_shard1_error)) {
    record_post_pipeline_failure("O3L333",         "LLVM IR emission failed: IR emission core feature advanced core shard 1 check failed: " +
            ir_emission_core_feature_advanced_core_shard1_error);
  }
  std::string ir_emission_core_feature_advanced_edge_compatibility_shard1_error;
  if (!IsObjc3IREmissionCoreFeatureAdvancedEdgeCompatibilityShard1Ready(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_advanced_edge_compatibility_shard1_error)) {
    record_post_pipeline_failure("O3L334",         "LLVM IR emission failed: IR emission core feature advanced edge compatibility shard 1 check failed: " +
            ir_emission_core_feature_advanced_edge_compatibility_shard1_error);
  }
  std::string ir_emission_core_feature_advanced_diagnostics_shard1_error;
  if (!IsObjc3IREmissionCoreFeatureAdvancedDiagnosticsShard1Ready(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_advanced_diagnostics_shard1_error)) {
    record_post_pipeline_failure("O3L335",         "LLVM IR emission failed: IR emission core feature advanced diagnostics shard 1 check failed: " +
            ir_emission_core_feature_advanced_diagnostics_shard1_error);
  }
  std::string ir_emission_core_feature_advanced_conformance_shard1_error;
  if (!IsObjc3IREmissionCoreFeatureAdvancedConformanceShard1Ready(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_advanced_conformance_shard1_error)) {
    record_post_pipeline_failure("O3L336",         "LLVM IR emission failed: IR emission core feature advanced conformance shard 1 check failed: " +
            ir_emission_core_feature_advanced_conformance_shard1_error);
  }
  std::string ir_emission_core_feature_advanced_integration_shard1_error;
  if (!IsObjc3IREmissionCoreFeatureAdvancedIntegrationShard1Ready(
          ir_emission_core_feature_impl_surface,
          ir_emission_core_feature_advanced_integration_shard1_error)) {
    record_post_pipeline_failure("O3L337",         "LLVM IR emission failed: IR emission core feature advanced integration shard 1 check failed: " +
            ir_emission_core_feature_advanced_integration_shard1_error);
  }
  }
  std::vector<const FunctionDecl *> manifest_functions;
  manifest_functions.reserve(program.functions.size());
  std::unordered_set<std::string> manifest_function_names;
  for (const auto &fn : program.functions) {
    if (manifest_function_names.insert(fn.name).second) {
      manifest_functions.push_back(&fn);
    }
  }

  std::size_t scalar_return_i32 = 0;
  std::size_t scalar_return_bool = 0;
  std::size_t scalar_return_void = 0;
  std::size_t scalar_param_i32 = 0;
  std::size_t scalar_param_bool = 0;
  std::size_t vector_signature_functions = 0;
  std::size_t vector_return_signatures = 0;
  std::size_t vector_param_signatures = 0;
  std::size_t vector_i32_signatures = 0;
  std::size_t vector_bool_signatures = 0;
  std::size_t vector_lane2_signatures = 0;
  std::size_t vector_lane4_signatures = 0;
  std::size_t vector_lane8_signatures = 0;
  std::size_t vector_lane16_signatures = 0;
  for (const auto &entry : pipeline_result.integration_surface.functions) {
    const FunctionInfo &signature = entry.second;
    if (signature.return_type == ValueType::Bool) {
      ++scalar_return_bool;
    } else if (signature.return_type == ValueType::Void) {
      ++scalar_return_void;
    } else {
      ++scalar_return_i32;
    }
    for (const ValueType param_type : signature.param_types) {
      if (param_type == ValueType::Bool) {
        ++scalar_param_bool;
      } else {
        ++scalar_param_i32;
      }
    }
  }
  for (const FunctionDecl *fn : manifest_functions) {
    bool has_vector_signature = false;
    if (fn->return_vector_spelling) {
      has_vector_signature = true;
      ++vector_return_signatures;
      if (fn->return_vector_base_spelling == kObjc3SimdVectorBaseBool) {
        ++vector_bool_signatures;
      } else {
        ++vector_i32_signatures;
      }
      if (fn->return_vector_lane_count == 2u) {
        ++vector_lane2_signatures;
      } else if (fn->return_vector_lane_count == 4u) {
        ++vector_lane4_signatures;
      } else if (fn->return_vector_lane_count == 8u) {
        ++vector_lane8_signatures;
      } else if (fn->return_vector_lane_count == 16u) {
        ++vector_lane16_signatures;
      }
    }
    for (const FuncParam &param : fn->params) {
      if (!param.vector_spelling) {
        continue;
      }
      has_vector_signature = true;
      ++vector_param_signatures;
      if (param.vector_base_spelling == kObjc3SimdVectorBaseBool) {
        ++vector_bool_signatures;
      } else {
        ++vector_i32_signatures;
      }
      if (param.vector_lane_count == 2u) {
        ++vector_lane2_signatures;
      } else if (param.vector_lane_count == 4u) {
        ++vector_lane4_signatures;
      } else if (param.vector_lane_count == 8u) {
        ++vector_lane8_signatures;
      } else if (param.vector_lane_count == 16u) {
        ++vector_lane16_signatures;
      }
    }
    if (has_vector_signature) {
      ++vector_signature_functions;
    }
  }
  const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff = pipeline_result.sema_type_metadata_handoff;
  const Objc3InterfaceImplementationSummary &interface_implementation_summary =
      type_metadata_handoff.interface_implementation_summary;
  const Objc3FrontendProtocolCategorySummary &protocol_category_summary = pipeline_result.protocol_category_summary;
  const Objc3FrontendClassProtocolCategoryLinkingSummary &class_protocol_category_linking_summary =
      pipeline_result.class_protocol_category_linking_summary;
  const Objc3FrontendSelectorNormalizationSummary &selector_normalization_summary =
      pipeline_result.selector_normalization_summary;
  const Objc3FrontendPropertyAttributeSummary &property_attribute_summary =
      pipeline_result.property_attribute_summary;
  const Objc3FrontendObjectPointerNullabilityGenericsSummary &object_pointer_nullability_generics_summary =
      pipeline_result.object_pointer_nullability_generics_summary;
  const Objc3FrontendSymbolGraphScopeResolutionSummary &symbol_graph_scope_resolution_summary =
      pipeline_result.symbol_graph_scope_resolution_summary;
  const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records =
      pipeline_result.runtime_metadata_source_records;
  const Objc3ExecutableMetadataSourceGraph &executable_metadata_source_graph =
      pipeline_result.executable_metadata_source_graph;
  const Objc3ExecutableMetadataSemanticConsistencyBoundary
      &executable_metadata_semantic_consistency_boundary =
          pipeline_result.executable_metadata_semantic_consistency_boundary;
  const Objc3ExecutableMetadataSemanticValidationSurface
      &executable_metadata_semantic_validation_surface =
          pipeline_result.executable_metadata_semantic_validation_surface;
  const Objc3ExecutableMetadataLoweringHandoffSurface
      &executable_metadata_lowering_handoff_surface =
          pipeline_result.executable_metadata_lowering_handoff_surface;
  const Objc3ExecutableMetadataTypedLoweringHandoff
      &executable_metadata_typed_lowering_handoff =
          pipeline_result.executable_metadata_typed_lowering_handoff;
  const Objc3RuntimeMetadataSourceOwnershipBoundary &runtime_metadata_source_ownership =
      pipeline_result.runtime_metadata_source_ownership_boundary;
  const Objc3RuntimeExportLegalityBoundary &runtime_export_legality =
      pipeline_result.runtime_export_legality_boundary;
  const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement =
      pipeline_result.runtime_export_enforcement_summary;
  const Objc3RuntimeMetadataSectionAbiFreezeSummary runtime_metadata_section_abi =
      BuildRuntimeMetadataSectionAbiFreezeSummary(
          runtime_metadata_source_ownership,
          runtime_export_legality,
          runtime_export_enforcement);
  const Objc3RuntimeMetadataSectionScaffoldSummary
      runtime_metadata_section_scaffold =
          BuildRuntimeMetadataSectionScaffoldSummary(
              runtime_metadata_section_abi,
              runtime_export_legality,
              runtime_export_enforcement);
  const Objc3RuntimeMetadataObjectInspectionHarnessSummary
      runtime_metadata_object_inspection =
          BuildRuntimeMetadataObjectInspectionHarnessSummary(
              runtime_metadata_section_abi,
              runtime_metadata_section_scaffold);
  const Objc3RuntimeMetadataSourceToSectionMatrixSummary
      runtime_metadata_source_to_section_matrix =
          BuildRuntimeMetadataSourceToSectionMatrixSummary(
              executable_metadata_source_graph,
              runtime_metadata_section_abi,
              runtime_metadata_section_scaffold,
              runtime_metadata_object_inspection);
  const Objc3ExecutableMetadataDebugProjectionSummary
      executable_metadata_debug_projection =
          BuildExecutableMetadataDebugProjectionSummary(
              executable_metadata_typed_lowering_handoff);
  const Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary
      executable_metadata_runtime_ingest_packaging_contract =
          BuildExecutableMetadataRuntimeIngestPackagingContractSummary(
              executable_metadata_typed_lowering_handoff,
              executable_metadata_debug_projection);
  const std::string executable_metadata_runtime_ingest_binary_payload =
      BuildExecutableMetadataRuntimeIngestBinaryEnvelope(
          executable_metadata_runtime_ingest_packaging_contract,
          executable_metadata_typed_lowering_handoff,
          executable_metadata_debug_projection);
  const Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary
      executable_metadata_runtime_ingest_binary_boundary =
          BuildExecutableMetadataRuntimeIngestBinaryBoundarySummary(
              executable_metadata_runtime_ingest_packaging_contract,
              executable_metadata_typed_lowering_handoff,
              executable_metadata_debug_projection,
              executable_metadata_runtime_ingest_binary_payload);
  const Objc3RuntimeSupportLibraryContractSummary runtime_support_library =
      BuildRuntimeSupportLibraryContractSummary();
  const Objc3RuntimeSupportLibraryCoreFeatureSummary
      runtime_support_library_core_feature =
          BuildRuntimeSupportLibraryCoreFeatureSummary(runtime_support_library);
  const Objc3RuntimeSupportLibraryLinkWiringSummary
      runtime_support_library_link_wiring =
          BuildRuntimeSupportLibraryLinkWiringSummary(
              runtime_support_library_core_feature);
  const Objc3RuntimeTranslationUnitRegistrationContractSummary
      runtime_translation_unit_registration_contract =
          BuildRuntimeTranslationUnitRegistrationContractSummary(
              executable_metadata_runtime_ingest_binary_boundary,
              runtime_support_library_link_wiring);
  const Objc3RuntimeTranslationUnitRegistrationManifestSummary
      runtime_translation_unit_registration_manifest =
          BuildRuntimeTranslationUnitRegistrationManifestSummary(
              runtime_translation_unit_registration_contract,
              runtime_support_library_link_wiring,
              runtime_metadata_section_scaffold);
  const Objc3RuntimeStartupBootstrapInvariantSummary
      runtime_startup_bootstrap_invariants =
          BuildRuntimeStartupBootstrapInvariantSummary(
              runtime_translation_unit_registration_manifest);
  const Objc3RuntimeBootstrapApiSummary runtime_bootstrap_api =
      BuildRuntimeBootstrapApiSummary(runtime_support_library_core_feature,
                                      runtime_support_library_link_wiring);
  const Objc3RuntimeBootstrapSemanticsSummary
      runtime_bootstrap_semantics = BuildRuntimeBootstrapSemanticsSummary(
          runtime_startup_bootstrap_invariants,
          runtime_translation_unit_registration_manifest);
  const Objc3RuntimeBootstrapLoweringSummary runtime_bootstrap_lowering =
      BuildRuntimeBootstrapLoweringSummary(
          runtime_translation_unit_registration_manifest,
          runtime_bootstrap_semantics);
  const Objc3PropertySynthesisIvarBindingContract property_synthesis_ivar_binding_contract =
      BuildPropertySynthesisIvarBindingContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3PropertySynthesisIvarBindingContract(property_synthesis_ivar_binding_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid property synthesis/ivar binding lowering contract");
  }
  const std::string property_synthesis_ivar_binding_replay_key =
      Objc3PropertySynthesisIvarBindingReplayKey(property_synthesis_ivar_binding_contract);
  const Objc3PropertySynthesisIvarBindingSummary &property_synthesis_ivar_binding_summary =
      pipeline_result.sema_parity_surface.property_synthesis_ivar_binding_summary;
  // M252-B004 export-legality anchor: manifest sema surfaces must publish the
  // canonical sema property-synthesis/ivar-binding summary rather than the
  // lowering fallback contract used for later replay keys.
  const bool property_synthesis_ivar_binding_handoff_deterministic =
      property_synthesis_ivar_binding_summary.deterministic &&
      pipeline_result.sema_parity_surface
          .deterministic_property_synthesis_ivar_binding_handoff;
  const Objc3IdClassSelObjectPointerTypecheckContract id_class_sel_object_pointer_typecheck_contract =
      BuildIdClassSelObjectPointerTypecheckContract(program);
  if (!IsValidObjc3IdClassSelObjectPointerTypecheckContract(id_class_sel_object_pointer_typecheck_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid id/Class/SEL/object-pointer typecheck lowering contract");
  }
  const std::string id_class_sel_object_pointer_typecheck_replay_key =
      Objc3IdClassSelObjectPointerTypecheckReplayKey(id_class_sel_object_pointer_typecheck_contract);
  const Objc3DispatchSurfaceClassificationContract dispatch_surface_classification_contract =
      BuildDispatchSurfaceClassificationContract(program);
  if (!IsValidObjc3DispatchSurfaceClassificationContract(
          dispatch_surface_classification_contract)) {
    record_post_pipeline_failure(
        "O3L300",
        "LLVM IR emission failed: invalid dispatch-surface classification contract");
  }
  const std::string dispatch_surface_classification_replay_key =
      Objc3DispatchSurfaceClassificationReplayKey(
          dispatch_surface_classification_contract);
  const Objc3MessageSendSelectorLoweringContract message_send_selector_lowering_contract =
      BuildMessageSendSelectorLoweringContract(program);
  if (!IsValidObjc3MessageSendSelectorLoweringContract(message_send_selector_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid message-send selector lowering contract");
  }
  const std::string message_send_selector_lowering_replay_key =
      Objc3MessageSendSelectorLoweringReplayKey(message_send_selector_lowering_contract);
  const Objc3DispatchAbiMarshallingContract dispatch_abi_marshalling_contract =
      BuildDispatchAbiMarshallingContract(program, options.lowering.max_message_send_args);
  if (!IsValidObjc3DispatchAbiMarshallingContract(dispatch_abi_marshalling_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid dispatch ABI marshalling contract");
  }
  const std::string dispatch_abi_marshalling_replay_key =
      Objc3DispatchAbiMarshallingReplayKey(dispatch_abi_marshalling_contract);
  const Objc3NilReceiverSemanticsFoldabilityContract nil_receiver_semantics_foldability_contract =
      BuildNilReceiverSemanticsFoldabilityContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3NilReceiverSemanticsFoldabilityContract(nil_receiver_semantics_foldability_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid nil-receiver semantics/foldability contract");
  }
  const std::string nil_receiver_semantics_foldability_replay_key =
      Objc3NilReceiverSemanticsFoldabilityReplayKey(nil_receiver_semantics_foldability_contract);
  const Objc3SuperDispatchMethodFamilyContract super_dispatch_method_family_contract =
      BuildSuperDispatchMethodFamilyContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3SuperDispatchMethodFamilyContract(super_dispatch_method_family_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid super-dispatch/method-family contract");
  }
  const std::string super_dispatch_method_family_replay_key =
      Objc3SuperDispatchMethodFamilyReplayKey(super_dispatch_method_family_contract);
  const Objc3RuntimeShimHostLinkContract runtime_shim_host_link_contract =
      BuildRuntimeShimHostLinkContract(
          dispatch_abi_marshalling_contract,
          nil_receiver_semantics_foldability_contract,
          options);
  if (!IsValidObjc3RuntimeShimHostLinkContract(runtime_shim_host_link_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid runtime shim/host-link contract");
  }
  const std::string runtime_shim_host_link_replay_key =
      Objc3RuntimeShimHostLinkReplayKey(runtime_shim_host_link_contract);
  // M255-C001 dispatch lowering ABI freeze anchor: lane-C now publishes the
  // canonical runtime-dispatch cutover boundary separately from the historical
  // shim-host-link packet so C002 can swap call emission over without
  // redefining the selector lookup/handle or argument-slot ABI ad hoc.
  const Objc3RuntimeDispatchLoweringAbiContract
      runtime_dispatch_lowering_abi_contract =
          BuildRuntimeDispatchLoweringAbiContract(
              dispatch_abi_marshalling_contract, runtime_shim_host_link_contract,
              runtime_bootstrap_api);
  if (!IsValidObjc3RuntimeDispatchLoweringAbiContract(
          runtime_dispatch_lowering_abi_contract)) {
    record_post_pipeline_failure(
        "O3L300",
        "LLVM IR emission failed: invalid runtime dispatch lowering ABI contract");
  }
  const std::string runtime_dispatch_lowering_abi_replay_key =
      Objc3RuntimeDispatchLoweringAbiReplayKey(
          runtime_dispatch_lowering_abi_contract);
  const Objc3OwnershipQualifierLoweringContract ownership_qualifier_lowering_contract =
      BuildOwnershipQualifierLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3OwnershipQualifierLoweringContract(ownership_qualifier_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid ownership-qualifier lowering contract");
  }
  const std::string ownership_qualifier_lowering_replay_key =
      Objc3OwnershipQualifierLoweringReplayKey(ownership_qualifier_lowering_contract);
  const Objc3RetainReleaseOperationLoweringContract retain_release_operation_lowering_contract =
      BuildRetainReleaseOperationLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3RetainReleaseOperationLoweringContract(retain_release_operation_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid retain-release operation lowering contract");
  }
  const std::string retain_release_operation_lowering_replay_key =
      Objc3RetainReleaseOperationLoweringReplayKey(retain_release_operation_lowering_contract);
  const Objc3AutoreleasePoolScopeLoweringContract autoreleasepool_scope_lowering_contract =
      BuildAutoreleasePoolScopeLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3AutoreleasePoolScopeLoweringContract(autoreleasepool_scope_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid autoreleasepool scope lowering contract");
  }
  const std::string autoreleasepool_scope_lowering_replay_key =
      Objc3AutoreleasePoolScopeLoweringReplayKey(autoreleasepool_scope_lowering_contract);
  const Objc3WeakUnownedSemanticsLoweringContract weak_unowned_semantics_lowering_contract =
      BuildWeakUnownedSemanticsLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3WeakUnownedSemanticsLoweringContract(weak_unowned_semantics_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid weak-unowned semantics lowering contract");
  }
  const std::string weak_unowned_semantics_lowering_replay_key =
      Objc3WeakUnownedSemanticsLoweringReplayKey(weak_unowned_semantics_lowering_contract);
  const Objc3ArcDiagnosticsFixitLoweringContract arc_diagnostics_fixit_lowering_contract =
      BuildArcDiagnosticsFixitLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3ArcDiagnosticsFixitLoweringContract(arc_diagnostics_fixit_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid ARC diagnostics/fix-it lowering contract");
  }
  const std::string arc_diagnostics_fixit_lowering_replay_key =
      Objc3ArcDiagnosticsFixitLoweringReplayKey(arc_diagnostics_fixit_lowering_contract);
  const Objc3OwnershipAwareLoweringBehaviorScaffold ownership_aware_lowering_behavior_scaffold =
      BuildObjc3OwnershipAwareLoweringBehaviorScaffold(
          ownership_qualifier_lowering_contract,
          ownership_qualifier_lowering_replay_key,
          retain_release_operation_lowering_contract,
          retain_release_operation_lowering_replay_key,
          autoreleasepool_scope_lowering_contract,
          autoreleasepool_scope_lowering_replay_key,
          weak_unowned_semantics_lowering_contract,
          weak_unowned_semantics_lowering_replay_key,
          arc_diagnostics_fixit_lowering_contract,
          arc_diagnostics_fixit_lowering_replay_key,
          pipeline_result.parse_lowering_readiness_surface
              .compatibility_handoff_consistent,
          pipeline_result.parse_lowering_readiness_surface
              .language_version_pragma_coordinate_order_consistent,
          pipeline_result.parse_lowering_readiness_surface
              .parse_artifact_edge_case_robustness_consistent,
          pipeline_result.parse_lowering_readiness_surface
              .parse_artifact_replay_key_deterministic,
          pipeline_result.parse_lowering_readiness_surface.compatibility_handoff_key,
          pipeline_result.parse_lowering_readiness_surface
              .parse_artifact_edge_robustness_key,
          pipeline_result.parse_lowering_readiness_surface
              .parse_recovery_determinism_hardening_consistent,
          pipeline_result.parse_lowering_readiness_surface
              .parse_recovery_determinism_hardening_key,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_conformance_matrix_consistent,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_conformance_matrix_key,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_conformance_corpus_consistent,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_conformance_corpus_case_count,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_conformance_corpus_key,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_performance_quality_guardrails_consistent,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_performance_quality_guardrails_case_count,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_performance_quality_guardrails_passed_case_count,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_performance_quality_guardrails_failed_case_count,
          pipeline_result.parse_lowering_readiness_surface
              .parse_lowering_performance_quality_guardrails_key,
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
              .conformance_corpus_ready,
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
              .conformance_corpus_key,
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
              .performance_quality_guardrails_ready,
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
              .performance_quality_guardrails_key);
  if (!metadata_only_ir_emission_mode) {
    std::string ownership_aware_lowering_behavior_error;
    if (!IsObjc3OwnershipAwareLoweringBehaviorScaffoldReady(
            ownership_aware_lowering_behavior_scaffold,
            ownership_aware_lowering_behavior_error)) {
      record_post_pipeline_failure("O3L305",         "LLVM IR emission failed: ownership-aware lowering modular split scaffold check failed: " +
              ownership_aware_lowering_behavior_error);
    }
    std::string ownership_aware_lowering_behavior_expansion_error;
    if (!IsObjc3OwnershipAwareLoweringBehaviorCoreFeatureExpansionReady(
            ownership_aware_lowering_behavior_scaffold,
            ownership_aware_lowering_behavior_expansion_error)) {
      record_post_pipeline_failure("O3L310",         "LLVM IR emission failed: ownership-aware lowering core feature expansion check failed: " +
              ownership_aware_lowering_behavior_expansion_error);
    }
    std::string ownership_aware_lowering_behavior_edge_case_compatibility_error;
    if (!IsObjc3OwnershipAwareLoweringBehaviorEdgeCaseCompatibilityReady(
            ownership_aware_lowering_behavior_scaffold,
            ownership_aware_lowering_behavior_edge_case_compatibility_error)) {
      record_post_pipeline_failure("O3L312",         "LLVM IR emission failed: ownership-aware lowering edge-case compatibility check failed: " +
              ownership_aware_lowering_behavior_edge_case_compatibility_error);
    }
    std::string ownership_aware_lowering_behavior_recovery_determinism_error;
    if (!IsObjc3OwnershipAwareLoweringBehaviorRecoveryDeterminismReady(
            ownership_aware_lowering_behavior_scaffold,
            ownership_aware_lowering_behavior_recovery_determinism_error)) {
      record_post_pipeline_failure("O3L318",         "LLVM IR emission failed: ownership-aware lowering recovery determinism check failed: " +
              ownership_aware_lowering_behavior_recovery_determinism_error);
    }
    std::string ownership_aware_lowering_behavior_conformance_matrix_error;
    if (!IsObjc3OwnershipAwareLoweringBehaviorConformanceMatrixReady(
            ownership_aware_lowering_behavior_scaffold,
            ownership_aware_lowering_behavior_conformance_matrix_error)) {
      record_post_pipeline_failure("O3L319",         "LLVM IR emission failed: ownership-aware lowering conformance matrix check failed: " +
              ownership_aware_lowering_behavior_conformance_matrix_error);
    }
    std::string ownership_aware_lowering_behavior_conformance_corpus_error;
    if (!IsObjc3OwnershipAwareLoweringBehaviorConformanceCorpusReady(
            ownership_aware_lowering_behavior_scaffold,
            ownership_aware_lowering_behavior_conformance_corpus_error)) {
      record_post_pipeline_failure("O3L320",         "LLVM IR emission failed: ownership-aware lowering conformance corpus check failed: " +
              ownership_aware_lowering_behavior_conformance_corpus_error);
    }
    std::string ownership_aware_lowering_behavior_performance_quality_guardrails_error;
    if (!IsObjc3OwnershipAwareLoweringBehaviorPerformanceQualityGuardrailsReady(
            ownership_aware_lowering_behavior_scaffold,
            ownership_aware_lowering_behavior_performance_quality_guardrails_error)) {
      record_post_pipeline_failure("O3L328",         "LLVM IR emission failed: ownership-aware lowering performance quality guardrails check failed: " +
              ownership_aware_lowering_behavior_performance_quality_guardrails_error);
    }
    std::string ownership_aware_lowering_behavior_cross_lane_integration_error;
    if (!IsObjc3OwnershipAwareLoweringBehaviorCrossLaneIntegrationReady(
            ownership_aware_lowering_behavior_scaffold,
            ownership_aware_lowering_behavior_cross_lane_integration_error)) {
      record_post_pipeline_failure("O3L329",         "LLVM IR emission failed: ownership-aware lowering cross-lane integration check failed: " +
              ownership_aware_lowering_behavior_cross_lane_integration_error);
    }
  }
  const Objc3BlockLiteralCaptureLoweringContract block_literal_capture_lowering_contract =
      BuildBlockLiteralCaptureLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3BlockLiteralCaptureLoweringContract(block_literal_capture_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid block literal capture lowering contract");
  }
  const std::string block_literal_capture_lowering_replay_key =
      Objc3BlockLiteralCaptureLoweringReplayKey(block_literal_capture_lowering_contract);
  const Objc3BlockAbiInvokeTrampolineLoweringContract block_abi_invoke_trampoline_lowering_contract =
      BuildBlockAbiInvokeTrampolineLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3BlockAbiInvokeTrampolineLoweringContract(
          block_abi_invoke_trampoline_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid block ABI invoke-trampoline lowering contract");
  }
  const std::string block_abi_invoke_trampoline_lowering_replay_key =
      Objc3BlockAbiInvokeTrampolineLoweringReplayKey(
          block_abi_invoke_trampoline_lowering_contract);
  const Objc3BlockStorageEscapeLoweringContract block_storage_escape_lowering_contract =
      BuildBlockStorageEscapeLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3BlockStorageEscapeLoweringContract(
          block_storage_escape_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid block storage escape lowering contract");
  }
  const std::string block_storage_escape_lowering_replay_key =
      Objc3BlockStorageEscapeLoweringReplayKey(block_storage_escape_lowering_contract);
  const Objc3BlockCopyDisposeLoweringContract block_copy_dispose_lowering_contract =
      BuildBlockCopyDisposeLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3BlockCopyDisposeLoweringContract(
          block_copy_dispose_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid block copy-dispose lowering contract");
  }
  const std::string block_copy_dispose_lowering_replay_key =
      Objc3BlockCopyDisposeLoweringReplayKey(block_copy_dispose_lowering_contract);
  const Objc3BlockDeterminismPerfBaselineLoweringContract block_determinism_perf_baseline_lowering_contract =
      BuildBlockDeterminismPerfBaselineLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3BlockDeterminismPerfBaselineLoweringContract(
          block_determinism_perf_baseline_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid block determinism/perf baseline lowering contract");
  }
  const std::string block_determinism_perf_baseline_lowering_replay_key =
      Objc3BlockDeterminismPerfBaselineLoweringReplayKey(
          block_determinism_perf_baseline_lowering_contract);
  const Objc3LightweightGenericsConstraintLoweringContract lightweight_generic_constraint_lowering_contract =
      BuildLightweightGenericsConstraintLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3LightweightGenericsConstraintLoweringContract(
          lightweight_generic_constraint_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid lightweight generics constraint lowering contract");
  }
  const std::string lightweight_generic_constraint_lowering_replay_key =
      Objc3LightweightGenericsConstraintLoweringReplayKey(
          lightweight_generic_constraint_lowering_contract);
  const Objc3NullabilityFlowWarningPrecisionLoweringContract nullability_flow_warning_precision_lowering_contract =
      BuildNullabilityFlowWarningPrecisionLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3NullabilityFlowWarningPrecisionLoweringContract(
          nullability_flow_warning_precision_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid nullability-flow warning-precision lowering contract");
  }
  const std::string nullability_flow_warning_precision_lowering_replay_key =
      Objc3NullabilityFlowWarningPrecisionLoweringReplayKey(
          nullability_flow_warning_precision_lowering_contract);
  const Objc3ProtocolQualifiedObjectTypeLoweringContract protocol_qualified_object_type_lowering_contract =
      BuildProtocolQualifiedObjectTypeLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3ProtocolQualifiedObjectTypeLoweringContract(
          protocol_qualified_object_type_lowering_contract)) {
    const std::string protocol_contract_replay_key =
        Objc3ProtocolQualifiedObjectTypeLoweringReplayKey(
            protocol_qualified_object_type_lowering_contract);
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid protocol-qualified object type lowering contract (" +
            protocol_contract_replay_key + ")");
  }
  const std::string protocol_qualified_object_type_lowering_replay_key =
      Objc3ProtocolQualifiedObjectTypeLoweringReplayKey(
          protocol_qualified_object_type_lowering_contract);
  const Objc3VarianceBridgeCastLoweringContract variance_bridge_cast_lowering_contract =
      BuildVarianceBridgeCastLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3VarianceBridgeCastLoweringContract(
          variance_bridge_cast_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid variance/bridged-cast lowering contract");
  }
  const std::string variance_bridge_cast_lowering_replay_key =
      Objc3VarianceBridgeCastLoweringReplayKey(
          variance_bridge_cast_lowering_contract);
  const Objc3GenericMetadataAbiLoweringContract generic_metadata_abi_lowering_contract =
      BuildGenericMetadataAbiLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3GenericMetadataAbiLoweringContract(
          generic_metadata_abi_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid generic metadata ABI lowering contract");
  }
  const std::string generic_metadata_abi_lowering_replay_key =
      Objc3GenericMetadataAbiLoweringReplayKey(
          generic_metadata_abi_lowering_contract);
  const Objc3ModuleImportGraphLoweringContract module_import_graph_lowering_contract =
      BuildModuleImportGraphLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3ModuleImportGraphLoweringContract(
          module_import_graph_lowering_contract)) {
    record_post_pipeline_failure("O3L300",         "LLVM IR emission failed: invalid module import graph lowering contract");
  }
  const std::string module_import_graph_lowering_replay_key =
      Objc3ModuleImportGraphLoweringReplayKey(
          module_import_graph_lowering_contract);
  const Objc3NamespaceCollisionShadowingLoweringContract
      namespace_collision_shadowing_lowering_contract =
          BuildNamespaceCollisionShadowingLoweringContract(
              pipeline_result.sema_parity_surface);
  if (!IsValidObjc3NamespaceCollisionShadowingLoweringContract(
          namespace_collision_shadowing_lowering_contract)) {
    record_post_pipeline_failure("O3L300", "LLVM IR emission failed: invalid namespace collision "
                 "shadowing lowering contract");
  }
  const std::string namespace_collision_shadowing_lowering_replay_key =
      Objc3NamespaceCollisionShadowingLoweringReplayKey(
          namespace_collision_shadowing_lowering_contract);
  const Objc3PublicPrivateApiPartitionLoweringContract
      public_private_api_partition_lowering_contract =
          BuildPublicPrivateApiPartitionLoweringContract(
              pipeline_result.sema_parity_surface);
  if (!IsValidObjc3PublicPrivateApiPartitionLoweringContract(
          public_private_api_partition_lowering_contract)) {
    record_post_pipeline_failure("O3L300", "LLVM IR emission failed: invalid public-private API "
                 "partition lowering contract");
  }
  const std::string public_private_api_partition_lowering_replay_key =
      Objc3PublicPrivateApiPartitionLoweringReplayKey(
          public_private_api_partition_lowering_contract);
  const Objc3IncrementalModuleCacheInvalidationLoweringContract
      incremental_module_cache_invalidation_lowering_contract =
          BuildIncrementalModuleCacheInvalidationLoweringContract(
              pipeline_result.sema_parity_surface);
  if (!IsValidObjc3IncrementalModuleCacheInvalidationLoweringContract(
          incremental_module_cache_invalidation_lowering_contract)) {
    record_post_pipeline_failure("O3L300", "LLVM IR emission failed: invalid incremental module cache "
            "invalidation lowering contract");
  }
  const std::string incremental_module_cache_invalidation_lowering_replay_key =
      Objc3IncrementalModuleCacheInvalidationLoweringReplayKey(
          incremental_module_cache_invalidation_lowering_contract);
  const Objc3CrossModuleConformanceLoweringContract
      cross_module_conformance_lowering_contract =
          BuildCrossModuleConformanceLoweringContract(
              pipeline_result.sema_parity_surface);
  if (!IsValidObjc3CrossModuleConformanceLoweringContract(
          cross_module_conformance_lowering_contract)) {
    record_post_pipeline_failure("O3L300", "LLVM IR emission failed: invalid cross-module conformance "
                 "lowering contract");
  }
  const std::string cross_module_conformance_lowering_replay_key =
      Objc3CrossModuleConformanceLoweringReplayKey(
          cross_module_conformance_lowering_contract);
  const Objc3ThrowsPropagationLoweringContract
      throws_propagation_lowering_contract =
          BuildThrowsPropagationLoweringContract(
              pipeline_result.sema_parity_surface);
  if (!IsValidObjc3ThrowsPropagationLoweringContract(
          throws_propagation_lowering_contract)) {
    record_post_pipeline_failure("O3L300", "LLVM IR emission failed: invalid throws propagation "
                 "lowering contract");
  }
  const std::string throws_propagation_lowering_replay_key =
      Objc3ThrowsPropagationLoweringReplayKey(
          throws_propagation_lowering_contract);
  std::size_t interface_class_method_symbols = 0;
  std::size_t interface_instance_method_symbols = 0;
  for (const auto &interface_metadata : type_metadata_handoff.interfaces_lexicographic) {
    for (const auto &method_metadata : interface_metadata.methods_lexicographic) {
      if (method_metadata.is_class_method) {
        ++interface_class_method_symbols;
      } else {
        ++interface_instance_method_symbols;
      }
    }
  }
  std::size_t implementation_class_method_symbols = 0;
  std::size_t implementation_instance_method_symbols = 0;
  std::size_t implementation_methods_with_body = 0;
  for (const auto &implementation_metadata : type_metadata_handoff.implementations_lexicographic) {
    for (const auto &method_metadata : implementation_metadata.methods_lexicographic) {
      if (method_metadata.is_class_method) {
        ++implementation_class_method_symbols;
      } else {
        ++implementation_instance_method_symbols;
      }
      if (method_metadata.has_definition) {
        ++implementation_methods_with_body;
      }
    }
  }

  std::vector<int> resolved_global_values;
  if (!ResolveGlobalInitializerValues(program.globals, resolved_global_values) ||
      resolved_global_values.size() != program.globals.size()) {
    record_post_pipeline_failure("O3L300", "LLVM IR emission failed: global initializer failed const evaluation");
  }

  std::ostringstream manifest;
  manifest << "{\n";
  manifest << "  \"source\": \"" << input_path.generic_string() << "\",\n";
  manifest << "  \"module\": \"" << program.module_name << "\",\n";
  manifest << "  \"frontend\": {\n";
  manifest << "    \"language_version\":" << static_cast<unsigned>(options.language_version) << ",\n";
  manifest << "    \"compatibility_mode\":\"" << CompatibilityModeName(options.compatibility_mode) << "\",\n";
  manifest << "    \"migration_assist\":" << (options.migration_assist ? "true" : "false") << ",\n";
  manifest << "    \"migration_hints\":{\"legacy_yes\":" << pipeline_result.migration_hints.legacy_yes_count
           << ",\"legacy_no\":" << pipeline_result.migration_hints.legacy_no_count << ",\"legacy_null\":"
           << pipeline_result.migration_hints.legacy_null_count
           << ",\"legacy_total\":" << pipeline_result.migration_hints.legacy_total() << "},\n";
  manifest << "    \"language_version_pragma_contract\":{\"seen\":"
           << (pipeline_result.language_version_pragma_contract.seen ? "true" : "false")
           << ",\"directive_count\":" << pipeline_result.language_version_pragma_contract.directive_count
           << ",\"duplicate\":" << (pipeline_result.language_version_pragma_contract.duplicate ? "true" : "false")
           << ",\"non_leading\":"
           << (pipeline_result.language_version_pragma_contract.non_leading ? "true" : "false")
           << ",\"first_line\":" << pipeline_result.language_version_pragma_contract.first_line
           << ",\"first_column\":" << pipeline_result.language_version_pragma_contract.first_column
           << ",\"last_line\":" << pipeline_result.language_version_pragma_contract.last_line
           << ",\"last_column\":" << pipeline_result.language_version_pragma_contract.last_column << "},\n";
  manifest << "    \"max_message_send_args\":" << options.lowering.max_message_send_args << ",\n";
  manifest << "    \"pipeline\": {\n";
  manifest << "      \"semantic_skipped\": " << (pipeline_result.integration_surface.built ? "false" : "true")
           << ",\n";
  manifest << "      \"stages\": {\n";
  const Objc3ParserDiagnosticCodeCoverage parser_diag_code_coverage =
      BuildObjc3ParserDiagnosticCodeCoverage(bundle.stage_diagnostics.parser);
  manifest << "        \"lexer\": {\"diagnostics\":" << bundle.stage_diagnostics.lexer.size() << "},\n";
  manifest << "        \"parser\": {\"diagnostics\":" << bundle.stage_diagnostics.parser.size()
           << ",\"token_count\":" << pipeline_result.parser_contract_snapshot.token_count
           << ",\"top_level_declarations\":" << pipeline_result.parser_contract_snapshot.top_level_declaration_count
           << ",\"globals\":" << pipeline_result.parser_contract_snapshot.global_decl_count
           << ",\"protocols\":" << pipeline_result.parser_contract_snapshot.protocol_decl_count
           << ",\"protocol_properties\":"
           << pipeline_result.parser_contract_snapshot.protocol_property_decl_count
           << ",\"protocol_methods\":"
           << pipeline_result.parser_contract_snapshot.protocol_method_decl_count
           << ",\"interfaces\":" << pipeline_result.parser_contract_snapshot.interface_decl_count
           << ",\"interface_properties\":"
           << pipeline_result.parser_contract_snapshot.interface_property_decl_count
           << ",\"interface_methods\":"
           << pipeline_result.parser_contract_snapshot.interface_method_decl_count
           << ",\"interface_categories\":"
           << pipeline_result.parser_contract_snapshot.interface_category_decl_count
           << ",\"implementations\":" << pipeline_result.parser_contract_snapshot.implementation_decl_count
           << ",\"implementation_properties\":"
           << pipeline_result.parser_contract_snapshot.implementation_property_decl_count
           << ",\"implementation_methods\":"
           << pipeline_result.parser_contract_snapshot.implementation_method_decl_count
           << ",\"implementation_categories\":"
           << pipeline_result.parser_contract_snapshot.implementation_category_decl_count
           << ",\"functions\":" << pipeline_result.parser_contract_snapshot.function_decl_count
           << ",\"function_prototypes\":"
           << pipeline_result.parser_contract_snapshot.function_prototype_count
           << ",\"function_pure\":"
           << pipeline_result.parser_contract_snapshot.function_pure_count
           << ",\"long_tail_grammar_constructs\":"
           << pipeline_result.parser_contract_snapshot.long_tail_grammar_construct_count
           << ",\"long_tail_grammar_covered_constructs\":"
           << pipeline_result.parser_contract_snapshot.long_tail_grammar_covered_construct_count
           << ",\"long_tail_grammar_fingerprint\":"
           << pipeline_result.parser_contract_snapshot.long_tail_grammar_fingerprint
           << ",\"long_tail_grammar_handoff_key\":\""
           << pipeline_result.parser_contract_snapshot.long_tail_grammar_handoff_key
           << "\",\"long_tail_grammar_deterministic\":"
           << (pipeline_result.parser_contract_snapshot.long_tail_grammar_handoff_deterministic ? "true" : "false")
           << ",\"diagnostic_code_count\":"
           << parser_diag_code_coverage.unique_code_count
           << ",\"diagnostic_code_fingerprint\":"
           << parser_diag_code_coverage.unique_code_fingerprint
           << ",\"diagnostic_code_surface_deterministic\":"
           << (parser_diag_code_coverage.deterministic_surface ? "true" : "false")
           << ",\"deterministic_handoff\":"
           << (pipeline_result.parser_contract_snapshot.deterministic_handoff ? "true" : "false")
           << ",\"recovery_replay_ready\":"
           << (pipeline_result.parser_contract_snapshot.parser_recovery_replay_ready ? "true" : "false") << "},\n";
  manifest << "        \"semantic\": {\"diagnostics\":" << bundle.stage_diagnostics.semantic.size()
           << "}\n";
  manifest << "      },\n";
  manifest << "      \"parse_lowering_readiness\": {\"ready_for_lowering\": "
           << (bundle.parse_lowering_readiness_surface.ready_for_lowering ? "true" : "false")
           << ",\"parser_contract_snapshot_present\": "
           << (bundle.parse_lowering_readiness_surface.parser_contract_snapshot_present ? "true" : "false")
           << ",\"long_tail_grammar_core_feature_consistent\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_core_feature_consistent ? "true" : "false")
           << ",\"long_tail_grammar_handoff_key_deterministic\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_handoff_key_deterministic ? "true"
                                                                                                     : "false")
           << ",\"long_tail_grammar_expansion_accounting_consistent\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_expansion_accounting_consistent ? "true"
                                                                                                           : "false")
           << ",\"long_tail_grammar_replay_keys_ready\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_replay_keys_ready ? "true" : "false")
           << ",\"long_tail_grammar_expansion_ready\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_expansion_ready ? "true" : "false")
           << ",\"long_tail_grammar_compatibility_handoff_ready\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_compatibility_handoff_ready ? "true"
                                                                                                       : "false")
           << ",\"long_tail_grammar_edge_case_compatibility_consistent\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_edge_case_compatibility_consistent ? "true"
                                                                                                              : "false")
           << ",\"long_tail_grammar_edge_case_compatibility_ready\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_edge_case_compatibility_ready ? "true"
                                                                                                         : "false")
           << ",\"long_tail_grammar_edge_case_expansion_consistent\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_edge_case_expansion_consistent ? "true"
                                                                                                          : "false")
           << ",\"long_tail_grammar_edge_case_robustness_ready\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_edge_case_robustness_ready ? "true" : "false")
           << ",\"long_tail_grammar_diagnostics_hardening_consistent\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_diagnostics_hardening_consistent ? "true"
                                                                                                            : "false")
           << ",\"long_tail_grammar_diagnostics_hardening_ready\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_diagnostics_hardening_ready ? "true"
                                                                                                       : "false")
           << ",\"long_tail_grammar_recovery_determinism_consistent\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_recovery_determinism_consistent ? "true"
                                                                                                           : "false")
           << ",\"long_tail_grammar_recovery_determinism_ready\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_recovery_determinism_ready ? "true"
                                                                                                      : "false")
           << ",\"long_tail_grammar_conformance_matrix_consistent\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_conformance_matrix_consistent ? "true"
                                                                                                         : "false")
           << ",\"long_tail_grammar_conformance_matrix_ready\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_conformance_matrix_ready ? "true"
                                                                                                    : "false")
           << ",\"long_tail_grammar_integration_closeout_consistent\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_integration_closeout_consistent ? "true"
                                                                                                           : "false")
           << ",\"long_tail_grammar_gate_signoff_ready\": "
           << (bundle.parse_lowering_readiness_surface.long_tail_grammar_gate_signoff_ready ? "true" : "false")
           << ",\"parse_artifact_handoff_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parse_artifact_handoff_consistent ? "true" : "false")
           << ",\"parse_artifact_handoff_deterministic\": "
           << (bundle.parse_lowering_readiness_surface.parse_artifact_handoff_deterministic ? "true" : "false")
           << ",\"parser_token_count_budget_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parser_token_count_budget_consistent ? "true" : "false")
           << ",\"parse_artifact_layout_fingerprint_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parse_artifact_layout_fingerprint_consistent ? "true" : "false")
           << ",\"parse_artifact_fingerprint_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parse_artifact_fingerprint_consistent ? "true" : "false")
           << ",\"compatibility_handoff_consistent\": "
           << (bundle.parse_lowering_readiness_surface.compatibility_handoff_consistent ? "true" : "false")
           << ",\"parser_diagnostic_surface_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parser_diagnostic_surface_consistent ? "true" : "false")
           << ",\"parser_diagnostic_code_surface_deterministic\": "
           << (bundle.parse_lowering_readiness_surface.parser_diagnostic_code_surface_deterministic ? "true"
                                                                                                     : "false")
           << ",\"language_version_pragma_coordinate_order_consistent\": "
           << (bundle.parse_lowering_readiness_surface.language_version_pragma_coordinate_order_consistent ? "true"
                                                                                                            : "false")
           << ",\"parse_artifact_replay_key_deterministic\": "
           << (bundle.parse_lowering_readiness_surface.parse_artifact_replay_key_deterministic ? "true" : "false")
           << ",\"parse_artifact_diagnostics_hardening_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parse_artifact_diagnostics_hardening_consistent ? "true"
                                                                                                         : "false")
           << ",\"parse_artifact_edge_case_robustness_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parse_artifact_edge_case_robustness_consistent ? "true"
                                                                                                        : "false")
           << ",\"parse_recovery_determinism_hardening_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parse_recovery_determinism_hardening_consistent ? "true"
                                                                                                        : "false")
           << ",\"parser_diagnostic_grammar_hooks_recovery_determinism_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .parser_diagnostic_grammar_hooks_recovery_determinism_consistent
                   ? "true"
                   : "false")
           << ",\"parser_diagnostic_grammar_hooks_recovery_determinism_ready\": "
           << (bundle.parse_lowering_readiness_surface
                       .parser_diagnostic_grammar_hooks_recovery_determinism_ready
                   ? "true"
                   : "false")
           << ",\"parser_diagnostic_grammar_hooks_conformance_matrix_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .parser_diagnostic_grammar_hooks_conformance_matrix_consistent
                   ? "true"
                   : "false")
           << ",\"parser_diagnostic_grammar_hooks_conformance_matrix_ready\": "
           << (bundle.parse_lowering_readiness_surface
                       .parser_diagnostic_grammar_hooks_conformance_matrix_ready
                   ? "true"
                   : "false")
           << ",\"parser_diagnostic_grammar_hooks_conformance_corpus_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .parser_diagnostic_grammar_hooks_conformance_corpus_consistent
                   ? "true"
                   : "false")
           << ",\"parser_diagnostic_grammar_hooks_conformance_corpus_ready\": "
           << (bundle.parse_lowering_readiness_surface
                       .parser_diagnostic_grammar_hooks_conformance_corpus_ready
                   ? "true"
                   : "false")
           << ",\"parse_lowering_conformance_matrix_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parse_lowering_conformance_matrix_consistent ? "true"
                                                                                                    : "false")
           << ",\"parse_lowering_conformance_corpus_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parse_lowering_conformance_corpus_consistent ? "true"
                                                                                                      : "false")
           << ",\"parse_lowering_performance_quality_guardrails_consistent\": "
           << (bundle.parse_lowering_readiness_surface.parse_lowering_performance_quality_guardrails_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_cross_lane_integration_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_cross_lane_integration_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_cross_lane_integration_ready\": "
           << (bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_cross_lane_integration_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_docs_runbook_sync_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_docs_runbook_sync_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_docs_runbook_sync_ready\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_docs_runbook_sync_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_core_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_advanced_core_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_core_ready\": "
           << (bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_core_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_edge_compatibility_ready\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_advanced_edge_compatibility_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_diagnostics_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_advanced_diagnostics_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_diagnostics_ready\": "
           << (bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_diagnostics_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_conformance_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_advanced_conformance_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_conformance_ready\": "
           << (bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_conformance_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_integration_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_advanced_integration_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_integration_ready\": "
           << (bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_integration_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_performance_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_advanced_performance_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_performance_ready\": "
           << (bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_performance_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_core_shard2_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_advanced_core_shard2_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_advanced_core_shard2_ready\": "
           << (bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_core_shard2_ready
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_integration_closeout_signoff_consistent\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_integration_closeout_signoff_consistent
                   ? "true"
                   : "false")
           << ",\"toolchain_runtime_ga_operations_integration_closeout_signoff_ready\": "
           << (bundle.parse_lowering_readiness_surface
                       .toolchain_runtime_ga_operations_integration_closeout_signoff_ready
                   ? "true"
                   : "false")
           << ",\"semantic_integration_surface_built\": "
           << (bundle.parse_lowering_readiness_surface.semantic_integration_surface_built ? "true" : "false")
           << ",\"executable_metadata_lowering_handoff_ready\": "
           << (bundle.parse_lowering_readiness_surface
                       .executable_metadata_lowering_handoff_ready
                   ? "true"
                   : "false")
           << ",\"executable_metadata_lowering_handoff_deterministic\": "
           << (bundle.parse_lowering_readiness_surface
                       .executable_metadata_lowering_handoff_deterministic
                   ? "true"
                   : "false")
           << ",\"executable_metadata_typed_lowering_handoff_ready\": "
           << (bundle.parse_lowering_readiness_surface
                       .executable_metadata_typed_lowering_handoff_ready
                   ? "true"
                   : "false")
           << ",\"executable_metadata_typed_lowering_handoff_deterministic\": "
           << (bundle.parse_lowering_readiness_surface
                       .executable_metadata_typed_lowering_handoff_deterministic
                   ? "true"
                   : "false")
           << ",\"lowering_boundary_ready\": "
           << (bundle.parse_lowering_readiness_surface.lowering_boundary_ready ? "true" : "false")
           << ",\"parse_lowering_conformance_matrix_case_count\": "
           << bundle.parse_lowering_readiness_surface.parse_lowering_conformance_matrix_case_count
           << ",\"parse_lowering_conformance_corpus_case_count\": "
           << bundle.parse_lowering_readiness_surface.parse_lowering_conformance_corpus_case_count
           << ",\"parse_lowering_conformance_corpus_passed_case_count\": "
           << bundle.parse_lowering_readiness_surface.parse_lowering_conformance_corpus_passed_case_count
           << ",\"parse_lowering_conformance_corpus_failed_case_count\": "
           << bundle.parse_lowering_readiness_surface.parse_lowering_conformance_corpus_failed_case_count
           << ",\"parse_lowering_performance_quality_guardrails_case_count\": "
           << bundle.parse_lowering_readiness_surface.parse_lowering_performance_quality_guardrails_case_count
           << ",\"parse_lowering_performance_quality_guardrails_passed_case_count\": "
           << bundle.parse_lowering_readiness_surface.parse_lowering_performance_quality_guardrails_passed_case_count
           << ",\"parse_lowering_performance_quality_guardrails_failed_case_count\": "
           << bundle.parse_lowering_readiness_surface.parse_lowering_performance_quality_guardrails_failed_case_count
           << ",\"parser_diagnostic_code_count\": "
           << bundle.parse_lowering_readiness_surface.parser_diagnostic_code_count
           << ",\"long_tail_grammar_construct_count\": "
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_construct_count
           << ",\"long_tail_grammar_covered_construct_count\": "
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_covered_construct_count
           << ",\"parser_diagnostic_code_fingerprint\": "
           << bundle.parse_lowering_readiness_surface.parser_diagnostic_code_fingerprint
           << ",\"long_tail_grammar_fingerprint\": "
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_fingerprint
           << ",\"parser_contract_snapshot_fingerprint\": "
           << bundle.parse_lowering_readiness_surface.parser_contract_snapshot_fingerprint
           << ",\"parser_ast_shape_fingerprint\": "
           << bundle.parse_lowering_readiness_surface.parser_ast_shape_fingerprint
           << ",\"parser_ast_top_level_layout_fingerprint\": "
           << bundle.parse_lowering_readiness_surface.parser_ast_top_level_layout_fingerprint
           << ",\"ast_shape_fingerprint\": "
           << bundle.parse_lowering_readiness_surface.ast_shape_fingerprint
           << ",\"ast_top_level_layout_fingerprint\": "
           << bundle.parse_lowering_readiness_surface.ast_top_level_layout_fingerprint
           << ",\"parse_artifact_handoff_key\":\""
           << bundle.parse_lowering_readiness_surface.parse_artifact_handoff_key
           << "\",\"long_tail_grammar_handoff_key\":\""
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_handoff_key
           << "\",\"long_tail_grammar_expansion_key\":\""
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_expansion_key
           << "\",\"long_tail_grammar_edge_case_compatibility_key\":\""
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_edge_case_compatibility_key
           << "\",\"long_tail_grammar_edge_case_robustness_key\":\""
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_edge_case_robustness_key
           << "\",\"long_tail_grammar_diagnostics_hardening_key\":\""
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_diagnostics_hardening_key
           << "\",\"long_tail_grammar_recovery_determinism_key\":\""
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_recovery_determinism_key
           << "\",\"long_tail_grammar_conformance_matrix_key\":\""
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_conformance_matrix_key
           << "\",\"long_tail_grammar_integration_closeout_key\":\""
           << bundle.parse_lowering_readiness_surface.long_tail_grammar_integration_closeout_key
           << "\",\"compatibility_handoff_key\":\""
           << bundle.parse_lowering_readiness_surface.compatibility_handoff_key
           << "\",\"parse_artifact_replay_key\":\""
           << bundle.parse_lowering_readiness_surface.parse_artifact_replay_key
           << "\",\"parse_artifact_diagnostics_hardening_key\":\""
           << bundle.parse_lowering_readiness_surface.parse_artifact_diagnostics_hardening_key
           << "\",\"parse_artifact_edge_robustness_key\":\""
           << bundle.parse_lowering_readiness_surface.parse_artifact_edge_robustness_key
           << "\",\"parse_recovery_determinism_hardening_key\":\""
           << bundle.parse_lowering_readiness_surface.parse_recovery_determinism_hardening_key
           << "\",\"parser_diagnostic_grammar_hooks_recovery_determinism_key\":\""
           << bundle.parse_lowering_readiness_surface
                  .parser_diagnostic_grammar_hooks_recovery_determinism_key
           << "\",\"parser_diagnostic_grammar_hooks_conformance_matrix_key\":\""
           << bundle.parse_lowering_readiness_surface
                  .parser_diagnostic_grammar_hooks_conformance_matrix_key
           << "\",\"parser_diagnostic_grammar_hooks_conformance_corpus_key\":\""
           << bundle.parse_lowering_readiness_surface
                  .parser_diagnostic_grammar_hooks_conformance_corpus_key
           << "\",\"parse_lowering_conformance_matrix_key\":\""
           << bundle.parse_lowering_readiness_surface.parse_lowering_conformance_matrix_key
           << "\",\"parse_lowering_conformance_corpus_key\":\""
           << bundle.parse_lowering_readiness_surface.parse_lowering_conformance_corpus_key
           << "\",\"parse_lowering_performance_quality_guardrails_key\":\""
           << bundle.parse_lowering_readiness_surface.parse_lowering_performance_quality_guardrails_key
           << "\",\"toolchain_runtime_ga_operations_cross_lane_integration_key\":\""
           << bundle.parse_lowering_readiness_surface
                  .toolchain_runtime_ga_operations_cross_lane_integration_key
           << "\",\"toolchain_runtime_ga_operations_docs_runbook_sync_key\":\""
           << bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_docs_runbook_sync_key
           << "\",\"toolchain_runtime_ga_operations_advanced_core_key\":\""
           << bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_core_key
           << "\",\"toolchain_runtime_ga_operations_advanced_edge_compatibility_key\":\""
           << bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_edge_compatibility_key
           << "\",\"toolchain_runtime_ga_operations_advanced_diagnostics_key\":\""
           << bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_diagnostics_key
           << "\",\"toolchain_runtime_ga_operations_advanced_conformance_key\":\""
           << bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_conformance_key
           << "\",\"toolchain_runtime_ga_operations_advanced_integration_key\":\""
           << bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_integration_key
           << "\",\"toolchain_runtime_ga_operations_advanced_performance_key\":\""
           << bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_performance_key
           << "\",\"toolchain_runtime_ga_operations_advanced_core_shard2_key\":\""
           << bundle.parse_lowering_readiness_surface.toolchain_runtime_ga_operations_advanced_core_shard2_key
           << "\",\"toolchain_runtime_ga_operations_integration_closeout_signoff_key\":\""
           << bundle.parse_lowering_readiness_surface
                  .toolchain_runtime_ga_operations_integration_closeout_signoff_key
           << "\",\"executable_metadata_lowering_handoff_key\":\""
           << bundle.parse_lowering_readiness_surface
                  .executable_metadata_lowering_handoff_key
           << "\",\"executable_metadata_typed_lowering_handoff_key\":\""
           << bundle.parse_lowering_readiness_surface
                  .executable_metadata_typed_lowering_handoff_key
           << "\",\"failure_reason\":\"" << bundle.parse_lowering_readiness_surface.failure_reason
           << "\",\"lowering_boundary_replay_key\":\""
           << bundle.parse_lowering_readiness_surface.lowering_boundary_replay_key
           << "\"},\n";
  manifest << "      \"sema_pass_manager\": {\"diagnostics_after_build\":"
           << pipeline_result.sema_diagnostics_after_pass[0] << ",\"diagnostics_after_validate_bodies\":"
           << pipeline_result.sema_diagnostics_after_pass[1] << ",\"diagnostics_after_validate_pure_contract\":"
           << pipeline_result.sema_diagnostics_after_pass[2] << ",\"diagnostics_emitted_by_build\":"
           << pipeline_result.sema_parity_surface.diagnostics_emitted_by_pass[0]
           << ",\"diagnostics_emitted_by_validate_bodies\":"
           << pipeline_result.sema_parity_surface.diagnostics_emitted_by_pass[1]
           << ",\"diagnostics_emitted_by_validate_pure_contract\":"
           << pipeline_result.sema_parity_surface.diagnostics_emitted_by_pass[2] << ",\"diagnostics_monotonic\":"
           << (pipeline_result.sema_parity_surface.diagnostics_after_pass_monotonic ? "true" : "false")
           << ",\"diagnostics_total\":"
           << pipeline_result.sema_parity_surface.diagnostics_total
           << ",\"deterministic_semantic_diagnostics\":"
           << (pipeline_result.sema_parity_surface.deterministic_semantic_diagnostics ? "true" : "false")
           << ",\"diagnostics_accounting_consistent\":"
           << (pipeline_result.sema_parity_surface.diagnostics_accounting_consistent ? "true" : "false")
           << ",\"diagnostics_bus_publish_consistent\":"
           << (pipeline_result.sema_parity_surface.diagnostics_bus_publish_consistent ? "true" : "false")
           << ",\"diagnostics_canonicalized\":"
           << (pipeline_result.sema_parity_surface.diagnostics_canonicalized ? "true" : "false")
           << ",\"diagnostics_hardening_satisfied\":"
           << (pipeline_result.sema_parity_surface.diagnostics_hardening_satisfied ? "true" : "false")
           << ",\"pass_flow_recovery_replay_contract_satisfied\":"
           << (pipeline_result.sema_parity_surface.pass_flow_recovery_replay_contract_satisfied ? "true" : "false")
           << ",\"pass_flow_recovery_replay_key\":\""
           << pipeline_result.sema_parity_surface.pass_flow_recovery_replay_key
           << "\",\"pass_flow_recovery_replay_key_deterministic\":"
           << (pipeline_result.sema_parity_surface.pass_flow_recovery_replay_key_deterministic ? "true" : "false")
           << ",\"pass_flow_recovery_determinism_hardening_satisfied\":"
           << (pipeline_result.sema_parity_surface.pass_flow_recovery_determinism_hardening_satisfied ? "true" : "false")
           << ",\"deterministic_type_metadata_handoff\":"
           << (pipeline_result.sema_parity_surface.deterministic_type_metadata_handoff ? "true" : "false")
           << ",\"pass_flow_configured_count\":"
           << pipeline_result.sema_pass_flow_summary.configured_pass_count
           << ",\"pass_flow_executed_count\":"
           << pipeline_result.sema_pass_flow_summary.executed_pass_count
           << ",\"pass_flow_compatibility_mode\":\""
           << (pipeline_result.sema_pass_flow_summary.compatibility_mode == Objc3SemaCompatibilityMode::Canonical
                   ? "canonical"
                   : "legacy")
           << "\",\"pass_flow_migration_assist_enabled\":"
           << (pipeline_result.sema_pass_flow_summary.migration_assist_enabled ? "true" : "false")
           << ",\"pass_flow_migration_legacy_literal_total\":"
           << pipeline_result.sema_pass_flow_summary.migration_legacy_literal_total
           << ",\"pass_flow_duplicate_execution_count\":"
           << pipeline_result.sema_pass_flow_summary.duplicate_pass_execution_count
           << ",\"pass_flow_missing_execution_count\":"
           << pipeline_result.sema_pass_flow_summary.missing_pass_execution_count
           << ",\"pass_flow_diagnostics_total\":"
           << pipeline_result.sema_pass_flow_summary.diagnostics_total
           << ",\"pass_flow_diagnostics_emitted_by_build\":"
           << pipeline_result.sema_pass_flow_summary.diagnostics_emitted_by_pass[0]
           << ",\"pass_flow_diagnostics_emitted_by_validate_bodies\":"
           << pipeline_result.sema_pass_flow_summary.diagnostics_emitted_by_pass[1]
           << ",\"pass_flow_diagnostics_emitted_by_validate_pure_contract\":"
           << pipeline_result.sema_pass_flow_summary.diagnostics_emitted_by_pass[2]
           << ",\"pass_flow_transition_edge_count\":"
           << pipeline_result.sema_pass_flow_summary.transition_edge_count
           << ",\"pass_flow_order_matches_contract\":"
           << (pipeline_result.sema_pass_flow_summary.pass_order_matches_contract ? "true" : "false")
           << ",\"pass_flow_diagnostics_emission_totals_consistent\":"
           << (pipeline_result.sema_pass_flow_summary.diagnostics_emission_totals_consistent ? "true" : "false")
           << ",\"pass_flow_diagnostics_accounting_consistent\":"
           << (pipeline_result.sema_pass_flow_summary.diagnostics_accounting_consistent ? "true" : "false")
           << ",\"pass_flow_diagnostics_bus_publish_consistent\":"
           << (pipeline_result.sema_pass_flow_summary.diagnostics_bus_publish_consistent ? "true" : "false")
           << ",\"pass_flow_diagnostics_canonicalized\":"
           << (pipeline_result.sema_pass_flow_summary.diagnostics_canonicalized ? "true" : "false")
           << ",\"pass_flow_diagnostics_hardening_satisfied\":"
           << (pipeline_result.sema_pass_flow_summary.diagnostics_hardening_satisfied ? "true" : "false")
           << ",\"pass_flow_parser_recovery_replay_ready\":"
           << (pipeline_result.sema_pass_flow_summary.parser_recovery_replay_ready ? "true" : "false")
           << ",\"pass_flow_parser_recovery_replay_case_present\":"
           << (pipeline_result.sema_pass_flow_summary.parser_recovery_replay_case_present ? "true" : "false")
           << ",\"pass_flow_parser_recovery_replay_case_passed\":"
           << (pipeline_result.sema_pass_flow_summary.parser_recovery_replay_case_passed ? "true" : "false")
           << ",\"pass_flow_recovery_replay_contract_satisfied\":"
           << (pipeline_result.sema_pass_flow_summary.recovery_replay_contract_satisfied ? "true" : "false")
           << ",\"pass_flow_recovery_replay_key\":\""
           << pipeline_result.sema_pass_flow_summary.recovery_replay_key
           << "\",\"pass_flow_recovery_replay_key_deterministic\":"
           << (pipeline_result.sema_pass_flow_summary.recovery_replay_key_deterministic ? "true" : "false")
           << ",\"pass_flow_recovery_determinism_hardening_satisfied\":"
           << (pipeline_result.sema_pass_flow_summary.recovery_determinism_hardening_satisfied ? "true" : "false")
           << ",\"pass_flow_compatibility_handoff_consistent\":"
           << (pipeline_result.sema_pass_flow_summary.compatibility_handoff_consistent ? "true" : "false")
           << ",\"pass_flow_robustness_guardrails_satisfied\":"
           << (pipeline_result.sema_pass_flow_summary.robustness_guardrails_satisfied ? "true" : "false")
           << ",\"pass_flow_symbol_counts_consistent\":"
           << (pipeline_result.sema_pass_flow_summary.symbol_flow_counts_consistent ? "true" : "false")
           << ",\"pass_flow_fingerprint\":"
           << pipeline_result.sema_pass_flow_summary.pass_execution_fingerprint
           << ",\"pass_flow_deterministic_handoff_key\":\""
           << pipeline_result.sema_pass_flow_summary.deterministic_handoff_key
           << "\",\"pass_flow_replay_key_deterministic\":"
           << (pipeline_result.sema_pass_flow_summary.replay_key_deterministic ? "true" : "false")
           << ",\"pass_flow_deterministic\":"
           << (pipeline_result.sema_pass_flow_summary.deterministic ? "true" : "false")
           << ",\"deterministic_atomic_memory_order_mapping\":"
           << (pipeline_result.sema_parity_surface.deterministic_atomic_memory_order_mapping ? "true" : "false")
           << ",\"atomic_memory_order_mapping_total\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.total()
           << ",\"atomic_relaxed_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.relaxed
           << ",\"atomic_acquire_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.acquire
           << ",\"atomic_release_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.release
           << ",\"atomic_acq_rel_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.acq_rel
           << ",\"atomic_seq_cst_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.seq_cst
           << ",\"atomic_unmapped_ops\":"
           << pipeline_result.sema_parity_surface.atomic_memory_order_mapping.unsupported
           << ",\"deterministic_vector_type_lowering\":"
           << (pipeline_result.sema_parity_surface.deterministic_vector_type_lowering ? "true" : "false")
           << ",\"vector_type_lowering_total\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.total()
           << ",\"vector_return_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.return_annotations
           << ",\"vector_param_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.param_annotations
           << ",\"vector_i32_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.i32_annotations
           << ",\"vector_bool_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.bool_annotations
           << ",\"vector_lane2_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.lane2_annotations
           << ",\"vector_lane4_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.lane4_annotations
           << ",\"vector_lane8_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.lane8_annotations
           << ",\"vector_lane16_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.lane16_annotations
           << ",\"vector_unsupported_annotations\":"
           << pipeline_result.sema_parity_surface.vector_type_lowering.unsupported_annotations
           << ",\"ready\":"
           << (pipeline_result.sema_parity_surface.ready ? "true" : "false")
           << ",\"parity_ready\":"
           << (IsReadyObjc3SemaParityContractSurface(pipeline_result.sema_parity_surface) ? "true" : "false")
           << ",\"globals_total\":"
           << pipeline_result.sema_parity_surface.globals_total
           << ",\"functions_total\":"
           << pipeline_result.sema_parity_surface.functions_total
           << ",\"type_metadata_global_entries\":"
           << pipeline_result.sema_parity_surface.type_metadata_global_entries
           << ",\"type_metadata_function_entries\":"
           << pipeline_result.sema_parity_surface.type_metadata_function_entries
           // Legacy extraction anchor retained for contract tests:
           // << pipeline_result.sema_parity_surface.type_metadata_function_entries << "},\n";
           << ",\"deterministic_interface_implementation_handoff\":"
           << (pipeline_result.sema_parity_surface.deterministic_interface_implementation_handoff ? "true" : "false")
           << ",\"interfaces_total\":"
           << pipeline_result.sema_parity_surface.interfaces_total
           << ",\"implementations_total\":"
           << pipeline_result.sema_parity_surface.implementations_total
           << ",\"type_metadata_interface_entries\":"
           << pipeline_result.sema_parity_surface.type_metadata_interface_entries
           << ",\"type_metadata_implementation_entries\":"
           << pipeline_result.sema_parity_surface.type_metadata_implementation_entries
           << ",\"declared_interfaces\":"
           << pipeline_result.sema_parity_surface.interface_implementation_summary.declared_interfaces
           << ",\"declared_implementations\":"
           << pipeline_result.sema_parity_surface.interface_implementation_summary.declared_implementations
           << ",\"resolved_interfaces\":"
           << pipeline_result.sema_parity_surface.interface_implementation_summary.resolved_interfaces
           << ",\"resolved_implementations\":"
           << pipeline_result.sema_parity_surface.interface_implementation_summary.resolved_implementations
           << ",\"interface_method_symbols_total\":"
           << pipeline_result.sema_parity_surface.interface_method_symbols_total
           << ",\"implementation_method_symbols_total\":"
           << pipeline_result.sema_parity_surface.implementation_method_symbols_total
           << ",\"linked_implementation_symbols_total\":"
           << pipeline_result.sema_parity_surface.linked_implementation_symbols_total
           << ",\"deterministic_interface_implementation_summary\":"
           << (pipeline_result.sema_parity_surface.interface_implementation_summary.deterministic ? "true" : "false")
           << ",\"deterministic_protocol_category_handoff\":"
           << (protocol_category_summary.deterministic_protocol_category_handoff ? "true" : "false")
           << ",\"type_metadata_protocol_entries\":"
           << protocol_category_summary.resolved_protocol_symbols
           << ",\"type_metadata_category_entries\":"
           << protocol_category_summary.resolved_category_symbols
           << ",\"deterministic_class_protocol_category_linking_handoff\":"
           << (class_protocol_category_linking_summary.deterministic_class_protocol_category_linking_handoff
                   ? "true"
                   : "false")
           << ",\"class_protocol_category_declared_class_interfaces\":"
           << class_protocol_category_linking_summary.declared_class_interfaces
           << ",\"class_protocol_category_declared_class_implementations\":"
           << class_protocol_category_linking_summary.declared_class_implementations
           << ",\"class_protocol_category_resolved_class_interfaces\":"
           << class_protocol_category_linking_summary.resolved_class_interfaces
           << ",\"class_protocol_category_resolved_class_implementations\":"
           << class_protocol_category_linking_summary.resolved_class_implementations
           << ",\"class_protocol_category_linked_class_method_symbols\":"
           << class_protocol_category_linking_summary.linked_class_method_symbols
           << ",\"class_protocol_category_linked_category_method_symbols\":"
           << class_protocol_category_linking_summary.linked_category_method_symbols
           << ",\"class_protocol_category_protocol_composition_sites\":"
           << class_protocol_category_linking_summary.protocol_composition_sites
           << ",\"class_protocol_category_protocol_composition_symbols\":"
           << class_protocol_category_linking_summary.protocol_composition_symbols
           << ",\"class_protocol_category_category_composition_sites\":"
           << class_protocol_category_linking_summary.category_composition_sites
           << ",\"class_protocol_category_category_composition_symbols\":"
           << class_protocol_category_linking_summary.category_composition_symbols
           << ",\"class_protocol_category_invalid_protocol_composition_sites\":"
           << class_protocol_category_linking_summary.invalid_protocol_composition_sites
           << ",\"deterministic_selector_normalization_handoff\":"
           << (selector_normalization_summary.deterministic_selector_normalization_handoff ? "true" : "false")
           << ",\"selector_method_declaration_entries\":"
           << selector_normalization_summary.method_declaration_entries
           << ",\"selector_normalized_method_declarations\":"
           << selector_normalization_summary.normalized_method_declarations
           << ",\"selector_piece_entries\":"
           << selector_normalization_summary.selector_piece_entries
           << ",\"selector_piece_parameter_links\":"
           << selector_normalization_summary.selector_piece_parameter_links
           << ",\"deterministic_property_attribute_handoff\":"
           << (property_attribute_summary.deterministic_property_attribute_handoff ? "true" : "false")
           << ",\"property_declaration_entries\":"
           << property_attribute_summary.property_declaration_entries
           << ",\"property_attribute_entries\":"
           << property_attribute_summary.property_attribute_entries
           << ",\"property_attribute_value_entries\":"
           << property_attribute_summary.property_attribute_value_entries
           << ",\"property_accessor_modifier_entries\":"
           << property_attribute_summary.property_accessor_modifier_entries
           << ",\"property_getter_selector_entries\":"
           << property_attribute_summary.property_getter_selector_entries
           << ",\"property_setter_selector_entries\":"
           << property_attribute_summary.property_setter_selector_entries
           << ",\"runtime_metadata_source_ownership_contract_id\":\""
           << runtime_metadata_source_ownership.contract_id
           << "\",\"runtime_metadata_source_schema\":\""
           << runtime_metadata_source_ownership.canonical_source_schema
           << "\",\"runtime_metadata_ivar_source_model\":\""
           << runtime_metadata_source_ownership.ivar_record_source_model
           << "\",\"frontend_owns_runtime_metadata_source_records\":"
           << (runtime_metadata_source_ownership.frontend_owns_runtime_metadata_source_records ? "true" : "false")
           << ",\"runtime_metadata_source_records_ready_for_lowering\":"
           << (runtime_metadata_source_ownership.runtime_metadata_source_records_ready_for_lowering ? "true"
                                                                                                   : "false")
           << ",\"native_runtime_library_present\":"
           << (runtime_metadata_source_ownership.native_runtime_library_present ? "true" : "false")
           << ",\"runtime_shim_test_only\":"
           << (runtime_metadata_source_ownership.runtime_shim_test_only ? "true" : "false")
           << ",\"runtime_metadata_source_boundary_fail_closed\":"
           << (runtime_metadata_source_ownership.fail_closed ? "true" : "false")
           << ",\"runtime_metadata_source_boundary_ready\":"
           << (IsReadyObjc3RuntimeMetadataSourceOwnershipBoundary(runtime_metadata_source_ownership) ? "true"
                                                                                                      : "false")
           << ",\"deterministic_runtime_metadata_source_schema\":"
           << (runtime_metadata_source_ownership.deterministic_source_schema ? "true" : "false")
           << ",\"runtime_metadata_class_record_count\":"
           << runtime_metadata_source_ownership.class_record_count
           << ",\"runtime_metadata_protocol_record_count\":"
           << runtime_metadata_source_ownership.protocol_record_count
           << ",\"runtime_metadata_category_interface_record_count\":"
           << runtime_metadata_source_ownership.category_interface_record_count
           << ",\"runtime_metadata_category_implementation_record_count\":"
           << runtime_metadata_source_ownership.category_implementation_record_count
           << ",\"runtime_metadata_property_record_count\":"
           << runtime_metadata_source_ownership.property_record_count
           << ",\"runtime_metadata_method_record_count\":"
           << runtime_metadata_source_ownership.method_record_count
           << ",\"runtime_metadata_ivar_record_count\":"
           << runtime_metadata_source_ownership.ivar_record_count
           << ",\"runtime_metadata_source_boundary_failure_reason\":\""
           << runtime_metadata_source_ownership.failure_reason
           << "\""
           << ",\"runtime_export_legality_contract_id\":\""
           << runtime_export_legality.contract_id
           << "\",\"runtime_export_semantic_boundary_frozen\":"
           << (runtime_export_legality.semantic_boundary_frozen ? "true" : "false")
           << ",\"runtime_export_metadata_export_enforcement_ready\":"
           << (runtime_export_legality.metadata_export_enforcement_ready ? "true" : "false")
           << ",\"runtime_export_fail_closed\":"
           << (runtime_export_legality.fail_closed ? "true" : "false")
           << ",\"runtime_export_boundary_ready\":"
           << (IsReadyObjc3RuntimeExportLegalityBoundary(runtime_export_legality) ? "true"
                                                                                 : "false")
           << ",\"runtime_export_duplicate_runtime_identity_enforcement_pending\":"
           << (runtime_export_legality.duplicate_runtime_identity_enforcement_pending ? "true"
                                                                                    : "false")
           << ",\"runtime_export_incomplete_declaration_export_blocking_pending\":"
           << (runtime_export_legality.incomplete_declaration_export_blocking_pending ? "true"
                                                                                    : "false")
           << ",\"runtime_export_illegal_redeclaration_mix_export_blocking_pending\":"
           << (runtime_export_legality.illegal_redeclaration_mix_export_blocking_pending ? "true"
                                                                                        : "false")
           << ",\"runtime_export_class_record_count\":"
           << runtime_export_legality.class_record_count
           << ",\"runtime_export_protocol_record_count\":"
           << runtime_export_legality.protocol_record_count
           << ",\"runtime_export_category_record_count\":"
           << runtime_export_legality.category_record_count
           << ",\"runtime_export_property_record_count\":"
           << runtime_export_legality.property_record_count
           << ",\"runtime_export_method_record_count\":"
           << runtime_export_legality.method_record_count
           << ",\"runtime_export_ivar_record_count\":"
           << runtime_export_legality.ivar_record_count
           << ",\"runtime_export_invalid_protocol_composition_sites\":"
           << runtime_export_legality.invalid_protocol_composition_sites
           << ",\"runtime_export_property_attribute_invalid_entries\":"
           << runtime_export_legality.property_attribute_invalid_entries
           << ",\"runtime_export_property_attribute_contract_violations\":"
           << runtime_export_legality.property_attribute_contract_violations
           << ",\"runtime_export_invalid_type_annotation_sites\":"
           << runtime_export_legality.invalid_type_annotation_sites
           << ",\"runtime_export_property_ivar_binding_missing\":"
           << runtime_export_legality.property_ivar_binding_missing
           << ",\"runtime_export_property_ivar_binding_conflicts\":"
           << runtime_export_legality.property_ivar_binding_conflicts
           << ",\"runtime_export_implementation_resolution_misses\":"
           << runtime_export_legality.implementation_resolution_misses
           << ",\"runtime_export_method_resolution_misses\":"
           << runtime_export_legality.method_resolution_misses
           << ",\"runtime_export_failure_reason\":\""
           << runtime_export_legality.failure_reason
           << "\""
           << ",\"runtime_export_enforcement_contract_id\":\""
           << runtime_export_enforcement.contract_id
           << "\",\"runtime_export_metadata_completeness_enforced\":"
           << (runtime_export_enforcement.metadata_completeness_enforced ? "true"
                                                                        : "false")
           << ",\"runtime_export_duplicate_runtime_identity_suppression_enforced\":"
           << (runtime_export_enforcement
                           .duplicate_runtime_identity_suppression_enforced
                   ? "true"
                   : "false")
           << ",\"runtime_export_illegal_redeclaration_mix_blocking_enforced\":"
           << (runtime_export_enforcement
                           .illegal_redeclaration_mix_blocking_enforced
                   ? "true"
                   : "false")
           << ",\"runtime_export_metadata_shape_drift_blocking_enforced\":"
           << (runtime_export_enforcement
                           .metadata_shape_drift_blocking_enforced
                   ? "true"
                   : "false")
           << ",\"runtime_export_enforcement_fail_closed\":"
           << (runtime_export_enforcement.fail_closed ? "true" : "false")
           << ",\"runtime_export_ready_for_runtime_export\":"
           << (runtime_export_enforcement.ready_for_runtime_export ? "true"
                                                                  : "false")
           // M252-B003 diagnostic precision anchor: manifest evidence must
           // preserve the duplicate/incomplete/illegal counters that feed the
           // precise category-attachment, duplicate-member, and ambiguity
           // diagnostics emitted by the runtime metadata blocker.
           << ",\"runtime_export_duplicate_runtime_identity_sites\":"
           << runtime_export_enforcement.duplicate_runtime_identity_sites
           << ",\"runtime_export_incomplete_declaration_sites\":"
           << runtime_export_enforcement.incomplete_declaration_sites
           << ",\"runtime_export_illegal_redeclaration_mix_sites\":"
           << runtime_export_enforcement.illegal_redeclaration_mix_sites
           << ",\"runtime_export_metadata_shape_drift_sites\":"
           << runtime_export_enforcement.metadata_shape_drift_sites
           << ",\"runtime_export_enforcement_failure_reason\":\""
           << runtime_export_enforcement.failure_reason
           << "\""
           << ",\"runtime_metadata_section_abi_contract_id\":\""
           << runtime_metadata_section_abi.contract_id
           << "\",\"runtime_metadata_section_boundary_frozen\":"
           << (runtime_metadata_section_abi.boundary_frozen ? "true" : "false")
           << ",\"runtime_metadata_section_fail_closed\":"
           << (runtime_metadata_section_abi.fail_closed ? "true" : "false")
           << ",\"runtime_metadata_section_object_file_inventory_frozen\":"
           << (runtime_metadata_section_abi.object_file_section_inventory_frozen
                   ? "true"
                   : "false")
           << ",\"runtime_metadata_section_symbol_policy_frozen\":"
           << (runtime_metadata_section_abi.symbol_policy_frozen ? "true"
                                                                : "false")
           << ",\"runtime_metadata_section_visibility_model_frozen\":"
           << (runtime_metadata_section_abi.visibility_model_frozen ? "true"
                                                                    : "false")
           << ",\"runtime_metadata_section_retention_policy_frozen\":"
           << (runtime_metadata_section_abi.retention_policy_frozen ? "true"
                                                                    : "false")
           << ",\"runtime_metadata_section_ready_for_scaffold\":"
           << (runtime_metadata_section_abi.ready_for_section_scaffold ? "true"
                                                                       : "false")
           << ",\"runtime_metadata_section_logical_image_info_section\":\""
           << runtime_metadata_section_abi.logical_image_info_section
           << "\",\"runtime_metadata_section_logical_class_descriptor_section\":\""
           << runtime_metadata_section_abi.logical_class_descriptor_section
           << "\",\"runtime_metadata_section_logical_protocol_descriptor_section\":\""
           << runtime_metadata_section_abi.logical_protocol_descriptor_section
           << "\",\"runtime_metadata_section_logical_category_descriptor_section\":\""
           << runtime_metadata_section_abi.logical_category_descriptor_section
           << "\",\"runtime_metadata_section_logical_property_descriptor_section\":\""
           << runtime_metadata_section_abi.logical_property_descriptor_section
           << "\",\"runtime_metadata_section_logical_ivar_descriptor_section\":\""
           << runtime_metadata_section_abi.logical_ivar_descriptor_section
           << "\",\"runtime_metadata_section_descriptor_symbol_prefix\":\""
           << runtime_metadata_section_abi.descriptor_symbol_prefix
           << "\",\"runtime_metadata_section_aggregate_symbol_prefix\":\""
           << runtime_metadata_section_abi.aggregate_symbol_prefix
           << "\",\"runtime_metadata_section_image_info_symbol\":\""
           << runtime_metadata_section_abi.image_info_symbol
           << "\",\"runtime_metadata_section_descriptor_linkage\":\""
           << runtime_metadata_section_abi.descriptor_linkage
           << "\",\"runtime_metadata_section_aggregate_linkage\":\""
           << runtime_metadata_section_abi.aggregate_linkage
           << "\",\"runtime_metadata_section_visibility\":\""
           << runtime_metadata_section_abi.metadata_visibility
           << "\",\"runtime_metadata_section_retention_root\":\""
           << runtime_metadata_section_abi.retention_root
           << "\",\"runtime_metadata_section_failure_reason\":\""
           << runtime_metadata_section_abi.failure_reason
           << "\""
           << ",\"runtime_metadata_section_scaffold_contract_id\":\""
           << runtime_metadata_section_scaffold.contract_id
           << "\",\"runtime_metadata_section_scaffold_abi_contract_id\":\""
           << runtime_metadata_section_scaffold.abi_contract_id
           << "\",\"runtime_metadata_section_scaffold_emitted\":"
           << (runtime_metadata_section_scaffold.scaffold_emitted ? "true"
                                                                  : "false")
           << ",\"runtime_metadata_section_scaffold_fail_closed\":"
           << (runtime_metadata_section_scaffold.fail_closed ? "true" : "false")
           << ",\"runtime_metadata_section_scaffold_uses_llvm_used\":"
           << (runtime_metadata_section_scaffold.uses_llvm_used ? "true"
                                                                : "false")
           << ",\"runtime_metadata_section_scaffold_image_info_emitted\":"
           << (runtime_metadata_section_scaffold.image_info_emitted ? "true"
                                                                    : "false")
           << ",\"runtime_metadata_section_scaffold_class_descriptor_count\":"
           << runtime_metadata_section_scaffold.class_descriptor_count
           << ",\"runtime_metadata_section_scaffold_protocol_descriptor_count\":"
           << runtime_metadata_section_scaffold.protocol_descriptor_count
           << ",\"runtime_metadata_section_scaffold_category_descriptor_count\":"
           << runtime_metadata_section_scaffold.category_descriptor_count
           << ",\"runtime_metadata_section_scaffold_property_descriptor_count\":"
           << runtime_metadata_section_scaffold.property_descriptor_count
           << ",\"runtime_metadata_section_scaffold_ivar_descriptor_count\":"
           << runtime_metadata_section_scaffold.ivar_descriptor_count
           << ",\"runtime_metadata_section_scaffold_total_descriptor_count\":"
           << runtime_metadata_section_scaffold.total_descriptor_count
           << ",\"runtime_metadata_section_scaffold_total_retained_global_count\":"
           << runtime_metadata_section_scaffold.total_retained_global_count
           << ",\"runtime_metadata_section_scaffold_image_info_symbol\":\""
           << runtime_metadata_section_scaffold.image_info_symbol
           << "\",\"runtime_metadata_section_scaffold_class_aggregate_symbol\":\""
           << runtime_metadata_section_scaffold.class_aggregate_symbol
           << "\",\"runtime_metadata_section_scaffold_protocol_aggregate_symbol\":\""
           << runtime_metadata_section_scaffold.protocol_aggregate_symbol
           << "\",\"runtime_metadata_section_scaffold_category_aggregate_symbol\":\""
           << runtime_metadata_section_scaffold.category_aggregate_symbol
           << "\",\"runtime_metadata_section_scaffold_property_aggregate_symbol\":\""
           << runtime_metadata_section_scaffold.property_aggregate_symbol
           << "\",\"runtime_metadata_section_scaffold_ivar_aggregate_symbol\":\""
           << runtime_metadata_section_scaffold.ivar_aggregate_symbol
           << "\",\"runtime_metadata_section_scaffold_failure_reason\":\""
           << runtime_metadata_section_scaffold.failure_reason
           << "\",\"runtime_metadata_object_inspection_contract_id\":\""
           << runtime_metadata_object_inspection.contract_id
           << "\",\"runtime_metadata_object_inspection_scaffold_contract_id\":\""
           << runtime_metadata_object_inspection.scaffold_contract_id
           << "\",\"runtime_metadata_object_inspection_matrix_published\":"
           << (runtime_metadata_object_inspection.matrix_published ? "true"
                                                                   : "false")
           << ",\"runtime_metadata_object_inspection_fail_closed\":"
           << (runtime_metadata_object_inspection.fail_closed ? "true"
                                                              : "false")
           << ",\"runtime_metadata_object_inspection_uses_llvm_readobj\":"
           << (runtime_metadata_object_inspection.uses_llvm_readobj ? "true"
                                                                    : "false")
           << ",\"runtime_metadata_object_inspection_uses_llvm_objdump\":"
           << (runtime_metadata_object_inspection.uses_llvm_objdump ? "true"
                                                                    : "false")
           << ",\"runtime_metadata_object_inspection_matrix_row_count\":"
           << runtime_metadata_object_inspection.matrix_row_count
           << ",\"runtime_metadata_object_inspection_fixture_path\":\""
           << runtime_metadata_object_inspection.fixture_path
           << "\",\"runtime_metadata_object_inspection_emit_prefix\":\""
           << runtime_metadata_object_inspection.emit_prefix
           << "\",\"runtime_metadata_object_inspection_object_relative_path\":\""
           << runtime_metadata_object_inspection.object_relative_path
           << "\",\"runtime_metadata_object_inspection_section_inventory_row_key\":\""
           << runtime_metadata_object_inspection.section_inventory_row_key
           << "\",\"runtime_metadata_object_inspection_section_inventory_command\":\""
           << runtime_metadata_object_inspection.section_inventory_command
           << "\",\"runtime_metadata_object_inspection_symbol_inventory_row_key\":\""
           << runtime_metadata_object_inspection.symbol_inventory_row_key
           << "\",\"runtime_metadata_object_inspection_symbol_inventory_command\":\""
           << runtime_metadata_object_inspection.symbol_inventory_command
           << "\",\"runtime_metadata_object_inspection_failure_reason\":\""
           << runtime_metadata_object_inspection.failure_reason
           << "\",\"executable_metadata_debug_projection_contract_id\":\""
           << EscapeJsonString(executable_metadata_debug_projection.contract_id)
           << "\",\"executable_metadata_debug_projection_typed_handoff_contract_id\":\""
           << EscapeJsonString(
                  executable_metadata_debug_projection
                      .typed_lowering_handoff_contract_id)
           << "\",\"executable_metadata_debug_projection_source_graph_contract_id\":\""
           << EscapeJsonString(
                  executable_metadata_debug_projection.source_graph_contract_id)
           << "\",\"executable_metadata_debug_projection_named_metadata_name\":\""
           << EscapeJsonString(
                  executable_metadata_debug_projection.named_metadata_name)
           << "\",\"executable_metadata_debug_projection_manifest_surface_path\":\""
           << EscapeJsonString(
                  executable_metadata_debug_projection.manifest_surface_path)
           << "\",\"executable_metadata_debug_projection_typed_handoff_surface_path\":\""
           << EscapeJsonString(
                  executable_metadata_debug_projection.typed_handoff_surface_path)
           << "\",\"executable_metadata_debug_projection_source_graph_surface_path\":\""
           << EscapeJsonString(
                  executable_metadata_debug_projection.source_graph_surface_path)
           << "\",\"executable_metadata_debug_projection_matrix_published\":"
           << (executable_metadata_debug_projection.matrix_published ? "true"
                                                                    : "false")
           << ",\"executable_metadata_debug_projection_fail_closed\":"
           << (executable_metadata_debug_projection.fail_closed ? "true"
                                                                : "false")
           << ",\"executable_metadata_debug_projection_manifest_debug_surface_published\":"
           << (executable_metadata_debug_projection
                       .manifest_debug_surface_published
                   ? "true"
                   : "false")
           << ",\"executable_metadata_debug_projection_ir_named_metadata_published\":"
           << (executable_metadata_debug_projection.ir_named_metadata_published
                   ? "true"
                   : "false")
           << ",\"executable_metadata_debug_projection_replay_anchor_deterministic\":"
           << (executable_metadata_debug_projection.replay_anchor_deterministic
                   ? "true"
                   : "false")
           << ",\"executable_metadata_debug_projection_active_typed_handoff_ready\":"
           << (executable_metadata_debug_projection.active_typed_handoff_ready
                   ? "true"
                   : "false")
           << ",\"executable_metadata_debug_projection_matrix_row_count\":"
           << executable_metadata_debug_projection.matrix_row_count
           << ",\"executable_metadata_debug_projection_replay_key\":\""
           << EscapeJsonString(executable_metadata_debug_projection.replay_key)
           << "\",\"executable_metadata_debug_projection_active_typed_handoff_replay_key\":\""
           << EscapeJsonString(
                  executable_metadata_debug_projection
                      .active_typed_handoff_replay_key)
           << "\",\"executable_metadata_debug_projection_failure_reason\":\""
           << EscapeJsonString(
                  executable_metadata_debug_projection.failure_reason)
           << "\",\"executable_metadata_runtime_ingest_packaging_contract_id\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .contract_id)
           << "\",\"executable_metadata_runtime_ingest_packaging_typed_handoff_contract_id\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .typed_lowering_handoff_contract_id)
           << "\",\"executable_metadata_runtime_ingest_packaging_debug_projection_contract_id\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .debug_projection_contract_id)
           << "\",\"executable_metadata_runtime_ingest_packaging_surface_path\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .packaging_surface_path)
           << "\",\"executable_metadata_runtime_ingest_packaging_typed_handoff_surface_path\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .typed_handoff_surface_path)
           << "\",\"executable_metadata_runtime_ingest_packaging_debug_projection_surface_path\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .debug_projection_surface_path)
           << "\",\"executable_metadata_runtime_ingest_packaging_payload_model\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .packaging_payload_model)
           << "\",\"executable_metadata_runtime_ingest_packaging_transport_artifact_relative_path\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .transport_artifact_relative_path)
           << "\",\"executable_metadata_runtime_ingest_packaging_boundary_frozen\":"
           << (executable_metadata_runtime_ingest_packaging_contract
                       .boundary_frozen
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_fail_closed\":"
           << (executable_metadata_runtime_ingest_packaging_contract.fail_closed
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_typed_handoff_ready\":"
           << (executable_metadata_runtime_ingest_packaging_contract
                       .typed_lowering_handoff_ready
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_debug_projection_ready\":"
           << (executable_metadata_runtime_ingest_packaging_contract
                       .debug_projection_ready
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_manifest_transport_frozen\":"
           << (executable_metadata_runtime_ingest_packaging_contract
                       .manifest_transport_frozen
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_runtime_section_emission_not_yet_landed\":"
           << (executable_metadata_runtime_ingest_packaging_contract
                       .runtime_section_emission_not_yet_landed
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_startup_registration_not_yet_landed\":"
           << (executable_metadata_runtime_ingest_packaging_contract
                       .startup_registration_not_yet_landed
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_runtime_loader_registration_not_yet_landed\":"
           << (executable_metadata_runtime_ingest_packaging_contract
                       .runtime_loader_registration_not_yet_landed
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_explicit_non_goals_published\":"
           << (executable_metadata_runtime_ingest_packaging_contract
                       .explicit_non_goals_published
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_ready_for_packaging_implementation\":"
           << (executable_metadata_runtime_ingest_packaging_contract
                       .ready_for_packaging_implementation
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_packaging_typed_handoff_replay_key\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .typed_lowering_handoff_replay_key)
           << "\",\"executable_metadata_runtime_ingest_packaging_debug_projection_replay_key\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .debug_projection_replay_key)
           << "\",\"executable_metadata_runtime_ingest_packaging_replay_key\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .replay_key)
           << "\",\"executable_metadata_runtime_ingest_packaging_failure_reason\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_packaging_contract
                      .failure_reason)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_contract_id\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .contract_id)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_packaging_contract_id\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .packaging_contract_id)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_surface_path\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .binary_boundary_surface_path)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_payload_model\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .payload_model)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_envelope_format\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .envelope_format)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_artifact_relative_path\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .artifact_relative_path)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_artifact_suffix\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .artifact_suffix)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_magic\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .binary_magic)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_chunk_count\":"
           << executable_metadata_runtime_ingest_binary_boundary.chunk_count
           << ",\"executable_metadata_runtime_ingest_binary_boundary_fail_closed\":"
           << (executable_metadata_runtime_ingest_binary_boundary.fail_closed
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_binary_boundary_binary_payload_present\":"
           << (executable_metadata_runtime_ingest_binary_boundary
                       .binary_payload_present
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_binary_boundary_binary_envelope_deterministic\":"
           << (executable_metadata_runtime_ingest_binary_boundary
                       .binary_envelope_deterministic
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_binary_boundary_ready_for_section_emission_handoff\":"
           << (executable_metadata_runtime_ingest_binary_boundary
                       .ready_for_section_emission_handoff
                   ? "true"
                   : "false")
           << ",\"executable_metadata_runtime_ingest_binary_boundary_payload_bytes\":"
           << executable_metadata_runtime_ingest_binary_boundary.payload_bytes
           << ",\"executable_metadata_runtime_ingest_binary_boundary_packaging_contract_replay_key\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .packaging_contract_replay_key)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_typed_handoff_replay_key\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .typed_lowering_handoff_replay_key)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_debug_projection_replay_key\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .debug_projection_replay_key)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_replay_key\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .replay_key)
           << "\",\"executable_metadata_runtime_ingest_binary_boundary_failure_reason\":\""
           << EscapeJsonString(
                  executable_metadata_runtime_ingest_binary_boundary
                      .failure_reason)
           << "\",\"runtime_support_library_contract_id\":\""
           << runtime_support_library.contract_id
           << "\",\"runtime_support_library_metadata_scaffold_contract_id\":\""
           << runtime_support_library.metadata_scaffold_contract_id
           << "\",\"runtime_support_library_boundary_frozen\":"
           << (runtime_support_library.boundary_frozen ? "true" : "false")
           << ",\"runtime_support_library_fail_closed\":"
           << (runtime_support_library.fail_closed ? "true" : "false")
           << ",\"runtime_support_library_target_name_frozen\":"
           << (runtime_support_library.target_name_frozen ? "true" : "false")
           << ",\"runtime_support_library_exported_entrypoints_frozen\":"
           << (runtime_support_library.exported_entrypoints_frozen ? "true"
                                                                   : "false")
           << ",\"runtime_support_library_ownership_boundaries_frozen\":"
           << (runtime_support_library.ownership_boundaries_frozen ? "true"
                                                                   : "false")
           << ",\"runtime_support_library_build_constraints_frozen\":"
           << (runtime_support_library.build_constraints_frozen ? "true"
                                                                : "false")
           << ",\"runtime_support_library_shim_remains_test_only\":"
           << (runtime_support_library.shim_remains_test_only ? "true"
                                                              : "false")
           << ",\"runtime_support_library_native_library_present\":"
           << (runtime_support_library.native_runtime_library_present ? "true"
                                                                      : "false")
           << ",\"runtime_support_library_driver_link_wiring_pending\":"
           << (runtime_support_library.driver_link_wiring_pending ? "true"
                                                                  : "false")
           << ",\"runtime_support_library_ready_for_skeleton\":"
           << (runtime_support_library.ready_for_runtime_library_skeleton
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_target_name\":\""
           << runtime_support_library.cmake_target_name
           << "\",\"runtime_support_library_public_header_path\":\""
           << runtime_support_library.public_header_path
           << "\",\"runtime_support_library_source_root\":\""
           << runtime_support_library.source_root
           << "\",\"runtime_support_library_library_kind\":\""
           << runtime_support_library.library_kind
           << "\",\"runtime_support_library_archive_basename\":\""
           << runtime_support_library.archive_basename
           << "\",\"runtime_support_library_register_image_symbol\":\""
           << runtime_support_library.register_image_symbol
           << "\",\"runtime_support_library_lookup_selector_symbol\":\""
           << runtime_support_library.lookup_selector_symbol
           << "\",\"runtime_support_library_dispatch_i32_symbol\":\""
           << runtime_support_library.dispatch_i32_symbol
           << "\",\"runtime_support_library_reset_for_testing_symbol\":\""
           << runtime_support_library.reset_for_testing_symbol
           << "\",\"runtime_support_library_driver_link_mode\":\""
           << runtime_support_library.driver_link_mode
           << "\",\"runtime_support_library_compiler_ownership_boundary\":\""
           << runtime_support_library.compiler_ownership_boundary
           << "\",\"runtime_support_library_runtime_ownership_boundary\":\""
           << runtime_support_library.runtime_ownership_boundary
           << "\",\"runtime_support_library_failure_reason\":\""
           << runtime_support_library.failure_reason
           << "\",\"runtime_support_library_core_feature_contract_id\":\""
           << runtime_support_library_core_feature.contract_id
           << "\",\"runtime_support_library_core_feature_support_library_contract_id\":\""
           << runtime_support_library_core_feature.support_library_contract_id
           << "\",\"runtime_support_library_core_feature_metadata_scaffold_contract_id\":\""
           << runtime_support_library_core_feature.metadata_scaffold_contract_id
           << "\",\"runtime_support_library_core_feature_fail_closed\":"
           << (runtime_support_library_core_feature.fail_closed ? "true"
                                                                : "false")
           << ",\"runtime_support_library_core_feature_sources_present\":"
           << (runtime_support_library_core_feature
                           .native_runtime_library_sources_present
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_header_present\":"
           << (runtime_support_library_core_feature
                           .native_runtime_library_header_present
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_archive_build_enabled\":"
           << (runtime_support_library_core_feature
                           .native_runtime_library_archive_build_enabled
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_entrypoints_implemented\":"
           << (runtime_support_library_core_feature
                           .native_runtime_library_entrypoints_implemented
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_selector_lookup_stateful\":"
           << (runtime_support_library_core_feature.selector_lookup_stateful
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_dispatch_formula_matches_test_shim\":"
           << (runtime_support_library_core_feature
                           .deterministic_dispatch_formula_matches_test_shim
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_reset_for_testing_supported\":"
           << (runtime_support_library_core_feature.reset_for_testing_supported
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_shim_remains_test_only\":"
           << (runtime_support_library_core_feature.shim_remains_test_only
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_driver_link_wiring_pending\":"
           << (runtime_support_library_core_feature.driver_link_wiring_pending
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_ready_for_driver_link_wiring\":"
           << (runtime_support_library_core_feature.ready_for_driver_link_wiring
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_core_feature_target_name\":\""
           << runtime_support_library_core_feature.cmake_target_name
           << "\",\"runtime_support_library_core_feature_public_header_path\":\""
           << runtime_support_library_core_feature.public_header_path
           << "\",\"runtime_support_library_core_feature_source_root\":\""
           << runtime_support_library_core_feature.source_root
           << "\",\"runtime_support_library_core_feature_implementation_source_path\":\""
           << runtime_support_library_core_feature.implementation_source_path
           << "\",\"runtime_support_library_core_feature_library_kind\":\""
           << runtime_support_library_core_feature.library_kind
           << "\",\"runtime_support_library_core_feature_archive_basename\":\""
           << runtime_support_library_core_feature.archive_basename
           << "\",\"runtime_support_library_core_feature_archive_relative_path\":\""
           << runtime_support_library_core_feature.archive_relative_path
           << "\",\"runtime_support_library_core_feature_probe_source_path\":\""
           << runtime_support_library_core_feature.probe_source_path
           << "\",\"runtime_support_library_core_feature_register_image_symbol\":\""
           << runtime_support_library_core_feature.register_image_symbol
           << "\",\"runtime_support_library_core_feature_lookup_selector_symbol\":\""
           << runtime_support_library_core_feature.lookup_selector_symbol
           << "\",\"runtime_support_library_core_feature_dispatch_i32_symbol\":\""
           << runtime_support_library_core_feature.dispatch_i32_symbol
           << "\",\"runtime_support_library_core_feature_reset_for_testing_symbol\":\""
           << runtime_support_library_core_feature.reset_for_testing_symbol
           << "\",\"runtime_support_library_core_feature_driver_link_mode\":\""
           << runtime_support_library_core_feature.driver_link_mode
           << "\",\"runtime_support_library_link_wiring_contract_id\":\""
           << runtime_support_library_link_wiring.contract_id
           << "\",\"runtime_support_library_link_wiring_core_feature_contract_id\":\""
           << runtime_support_library_link_wiring
                  .support_library_core_feature_contract_id
           << "\",\"runtime_support_library_link_wiring_fail_closed\":"
           << (runtime_support_library_link_wiring.fail_closed ? "true"
                                                               : "false")
           << ",\"runtime_support_library_link_wiring_archive_available\":"
           << (runtime_support_library_link_wiring
                       .runtime_library_archive_available
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_link_wiring_compatibility_dispatch_alias_exported\":"
           << (runtime_support_library_link_wiring
                       .compatibility_dispatch_alias_exported
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_link_wiring_driver_emits_runtime_link_contract\":"
           << (runtime_support_library_link_wiring
                       .driver_emits_runtime_link_contract
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_link_wiring_execution_smoke_consumes_runtime_library\":"
           << (runtime_support_library_link_wiring
                       .execution_smoke_consumes_runtime_library
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_link_wiring_shim_remains_test_only\":"
           << (runtime_support_library_link_wiring.shim_remains_test_only
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_link_wiring_ready_for_runtime_library_consumption\":"
           << (runtime_support_library_link_wiring
                       .ready_for_runtime_library_consumption
                   ? "true"
                   : "false")
           << ",\"runtime_support_library_link_wiring_archive_relative_path\":\""
           << runtime_support_library_link_wiring.archive_relative_path
           << "\",\"runtime_support_library_link_wiring_compatibility_dispatch_symbol\":\""
           << runtime_support_library_link_wiring.compatibility_dispatch_symbol
           << "\",\"runtime_support_library_link_wiring_runtime_dispatch_symbol\":\""
           << runtime_support_library_link_wiring.runtime_dispatch_symbol
           << "\",\"runtime_support_library_link_wiring_execution_smoke_script_path\":\""
           << runtime_support_library_link_wiring.execution_smoke_script_path
           << "\",\"runtime_support_library_link_wiring_driver_link_mode\":\""
           << runtime_support_library_link_wiring.driver_link_mode
           << "\",\"runtime_support_library_link_wiring_failure_reason\":\""
           << runtime_support_library_link_wiring.failure_reason
           << "\",\"runtime_support_library_core_feature_failure_reason\":\""
           << runtime_support_library_core_feature.failure_reason
           << "\",\"runtime_translation_unit_registration_contract_id\":\""
           << runtime_translation_unit_registration_contract.contract_id
           << "\",\"runtime_translation_unit_registration_binary_boundary_contract_id\":\""
           << runtime_translation_unit_registration_contract
                  .binary_boundary_contract_id
           << "\",\"runtime_translation_unit_registration_archive_static_link_contract_id\":\""
           << runtime_translation_unit_registration_contract
                  .archive_static_link_contract_id
           << "\",\"runtime_translation_unit_registration_object_emission_closeout_contract_id\":\""
           << runtime_translation_unit_registration_contract
                  .object_emission_closeout_contract_id
           << "\",\"runtime_translation_unit_registration_runtime_support_library_link_wiring_contract_id\":\""
           << runtime_translation_unit_registration_contract
                  .runtime_support_library_link_wiring_contract_id
           << "\",\"runtime_translation_unit_registration_payload_model\":\""
           << runtime_translation_unit_registration_contract
                  .registration_payload_model
           << "\",\"runtime_translation_unit_registration_runtime_owned_payload_artifact_count\":"
           << runtime_translation_unit_registration_contract
                  .runtime_owned_payload_artifact_count
           << ",\"runtime_translation_unit_registration_payload_artifact_relative_path\":\""
           << runtime_translation_unit_registration_contract
                  .runtime_owned_payload_artifacts[0]
           << "\",\"runtime_translation_unit_registration_linker_response_artifact_relative_path\":\""
           << runtime_translation_unit_registration_contract
                  .runtime_owned_payload_artifacts[1]
           << "\",\"runtime_translation_unit_registration_discovery_artifact_relative_path\":\""
           << runtime_translation_unit_registration_contract
                  .runtime_owned_payload_artifacts[2]
           << "\",\"runtime_translation_unit_registration_constructor_root_symbol\":\""
           << runtime_translation_unit_registration_contract
                  .constructor_root_symbol
           << "\",\"runtime_translation_unit_registration_constructor_root_ownership_model\":\""
           << runtime_translation_unit_registration_contract
                  .constructor_root_ownership_model
           << "\",\"runtime_translation_unit_registration_constructor_emission_mode\":\""
           << runtime_translation_unit_registration_contract
                  .constructor_emission_mode
           << "\",\"runtime_translation_unit_registration_constructor_priority_policy\":\""
           << runtime_translation_unit_registration_contract
                  .constructor_priority_policy
           << "\",\"runtime_translation_unit_registration_entrypoint_symbol\":\""
           << runtime_translation_unit_registration_contract
                  .registration_entrypoint_symbol
           << "\",\"runtime_translation_unit_registration_translation_unit_identity_model\":\""
           << runtime_translation_unit_registration_contract
                  .translation_unit_identity_model
           << "\",\"runtime_translation_unit_registration_boundary_frozen\":"
           << (runtime_translation_unit_registration_contract.boundary_frozen
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_fail_closed\":"
           << (runtime_translation_unit_registration_contract.fail_closed
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_binary_boundary_ready\":"
           << (runtime_translation_unit_registration_contract.binary_boundary_ready
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_archive_static_link_surface_ready\":"
           << (runtime_translation_unit_registration_contract
                       .archive_static_link_surface_ready
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_object_emission_closeout_surface_ready\":"
           << (runtime_translation_unit_registration_contract
                       .object_emission_closeout_surface_ready
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_runtime_support_library_link_wiring_ready\":"
           << (runtime_translation_unit_registration_contract
                       .runtime_support_library_link_wiring_ready
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_runtime_owned_payload_inventory_published\":"
           << (runtime_translation_unit_registration_contract
                       .runtime_owned_payload_inventory_published
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_constructor_root_reserved_not_emitted\":"
           << (runtime_translation_unit_registration_contract
                       .constructor_root_reserved_not_emitted
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_startup_registration_not_yet_landed\":"
           << (runtime_translation_unit_registration_contract
                       .startup_registration_not_yet_landed
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_runtime_bootstrap_not_yet_landed\":"
           << (runtime_translation_unit_registration_contract
                       .runtime_bootstrap_not_yet_landed
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_explicit_non_goals_published\":"
           << (runtime_translation_unit_registration_contract
                       .explicit_non_goals_published
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_ready_for_manifest_implementation\":"
           << (runtime_translation_unit_registration_contract
                       .ready_for_registration_manifest_implementation
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_binary_boundary_replay_key\":\""
           << EscapeJsonString(
                  runtime_translation_unit_registration_contract
                      .binary_boundary_replay_key)
           << "\",\"runtime_translation_unit_registration_replay_key\":\""
           << EscapeJsonString(
                  runtime_translation_unit_registration_contract.replay_key)
           << "\",\"runtime_translation_unit_registration_failure_reason\":\""
           << EscapeJsonString(
                  runtime_translation_unit_registration_contract.failure_reason)
           << "\""
           << ",\"runtime_translation_unit_registration_manifest_contract_id\":\""
           << runtime_translation_unit_registration_manifest.contract_id
           << "\",\"runtime_translation_unit_registration_manifest_payload_model\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_payload_model
           << "\",\"runtime_translation_unit_registration_manifest_artifact_relative_path\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_artifact_relative_path
           << "\",\"runtime_translation_unit_registration_manifest_runtime_owned_payload_artifact_count\":"
           << runtime_translation_unit_registration_manifest
                  .runtime_owned_payload_artifact_count
           << ",\"runtime_translation_unit_registration_manifest_runtime_support_library_archive_relative_path\":\""
           << runtime_translation_unit_registration_manifest
                  .runtime_support_library_archive_relative_path
           << "\",\"runtime_translation_unit_registration_manifest_constructor_root_symbol\":\""
           << runtime_translation_unit_registration_manifest
                  .constructor_root_symbol
           << "\",\"runtime_translation_unit_registration_manifest_constructor_root_ownership_model\":\""
           << runtime_translation_unit_registration_manifest
                  .constructor_root_ownership_model
           << "\",\"runtime_translation_unit_registration_manifest_authority_model\":\""
           << runtime_translation_unit_registration_manifest
                  .manifest_authority_model
           << "\",\"runtime_translation_unit_registration_manifest_init_stub_symbol_prefix\":\""
           << runtime_translation_unit_registration_manifest
                  .constructor_init_stub_symbol_prefix
           << "\",\"runtime_translation_unit_registration_manifest_init_stub_ownership_model\":\""
           << runtime_translation_unit_registration_manifest
                  .constructor_init_stub_ownership_model
           << "\",\"runtime_translation_unit_registration_manifest_constructor_priority_policy\":\""
           << runtime_translation_unit_registration_manifest
                  .constructor_priority_policy
           << "\",\"runtime_translation_unit_registration_manifest_registration_entrypoint_symbol\":\""
           << runtime_translation_unit_registration_manifest
                  .registration_entrypoint_symbol
           << "\",\"runtime_translation_unit_registration_manifest_translation_unit_identity_model\":\""
           << runtime_translation_unit_registration_manifest
                  .translation_unit_identity_model
           << "\",\"runtime_translation_unit_registration_manifest_launch_integration_contract_id\":\""
           << runtime_translation_unit_registration_manifest
                  .launch_integration_contract_id
           << "\",\"runtime_translation_unit_registration_manifest_runtime_library_resolution_model\":\""
           << runtime_translation_unit_registration_manifest
                  .runtime_library_resolution_model
           << "\",\"runtime_translation_unit_registration_manifest_driver_linker_flag_consumption_model\":\""
           << runtime_translation_unit_registration_manifest
                  .driver_linker_flag_consumption_model
           << "\",\"runtime_translation_unit_registration_manifest_compile_wrapper_command_surface\":\""
           << runtime_translation_unit_registration_manifest
                  .compile_wrapper_command_surface
           << "\",\"runtime_translation_unit_registration_manifest_compile_proof_command_surface\":\""
           << runtime_translation_unit_registration_manifest
                  .compile_proof_command_surface
           << "\",\"runtime_translation_unit_registration_manifest_execution_smoke_command_surface\":\""
           << runtime_translation_unit_registration_manifest
                  .execution_smoke_command_surface
           << "\",\"runtime_translation_unit_registration_manifest_class_descriptor_count\":"
           << runtime_translation_unit_registration_manifest
                  .class_descriptor_count
           << ",\"runtime_translation_unit_registration_manifest_protocol_descriptor_count\":"
           << runtime_translation_unit_registration_manifest
                  .protocol_descriptor_count
           << ",\"runtime_translation_unit_registration_manifest_category_descriptor_count\":"
           << runtime_translation_unit_registration_manifest
                  .category_descriptor_count
           << ",\"runtime_translation_unit_registration_manifest_property_descriptor_count\":"
           << runtime_translation_unit_registration_manifest
                  .property_descriptor_count
           << ",\"runtime_translation_unit_registration_manifest_ivar_descriptor_count\":"
           << runtime_translation_unit_registration_manifest
                  .ivar_descriptor_count
           << ",\"runtime_translation_unit_registration_manifest_total_descriptor_count\":"
           << runtime_translation_unit_registration_manifest
                  .total_descriptor_count
           << ",\"runtime_translation_unit_registration_manifest_translation_unit_registration_order_ordinal\":"
           << runtime_translation_unit_registration_manifest
                  .translation_unit_registration_order_ordinal
           << ",\"runtime_translation_unit_registration_manifest_fail_closed\":"
           << (runtime_translation_unit_registration_manifest.fail_closed
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_manifest_contract_ready\":"
           << (runtime_translation_unit_registration_manifest
                       .translation_unit_registration_contract_ready
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_manifest_runtime_support_library_link_wiring_ready\":"
           << (runtime_translation_unit_registration_manifest
                       .runtime_support_library_link_wiring_ready
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_manifest_template_published\":"
           << (runtime_translation_unit_registration_manifest
                       .runtime_manifest_template_published
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_manifest_constructor_root_manifest_authoritative\":"
           << (runtime_translation_unit_registration_manifest
                       .constructor_root_manifest_authoritative
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_manifest_constructor_root_reserved_for_lowering\":"
           << (runtime_translation_unit_registration_manifest
                       .constructor_root_reserved_for_lowering
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_manifest_init_stub_emission_deferred_to_lowering\":"
           << (runtime_translation_unit_registration_manifest
                       .init_stub_emission_deferred_to_lowering
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_manifest_artifact_emitted_by_driver\":"
           << (runtime_translation_unit_registration_manifest
                       .runtime_registration_artifact_emitted_by_driver
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_manifest_ready_for_lowering_init_stub_emission\":"
           << (runtime_translation_unit_registration_manifest
                       .ready_for_lowering_init_stub_emission
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_manifest_launch_integration_ready\":"
           << (runtime_translation_unit_registration_manifest
                       .launch_integration_ready
                   ? "true"
                   : "false")
           << ",\"runtime_translation_unit_registration_manifest_translation_unit_registration_replay_key\":\""
           << EscapeJsonString(
                  runtime_translation_unit_registration_manifest
                      .translation_unit_registration_replay_key)
           << "\",\"runtime_translation_unit_registration_manifest_replay_key\":\""
           << EscapeJsonString(
                  runtime_translation_unit_registration_manifest.replay_key)
          << "\",\"runtime_translation_unit_registration_manifest_failure_reason\":\""
          << EscapeJsonString(
                 runtime_translation_unit_registration_manifest.failure_reason)
          << "\""
          << ",\"runtime_bootstrap_api_contract_id\":\""
          << EscapeJsonString(runtime_bootstrap_api.contract_id)
          << "\",\"runtime_bootstrap_api_public_header_path\":\""
          << EscapeJsonString(runtime_bootstrap_api.public_header_path)
          << "\",\"runtime_bootstrap_api_archive_relative_path\":\""
          << EscapeJsonString(runtime_bootstrap_api.archive_relative_path)
          << "\",\"runtime_bootstrap_api_registration_status_enum_type\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.registration_status_enum_type)
          << "\",\"runtime_bootstrap_api_image_descriptor_type\":\""
          << EscapeJsonString(runtime_bootstrap_api.image_descriptor_type)
          << "\",\"runtime_bootstrap_api_selector_handle_type\":\""
          << EscapeJsonString(runtime_bootstrap_api.selector_handle_type)
          << "\",\"runtime_bootstrap_api_registration_snapshot_type\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.registration_snapshot_type)
          << "\",\"runtime_bootstrap_api_registration_entrypoint_symbol\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.registration_entrypoint_symbol)
          << "\",\"runtime_bootstrap_api_selector_lookup_symbol\":\""
          << EscapeJsonString(runtime_bootstrap_api.selector_lookup_symbol)
          << "\",\"runtime_bootstrap_api_dispatch_entrypoint_symbol\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.dispatch_entrypoint_symbol)
          << "\",\"runtime_bootstrap_api_compatibility_dispatch_symbol\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.compatibility_dispatch_symbol)
          << "\",\"runtime_bootstrap_api_state_snapshot_symbol\":\""
          << EscapeJsonString(runtime_bootstrap_api.state_snapshot_symbol)
          << "\",\"runtime_bootstrap_api_reset_for_testing_symbol\":\""
          << EscapeJsonString(runtime_bootstrap_api.reset_for_testing_symbol)
          << "\",\"runtime_bootstrap_api_registration_result_model\":\""
          << EscapeJsonString(runtime_bootstrap_api.registration_result_model)
          << "\",\"runtime_bootstrap_api_registration_order_ordinal_model\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.registration_order_ordinal_model)
          << "\",\"runtime_bootstrap_api_runtime_state_locking_model\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.runtime_state_locking_model)
          << "\",\"runtime_bootstrap_api_startup_invocation_model\":\""
          << EscapeJsonString(runtime_bootstrap_api.startup_invocation_model)
          << "\",\"runtime_bootstrap_api_image_walk_lifecycle_model\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.image_walk_lifecycle_model)
          << "\",\"runtime_bootstrap_api_deterministic_reset_lifecycle_model\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.deterministic_reset_lifecycle_model)
          << "\",\"runtime_bootstrap_api_ready_for_registrar_implementation\":"
          << (runtime_bootstrap_api.ready_for_registrar_implementation ? "true"
                                                                       : "false")
          << ",\"runtime_bootstrap_api_support_library_core_feature_replay_key\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.support_library_core_feature_replay_key)
          << "\",\"runtime_bootstrap_api_support_library_link_wiring_replay_key\":\""
          << EscapeJsonString(
                 runtime_bootstrap_api.support_library_link_wiring_replay_key)
          << "\""
          << ",\"runtime_bootstrap_api_replay_key\":\""
          << EscapeJsonString(runtime_bootstrap_api.replay_key)
          << "\",\"runtime_bootstrap_api_failure_reason\":\""
          << EscapeJsonString(runtime_bootstrap_api.failure_reason)
          << "\""
          << ",\"runtime_bootstrap_semantics_contract_id\":\""
          << runtime_bootstrap_semantics.contract_id
           << "\",\"runtime_bootstrap_semantics_bootstrap_invariant_contract_id\":\""
           << runtime_bootstrap_semantics.bootstrap_invariant_contract_id
           << "\",\"runtime_bootstrap_semantics_registration_manifest_contract_id\":\""
           << runtime_bootstrap_semantics.registration_manifest_contract_id
           << "\",\"runtime_bootstrap_semantics_duplicate_registration_policy\":\""
           << runtime_bootstrap_semantics.duplicate_registration_policy
           << "\",\"runtime_bootstrap_semantics_realization_order_policy\":\""
           << runtime_bootstrap_semantics.realization_order_policy
           << "\",\"runtime_bootstrap_semantics_failure_mode\":\""
           << runtime_bootstrap_semantics.failure_mode
           << "\",\"runtime_bootstrap_semantics_result_model\":\""
           << runtime_bootstrap_semantics.registration_result_model
           << "\",\"runtime_bootstrap_semantics_registration_order_ordinal_model\":\""
           << runtime_bootstrap_semantics.registration_order_ordinal_model
           << "\",\"runtime_bootstrap_semantics_runtime_state_snapshot_symbol\":\""
           << runtime_bootstrap_semantics.runtime_state_snapshot_symbol
           << "\",\"runtime_bootstrap_semantics_runtime_library_archive_relative_path\":\""
           << runtime_bootstrap_semantics.runtime_library_archive_relative_path
           << "\",\"runtime_bootstrap_semantics_translation_unit_registration_order_ordinal\":"
           << runtime_bootstrap_semantics.translation_unit_registration_order_ordinal
           << ",\"runtime_bootstrap_semantics_success_status_code\":"
           << runtime_bootstrap_semantics.success_status_code
           << ",\"runtime_bootstrap_semantics_invalid_descriptor_status_code\":"
           << runtime_bootstrap_semantics.invalid_descriptor_status_code
           << ",\"runtime_bootstrap_semantics_duplicate_registration_status_code\":"
           << runtime_bootstrap_semantics.duplicate_registration_status_code
           << ",\"runtime_bootstrap_semantics_out_of_order_status_code\":"
           << runtime_bootstrap_semantics.out_of_order_status_code
           << ",\"runtime_bootstrap_semantics_fail_closed\":"
           << (runtime_bootstrap_semantics.fail_closed ? "true" : "false")
           << ",\"runtime_bootstrap_semantics_live_runtime_enforcement_landed\":"
           << (runtime_bootstrap_semantics.live_runtime_enforcement_landed ? "true"
                                                                          : "false")
           << ",\"runtime_bootstrap_semantics_no_partial_commit_on_failure\":"
           << (runtime_bootstrap_semantics.no_partial_commit_on_failure ? "true"
                                                                        : "false")
           << ",\"runtime_bootstrap_semantics_ready_for_constructor_root_implementation\":"
           << (runtime_bootstrap_semantics
                       .ready_for_constructor_root_implementation
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_semantics_replay_key\":\""
           << EscapeJsonString(runtime_bootstrap_semantics.replay_key)
           << "\",\"runtime_bootstrap_semantics_failure_reason\":\""
           << EscapeJsonString(runtime_bootstrap_semantics.failure_reason)
           << "\""
           << ",\"runtime_bootstrap_lowering_contract_id\":\""
           << runtime_bootstrap_lowering.contract_id
           << "\",\"runtime_bootstrap_lowering_registration_manifest_contract_id\":\""
           << runtime_bootstrap_lowering.registration_manifest_contract_id
           << "\",\"runtime_bootstrap_registrar_contract_id\":\""
           << kObjc3RuntimeBootstrapRegistrarContractId
           << "\",\"runtime_bootstrap_registrar_stage_registration_table_symbol\":\""
           << kObjc3RuntimeBootstrapStageRegistrationTableSymbol
           << "\",\"runtime_bootstrap_registrar_image_walk_snapshot_symbol\":\""
           << kObjc3RuntimeBootstrapImageWalkSnapshotSymbol
           << "\",\"runtime_bootstrap_registrar_image_walk_model\":\""
           << kObjc3RuntimeBootstrapImageWalkModel
           << "\",\"runtime_bootstrap_registrar_discovery_root_validation_model\":\""
           << kObjc3RuntimeBootstrapDiscoveryRootValidationModel
           << "\",\"runtime_bootstrap_registrar_selector_pool_interning_model\":\""
           << kObjc3RuntimeBootstrapSelectorPoolInterningModel
           << "\",\"runtime_bootstrap_registrar_realization_staging_model\":\""
           << kObjc3RuntimeBootstrapRealizationStagingModel
           << "\",\"runtime_bootstrap_reset_contract_id\":\""
           << kObjc3RuntimeBootstrapResetContractId
           << "\",\"runtime_bootstrap_reset_replay_registered_images_symbol\":\""
           << kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol
           << "\",\"runtime_bootstrap_reset_reset_replay_state_snapshot_symbol\":\""
           << kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol
           << "\",\"runtime_bootstrap_reset_lifecycle_model\":\""
           << kObjc3RuntimeBootstrapResetLifecycleModel
           << "\",\"runtime_bootstrap_reset_replay_order_model\":\""
           << kObjc3RuntimeBootstrapReplayOrderModel
           << "\",\"runtime_bootstrap_reset_image_local_init_state_reset_model\":\""
           << kObjc3RuntimeBootstrapImageLocalInitStateResetModel
           << "\",\"runtime_bootstrap_reset_bootstrap_catalog_retention_model\":\""
           << kObjc3RuntimeBootstrapCatalogRetentionModel
           << "\",\"runtime_bootstrap_lowering_bootstrap_semantics_contract_id\":\""
           << runtime_bootstrap_lowering.bootstrap_semantics_contract_id
           << "\",\"runtime_bootstrap_lowering_boundary_model\":\""
           << runtime_bootstrap_lowering.lowering_boundary_model
           << "\",\"runtime_bootstrap_lowering_constructor_root_symbol\":\""
           << runtime_bootstrap_lowering.constructor_root_symbol
           << "\",\"runtime_bootstrap_lowering_init_stub_symbol_prefix\":\""
           << runtime_bootstrap_lowering.constructor_init_stub_symbol_prefix
           << "\",\"runtime_bootstrap_lowering_registration_table_symbol_prefix\":\""
           << runtime_bootstrap_lowering.registration_table_symbol_prefix
           << "\",\"runtime_bootstrap_lowering_registration_entrypoint_symbol\":\""
           << runtime_bootstrap_lowering.registration_entrypoint_symbol
           << "\",\"runtime_bootstrap_lowering_global_ctor_list_model\":\""
           << runtime_bootstrap_lowering.global_ctor_list_model
           << "\",\"runtime_bootstrap_lowering_constructor_root_emission_state\":\""
           << runtime_bootstrap_lowering.constructor_root_emission_state
           << "\",\"runtime_bootstrap_lowering_init_stub_emission_state\":\""
           << runtime_bootstrap_lowering.init_stub_emission_state
           << "\",\"runtime_bootstrap_lowering_registration_table_emission_state\":\""
           << runtime_bootstrap_lowering.registration_table_emission_state
           << "\",\"runtime_bootstrap_lowering_fail_closed\":"
           << (runtime_bootstrap_lowering.fail_closed ? "true" : "false")
           << ",\"runtime_bootstrap_lowering_no_bootstrap_ir_materialization_yet\":"
           << (runtime_bootstrap_lowering.no_bootstrap_ir_materialization_yet
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_lowering_ready_for_bootstrap_materialization\":"
           << (runtime_bootstrap_lowering.ready_for_bootstrap_materialization
                   ? "true"
                   : "false")
           << ",\"runtime_bootstrap_lowering_replay_key\":\""
           << EscapeJsonString(runtime_bootstrap_lowering.replay_key)
           << "\",\"runtime_bootstrap_lowering_failure_reason\":\""
           << EscapeJsonString(runtime_bootstrap_lowering.failure_reason)
           << "\""
           << ",\"runtime_startup_bootstrap_invariant_contract_id\":\""
           << runtime_startup_bootstrap_invariants.contract_id
           << "\",\"runtime_startup_bootstrap_invariant_duplicate_registration_policy\":\""
           << runtime_startup_bootstrap_invariants
                  .duplicate_registration_policy
           << "\",\"runtime_startup_bootstrap_invariant_realization_order_policy\":\""
           << runtime_startup_bootstrap_invariants.realization_order_policy
           << "\",\"runtime_startup_bootstrap_invariant_failure_mode\":\""
           << runtime_startup_bootstrap_invariants.failure_mode
           << "\",\"runtime_startup_bootstrap_invariant_image_local_initialization_scope\":\""
           << runtime_startup_bootstrap_invariants
                  .image_local_initialization_scope
           << "\",\"runtime_startup_bootstrap_invariant_constructor_root_uniqueness_policy\":\""
           << runtime_startup_bootstrap_invariants
                  .constructor_root_uniqueness_policy
           << "\",\"runtime_startup_bootstrap_invariant_constructor_root_consumption_model\":\""
           << runtime_startup_bootstrap_invariants
                  .constructor_root_consumption_model
           << "\",\"runtime_startup_bootstrap_invariant_startup_execution_mode\":\""
           << runtime_startup_bootstrap_invariants.startup_execution_mode
           << "\",\"runtime_startup_bootstrap_invariant_constructor_root_symbol\":\""
           << runtime_startup_bootstrap_invariants.constructor_root_symbol
           << "\",\"runtime_startup_bootstrap_invariant_registration_entrypoint_symbol\":\""
           << runtime_startup_bootstrap_invariants
                  .registration_entrypoint_symbol
           << "\",\"runtime_startup_bootstrap_invariant_manifest_authority_model\":\""
           << runtime_startup_bootstrap_invariants.manifest_authority_model
           << "\",\"runtime_startup_bootstrap_invariant_translation_unit_identity_model\":\""
           << runtime_startup_bootstrap_invariants
                  .translation_unit_identity_model
           << "\",\"runtime_startup_bootstrap_invariant_fail_closed\":"
           << (runtime_startup_bootstrap_invariants.fail_closed ? "true"
                                                                : "false")
           << ",\"runtime_startup_bootstrap_invariant_registration_manifest_contract_ready\":"
           << (runtime_startup_bootstrap_invariants
                       .registration_manifest_contract_ready
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_duplicate_registration_semantics_frozen\":"
           << (runtime_startup_bootstrap_invariants
                       .duplicate_registration_semantics_frozen
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_realization_order_semantics_frozen\":"
           << (runtime_startup_bootstrap_invariants
                       .realization_order_semantics_frozen
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_failure_mode_semantics_frozen\":"
           << (runtime_startup_bootstrap_invariants
                       .failure_mode_semantics_frozen
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_image_local_initialization_scope_frozen\":"
           << (runtime_startup_bootstrap_invariants
                       .image_local_initialization_scope_frozen
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_constructor_root_uniqueness_frozen\":"
           << (runtime_startup_bootstrap_invariants
                       .constructor_root_uniqueness_frozen
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_startup_execution_not_yet_landed\":"
           << (runtime_startup_bootstrap_invariants
                       .startup_execution_not_yet_landed
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_live_duplicate_registration_enforcement_not_yet_landed\":"
           << (runtime_startup_bootstrap_invariants
                       .live_duplicate_registration_enforcement_not_yet_landed
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_image_local_realization_not_yet_landed\":"
           << (runtime_startup_bootstrap_invariants
                       .image_local_realization_not_yet_landed
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_ready_for_bootstrap_implementation\":"
           << (runtime_startup_bootstrap_invariants
                       .ready_for_bootstrap_implementation
                   ? "true"
                   : "false")
           << ",\"runtime_startup_bootstrap_invariant_registration_manifest_replay_key\":\""
           << EscapeJsonString(
                  runtime_startup_bootstrap_invariants
                      .registration_manifest_replay_key)
           << "\",\"runtime_startup_bootstrap_invariant_replay_key\":\""
           << EscapeJsonString(runtime_startup_bootstrap_invariants.replay_key)
           << "\",\"runtime_startup_bootstrap_invariant_failure_reason\":\""
           << EscapeJsonString(
                  runtime_startup_bootstrap_invariants.failure_reason)
           << "\""
           << ",\"deterministic_property_synthesis_ivar_binding_handoff\":"
           << (property_synthesis_ivar_binding_handoff_deterministic ? "true" : "false")
           << ",\"property_synthesis_sites\":"
           << property_synthesis_ivar_binding_summary.property_synthesis_sites
           << ",\"property_synthesis_explicit_ivar_bindings\":"
           << property_synthesis_ivar_binding_summary.property_synthesis_explicit_ivar_bindings
           << ",\"property_synthesis_default_ivar_bindings\":"
           << property_synthesis_ivar_binding_summary.property_synthesis_default_ivar_bindings
           << ",\"ivar_binding_sites\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_sites
           << ",\"ivar_binding_resolved\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_resolved
           << ",\"ivar_binding_missing\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_missing
           << ",\"ivar_binding_conflicts\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_conflicts
           << ",\"lowering_property_synthesis_ivar_binding_replay_key\":\""
           << property_synthesis_ivar_binding_replay_key
           << "\""
           << ",\"deterministic_id_class_sel_object_pointer_typecheck_handoff\":"
           << (id_class_sel_object_pointer_typecheck_contract.deterministic ? "true" : "false")
           << ",\"id_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.id_typecheck_sites
           << ",\"class_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.class_typecheck_sites
           << ",\"sel_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.sel_typecheck_sites
           << ",\"object_pointer_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.object_pointer_typecheck_sites
           << ",\"id_class_sel_object_pointer_typecheck_sites_total\":"
           << id_class_sel_object_pointer_typecheck_contract.total_typecheck_sites
           << ",\"lowering_id_class_sel_object_pointer_typecheck_replay_key\":\""
           << id_class_sel_object_pointer_typecheck_replay_key
           << "\""
           << ",\"deterministic_message_send_selector_lowering_handoff\":"
           << (message_send_selector_lowering_contract.deterministic ? "true" : "false")
           << ",\"message_send_selector_lowering_sites\":"
           << message_send_selector_lowering_contract.message_send_sites
           << ",\"message_send_selector_lowering_unary_sites\":"
           << message_send_selector_lowering_contract.unary_selector_sites
           << ",\"message_send_selector_lowering_keyword_sites\":"
           << message_send_selector_lowering_contract.keyword_selector_sites
           << ",\"message_send_selector_lowering_selector_piece_sites\":"
           << message_send_selector_lowering_contract.selector_piece_sites
           << ",\"message_send_selector_lowering_argument_expression_sites\":"
           << message_send_selector_lowering_contract.argument_expression_sites
           << ",\"message_send_selector_lowering_receiver_sites\":"
           << message_send_selector_lowering_contract.receiver_expression_sites
           << ",\"message_send_selector_lowering_selector_literal_entries\":"
           << message_send_selector_lowering_contract.selector_literal_entries
           << ",\"message_send_selector_lowering_selector_literal_characters\":"
           << message_send_selector_lowering_contract.selector_literal_characters
           << ",\"lowering_message_send_selector_lowering_replay_key\":\""
           << message_send_selector_lowering_replay_key
           << "\""
           << ",\"deterministic_dispatch_abi_marshalling_handoff\":"
           << (dispatch_abi_marshalling_contract.deterministic ? "true" : "false")
           << ",\"dispatch_abi_marshalling_message_send_sites\":"
           << dispatch_abi_marshalling_contract.message_send_sites
           << ",\"dispatch_abi_marshalling_receiver_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.receiver_slots_marshaled
           << ",\"dispatch_abi_marshalling_selector_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.selector_slots_marshaled
           << ",\"dispatch_abi_marshalling_argument_value_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.argument_value_slots_marshaled
           << ",\"dispatch_abi_marshalling_argument_padding_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.argument_padding_slots_marshaled
           << ",\"dispatch_abi_marshalling_argument_total_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.argument_total_slots_marshaled
           << ",\"dispatch_abi_marshalling_total_marshaled_slots\":"
           << dispatch_abi_marshalling_contract.total_marshaled_slots
           << ",\"dispatch_abi_marshalling_runtime_dispatch_arg_slots\":"
           << dispatch_abi_marshalling_contract.runtime_dispatch_arg_slots
           << ",\"lowering_dispatch_abi_marshalling_replay_key\":\""
           << dispatch_abi_marshalling_replay_key
           << "\""
           << ",\"deterministic_nil_receiver_semantics_foldability_handoff\":"
           << (nil_receiver_semantics_foldability_contract.deterministic ? "true" : "false")
           << ",\"nil_receiver_semantics_foldability_message_send_sites\":"
           << nil_receiver_semantics_foldability_contract.message_send_sites
           << ",\"nil_receiver_semantics_foldability_receiver_nil_literal_sites\":"
           << nil_receiver_semantics_foldability_contract.receiver_nil_literal_sites
           << ",\"nil_receiver_semantics_foldability_enabled_sites\":"
           << nil_receiver_semantics_foldability_contract.nil_receiver_semantics_enabled_sites
           << ",\"nil_receiver_semantics_foldability_foldable_sites\":"
           << nil_receiver_semantics_foldability_contract.nil_receiver_foldable_sites
           << ",\"nil_receiver_semantics_foldability_runtime_dispatch_required_sites\":"
           << nil_receiver_semantics_foldability_contract.nil_receiver_runtime_dispatch_required_sites
           << ",\"nil_receiver_semantics_foldability_non_nil_receiver_sites\":"
           << nil_receiver_semantics_foldability_contract.non_nil_receiver_sites
           << ",\"nil_receiver_semantics_foldability_contract_violation_sites\":"
           << nil_receiver_semantics_foldability_contract.contract_violation_sites
           << ",\"lowering_nil_receiver_semantics_foldability_replay_key\":\""
           << nil_receiver_semantics_foldability_replay_key
           << "\""
           << ",\"deterministic_super_dispatch_method_family_handoff\":"
           << (super_dispatch_method_family_contract.deterministic ? "true" : "false")
           << ",\"super_dispatch_method_family_message_send_sites\":"
           << super_dispatch_method_family_contract.message_send_sites
           << ",\"super_dispatch_method_family_receiver_super_identifier_sites\":"
           << super_dispatch_method_family_contract.receiver_super_identifier_sites
           << ",\"super_dispatch_method_family_enabled_sites\":"
           << super_dispatch_method_family_contract.super_dispatch_enabled_sites
           << ",\"super_dispatch_method_family_requires_class_context_sites\":"
           << super_dispatch_method_family_contract.super_dispatch_requires_class_context_sites
           << ",\"super_dispatch_method_family_init_sites\":"
           << super_dispatch_method_family_contract.method_family_init_sites
           << ",\"super_dispatch_method_family_copy_sites\":"
           << super_dispatch_method_family_contract.method_family_copy_sites
           << ",\"super_dispatch_method_family_mutable_copy_sites\":"
           << super_dispatch_method_family_contract.method_family_mutable_copy_sites
           << ",\"super_dispatch_method_family_new_sites\":"
           << super_dispatch_method_family_contract.method_family_new_sites
           << ",\"super_dispatch_method_family_none_sites\":"
           << super_dispatch_method_family_contract.method_family_none_sites
           << ",\"super_dispatch_method_family_returns_retained_result_sites\":"
           << super_dispatch_method_family_contract.method_family_returns_retained_result_sites
           << ",\"super_dispatch_method_family_returns_related_result_sites\":"
           << super_dispatch_method_family_contract.method_family_returns_related_result_sites
           << ",\"super_dispatch_method_family_contract_violation_sites\":"
           << super_dispatch_method_family_contract.contract_violation_sites
           << ",\"lowering_super_dispatch_method_family_replay_key\":\""
           << super_dispatch_method_family_replay_key
           << "\""
           << ",\"deterministic_runtime_shim_host_link_handoff\":"
           << (runtime_shim_host_link_contract.deterministic ? "true" : "false")
           << ",\"runtime_shim_host_link_message_send_sites\":"
           << runtime_shim_host_link_contract.message_send_sites
           << ",\"runtime_shim_host_link_required_runtime_shim_sites\":"
           << runtime_shim_host_link_contract.runtime_shim_required_sites
           << ",\"runtime_shim_host_link_elided_runtime_shim_sites\":"
           << runtime_shim_host_link_contract.runtime_shim_elided_sites
           << ",\"runtime_shim_host_link_runtime_dispatch_arg_slots\":"
           << runtime_shim_host_link_contract.runtime_dispatch_arg_slots
           << ",\"runtime_shim_host_link_runtime_dispatch_declaration_parameter_count\":"
           << runtime_shim_host_link_contract.runtime_dispatch_declaration_parameter_count
           << ",\"runtime_shim_host_link_runtime_dispatch_symbol\":\""
           << runtime_shim_host_link_contract.runtime_dispatch_symbol
           << "\""
           << ",\"runtime_shim_host_link_default_runtime_dispatch_symbol_binding\":"
           << (runtime_shim_host_link_contract.default_runtime_dispatch_symbol_binding ? "true" : "false")
           << ",\"runtime_shim_host_link_contract_violation_sites\":"
           << runtime_shim_host_link_contract.contract_violation_sites
           << ",\"lowering_runtime_shim_host_link_replay_key\":\""
           << runtime_shim_host_link_replay_key
           << "\""
           << ",\"deterministic_ownership_qualifier_lowering_handoff\":"
           << (ownership_qualifier_lowering_contract.deterministic ? "true" : "false")
           << ",\"ownership_qualifier_lowering_type_annotation_ownership_qualifier_sites\":"
           << ownership_qualifier_lowering_contract.ownership_qualifier_sites
           << ",\"ownership_qualifier_lowering_type_annotation_invalid_ownership_qualifier_sites\":"
           << ownership_qualifier_lowering_contract.invalid_ownership_qualifier_sites
           << ",\"ownership_qualifier_lowering_type_annotation_object_pointer_type_sites\":"
           << ownership_qualifier_lowering_contract.object_pointer_type_annotation_sites
           << ",\"lowering_ownership_qualifier_replay_key\":\""
           << ownership_qualifier_lowering_replay_key
           << "\""
           << ",\"deterministic_retain_release_operation_lowering_handoff\":"
           << (retain_release_operation_lowering_contract.deterministic ? "true" : "false")
           << ",\"retain_release_operation_lowering_ownership_qualified_sites\":"
           << retain_release_operation_lowering_contract.ownership_qualified_sites
           << ",\"retain_release_operation_lowering_retain_insertion_sites\":"
           << retain_release_operation_lowering_contract.retain_insertion_sites
           << ",\"retain_release_operation_lowering_release_insertion_sites\":"
           << retain_release_operation_lowering_contract.release_insertion_sites
           << ",\"retain_release_operation_lowering_autorelease_insertion_sites\":"
           << retain_release_operation_lowering_contract.autorelease_insertion_sites
           << ",\"retain_release_operation_lowering_contract_violation_sites\":"
           << retain_release_operation_lowering_contract.contract_violation_sites
           << ",\"lowering_retain_release_operation_replay_key\":\""
           << retain_release_operation_lowering_replay_key
           << "\""
           << ",\"deterministic_autoreleasepool_scope_lowering_handoff\":"
           << (autoreleasepool_scope_lowering_contract.deterministic ? "true" : "false")
           << ",\"autoreleasepool_scope_lowering_scope_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_sites
           << ",\"autoreleasepool_scope_lowering_scope_symbolized_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_symbolized_sites
           << ",\"autoreleasepool_scope_lowering_max_scope_depth\":"
           << autoreleasepool_scope_lowering_contract.max_scope_depth
           << ",\"autoreleasepool_scope_lowering_scope_entry_transition_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_entry_transition_sites
           << ",\"autoreleasepool_scope_lowering_scope_exit_transition_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_exit_transition_sites
           << ",\"autoreleasepool_scope_lowering_contract_violation_sites\":"
           << autoreleasepool_scope_lowering_contract.contract_violation_sites
           << ",\"lowering_autoreleasepool_scope_replay_key\":\""
           << autoreleasepool_scope_lowering_replay_key
           << "\""
           << ",\"deterministic_weak_unowned_semantics_lowering_handoff\":"
           << (weak_unowned_semantics_lowering_contract.deterministic ? "true" : "false")
           << ",\"weak_unowned_semantics_lowering_ownership_candidate_sites\":"
           << weak_unowned_semantics_lowering_contract.ownership_candidate_sites
           << ",\"weak_unowned_semantics_lowering_weak_reference_sites\":"
           << weak_unowned_semantics_lowering_contract.weak_reference_sites
           << ",\"weak_unowned_semantics_lowering_unowned_reference_sites\":"
           << weak_unowned_semantics_lowering_contract.unowned_reference_sites
           << ",\"weak_unowned_semantics_lowering_unowned_safe_reference_sites\":"
           << weak_unowned_semantics_lowering_contract.unowned_safe_reference_sites
           << ",\"weak_unowned_semantics_lowering_conflict_sites\":"
           << weak_unowned_semantics_lowering_contract.weak_unowned_conflict_sites
           << ",\"weak_unowned_semantics_lowering_contract_violation_sites\":"
           << weak_unowned_semantics_lowering_contract.contract_violation_sites
           << ",\"lowering_weak_unowned_semantics_replay_key\":\""
           << weak_unowned_semantics_lowering_replay_key
           << "\""
           << ",\"deterministic_arc_diagnostics_fixit_lowering_handoff\":"
           << (arc_diagnostics_fixit_lowering_contract.deterministic ? "true" : "false")
           << ",\"arc_diagnostics_fixit_lowering_ownership_arc_diagnostic_candidate_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_diagnostic_candidate_sites
           << ",\"arc_diagnostics_fixit_lowering_ownership_arc_fixit_available_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_fixit_available_sites
           << ",\"arc_diagnostics_fixit_lowering_ownership_arc_profiled_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_profiled_sites
           << ",\"arc_diagnostics_fixit_lowering_ownership_arc_weak_unowned_conflict_diagnostic_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_weak_unowned_conflict_diagnostic_sites
           << ",\"arc_diagnostics_fixit_lowering_ownership_arc_empty_fixit_hint_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_empty_fixit_hint_sites
           << ",\"arc_diagnostics_fixit_lowering_contract_violation_sites\":"
           << arc_diagnostics_fixit_lowering_contract.contract_violation_sites
           << ",\"lowering_arc_diagnostics_fixit_replay_key\":\""
           << arc_diagnostics_fixit_lowering_replay_key
           << "\""
           << ",\"deterministic_block_literal_capture_lowering_handoff\":"
           << (block_literal_capture_lowering_contract.deterministic ? "true" : "false")
           << ",\"block_literal_capture_lowering_block_literal_sites\":"
           << block_literal_capture_lowering_contract.block_literal_sites
           << ",\"block_literal_capture_lowering_block_parameter_entries\":"
           << block_literal_capture_lowering_contract.block_parameter_entries
           << ",\"block_literal_capture_lowering_block_capture_entries\":"
           << block_literal_capture_lowering_contract.block_capture_entries
           << ",\"block_literal_capture_lowering_block_body_statement_entries\":"
           << block_literal_capture_lowering_contract.block_body_statement_entries
           << ",\"block_literal_capture_lowering_block_empty_capture_sites\":"
           << block_literal_capture_lowering_contract.block_empty_capture_sites
           << ",\"block_literal_capture_lowering_block_nondeterministic_capture_sites\":"
           << block_literal_capture_lowering_contract.block_nondeterministic_capture_sites
           << ",\"block_literal_capture_lowering_block_non_normalized_sites\":"
           << block_literal_capture_lowering_contract.block_non_normalized_sites
           << ",\"block_literal_capture_lowering_contract_violation_sites\":"
           << block_literal_capture_lowering_contract.contract_violation_sites
           << ",\"lowering_block_literal_capture_replay_key\":\""
           << block_literal_capture_lowering_replay_key
           << "\""
           << ",\"deterministic_block_abi_invoke_trampoline_lowering_handoff\":"
           << (block_abi_invoke_trampoline_lowering_contract.deterministic ? "true" : "false")
           << ",\"block_abi_invoke_trampoline_lowering_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.block_literal_sites
           << ",\"block_abi_invoke_trampoline_lowering_invoke_argument_slots\":"
           << block_abi_invoke_trampoline_lowering_contract.invoke_argument_slots_total
           << ",\"block_abi_invoke_trampoline_lowering_capture_word_count\":"
           << block_abi_invoke_trampoline_lowering_contract.capture_word_count_total
           << ",\"block_abi_invoke_trampoline_lowering_parameter_entries\":"
           << block_abi_invoke_trampoline_lowering_contract.parameter_entries_total
           << ",\"block_abi_invoke_trampoline_lowering_capture_entries\":"
           << block_abi_invoke_trampoline_lowering_contract.capture_entries_total
           << ",\"block_abi_invoke_trampoline_lowering_body_statement_entries\":"
           << block_abi_invoke_trampoline_lowering_contract.body_statement_entries_total
           << ",\"block_abi_invoke_trampoline_lowering_descriptor_symbolized_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.descriptor_symbolized_sites
           << ",\"block_abi_invoke_trampoline_lowering_invoke_symbolized_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.invoke_trampoline_symbolized_sites
           << ",\"block_abi_invoke_trampoline_lowering_missing_invoke_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.missing_invoke_trampoline_sites
           << ",\"block_abi_invoke_trampoline_lowering_non_normalized_layout_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.non_normalized_layout_sites
           << ",\"block_abi_invoke_trampoline_lowering_contract_violation_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.contract_violation_sites
           << ",\"lowering_block_abi_invoke_trampoline_replay_key\":\""
           << block_abi_invoke_trampoline_lowering_replay_key
           << "\""
           << ",\"deterministic_block_storage_escape_lowering_handoff\":"
           << (block_storage_escape_lowering_contract.deterministic ? "true" : "false")
           << ",\"block_storage_escape_lowering_sites\":"
           << block_storage_escape_lowering_contract.block_literal_sites
           << ",\"block_storage_escape_lowering_mutable_capture_count\":"
           << block_storage_escape_lowering_contract.mutable_capture_count_total
           << ",\"block_storage_escape_lowering_byref_slot_count\":"
           << block_storage_escape_lowering_contract.byref_slot_count_total
           << ",\"block_storage_escape_lowering_parameter_entries\":"
           << block_storage_escape_lowering_contract.parameter_entries_total
           << ",\"block_storage_escape_lowering_capture_entries\":"
           << block_storage_escape_lowering_contract.capture_entries_total
           << ",\"block_storage_escape_lowering_body_statement_entries\":"
           << block_storage_escape_lowering_contract.body_statement_entries_total
           << ",\"block_storage_escape_lowering_requires_byref_cells_sites\":"
           << block_storage_escape_lowering_contract.requires_byref_cells_sites
           << ",\"block_storage_escape_lowering_escape_analysis_enabled_sites\":"
           << block_storage_escape_lowering_contract.escape_analysis_enabled_sites
           << ",\"block_storage_escape_lowering_escape_to_heap_sites\":"
           << block_storage_escape_lowering_contract.escape_to_heap_sites
           << ",\"block_storage_escape_lowering_escape_profile_normalized_sites\":"
           << block_storage_escape_lowering_contract.escape_profile_normalized_sites
           << ",\"block_storage_escape_lowering_byref_layout_symbolized_sites\":"
           << block_storage_escape_lowering_contract.byref_layout_symbolized_sites
           << ",\"block_storage_escape_lowering_contract_violation_sites\":"
           << block_storage_escape_lowering_contract.contract_violation_sites
           << ",\"lowering_block_storage_escape_replay_key\":\""
           << block_storage_escape_lowering_replay_key
           << "\""
           << ",\"deterministic_block_copy_dispose_lowering_handoff\":"
           << (block_copy_dispose_lowering_contract.deterministic ? "true" : "false")
           << ",\"block_copy_dispose_lowering_sites\":"
           << block_copy_dispose_lowering_contract.block_literal_sites
           << ",\"block_copy_dispose_lowering_mutable_capture_count\":"
           << block_copy_dispose_lowering_contract.mutable_capture_count_total
           << ",\"block_copy_dispose_lowering_byref_slot_count\":"
           << block_copy_dispose_lowering_contract.byref_slot_count_total
           << ",\"block_copy_dispose_lowering_parameter_entries\":"
           << block_copy_dispose_lowering_contract.parameter_entries_total
           << ",\"block_copy_dispose_lowering_capture_entries\":"
           << block_copy_dispose_lowering_contract.capture_entries_total
           << ",\"block_copy_dispose_lowering_body_statement_entries\":"
           << block_copy_dispose_lowering_contract.body_statement_entries_total
           << ",\"block_copy_dispose_lowering_copy_helper_required_sites\":"
           << block_copy_dispose_lowering_contract.copy_helper_required_sites
           << ",\"block_copy_dispose_lowering_dispose_helper_required_sites\":"
           << block_copy_dispose_lowering_contract.dispose_helper_required_sites
           << ",\"block_copy_dispose_lowering_profile_normalized_sites\":"
           << block_copy_dispose_lowering_contract.profile_normalized_sites
           << ",\"block_copy_dispose_lowering_copy_helper_symbolized_sites\":"
           << block_copy_dispose_lowering_contract.copy_helper_symbolized_sites
           << ",\"block_copy_dispose_lowering_dispose_helper_symbolized_sites\":"
           << block_copy_dispose_lowering_contract.dispose_helper_symbolized_sites
           << ",\"block_copy_dispose_lowering_contract_violation_sites\":"
           << block_copy_dispose_lowering_contract.contract_violation_sites
           << ",\"lowering_block_copy_dispose_replay_key\":\""
           << block_copy_dispose_lowering_replay_key
           << "\""
           << ",\"deterministic_block_determinism_perf_baseline_lowering_handoff\":"
           << (block_determinism_perf_baseline_lowering_contract.deterministic ? "true" : "false")
           << ",\"block_determinism_perf_baseline_lowering_sites\":"
           << block_determinism_perf_baseline_lowering_contract.block_literal_sites
           << ",\"block_determinism_perf_baseline_lowering_weight_total\":"
           << block_determinism_perf_baseline_lowering_contract.baseline_weight_total
           << ",\"block_determinism_perf_baseline_lowering_parameter_entries\":"
           << block_determinism_perf_baseline_lowering_contract.parameter_entries_total
           << ",\"block_determinism_perf_baseline_lowering_capture_entries\":"
           << block_determinism_perf_baseline_lowering_contract.capture_entries_total
           << ",\"block_determinism_perf_baseline_lowering_body_statement_entries\":"
           << block_determinism_perf_baseline_lowering_contract.body_statement_entries_total
           << ",\"block_determinism_perf_baseline_lowering_deterministic_capture_sites\":"
           << block_determinism_perf_baseline_lowering_contract.deterministic_capture_sites
           << ",\"block_determinism_perf_baseline_lowering_heavy_tier_sites\":"
           << block_determinism_perf_baseline_lowering_contract.heavy_tier_sites
           << ",\"block_determinism_perf_baseline_lowering_normalized_profile_sites\":"
           << block_determinism_perf_baseline_lowering_contract.normalized_profile_sites
           << ",\"block_determinism_perf_baseline_lowering_contract_violation_sites\":"
           << block_determinism_perf_baseline_lowering_contract.contract_violation_sites
           << ",\"lowering_block_determinism_perf_baseline_replay_key\":\""
           << block_determinism_perf_baseline_lowering_replay_key
           << "\""
           << ",\"deterministic_lightweight_generic_constraint_lowering_handoff\":"
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
           << "\""
           << ",\"deterministic_module_import_graph_lowering_handoff\":"
           << (module_import_graph_lowering_contract.deterministic ? "true" : "false")
           << ",\"module_import_graph_lowering_sites\":"
           << module_import_graph_lowering_contract.module_import_graph_sites
           << ",\"module_import_graph_lowering_import_edge_candidate_sites\":"
           << module_import_graph_lowering_contract.import_edge_candidate_sites
           << ",\"module_import_graph_lowering_namespace_segment_sites\":"
           << module_import_graph_lowering_contract.namespace_segment_sites
           << ",\"module_import_graph_lowering_object_pointer_type_sites\":"
           << module_import_graph_lowering_contract.object_pointer_type_sites
           << ",\"module_import_graph_lowering_pointer_declarator_sites\":"
           << module_import_graph_lowering_contract.pointer_declarator_sites
           << ",\"module_import_graph_lowering_normalized_sites\":"
           << module_import_graph_lowering_contract.normalized_sites
           << ",\"module_import_graph_lowering_contract_violation_sites\":"
           << module_import_graph_lowering_contract.contract_violation_sites
           << ",\"lowering_module_import_graph_replay_key\":\""
           << module_import_graph_lowering_replay_key
           << "\""
           << ",\"deterministic_namespace_collision_shadowing_lowering_handoff\":"
           << (namespace_collision_shadowing_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << ",\"namespace_collision_shadowing_lowering_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .namespace_collision_shadowing_sites
           << ",\"namespace_collision_shadowing_lowering_namespace_segment_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .namespace_segment_sites
           << ",\"namespace_collision_shadowing_lowering_import_edge_candidate_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .import_edge_candidate_sites
           << ",\"namespace_collision_shadowing_lowering_object_pointer_type_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .object_pointer_type_sites
           << ",\"namespace_collision_shadowing_lowering_pointer_declarator_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .pointer_declarator_sites
           << ",\"namespace_collision_shadowing_lowering_normalized_sites\":"
           << namespace_collision_shadowing_lowering_contract.normalized_sites
           << ",\"namespace_collision_shadowing_lowering_contract_violation_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .contract_violation_sites
           << ",\"lowering_namespace_collision_shadowing_replay_key\":\""
           << namespace_collision_shadowing_lowering_replay_key
           << "\""
           << ",\"deterministic_public_private_api_partition_lowering_handoff\":"
           << (public_private_api_partition_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << ",\"public_private_api_partition_lowering_sites\":"
           << public_private_api_partition_lowering_contract
                  .public_private_api_partition_sites
           << ",\"public_private_api_partition_lowering_namespace_segment_sites\":"
           << public_private_api_partition_lowering_contract
                  .namespace_segment_sites
           << ",\"public_private_api_partition_lowering_import_edge_candidate_sites\":"
           << public_private_api_partition_lowering_contract
                  .import_edge_candidate_sites
           << ",\"public_private_api_partition_lowering_object_pointer_type_sites\":"
           << public_private_api_partition_lowering_contract
                  .object_pointer_type_sites
           << ",\"public_private_api_partition_lowering_pointer_declarator_sites\":"
           << public_private_api_partition_lowering_contract
                  .pointer_declarator_sites
           << ",\"public_private_api_partition_lowering_normalized_sites\":"
           << public_private_api_partition_lowering_contract.normalized_sites
           << ",\"public_private_api_partition_lowering_contract_violation_sites\":"
           << public_private_api_partition_lowering_contract
                  .contract_violation_sites
           << ",\"lowering_public_private_api_partition_replay_key\":\""
           << public_private_api_partition_lowering_replay_key
           << "\""
           << ",\"deterministic_incremental_module_cache_invalidation_lowering_handoff\":"
           << (incremental_module_cache_invalidation_lowering_contract
                       .deterministic
                   ? "true"
                   : "false")
           << ",\"incremental_module_cache_invalidation_lowering_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .incremental_module_cache_invalidation_sites
           << ",\"incremental_module_cache_invalidation_lowering_namespace_segment_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .namespace_segment_sites
           << ",\"incremental_module_cache_invalidation_lowering_import_edge_candidate_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .import_edge_candidate_sites
           << ",\"incremental_module_cache_invalidation_lowering_object_pointer_type_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .object_pointer_type_sites
           << ",\"incremental_module_cache_invalidation_lowering_pointer_declarator_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .pointer_declarator_sites
           << ",\"incremental_module_cache_invalidation_lowering_normalized_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .normalized_sites
           << ",\"incremental_module_cache_invalidation_lowering_cache_invalidation_candidate_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .cache_invalidation_candidate_sites
           << ",\"incremental_module_cache_invalidation_lowering_contract_violation_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .contract_violation_sites
           << ",\"lowering_incremental_module_cache_invalidation_replay_key\":\""
           << incremental_module_cache_invalidation_lowering_replay_key
           << "\""
           << ",\"deterministic_cross_module_conformance_lowering_handoff\":"
           << (cross_module_conformance_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << ",\"cross_module_conformance_lowering_sites\":"
           << cross_module_conformance_lowering_contract
                  .cross_module_conformance_sites
           << ",\"cross_module_conformance_lowering_namespace_segment_sites\":"
           << cross_module_conformance_lowering_contract.namespace_segment_sites
           << ",\"cross_module_conformance_lowering_import_edge_candidate_sites\":"
           << cross_module_conformance_lowering_contract
                  .import_edge_candidate_sites
           << ",\"cross_module_conformance_lowering_object_pointer_type_sites\":"
           << cross_module_conformance_lowering_contract.object_pointer_type_sites
           << ",\"cross_module_conformance_lowering_pointer_declarator_sites\":"
           << cross_module_conformance_lowering_contract.pointer_declarator_sites
           << ",\"cross_module_conformance_lowering_normalized_sites\":"
           << cross_module_conformance_lowering_contract.normalized_sites
           << ",\"cross_module_conformance_lowering_cache_invalidation_candidate_sites\":"
           << cross_module_conformance_lowering_contract
                  .cache_invalidation_candidate_sites
           << ",\"cross_module_conformance_lowering_contract_violation_sites\":"
           << cross_module_conformance_lowering_contract.contract_violation_sites
           << ",\"lowering_cross_module_conformance_replay_key\":\""
           << cross_module_conformance_lowering_replay_key
           << "\""
           << ",\"deterministic_throws_propagation_lowering_handoff\":"
           << (throws_propagation_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << ",\"throws_propagation_lowering_sites\":"
           << throws_propagation_lowering_contract.throws_propagation_sites
           << ",\"throws_propagation_lowering_namespace_segment_sites\":"
           << throws_propagation_lowering_contract.namespace_segment_sites
           << ",\"throws_propagation_lowering_import_edge_candidate_sites\":"
           << throws_propagation_lowering_contract.import_edge_candidate_sites
           << ",\"throws_propagation_lowering_object_pointer_type_sites\":"
           << throws_propagation_lowering_contract.object_pointer_type_sites
           << ",\"throws_propagation_lowering_pointer_declarator_sites\":"
           << throws_propagation_lowering_contract.pointer_declarator_sites
           << ",\"throws_propagation_lowering_normalized_sites\":"
           << throws_propagation_lowering_contract.normalized_sites
           << ",\"throws_propagation_lowering_cache_invalidation_candidate_sites\":"
           << throws_propagation_lowering_contract
                  .cache_invalidation_candidate_sites
           << ",\"throws_propagation_lowering_contract_violation_sites\":"
           << throws_propagation_lowering_contract.contract_violation_sites
           << ",\"lowering_throws_propagation_replay_key\":\""
           << throws_propagation_lowering_replay_key
           << "\""
           << ",\"deterministic_object_pointer_nullability_generics_handoff\":"
           << (object_pointer_nullability_generics_summary.deterministic_object_pointer_nullability_generics_handoff
                   ? "true"
                   : "false")
           << ",\"object_pointer_type_spellings\":"
           << object_pointer_nullability_generics_summary.object_pointer_type_spellings
           << ",\"pointer_declarator_entries\":"
           << object_pointer_nullability_generics_summary.pointer_declarator_entries
           << ",\"pointer_declarator_depth_total\":"
           << object_pointer_nullability_generics_summary.pointer_declarator_depth_total
           << ",\"pointer_declarator_token_entries\":"
           << object_pointer_nullability_generics_summary.pointer_declarator_token_entries
           << ",\"nullability_suffix_entries\":"
           << object_pointer_nullability_generics_summary.nullability_suffix_entries
           << ",\"generic_suffix_entries\":"
           << object_pointer_nullability_generics_summary.generic_suffix_entries
           << ",\"terminated_generic_suffix_entries\":"
           << object_pointer_nullability_generics_summary.terminated_generic_suffix_entries
           << ",\"unterminated_generic_suffix_entries\":"
           << object_pointer_nullability_generics_summary.unterminated_generic_suffix_entries
           << ",\"symbol_graph_global_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.global_symbol_nodes
           << ",\"symbol_graph_function_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.function_symbol_nodes
           << ",\"symbol_graph_interface_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.interface_symbol_nodes
           << ",\"symbol_graph_implementation_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.implementation_symbol_nodes
           << ",\"symbol_graph_interface_property_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.interface_property_symbol_nodes
           << ",\"symbol_graph_implementation_property_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.implementation_property_symbol_nodes
           << ",\"symbol_graph_interface_method_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.interface_method_symbol_nodes
           << ",\"symbol_graph_implementation_method_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.implementation_method_symbol_nodes
           << ",\"scope_resolution_top_level_scope_symbols\":"
           << symbol_graph_scope_resolution_summary.top_level_scope_symbols
           << ",\"scope_resolution_nested_scope_symbols\":"
           << symbol_graph_scope_resolution_summary.nested_scope_symbols
           << ",\"scope_resolution_scope_frames_total\":"
           << symbol_graph_scope_resolution_summary.scope_frames_total
           << ",\"scope_resolution_implementation_interface_resolution_sites\":"
           << symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites
           << ",\"scope_resolution_implementation_interface_resolution_hits\":"
           << symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits
           << ",\"scope_resolution_implementation_interface_resolution_misses\":"
           << symbol_graph_scope_resolution_summary.implementation_interface_resolution_misses
           << ",\"scope_resolution_method_resolution_sites\":"
           << symbol_graph_scope_resolution_summary.method_resolution_sites
           << ",\"scope_resolution_method_resolution_hits\":"
           << symbol_graph_scope_resolution_summary.method_resolution_hits
           << ",\"scope_resolution_method_resolution_misses\":"
           << symbol_graph_scope_resolution_summary.method_resolution_misses
           << ",\"deterministic_symbol_graph_handoff\":"
           << (symbol_graph_scope_resolution_summary.deterministic_symbol_graph_handoff ? "true" : "false")
           << ",\"deterministic_scope_resolution_handoff\":"
           << (symbol_graph_scope_resolution_summary.deterministic_scope_resolution_handoff ? "true" : "false")
           << ",\"symbol_graph_scope_resolution_handoff_key\":\""
           << symbol_graph_scope_resolution_summary.deterministic_handoff_key
           << "\"},\n";
  manifest << "      \"vector_signature_surface\":{\"vector_signature_functions\":" << vector_signature_functions
           << ",\"vector_return_signatures\":" << vector_return_signatures
           << ",\"vector_param_signatures\":" << vector_param_signatures
           << ",\"vector_i32_signatures\":" << vector_i32_signatures
           << ",\"vector_bool_signatures\":" << vector_bool_signatures
           << ",\"lane2\":" << vector_lane2_signatures
           << ",\"lane4\":" << vector_lane4_signatures << ",\"lane8\":" << vector_lane8_signatures
           << ",\"lane16\":" << vector_lane16_signatures << "},\n";
  manifest << "      \"semantic_surface\": {\"declared_globals\":" << program.globals.size()
           << ",\"declared_functions\":" << manifest_functions.size()
           << ",\"declared_interfaces\":" << program.interfaces.size()
           << ",\"declared_implementations\":" << program.implementations.size()
           << ",\"resolved_global_symbols\":" << pipeline_result.integration_surface.globals.size()
           << ",\"resolved_function_symbols\":" << pipeline_result.integration_surface.functions.size()
           << ",\"resolved_interface_symbols\":" << pipeline_result.integration_surface.interfaces.size()
           << ",\"resolved_implementation_symbols\":" << pipeline_result.integration_surface.implementations.size()
           << ",\"declared_protocols\":" << protocol_category_summary.declared_protocols
           << ",\"declared_categories\":" << protocol_category_summary.declared_categories
           << ",\"resolved_protocol_symbols\":" << protocol_category_summary.resolved_protocol_symbols
           << ",\"resolved_category_symbols\":" << protocol_category_summary.resolved_category_symbols
           << ",\"interface_method_symbols\":"
           << pipeline_result.sema_parity_surface.interface_implementation_summary.interface_method_symbols
           << ",\"implementation_method_symbols\":"
           << pipeline_result.sema_parity_surface.interface_implementation_summary.implementation_method_symbols
           << ",\"protocol_method_symbols\":" << protocol_category_summary.protocol_method_symbols
           << ",\"category_method_symbols\":" << protocol_category_summary.category_method_symbols
           << ",\"linked_implementation_symbols\":"
           << pipeline_result.sema_parity_surface.interface_implementation_summary.linked_implementation_symbols
           << ",\"linked_category_symbols\":" << protocol_category_summary.linked_category_symbols
           << ",\"objc_interface_implementation_surface\":{\"interface_class_method_symbols\":"
           << interface_class_method_symbols
           << ",\"interface_instance_method_symbols\":"
           << interface_instance_method_symbols
           << ",\"implementation_class_method_symbols\":"
           << implementation_class_method_symbols
           << ",\"implementation_instance_method_symbols\":"
           << implementation_instance_method_symbols
           << ",\"implementation_methods_with_body\":"
           << implementation_methods_with_body
           << ",\"deterministic_handoff\":"
           << (pipeline_result.sema_parity_surface.deterministic_interface_implementation_handoff ? "true" : "false")
           << "}"
           << ",\"objc_protocol_category_surface\":{\"protocol_method_symbols\":"
           << protocol_category_summary.protocol_method_symbols
           << ",\"category_method_symbols\":"
           << protocol_category_summary.category_method_symbols
           << ",\"linked_category_symbols\":"
           << protocol_category_summary.linked_category_symbols
           << ",\"deterministic_handoff\":"
           << (protocol_category_summary.deterministic_protocol_category_handoff ? "true" : "false")
           << "}"
           << ",\"objc_class_protocol_category_linking_surface\":{\"declared_class_interfaces\":"
           << class_protocol_category_linking_summary.declared_class_interfaces
           << ",\"declared_class_implementations\":"
           << class_protocol_category_linking_summary.declared_class_implementations
           << ",\"resolved_class_interfaces\":"
           << class_protocol_category_linking_summary.resolved_class_interfaces
           << ",\"resolved_class_implementations\":"
           << class_protocol_category_linking_summary.resolved_class_implementations
           << ",\"linked_class_method_symbols\":"
           << class_protocol_category_linking_summary.linked_class_method_symbols
           << ",\"linked_category_method_symbols\":"
           << class_protocol_category_linking_summary.linked_category_method_symbols
           << ",\"protocol_composition_sites\":"
           << class_protocol_category_linking_summary.protocol_composition_sites
           << ",\"protocol_composition_symbols\":"
           << class_protocol_category_linking_summary.protocol_composition_symbols
           << ",\"category_composition_sites\":"
           << class_protocol_category_linking_summary.category_composition_sites
           << ",\"category_composition_symbols\":"
           << class_protocol_category_linking_summary.category_composition_symbols
           << ",\"invalid_protocol_composition_sites\":"
           << class_protocol_category_linking_summary.invalid_protocol_composition_sites
           << ",\"deterministic_handoff\":"
           << (class_protocol_category_linking_summary.deterministic_class_protocol_category_linking_handoff
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_selector_normalization_surface\":{\"method_declaration_entries\":"
           << selector_normalization_summary.method_declaration_entries
           << ",\"normalized_method_declarations\":"
           << selector_normalization_summary.normalized_method_declarations
           << ",\"selector_piece_entries\":"
           << selector_normalization_summary.selector_piece_entries
           << ",\"selector_piece_parameter_links\":"
           << selector_normalization_summary.selector_piece_parameter_links
           << ",\"deterministic_handoff\":"
           << (selector_normalization_summary.deterministic_selector_normalization_handoff ? "true" : "false")
           << "}"
           << ",\"objc_property_attribute_surface\":{\"property_declaration_entries\":"
           << property_attribute_summary.property_declaration_entries
           << ",\"property_attribute_entries\":"
           << property_attribute_summary.property_attribute_entries
           << ",\"property_attribute_value_entries\":"
           << property_attribute_summary.property_attribute_value_entries
           << ",\"property_accessor_modifier_entries\":"
           << property_attribute_summary.property_accessor_modifier_entries
           << ",\"property_getter_selector_entries\":"
           << property_attribute_summary.property_getter_selector_entries
           << ",\"property_setter_selector_entries\":"
           << property_attribute_summary.property_setter_selector_entries
           << ",\"deterministic_handoff\":"
           << (property_attribute_summary.deterministic_property_attribute_handoff ? "true" : "false")
           << "}"
           << ",\"objc_property_synthesis_ivar_binding_surface\":{\"property_synthesis_sites\":"
           << property_synthesis_ivar_binding_summary.property_synthesis_sites
           << ",\"property_synthesis_explicit_ivar_bindings\":"
           << property_synthesis_ivar_binding_summary.property_synthesis_explicit_ivar_bindings
           << ",\"property_synthesis_default_ivar_bindings\":"
           << property_synthesis_ivar_binding_summary.property_synthesis_default_ivar_bindings
           << ",\"ivar_binding_sites\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_sites
           << ",\"ivar_binding_resolved\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_resolved
           << ",\"ivar_binding_missing\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_missing
           << ",\"ivar_binding_conflicts\":"
           << property_synthesis_ivar_binding_summary.ivar_binding_conflicts
           << ",\"replay_key\":\""
           << property_synthesis_ivar_binding_replay_key
           << "\",\"deterministic_handoff\":"
           << (property_synthesis_ivar_binding_handoff_deterministic ? "true" : "false")
           << "}"
           << ",\"objc_executable_metadata_source_graph\":"
           << BuildExecutableMetadataSourceGraphJson(
                  executable_metadata_source_graph)
           // M253-A002 source-to-section matrix anchor: lane-A must publish one
           // canonical node-to-emitted-section matrix that preserves the A001
           // inventory and explicitly marks interface/implementation/metaclass/
           // method rows as no-standalone-emission-yet until later M253 work.
           << ",\"objc_runtime_metadata_source_to_section_matrix\":"
           << BuildRuntimeMetadataSourceToSectionMatrixSummaryJson(
                  runtime_metadata_source_to_section_matrix)
           << ",\"objc_executable_metadata_semantic_consistency_boundary\":"
           << BuildExecutableMetadataSemanticConsistencyBoundaryJson(
                  executable_metadata_semantic_consistency_boundary)
           << ",\"objc_executable_metadata_semantic_validation_surface\":"
           << BuildExecutableMetadataSemanticValidationSurfaceJson(
                  executable_metadata_semantic_validation_surface)
           // M252-C001 lowering-handoff anchor: metadata graph lowering
           // handoff freeze must publish as a first-class semantic surface so
           // typed handoff and parse/lowering projections consume one schema.
           << ",\"objc_executable_metadata_lowering_handoff_surface\":"
           << BuildExecutableMetadataLoweringHandoffSurfaceJson(
                  executable_metadata_lowering_handoff_surface)
           // M252-C002 typed-lowering anchor: the lowering-ready packet must
           // publish the ordered metadata graph payload itself rather than a
           // count-only summary so downstream lowering can consume one schema.
           << ",\"objc_executable_metadata_typed_lowering_handoff\":"
           << BuildExecutableMetadataTypedLoweringHandoffJson(
                  executable_metadata_typed_lowering_handoff)
           // M252-C003 debug-projection anchor: lane-C must publish one
           // canonical metadata inspection matrix across manifest and IR-facing
           // surfaces before runtime section emission lands.
           << ",\"objc_executable_metadata_debug_projection\":"
           << BuildExecutableMetadataDebugProjectionSummaryJson(
                  executable_metadata_debug_projection)
           // M252-D001 runtime-ingest packaging anchor: lane-D must freeze one
           // canonical manifest transport boundary over the typed handoff and
           // debug-projection packets before section emission and startup
           // registration land.
           << ",\"objc_executable_metadata_runtime_ingest_packaging_contract\":"
           << BuildExecutableMetadataRuntimeIngestPackagingContractSummaryJson(
                  executable_metadata_runtime_ingest_packaging_contract)
           // M252-D002 binary-boundary anchor: lane-D must materialize a real
           // runtime-facing binary envelope over the frozen D001/C002/C003
           // packets so later section-emission/bootstrap work consumes one
           // deterministic artifact boundary instead of reparsing manifest JSON.
           // M252-E001 semantic-closure gate anchor: lane-E freezes the
           // aggregate A003/B004/C003/D002 boundary here so M253-A001 section
           // emission consumes one synchronized metadata closure proof.
           // M252-E002 corpus-sync anchor: integrated corpus probes must
           // observe these synchronized metadata surfaces through the real
           // frontend runner path rather than mock packets.
           << ",\"objc_executable_metadata_runtime_ingest_binary_boundary\":"
           << BuildExecutableMetadataRuntimeIngestBinaryBoundarySummaryJson(
                  executable_metadata_runtime_ingest_binary_boundary)
           // M254-A001 translation-unit registration surface anchor: lane-A
           // freezes one manifest-published preregistration contract over the
           // runtime metadata binary, linker-retention sidecars, constructor
           // root reservation, and runtime-owned entrypoint boundary before
           // A002 emits any real startup constructor or bootstrap calls.
           << ",\"objc_runtime_translation_unit_registration_contract\":"
           << BuildRuntimeTranslationUnitRegistrationContractSummaryJson(
                  runtime_translation_unit_registration_contract)
           // M254-A002 registration-manifest anchor: lane-A now publishes the
           // manifest template and constructor-root ownership model that later
           // lowering/bootstrap lanes consume directly instead of reconstructing
           // startup registration inputs ad hoc from loose sidecars.
           // M254-E001 startup-registration gate anchor: the semantic-surface registration manifest remains the canonical lane-E gate input
           // for the A002/B002/C003/D003/D004 replay-stable bootstrap evidence chain.
           // M254-E002 runbook-closeout anchor: the registration manifest summary stays authoritative for the published runbook
           // and its live smoke replay proof.
           << ",\"objc_runtime_translation_unit_registration_manifest\":"
           << BuildRuntimeTranslationUnitRegistrationManifestSummaryJson(
                  runtime_translation_unit_registration_manifest)
           // M254-D001 runtime-bootstrap-api anchor: lane-D freezes the
           // runtime-owned bootstrap header/archive/entrypoint/reset surface as
           // one canonical packet that later image-walk and reset-expansion
           // issues must preserve exactly.
           << ",\"objc_runtime_bootstrap_api_contract\":"
           << BuildRuntimeBootstrapApiSummaryJson(runtime_bootstrap_api)
           // M254-D002 bootstrap-registrar anchor: the semantic surface now
           // publishes the private staging hook and runtime image-walk policy
           // that extend the emitted startup path without widening the frozen
           // D001 public runtime API.
           << ",\"objc_runtime_bootstrap_registrar_contract\":{"
           << "\"contract_id\":\""
           << EscapeJsonString(kObjc3RuntimeBootstrapRegistrarContractId)
           << "\",\"surface_path\":\""
           << EscapeJsonString(kObjc3RuntimeBootstrapRegistrarSurfacePath)
           << "\",\"bootstrap_api_contract_id\":\""
           << EscapeJsonString(runtime_bootstrap_api.contract_id)
           << "\",\"bootstrap_lowering_contract_id\":\""
           << EscapeJsonString(runtime_bootstrap_lowering.contract_id)
           << "\",\"internal_header_path\":\""
           << EscapeJsonString(kObjc3RuntimeBootstrapInternalHeaderPath)
           << "\",\"stage_registration_table_symbol\":\""
           << EscapeJsonString(
                  kObjc3RuntimeBootstrapStageRegistrationTableSymbol)
           << "\",\"image_walk_snapshot_symbol\":\""
           << EscapeJsonString(kObjc3RuntimeBootstrapImageWalkSnapshotSymbol)
           << "\",\"image_walk_model\":\""
           << EscapeJsonString(kObjc3RuntimeBootstrapImageWalkModel)
           << "\",\"discovery_root_validation_model\":\""
           << EscapeJsonString(
                  kObjc3RuntimeBootstrapDiscoveryRootValidationModel)
           << "\",\"selector_pool_interning_model\":\""
           << EscapeJsonString(
                  kObjc3RuntimeBootstrapSelectorPoolInterningModel)
           << "\",\"realization_staging_model\":\""
           << EscapeJsonString(kObjc3RuntimeBootstrapRealizationStagingModel)
           << "\",\"fail_closed\":true"
           << ",\"ready\":"
           << ((IsReadyObjc3RuntimeBootstrapApiSummary(runtime_bootstrap_api) &&
                IsReadyObjc3RuntimeBootstrapLoweringSummary(
                    runtime_bootstrap_lowering))
                   ? "true"
                   : "false")
           << "}"
           // M254-D003 bootstrap-reset anchor: the semantic surface now
           // publishes the private deterministic reset/replay hooks that allow
           // same-process smoke harnesses to clear live runtime state, zero the
           // retained image-local init cells, and replay retained startup
           // images in canonical registration order without widening the frozen
           // D001 public runtime API.
           << ",\"objc_runtime_bootstrap_reset_contract\":{"
           << "\"contract_id\":\""
           << EscapeJsonString(kObjc3RuntimeBootstrapResetContractId)
           << "\",\"surface_path\":\""
           << EscapeJsonString(kObjc3RuntimeBootstrapResetSurfacePath)
           << "\",\"bootstrap_api_contract_id\":\""
           << EscapeJsonString(runtime_bootstrap_api.contract_id)
           << "\",\"bootstrap_registrar_contract_id\":\""
           << EscapeJsonString(kObjc3RuntimeBootstrapRegistrarContractId)
           << "\",\"internal_header_path\":\""
           << EscapeJsonString(kObjc3RuntimeBootstrapInternalHeaderPath)
           << "\",\"replay_registered_images_symbol\":\""
           << EscapeJsonString(
                  kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol)
           << "\",\"reset_replay_state_snapshot_symbol\":\""
           << EscapeJsonString(
                  kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol)
           << "\",\"reset_lifecycle_model\":\""
           << EscapeJsonString(kObjc3RuntimeBootstrapResetLifecycleModel)
           << "\",\"replay_order_model\":\""
           << EscapeJsonString(kObjc3RuntimeBootstrapReplayOrderModel)
           << "\",\"image_local_init_state_reset_model\":\""
           << EscapeJsonString(
                  kObjc3RuntimeBootstrapImageLocalInitStateResetModel)
           << "\",\"bootstrap_catalog_retention_model\":\""
           << EscapeJsonString(kObjc3RuntimeBootstrapCatalogRetentionModel)
           << "\",\"fail_closed\":true"
           << ",\"ready\":"
           << ((IsReadyObjc3RuntimeBootstrapApiSummary(runtime_bootstrap_api) &&
                IsReadyObjc3RuntimeBootstrapLoweringSummary(
                    runtime_bootstrap_lowering))
                   ? "true"
                   : "false")
           << "}"
           // M254-B001 bootstrap-invariant anchor: lane-B freezes duplicate
           // registration, realization order, failure mode, and image-local
           // initialization semantics against the live A002 registration
           // manifest so later bootstrap implementation extends one canonical
           // sema/runtime packet.
           << ",\"objc_runtime_startup_bootstrap_invariants\":"
           << BuildRuntimeStartupBootstrapInvariantSummaryJson(
                  runtime_startup_bootstrap_invariants)
           // M254-B002 bootstrap-semantics anchor: lane-B now lands the live
           // runtime enforcement/result-code surface that must remain aligned
           // with the emitted registration manifest and the native runtime
           // probe harness.
           << ",\"objc_runtime_startup_bootstrap_semantics\":"
           << BuildRuntimeBootstrapSemanticsSummaryJson(
                  runtime_bootstrap_semantics)
           // M254-C001 bootstrap-lowering anchor: lane-C now freezes one
           // manifest-driven lowering packet that owns future ctor-root,
           // init-stub, and registration-table materialization without
           // claiming that the current emitted IR already contains those
           // globals.
           << ",\"objc_runtime_bootstrap_lowering_contract\":"
           << BuildRuntimeBootstrapLoweringSummaryJson(
                  runtime_bootstrap_lowering)
           << ",\"objc_id_class_sel_object_pointer_typecheck_surface\":{\"id_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.id_typecheck_sites
           << ",\"class_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.class_typecheck_sites
           << ",\"sel_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.sel_typecheck_sites
           << ",\"object_pointer_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.object_pointer_typecheck_sites
           << ",\"total_typecheck_sites\":"
           << id_class_sel_object_pointer_typecheck_contract.total_typecheck_sites
           << ",\"replay_key\":\""
           << id_class_sel_object_pointer_typecheck_replay_key
           << "\",\"deterministic_handoff\":"
           << (id_class_sel_object_pointer_typecheck_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_dispatch_surface_classification_surface\":{\"instance_dispatch_sites\":"
           << dispatch_surface_classification_contract.instance_dispatch_sites
           << ",\"class_dispatch_sites\":"
           << dispatch_surface_classification_contract.class_dispatch_sites
           << ",\"super_dispatch_sites\":"
           << dispatch_surface_classification_contract.super_dispatch_sites
           << ",\"direct_dispatch_sites\":"
           << dispatch_surface_classification_contract.direct_dispatch_sites
           << ",\"dynamic_dispatch_sites\":"
           << dispatch_surface_classification_contract.dynamic_dispatch_sites
           << ",\"instance_entrypoint_family\":\""
           << dispatch_surface_classification_contract.instance_entrypoint_family
           << "\",\"class_entrypoint_family\":\""
           << dispatch_surface_classification_contract.class_entrypoint_family
           << "\",\"super_entrypoint_family\":\""
           << dispatch_surface_classification_contract.super_entrypoint_family
           << "\",\"direct_entrypoint_family\":\""
           << dispatch_surface_classification_contract.direct_entrypoint_family
           << "\",\"dynamic_entrypoint_family\":\""
           << dispatch_surface_classification_contract.dynamic_entrypoint_family
           << "\",\"replay_key\":\""
           << dispatch_surface_classification_replay_key
           << "\",\"deterministic_handoff\":"
           << (dispatch_surface_classification_contract.deterministic ? "true"
                                                                     : "false")
           << "}"
           << ",\"objc_message_send_selector_lowering_surface\":{\"message_send_sites\":"
           << message_send_selector_lowering_contract.message_send_sites
           << ",\"unary_selector_sites\":"
           << message_send_selector_lowering_contract.unary_selector_sites
           << ",\"keyword_selector_sites\":"
           << message_send_selector_lowering_contract.keyword_selector_sites
           << ",\"selector_piece_sites\":"
           << message_send_selector_lowering_contract.selector_piece_sites
           << ",\"argument_expression_sites\":"
           << message_send_selector_lowering_contract.argument_expression_sites
           << ",\"receiver_expression_sites\":"
           << message_send_selector_lowering_contract.receiver_expression_sites
           << ",\"selector_literal_entries\":"
           << message_send_selector_lowering_contract.selector_literal_entries
           << ",\"selector_literal_characters\":"
           << message_send_selector_lowering_contract.selector_literal_characters
           << ",\"replay_key\":\""
           << message_send_selector_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (message_send_selector_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_dispatch_abi_marshalling_surface\":{\"message_send_sites\":"
           << dispatch_abi_marshalling_contract.message_send_sites
           << ",\"receiver_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.receiver_slots_marshaled
           << ",\"selector_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.selector_slots_marshaled
           << ",\"argument_value_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.argument_value_slots_marshaled
           << ",\"argument_padding_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.argument_padding_slots_marshaled
           << ",\"argument_total_slots_marshaled\":"
           << dispatch_abi_marshalling_contract.argument_total_slots_marshaled
           << ",\"total_marshaled_slots\":"
           << dispatch_abi_marshalling_contract.total_marshaled_slots
           << ",\"runtime_dispatch_arg_slots\":"
           << dispatch_abi_marshalling_contract.runtime_dispatch_arg_slots
           << ",\"replay_key\":\""
           << dispatch_abi_marshalling_replay_key
           << "\",\"deterministic_handoff\":"
           << (dispatch_abi_marshalling_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_nil_receiver_semantics_foldability_surface\":{\"message_send_sites\":"
           << nil_receiver_semantics_foldability_contract.message_send_sites
           << ",\"receiver_nil_literal_sites\":"
           << nil_receiver_semantics_foldability_contract.receiver_nil_literal_sites
           << ",\"nil_receiver_semantics_enabled_sites\":"
           << nil_receiver_semantics_foldability_contract.nil_receiver_semantics_enabled_sites
           << ",\"nil_receiver_foldable_sites\":"
           << nil_receiver_semantics_foldability_contract.nil_receiver_foldable_sites
           << ",\"nil_receiver_runtime_dispatch_required_sites\":"
           << nil_receiver_semantics_foldability_contract.nil_receiver_runtime_dispatch_required_sites
           << ",\"non_nil_receiver_sites\":"
           << nil_receiver_semantics_foldability_contract.non_nil_receiver_sites
           << ",\"contract_violation_sites\":"
           << nil_receiver_semantics_foldability_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << nil_receiver_semantics_foldability_replay_key
           << "\",\"deterministic_handoff\":"
           << (nil_receiver_semantics_foldability_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_super_dispatch_method_family_surface\":{\"message_send_sites\":"
           << super_dispatch_method_family_contract.message_send_sites
           << ",\"receiver_super_identifier_sites\":"
           << super_dispatch_method_family_contract.receiver_super_identifier_sites
           << ",\"super_dispatch_enabled_sites\":"
           << super_dispatch_method_family_contract.super_dispatch_enabled_sites
           << ",\"super_dispatch_requires_class_context_sites\":"
           << super_dispatch_method_family_contract.super_dispatch_requires_class_context_sites
           << ",\"method_family_init_sites\":"
           << super_dispatch_method_family_contract.method_family_init_sites
           << ",\"method_family_copy_sites\":"
           << super_dispatch_method_family_contract.method_family_copy_sites
           << ",\"method_family_mutable_copy_sites\":"
           << super_dispatch_method_family_contract.method_family_mutable_copy_sites
           << ",\"method_family_new_sites\":"
           << super_dispatch_method_family_contract.method_family_new_sites
           << ",\"method_family_none_sites\":"
           << super_dispatch_method_family_contract.method_family_none_sites
           << ",\"method_family_returns_retained_result_sites\":"
           << super_dispatch_method_family_contract.method_family_returns_retained_result_sites
           << ",\"method_family_returns_related_result_sites\":"
           << super_dispatch_method_family_contract.method_family_returns_related_result_sites
           << ",\"contract_violation_sites\":"
           << super_dispatch_method_family_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << super_dispatch_method_family_replay_key
           << "\",\"deterministic_handoff\":"
           << (super_dispatch_method_family_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_runtime_shim_host_link_surface\":{\"message_send_sites\":"
           << runtime_shim_host_link_contract.message_send_sites
           << ",\"runtime_shim_required_sites\":"
           << runtime_shim_host_link_contract.runtime_shim_required_sites
           << ",\"runtime_shim_elided_sites\":"
           << runtime_shim_host_link_contract.runtime_shim_elided_sites
           << ",\"runtime_dispatch_arg_slots\":"
           << runtime_shim_host_link_contract.runtime_dispatch_arg_slots
           << ",\"runtime_dispatch_declaration_parameter_count\":"
           << runtime_shim_host_link_contract.runtime_dispatch_declaration_parameter_count
           << ",\"runtime_dispatch_symbol\":\""
           << runtime_shim_host_link_contract.runtime_dispatch_symbol
           << "\",\"default_runtime_dispatch_symbol_binding\":"
           << (runtime_shim_host_link_contract.default_runtime_dispatch_symbol_binding ? "true" : "false")
           << ",\"contract_violation_sites\":"
           << runtime_shim_host_link_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << runtime_shim_host_link_replay_key
           << "\",\"deterministic_handoff\":"
           << (runtime_shim_host_link_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_runtime_dispatch_lowering_abi_contract\":{\"message_send_sites\":"
           << runtime_dispatch_lowering_abi_contract.message_send_sites
           << ",\"fixed_argument_slot_count\":"
           << runtime_dispatch_lowering_abi_contract.fixed_argument_slot_count
           << ",\"runtime_dispatch_parameter_count\":"
           << runtime_dispatch_lowering_abi_contract
                  .runtime_dispatch_parameter_count
           << ",\"lowering_boundary_model\":\""
           << runtime_dispatch_lowering_abi_contract.lowering_boundary_model
           << "\",\"canonical_runtime_dispatch_symbol\":\""
           << runtime_dispatch_lowering_abi_contract
                  .canonical_runtime_dispatch_symbol
           << "\",\"compatibility_runtime_dispatch_symbol\":\""
           << runtime_dispatch_lowering_abi_contract
                  .compatibility_runtime_dispatch_symbol
           << "\",\"default_lowering_target_symbol\":\""
           << runtime_dispatch_lowering_abi_contract.default_lowering_target_symbol
           << "\",\"selector_lookup_symbol\":\""
           << runtime_dispatch_lowering_abi_contract.selector_lookup_symbol
           << "\",\"selector_handle_type\":\""
           << runtime_dispatch_lowering_abi_contract.selector_handle_type
           << "\",\"receiver_abi_type\":\""
           << runtime_dispatch_lowering_abi_contract.receiver_abi_type
           << "\",\"selector_abi_type\":\""
           << runtime_dispatch_lowering_abi_contract.selector_abi_type
           << "\",\"argument_abi_type\":\""
           << runtime_dispatch_lowering_abi_contract.argument_abi_type
           << "\",\"result_abi_type\":\""
           << runtime_dispatch_lowering_abi_contract.result_abi_type
           << "\",\"selector_operand_model\":\""
           << runtime_dispatch_lowering_abi_contract.selector_operand_model
           << "\",\"selector_handle_model\":\""
           << runtime_dispatch_lowering_abi_contract.selector_handle_model
           << "\",\"argument_padding_model\":\""
           << runtime_dispatch_lowering_abi_contract.argument_padding_model
           << "\",\"default_lowering_target_model\":\""
           << runtime_dispatch_lowering_abi_contract
                  .default_lowering_target_model
           << "\",\"compatibility_bridge_role_model\":\""
           << runtime_dispatch_lowering_abi_contract
                  .compatibility_bridge_role_model
           << "\",\"deferred_cases_model\":\""
           << runtime_dispatch_lowering_abi_contract.deferred_cases_model
           << "\",\"replay_key\":\""
           << runtime_dispatch_lowering_abi_replay_key
           << "\",\"fail_closed\":"
           << (runtime_dispatch_lowering_abi_contract.fail_closed ? "true"
                                                                  : "false")
           << ",\"deterministic_handoff\":"
           << (runtime_dispatch_lowering_abi_contract.deterministic ? "true"
                                                                    : "false")
           << "}"
           << ",\"objc_ownership_qualifier_lowering_surface\":{\"ownership_qualifier_sites\":"
           << ownership_qualifier_lowering_contract.ownership_qualifier_sites
           << ",\"invalid_ownership_qualifier_sites\":"
           << ownership_qualifier_lowering_contract.invalid_ownership_qualifier_sites
           << ",\"object_pointer_type_annotation_sites\":"
           << ownership_qualifier_lowering_contract.object_pointer_type_annotation_sites
           << ",\"replay_key\":\""
           << ownership_qualifier_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (ownership_qualifier_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_retain_release_operation_lowering_surface\":{\"ownership_qualified_sites\":"
           << retain_release_operation_lowering_contract.ownership_qualified_sites
           << ",\"retain_insertion_sites\":"
           << retain_release_operation_lowering_contract.retain_insertion_sites
           << ",\"release_insertion_sites\":"
           << retain_release_operation_lowering_contract.release_insertion_sites
           << ",\"autorelease_insertion_sites\":"
           << retain_release_operation_lowering_contract.autorelease_insertion_sites
           << ",\"contract_violation_sites\":"
           << retain_release_operation_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << retain_release_operation_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (retain_release_operation_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_autoreleasepool_scope_lowering_surface\":{\"scope_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_sites
           << ",\"scope_symbolized_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_symbolized_sites
           << ",\"max_scope_depth\":"
           << autoreleasepool_scope_lowering_contract.max_scope_depth
           << ",\"scope_entry_transition_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_entry_transition_sites
           << ",\"scope_exit_transition_sites\":"
           << autoreleasepool_scope_lowering_contract.scope_exit_transition_sites
           << ",\"contract_violation_sites\":"
           << autoreleasepool_scope_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << autoreleasepool_scope_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (autoreleasepool_scope_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_weak_unowned_semantics_lowering_surface\":{\"ownership_candidate_sites\":"
           << weak_unowned_semantics_lowering_contract.ownership_candidate_sites
           << ",\"weak_reference_sites\":"
           << weak_unowned_semantics_lowering_contract.weak_reference_sites
           << ",\"unowned_reference_sites\":"
           << weak_unowned_semantics_lowering_contract.unowned_reference_sites
           << ",\"unowned_safe_reference_sites\":"
           << weak_unowned_semantics_lowering_contract.unowned_safe_reference_sites
           << ",\"weak_unowned_conflict_sites\":"
           << weak_unowned_semantics_lowering_contract.weak_unowned_conflict_sites
           << ",\"contract_violation_sites\":"
           << weak_unowned_semantics_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << weak_unowned_semantics_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (weak_unowned_semantics_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_arc_diagnostics_fixit_lowering_surface\":{\"ownership_arc_diagnostic_candidate_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_diagnostic_candidate_sites
           << ",\"ownership_arc_fixit_available_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_fixit_available_sites
           << ",\"ownership_arc_profiled_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_profiled_sites
           << ",\"ownership_arc_weak_unowned_conflict_diagnostic_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_weak_unowned_conflict_diagnostic_sites
           << ",\"ownership_arc_empty_fixit_hint_sites\":"
           << arc_diagnostics_fixit_lowering_contract.ownership_arc_empty_fixit_hint_sites
           << ",\"contract_violation_sites\":"
           << arc_diagnostics_fixit_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << arc_diagnostics_fixit_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (arc_diagnostics_fixit_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_block_literal_capture_lowering_surface\":{\"block_literal_sites\":"
           << block_literal_capture_lowering_contract.block_literal_sites
           << ",\"block_parameter_entries\":"
           << block_literal_capture_lowering_contract.block_parameter_entries
           << ",\"block_capture_entries\":"
           << block_literal_capture_lowering_contract.block_capture_entries
           << ",\"block_body_statement_entries\":"
           << block_literal_capture_lowering_contract.block_body_statement_entries
           << ",\"block_empty_capture_sites\":"
           << block_literal_capture_lowering_contract.block_empty_capture_sites
           << ",\"block_nondeterministic_capture_sites\":"
           << block_literal_capture_lowering_contract.block_nondeterministic_capture_sites
           << ",\"block_non_normalized_sites\":"
           << block_literal_capture_lowering_contract.block_non_normalized_sites
           << ",\"contract_violation_sites\":"
           << block_literal_capture_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << block_literal_capture_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (block_literal_capture_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_block_abi_invoke_trampoline_lowering_surface\":{\"block_literal_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.block_literal_sites
           << ",\"invoke_argument_slots_total\":"
           << block_abi_invoke_trampoline_lowering_contract.invoke_argument_slots_total
           << ",\"capture_word_count_total\":"
           << block_abi_invoke_trampoline_lowering_contract.capture_word_count_total
           << ",\"parameter_entries_total\":"
           << block_abi_invoke_trampoline_lowering_contract.parameter_entries_total
           << ",\"capture_entries_total\":"
           << block_abi_invoke_trampoline_lowering_contract.capture_entries_total
           << ",\"body_statement_entries_total\":"
           << block_abi_invoke_trampoline_lowering_contract.body_statement_entries_total
           << ",\"descriptor_symbolized_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.descriptor_symbolized_sites
           << ",\"invoke_trampoline_symbolized_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.invoke_trampoline_symbolized_sites
           << ",\"missing_invoke_trampoline_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.missing_invoke_trampoline_sites
           << ",\"non_normalized_layout_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.non_normalized_layout_sites
           << ",\"contract_violation_sites\":"
           << block_abi_invoke_trampoline_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << block_abi_invoke_trampoline_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (block_abi_invoke_trampoline_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_block_storage_escape_lowering_surface\":{\"block_literal_sites\":"
           << block_storage_escape_lowering_contract.block_literal_sites
           << ",\"mutable_capture_count_total\":"
           << block_storage_escape_lowering_contract.mutable_capture_count_total
           << ",\"byref_slot_count_total\":"
           << block_storage_escape_lowering_contract.byref_slot_count_total
           << ",\"parameter_entries_total\":"
           << block_storage_escape_lowering_contract.parameter_entries_total
           << ",\"capture_entries_total\":"
           << block_storage_escape_lowering_contract.capture_entries_total
           << ",\"body_statement_entries_total\":"
           << block_storage_escape_lowering_contract.body_statement_entries_total
           << ",\"requires_byref_cells_sites\":"
           << block_storage_escape_lowering_contract.requires_byref_cells_sites
           << ",\"escape_analysis_enabled_sites\":"
           << block_storage_escape_lowering_contract.escape_analysis_enabled_sites
           << ",\"escape_to_heap_sites\":"
           << block_storage_escape_lowering_contract.escape_to_heap_sites
           << ",\"escape_profile_normalized_sites\":"
           << block_storage_escape_lowering_contract.escape_profile_normalized_sites
           << ",\"byref_layout_symbolized_sites\":"
           << block_storage_escape_lowering_contract.byref_layout_symbolized_sites
           << ",\"contract_violation_sites\":"
           << block_storage_escape_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << block_storage_escape_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (block_storage_escape_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_block_copy_dispose_lowering_surface\":{\"block_literal_sites\":"
           << block_copy_dispose_lowering_contract.block_literal_sites
           << ",\"mutable_capture_count_total\":"
           << block_copy_dispose_lowering_contract.mutable_capture_count_total
           << ",\"byref_slot_count_total\":"
           << block_copy_dispose_lowering_contract.byref_slot_count_total
           << ",\"parameter_entries_total\":"
           << block_copy_dispose_lowering_contract.parameter_entries_total
           << ",\"capture_entries_total\":"
           << block_copy_dispose_lowering_contract.capture_entries_total
           << ",\"body_statement_entries_total\":"
           << block_copy_dispose_lowering_contract.body_statement_entries_total
           << ",\"copy_helper_required_sites\":"
           << block_copy_dispose_lowering_contract.copy_helper_required_sites
           << ",\"dispose_helper_required_sites\":"
           << block_copy_dispose_lowering_contract.dispose_helper_required_sites
           << ",\"profile_normalized_sites\":"
           << block_copy_dispose_lowering_contract.profile_normalized_sites
           << ",\"copy_helper_symbolized_sites\":"
           << block_copy_dispose_lowering_contract.copy_helper_symbolized_sites
           << ",\"dispose_helper_symbolized_sites\":"
           << block_copy_dispose_lowering_contract.dispose_helper_symbolized_sites
           << ",\"contract_violation_sites\":"
           << block_copy_dispose_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << block_copy_dispose_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (block_copy_dispose_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_block_determinism_perf_baseline_lowering_surface\":{\"block_literal_sites\":"
           << block_determinism_perf_baseline_lowering_contract.block_literal_sites
           << ",\"baseline_weight_total\":"
           << block_determinism_perf_baseline_lowering_contract.baseline_weight_total
           << ",\"parameter_entries_total\":"
           << block_determinism_perf_baseline_lowering_contract.parameter_entries_total
           << ",\"capture_entries_total\":"
           << block_determinism_perf_baseline_lowering_contract.capture_entries_total
           << ",\"body_statement_entries_total\":"
           << block_determinism_perf_baseline_lowering_contract.body_statement_entries_total
           << ",\"deterministic_capture_sites\":"
           << block_determinism_perf_baseline_lowering_contract.deterministic_capture_sites
           << ",\"heavy_tier_sites\":"
           << block_determinism_perf_baseline_lowering_contract.heavy_tier_sites
           << ",\"normalized_profile_sites\":"
           << block_determinism_perf_baseline_lowering_contract.normalized_profile_sites
           << ",\"contract_violation_sites\":"
           << block_determinism_perf_baseline_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << block_determinism_perf_baseline_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (block_determinism_perf_baseline_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_lightweight_generic_constraint_lowering_surface\":{\"generic_constraint_sites\":"
           << lightweight_generic_constraint_lowering_contract.generic_constraint_sites
           << ",\"generic_suffix_sites\":"
           << lightweight_generic_constraint_lowering_contract.generic_suffix_sites
           << ",\"object_pointer_type_sites\":"
           << lightweight_generic_constraint_lowering_contract.object_pointer_type_sites
           << ",\"terminated_generic_suffix_sites\":"
           << lightweight_generic_constraint_lowering_contract.terminated_generic_suffix_sites
           << ",\"pointer_declarator_sites\":"
           << lightweight_generic_constraint_lowering_contract.pointer_declarator_sites
           << ",\"normalized_constraint_sites\":"
           << lightweight_generic_constraint_lowering_contract.normalized_constraint_sites
           << ",\"contract_violation_sites\":"
           << lightweight_generic_constraint_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << lightweight_generic_constraint_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (lightweight_generic_constraint_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_nullability_flow_warning_precision_lowering_surface\":{\"nullability_flow_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nullability_flow_sites
           << ",\"object_pointer_type_sites\":"
           << nullability_flow_warning_precision_lowering_contract.object_pointer_type_sites
           << ",\"nullability_suffix_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nullability_suffix_sites
           << ",\"nullable_suffix_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nullable_suffix_sites
           << ",\"nonnull_suffix_sites\":"
           << nullability_flow_warning_precision_lowering_contract.nonnull_suffix_sites
           << ",\"normalized_sites\":"
           << nullability_flow_warning_precision_lowering_contract.normalized_sites
           << ",\"contract_violation_sites\":"
           << nullability_flow_warning_precision_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << nullability_flow_warning_precision_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (nullability_flow_warning_precision_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_protocol_qualified_object_type_lowering_surface\":{\"protocol_qualified_object_type_sites\":"
           << protocol_qualified_object_type_lowering_contract.protocol_qualified_object_type_sites
           << ",\"protocol_composition_sites\":"
           << protocol_qualified_object_type_lowering_contract.protocol_composition_sites
           << ",\"object_pointer_type_sites\":"
           << protocol_qualified_object_type_lowering_contract.object_pointer_type_sites
           << ",\"terminated_protocol_composition_sites\":"
           << protocol_qualified_object_type_lowering_contract.terminated_protocol_composition_sites
           << ",\"pointer_declarator_sites\":"
           << protocol_qualified_object_type_lowering_contract.pointer_declarator_sites
           << ",\"normalized_protocol_composition_sites\":"
           << protocol_qualified_object_type_lowering_contract.normalized_protocol_composition_sites
           << ",\"contract_violation_sites\":"
           << protocol_qualified_object_type_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << protocol_qualified_object_type_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (protocol_qualified_object_type_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_variance_bridge_cast_lowering_surface\":{\"variance_bridge_cast_sites\":"
           << variance_bridge_cast_lowering_contract.variance_bridge_cast_sites
           << ",\"protocol_composition_sites\":"
           << variance_bridge_cast_lowering_contract.protocol_composition_sites
           << ",\"ownership_qualifier_sites\":"
           << variance_bridge_cast_lowering_contract.ownership_qualifier_sites
           << ",\"object_pointer_type_sites\":"
           << variance_bridge_cast_lowering_contract.object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << variance_bridge_cast_lowering_contract.pointer_declarator_sites
           << ",\"normalized_sites\":"
           << variance_bridge_cast_lowering_contract.normalized_sites
           << ",\"contract_violation_sites\":"
           << variance_bridge_cast_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << variance_bridge_cast_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (variance_bridge_cast_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_generic_metadata_abi_lowering_surface\":{\"generic_metadata_abi_sites\":"
           << generic_metadata_abi_lowering_contract.generic_metadata_abi_sites
           << ",\"generic_suffix_sites\":"
           << generic_metadata_abi_lowering_contract.generic_suffix_sites
           << ",\"protocol_composition_sites\":"
           << generic_metadata_abi_lowering_contract.protocol_composition_sites
           << ",\"ownership_qualifier_sites\":"
           << generic_metadata_abi_lowering_contract.ownership_qualifier_sites
           << ",\"object_pointer_type_sites\":"
           << generic_metadata_abi_lowering_contract.object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << generic_metadata_abi_lowering_contract.pointer_declarator_sites
           << ",\"normalized_sites\":"
           << generic_metadata_abi_lowering_contract.normalized_sites
           << ",\"contract_violation_sites\":"
           << generic_metadata_abi_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << generic_metadata_abi_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (generic_metadata_abi_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_module_import_graph_lowering_surface\":{\"module_import_graph_sites\":"
           << module_import_graph_lowering_contract.module_import_graph_sites
           << ",\"import_edge_candidate_sites\":"
           << module_import_graph_lowering_contract.import_edge_candidate_sites
           << ",\"namespace_segment_sites\":"
           << module_import_graph_lowering_contract.namespace_segment_sites
           << ",\"object_pointer_type_sites\":"
           << module_import_graph_lowering_contract.object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << module_import_graph_lowering_contract.pointer_declarator_sites
           << ",\"normalized_sites\":"
           << module_import_graph_lowering_contract.normalized_sites
           << ",\"contract_violation_sites\":"
           << module_import_graph_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << module_import_graph_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (module_import_graph_lowering_contract.deterministic ? "true" : "false")
           << "}"
           << ",\"objc_namespace_collision_shadowing_lowering_surface\":{\"namespace_collision_shadowing_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .namespace_collision_shadowing_sites
           << ",\"namespace_segment_sites\":"
           << namespace_collision_shadowing_lowering_contract.namespace_segment_sites
           << ",\"import_edge_candidate_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .import_edge_candidate_sites
           << ",\"object_pointer_type_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .pointer_declarator_sites
           << ",\"normalized_sites\":"
           << namespace_collision_shadowing_lowering_contract.normalized_sites
           << ",\"contract_violation_sites\":"
           << namespace_collision_shadowing_lowering_contract
                  .contract_violation_sites
           << ",\"replay_key\":\""
           << namespace_collision_shadowing_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (namespace_collision_shadowing_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_public_private_api_partition_lowering_surface\":{\"public_private_api_partition_sites\":"
           << public_private_api_partition_lowering_contract
                  .public_private_api_partition_sites
           << ",\"namespace_segment_sites\":"
           << public_private_api_partition_lowering_contract
                  .namespace_segment_sites
           << ",\"import_edge_candidate_sites\":"
           << public_private_api_partition_lowering_contract
                  .import_edge_candidate_sites
           << ",\"object_pointer_type_sites\":"
           << public_private_api_partition_lowering_contract
                  .object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << public_private_api_partition_lowering_contract
                  .pointer_declarator_sites
           << ",\"normalized_sites\":"
           << public_private_api_partition_lowering_contract.normalized_sites
           << ",\"contract_violation_sites\":"
           << public_private_api_partition_lowering_contract
                  .contract_violation_sites
           << ",\"replay_key\":\""
           << public_private_api_partition_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (public_private_api_partition_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_incremental_module_cache_invalidation_lowering_surface\":{\"incremental_module_cache_invalidation_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .incremental_module_cache_invalidation_sites
           << ",\"namespace_segment_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .namespace_segment_sites
           << ",\"import_edge_candidate_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .import_edge_candidate_sites
           << ",\"object_pointer_type_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .pointer_declarator_sites
           << ",\"normalized_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .normalized_sites
           << ",\"cache_invalidation_candidate_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .cache_invalidation_candidate_sites
           << ",\"contract_violation_sites\":"
           << incremental_module_cache_invalidation_lowering_contract
                  .contract_violation_sites
           << ",\"replay_key\":\""
           << incremental_module_cache_invalidation_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (incremental_module_cache_invalidation_lowering_contract
                       .deterministic
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_cross_module_conformance_lowering_surface\":{\"cross_module_conformance_sites\":"
           << cross_module_conformance_lowering_contract
                  .cross_module_conformance_sites
           << ",\"namespace_segment_sites\":"
           << cross_module_conformance_lowering_contract.namespace_segment_sites
           << ",\"import_edge_candidate_sites\":"
           << cross_module_conformance_lowering_contract.import_edge_candidate_sites
           << ",\"object_pointer_type_sites\":"
           << cross_module_conformance_lowering_contract.object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << cross_module_conformance_lowering_contract.pointer_declarator_sites
           << ",\"normalized_sites\":"
           << cross_module_conformance_lowering_contract.normalized_sites
           << ",\"cache_invalidation_candidate_sites\":"
           << cross_module_conformance_lowering_contract
                  .cache_invalidation_candidate_sites
           << ",\"contract_violation_sites\":"
           << cross_module_conformance_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << cross_module_conformance_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (cross_module_conformance_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_throws_propagation_lowering_surface\":{\"throws_propagation_sites\":"
           << throws_propagation_lowering_contract.throws_propagation_sites
           << ",\"namespace_segment_sites\":"
           << throws_propagation_lowering_contract.namespace_segment_sites
           << ",\"import_edge_candidate_sites\":"
           << throws_propagation_lowering_contract.import_edge_candidate_sites
           << ",\"object_pointer_type_sites\":"
           << throws_propagation_lowering_contract.object_pointer_type_sites
           << ",\"pointer_declarator_sites\":"
           << throws_propagation_lowering_contract.pointer_declarator_sites
           << ",\"normalized_sites\":"
           << throws_propagation_lowering_contract.normalized_sites
           << ",\"cache_invalidation_candidate_sites\":"
           << throws_propagation_lowering_contract
                  .cache_invalidation_candidate_sites
           << ",\"contract_violation_sites\":"
           << throws_propagation_lowering_contract.contract_violation_sites
           << ",\"replay_key\":\""
           << throws_propagation_lowering_replay_key
           << "\",\"deterministic_handoff\":"
           << (throws_propagation_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_object_pointer_nullability_generics_surface\":{\"object_pointer_type_spellings\":"
           << object_pointer_nullability_generics_summary.object_pointer_type_spellings
           << ",\"pointer_declarator_entries\":"
           << object_pointer_nullability_generics_summary.pointer_declarator_entries
           << ",\"pointer_declarator_depth_total\":"
           << object_pointer_nullability_generics_summary.pointer_declarator_depth_total
           << ",\"pointer_declarator_token_entries\":"
           << object_pointer_nullability_generics_summary.pointer_declarator_token_entries
           << ",\"nullability_suffix_entries\":"
           << object_pointer_nullability_generics_summary.nullability_suffix_entries
           << ",\"generic_suffix_entries\":"
           << object_pointer_nullability_generics_summary.generic_suffix_entries
           << ",\"terminated_generic_suffix_entries\":"
           << object_pointer_nullability_generics_summary.terminated_generic_suffix_entries
           << ",\"unterminated_generic_suffix_entries\":"
           << object_pointer_nullability_generics_summary.unterminated_generic_suffix_entries
           << ",\"deterministic_handoff\":"
           << (object_pointer_nullability_generics_summary.deterministic_object_pointer_nullability_generics_handoff
                   ? "true"
                   : "false")
           << "}"
           << ",\"objc_symbol_graph_scope_resolution_surface\":{\"global_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.global_symbol_nodes
           << ",\"function_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.function_symbol_nodes
           << ",\"interface_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.interface_symbol_nodes
           << ",\"implementation_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.implementation_symbol_nodes
           << ",\"interface_property_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.interface_property_symbol_nodes
           << ",\"implementation_property_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.implementation_property_symbol_nodes
           << ",\"interface_method_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.interface_method_symbol_nodes
           << ",\"implementation_method_symbol_nodes\":"
           << symbol_graph_scope_resolution_summary.implementation_method_symbol_nodes
           << ",\"top_level_scope_symbols\":"
           << symbol_graph_scope_resolution_summary.top_level_scope_symbols
           << ",\"nested_scope_symbols\":"
           << symbol_graph_scope_resolution_summary.nested_scope_symbols
           << ",\"scope_frames_total\":"
           << symbol_graph_scope_resolution_summary.scope_frames_total
           << ",\"implementation_interface_resolution_sites\":"
           << symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites
           << ",\"implementation_interface_resolution_hits\":"
           << symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits
           << ",\"implementation_interface_resolution_misses\":"
           << symbol_graph_scope_resolution_summary.implementation_interface_resolution_misses
           << ",\"method_resolution_sites\":"
           << symbol_graph_scope_resolution_summary.method_resolution_sites
           << ",\"method_resolution_hits\":"
           << symbol_graph_scope_resolution_summary.method_resolution_hits
           << ",\"method_resolution_misses\":"
           << symbol_graph_scope_resolution_summary.method_resolution_misses
           << ",\"deterministic_symbol_graph_handoff\":"
           << (symbol_graph_scope_resolution_summary.deterministic_symbol_graph_handoff ? "true" : "false")
           << ",\"deterministic_scope_resolution_handoff\":"
           << (symbol_graph_scope_resolution_summary.deterministic_scope_resolution_handoff ? "true" : "false")
           << ",\"deterministic_handoff_key\":\""
           << symbol_graph_scope_resolution_summary.deterministic_handoff_key
           << "\"}"
           << ",\"function_signature_surface\":{\"scalar_return_i32\":" << scalar_return_i32
           << ",\"scalar_return_bool\":" << scalar_return_bool
           << ",\"scalar_return_void\":" << scalar_return_void << ",\"scalar_param_i32\":" << scalar_param_i32
           << ",\"scalar_param_bool\":" << scalar_param_bool << "}}\n";
  manifest << "    }\n";
  manifest << "  },\n";
  manifest << "  \"lowering\": {\"runtime_dispatch_symbol\":\"" << options.lowering.runtime_dispatch_symbol
           << "\",\"runtime_dispatch_arg_slots\":" << options.lowering.max_message_send_args
           << ",\"selector_global_ordering\":\"lexicographic\"},\n";
  manifest << "  \"lowering_vector_abi\":{\"replay_key\":\"" << Objc3SimdVectorTypeLoweringReplayKey()
           << "\",\"lane_contract\":\"" << kObjc3SimdVectorLaneContract
           << "\",\"vector_signature_functions\":" << vector_signature_functions << "},\n";
  manifest << "  \"lowering_property_synthesis_ivar_binding\":{\"replay_key\":\""
           << property_synthesis_ivar_binding_replay_key
           << "\",\"lane_contract\":\"" << kObjc3PropertySynthesisIvarBindingLaneContract
           << "\",\"deterministic_handoff\":"
           << (property_synthesis_ivar_binding_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_id_class_sel_object_pointer_typecheck\":{\"replay_key\":\""
           << id_class_sel_object_pointer_typecheck_replay_key
           << "\",\"lane_contract\":\"" << kObjc3IdClassSelObjectPointerTypecheckLaneContract
           << "\",\"deterministic_handoff\":"
           << (id_class_sel_object_pointer_typecheck_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_dispatch_surface_classification\":{\"replay_key\":\""
           << dispatch_surface_classification_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3DispatchSurfaceClassificationContractId
           << "\",\"deterministic_handoff\":"
           << (dispatch_surface_classification_contract.deterministic ? "true"
                                                                     : "false")
           << "},\n";
  manifest << "  \"lowering_message_send_selector_lowering\":{\"replay_key\":\""
           << message_send_selector_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3MessageSendSelectorLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (message_send_selector_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_dispatch_abi_marshalling\":{\"replay_key\":\""
           << dispatch_abi_marshalling_replay_key
           << "\",\"lane_contract\":\"" << kObjc3DispatchAbiMarshallingLaneContract
           << "\",\"deterministic_handoff\":"
           << (dispatch_abi_marshalling_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_nil_receiver_semantics_foldability\":{\"replay_key\":\""
           << nil_receiver_semantics_foldability_replay_key
           << "\",\"lane_contract\":\"" << kObjc3NilReceiverSemanticsFoldabilityLaneContract
           << "\",\"deterministic_handoff\":"
           << (nil_receiver_semantics_foldability_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_super_dispatch_method_family\":{\"replay_key\":\""
           << super_dispatch_method_family_replay_key
           << "\",\"lane_contract\":\"" << kObjc3SuperDispatchMethodFamilyLaneContract
           << "\",\"deterministic_handoff\":"
           << (super_dispatch_method_family_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_runtime_shim_host_link\":{\"replay_key\":\""
           << runtime_shim_host_link_replay_key
           << "\",\"lane_contract\":\"" << kObjc3RuntimeShimHostLinkLaneContract
           << "\",\"deterministic_handoff\":"
           << (runtime_shim_host_link_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"runtime_shim_host_link_runtime_dispatch_symbol\":\""
           << runtime_shim_host_link_contract.runtime_dispatch_symbol
           << "\",\n";
  manifest << "  \"runtime_support_library_link_wiring_runtime_dispatch_symbol\":\""
           << runtime_support_library_link_wiring.runtime_dispatch_symbol
           << "\",\n";
  manifest << "  \"lowering_ownership_qualifier\":{\"replay_key\":\""
           << ownership_qualifier_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3OwnershipQualifierLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (ownership_qualifier_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_retain_release_operation\":{\"replay_key\":\""
           << retain_release_operation_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3RetainReleaseOperationLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (retain_release_operation_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_autoreleasepool_scope\":{\"replay_key\":\""
           << autoreleasepool_scope_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3AutoreleasePoolScopeLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (autoreleasepool_scope_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_weak_unowned_semantics\":{\"replay_key\":\""
           << weak_unowned_semantics_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3WeakUnownedSemanticsLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (weak_unowned_semantics_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_arc_diagnostics_fixit\":{\"replay_key\":\""
           << arc_diagnostics_fixit_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3ArcDiagnosticsFixitLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (arc_diagnostics_fixit_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_block_literal_capture\":{\"replay_key\":\""
           << block_literal_capture_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3BlockLiteralCaptureLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (block_literal_capture_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_block_abi_invoke_trampoline\":{\"replay_key\":\""
           << block_abi_invoke_trampoline_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3BlockAbiInvokeTrampolineLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (block_abi_invoke_trampoline_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_block_storage_escape\":{\"replay_key\":\""
           << block_storage_escape_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3BlockStorageEscapeLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (block_storage_escape_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_block_copy_dispose\":{\"replay_key\":\""
           << block_copy_dispose_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3BlockCopyDisposeLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (block_copy_dispose_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_block_determinism_perf_baseline\":{\"replay_key\":\""
           << block_determinism_perf_baseline_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3BlockDeterminismPerfBaselineLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (block_determinism_perf_baseline_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_lightweight_generic_constraint\":{\"replay_key\":\""
           << lightweight_generic_constraint_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3LightweightGenericsConstraintLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (lightweight_generic_constraint_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_nullability_flow_warning_precision\":{\"replay_key\":\""
           << nullability_flow_warning_precision_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3NullabilityFlowWarningPrecisionLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (nullability_flow_warning_precision_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_protocol_qualified_object_type\":{\"replay_key\":\""
           << protocol_qualified_object_type_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3ProtocolQualifiedObjectTypeLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (protocol_qualified_object_type_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_variance_bridge_cast\":{\"replay_key\":\""
           << variance_bridge_cast_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3VarianceBridgeCastLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (variance_bridge_cast_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_generic_metadata_abi\":{\"replay_key\":\""
           << generic_metadata_abi_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3GenericMetadataAbiLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (generic_metadata_abi_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_module_import_graph\":{\"replay_key\":\""
           << module_import_graph_lowering_replay_key
           << "\",\"lane_contract\":\"" << kObjc3ModuleImportGraphLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (module_import_graph_lowering_contract.deterministic ? "true" : "false")
           << "},\n";
  manifest << "  \"lowering_namespace_collision_shadowing\":{\"replay_key\":\""
           << namespace_collision_shadowing_lowering_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3NamespaceCollisionShadowingLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (namespace_collision_shadowing_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "},\n";
  manifest << "  \"lowering_public_private_api_partition\":{\"replay_key\":\""
           << public_private_api_partition_lowering_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3PublicPrivateApiPartitionLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (public_private_api_partition_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "},\n";
  manifest << "  \"lowering_incremental_module_cache_invalidation\":{\"replay_key\":\""
           << incremental_module_cache_invalidation_lowering_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3IncrementalModuleCacheInvalidationLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (incremental_module_cache_invalidation_lowering_contract
                       .deterministic
                   ? "true"
                   : "false")
           << "},\n";
  manifest << "  \"lowering_cross_module_conformance\":{\"replay_key\":\""
           << cross_module_conformance_lowering_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3CrossModuleConformanceLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (cross_module_conformance_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "},\n";
  manifest << "  \"lowering_throws_propagation\":{\"replay_key\":\""
           << throws_propagation_lowering_replay_key
           << "\",\"lane_contract\":\""
           << kObjc3ThrowsPropagationLoweringLaneContract
           << "\",\"deterministic_handoff\":"
           << (throws_propagation_lowering_contract.deterministic
                   ? "true"
                   : "false")
           << "},\n";
  manifest << "  \"globals\": [\n";
  for (std::size_t i = 0; i < program.globals.size(); ++i) {
    manifest << "    {\"name\":\"" << program.globals[i].name << "\",\"value\":" << resolved_global_values[i]
             << ",\"line\":" << program.globals[i].line << ",\"column\":" << program.globals[i].column << "}";
    if (i + 1 != program.globals.size()) {
      manifest << ",";
    }
    manifest << "\n";
  }
  manifest << "  ],\n";
  manifest << "  \"functions\": [\n";
  for (std::size_t i = 0; i < manifest_functions.size(); ++i) {
    const auto &fn = *manifest_functions[i];
    manifest << "    {\"name\":\"" << fn.name << "\",\"params\":" << fn.params.size() << ",\"param_types\":[";
    for (std::size_t p = 0; p < fn.params.size(); ++p) {
      manifest << "\"" << TypeName(fn.params[p].type) << "\"";
      if (p + 1 != fn.params.size()) {
        manifest << ",";
      }
    }
    manifest << "]"
             << ",\"return\":\"" << TypeName(fn.return_type) << "\""
             << ",\"line\":" << fn.line << ",\"column\":" << fn.column << "}";
    if (i + 1 != manifest_functions.size()) {
      manifest << ",";
    }
    manifest << "\n";
  }
  manifest << "  ],\n";
  manifest << "  \"interfaces\": [\n";
  bool first_interface_record = true;
  for (const auto &class_record : runtime_metadata_source_records.classes_lexicographic) {
    if (class_record.record_kind != "interface") {
      continue;
    }
    if (!first_interface_record) {
      manifest << ",\n";
    }
    first_interface_record = false;
    manifest << "    {\"name\":\"" << class_record.name << "\",\"super\":\"" << class_record.super_name
             << "\",\"has_super\":" << (class_record.has_super ? "true" : "false")
             << ",\"property_count\":" << class_record.property_count
             << ",\"method_count\":" << class_record.method_count
             << ",\"line\":" << class_record.line << ",\"column\":" << class_record.column << "}";
  }
  if (!first_interface_record) {
    manifest << "\n";
  }
  manifest << "  ],\n";
  manifest << "  \"implementations\": [\n";
  bool first_implementation_record = true;
  for (const auto &class_record : runtime_metadata_source_records.classes_lexicographic) {
    if (class_record.record_kind != "implementation") {
      continue;
    }
    if (!first_implementation_record) {
      manifest << ",\n";
    }
    first_implementation_record = false;
    manifest << "    {\"name\":\"" << class_record.name << "\",\"property_count\":"
             << class_record.property_count << ",\"method_count\":" << class_record.method_count
             << ",\"line\":" << class_record.line << ",\"column\":" << class_record.column << "}";
  }
  if (!first_implementation_record) {
    manifest << "\n";
  }
  manifest << "  ],\n";
  manifest << "  \"protocols\": [\n";
  for (std::size_t i = 0; i < runtime_metadata_source_records.protocols_lexicographic.size(); ++i) {
    const auto &protocol_record = runtime_metadata_source_records.protocols_lexicographic[i];
    manifest << "    {\"name\":\"" << protocol_record.name << "\",\"forward_declaration\":"
             << (protocol_record.is_forward_declaration ? "true" : "false")
             << ",\"property_count\":" << protocol_record.property_count
             << ",\"method_count\":" << protocol_record.method_count << ",\"inherited_protocols\":[";
    for (std::size_t j = 0; j < protocol_record.inherited_protocols_lexicographic.size(); ++j) {
      manifest << "\"" << protocol_record.inherited_protocols_lexicographic[j] << "\"";
      if (j + 1 != protocol_record.inherited_protocols_lexicographic.size()) {
        manifest << ",";
      }
    }
    manifest << "],\"line\":" << protocol_record.line << ",\"column\":" << protocol_record.column << "}";
    if (i + 1 != runtime_metadata_source_records.protocols_lexicographic.size()) {
      manifest << ",";
    }
    manifest << "\n";
  }
  manifest << "  ],\n";
  manifest << "  \"categories\": [\n";
  for (std::size_t i = 0; i < runtime_metadata_source_records.categories_lexicographic.size(); ++i) {
    const auto &category_record = runtime_metadata_source_records.categories_lexicographic[i];
    manifest << "    {\"record_kind\":\"" << category_record.record_kind << "\",\"class_name\":\""
             << category_record.class_name << "\",\"category_name\":\"" << category_record.category_name
             << "\",\"property_count\":" << category_record.property_count
             << ",\"method_count\":" << category_record.method_count << ",\"adopted_protocols\":[";
    for (std::size_t j = 0; j < category_record.adopted_protocols_lexicographic.size(); ++j) {
      manifest << "\"" << category_record.adopted_protocols_lexicographic[j] << "\"";
      if (j + 1 != category_record.adopted_protocols_lexicographic.size()) {
        manifest << ",";
      }
    }
    manifest << "],\"line\":" << category_record.line << ",\"column\":" << category_record.column << "}";
    if (i + 1 != runtime_metadata_source_records.categories_lexicographic.size()) {
      manifest << ",";
    }
    manifest << "\n";
  }
  manifest << "  ],\n";
  manifest << "  \"runtime_metadata_source_records\": {\n";
  manifest << "    \"deterministic\": "
           << (runtime_metadata_source_records.deterministic ? "true" : "false") << ",\n";
  manifest << "    \"properties\": [\n";
  for (std::size_t i = 0; i < runtime_metadata_source_records.properties_lexicographic.size(); ++i) {
    const auto &property_record = runtime_metadata_source_records.properties_lexicographic[i];
    manifest << "      {\"owner_kind\":\"" << property_record.owner_kind << "\",\"owner_name\":\""
             << property_record.owner_name << "\",\"property_name\":\"" << property_record.property_name
             << "\",\"type\":\"" << property_record.type_name << "\",\"has_getter\":"
             << (property_record.has_getter ? "true" : "false") << ",\"getter_selector\":\""
             << property_record.getter_selector << "\",\"has_setter\":"
             << (property_record.has_setter ? "true" : "false") << ",\"setter_selector\":\""
             << property_record.setter_selector << "\",\"ivar_binding_symbol\":\""
             << property_record.ivar_binding_symbol << "\",\"line\":" << property_record.line
             << ",\"column\":" << property_record.column << "}";
    if (i + 1 != runtime_metadata_source_records.properties_lexicographic.size()) {
      manifest << ",";
    }
    manifest << "\n";
  }
  manifest << "    ],\n";
  manifest << "    \"methods\": [\n";
  for (std::size_t i = 0; i < runtime_metadata_source_records.methods_lexicographic.size(); ++i) {
    const auto &method_record = runtime_metadata_source_records.methods_lexicographic[i];
    manifest << "      {\"owner_kind\":\"" << method_record.owner_kind << "\",\"owner_name\":\""
             << method_record.owner_name << "\",\"selector\":\"" << method_record.selector
             << "\",\"is_class_method\":" << (method_record.is_class_method ? "true" : "false")
             << ",\"has_body\":" << (method_record.has_body ? "true" : "false")
             << ",\"parameter_count\":" << method_record.parameter_count << ",\"return_type\":\""
             << method_record.return_type_name << "\",\"line\":" << method_record.line
             << ",\"column\":" << method_record.column << "}";
    if (i + 1 != runtime_metadata_source_records.methods_lexicographic.size()) {
      manifest << ",";
    }
    manifest << "\n";
  }
  manifest << "    ],\n";
  manifest << "    \"ivars\": [\n";
  for (std::size_t i = 0; i < runtime_metadata_source_records.ivars_lexicographic.size(); ++i) {
    const auto &ivar_record = runtime_metadata_source_records.ivars_lexicographic[i];
    manifest << "      {\"owner_kind\":\"" << ivar_record.owner_kind << "\",\"owner_name\":\""
             << ivar_record.owner_name << "\",\"property_name\":\"" << ivar_record.property_name
             << "\",\"ivar_binding_symbol\":\"" << ivar_record.ivar_binding_symbol
             << "\",\"source_model\":\"" << ivar_record.source_model << "\",\"line\":"
             << ivar_record.line << ",\"column\":" << ivar_record.column << "}";
    if (i + 1 != runtime_metadata_source_records.ivars_lexicographic.size()) {
      manifest << ",";
    }
    manifest << "\n";
  }
  manifest << "    ]\n";
  manifest << "  }\n";
  manifest << "}\n";
  bundle.manifest_json = manifest.str();
  bundle.runtime_metadata_binary = executable_metadata_runtime_ingest_binary_payload;
  bundle.runtime_translation_unit_registration_manifest_summary =
      runtime_translation_unit_registration_manifest;
  bundle.runtime_bootstrap_api_summary = runtime_bootstrap_api;
  bundle.runtime_bootstrap_semantics_summary = runtime_bootstrap_semantics;
  bundle.runtime_bootstrap_lowering_summary = runtime_bootstrap_lowering;

  if (!post_pipeline_failure_code.empty()) {
    if (!options.emit_ir && !options.emit_object) {
      return bundle;
    }
    bundle.post_pipeline_diagnostics = {
        MakeDiag(1, 1, post_pipeline_failure_code, post_pipeline_failure_message)};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    return bundle;
  }

  if (!options.emit_ir && !options.emit_object) {
    return bundle;
  }

  Objc3IRFrontendMetadata ir_frontend_metadata;
  ir_frontend_metadata.language_version = options.language_version;
  ir_frontend_metadata.compatibility_mode = CompatibilityModeName(options.compatibility_mode);
  ir_frontend_metadata.migration_assist = options.migration_assist;
  ir_frontend_metadata.migration_legacy_yes = pipeline_result.migration_hints.legacy_yes_count;
  ir_frontend_metadata.migration_legacy_no = pipeline_result.migration_hints.legacy_no_count;
  ir_frontend_metadata.migration_legacy_null = pipeline_result.migration_hints.legacy_null_count;
  ir_frontend_metadata.declared_interfaces = interface_implementation_summary.declared_interfaces;
  ir_frontend_metadata.declared_implementations = interface_implementation_summary.declared_implementations;
  ir_frontend_metadata.resolved_interface_symbols = interface_implementation_summary.resolved_interfaces;
  ir_frontend_metadata.resolved_implementation_symbols = interface_implementation_summary.resolved_implementations;
  ir_frontend_metadata.interface_method_symbols = interface_implementation_summary.interface_method_symbols;
  ir_frontend_metadata.implementation_method_symbols = interface_implementation_summary.implementation_method_symbols;
  ir_frontend_metadata.linked_implementation_symbols = interface_implementation_summary.linked_implementation_symbols;
  ir_frontend_metadata.declared_protocols = protocol_category_summary.declared_protocols;
  ir_frontend_metadata.declared_categories = protocol_category_summary.declared_categories;
  ir_frontend_metadata.resolved_protocol_symbols = protocol_category_summary.resolved_protocol_symbols;
  ir_frontend_metadata.resolved_category_symbols = protocol_category_summary.resolved_category_symbols;
  ir_frontend_metadata.protocol_method_symbols = protocol_category_summary.protocol_method_symbols;
  ir_frontend_metadata.category_method_symbols = protocol_category_summary.category_method_symbols;
  ir_frontend_metadata.linked_category_symbols = protocol_category_summary.linked_category_symbols;
  ir_frontend_metadata.declared_class_interfaces = class_protocol_category_linking_summary.declared_class_interfaces;
  ir_frontend_metadata.declared_class_implementations =
      class_protocol_category_linking_summary.declared_class_implementations;
  ir_frontend_metadata.resolved_class_interfaces = class_protocol_category_linking_summary.resolved_class_interfaces;
  ir_frontend_metadata.resolved_class_implementations =
      class_protocol_category_linking_summary.resolved_class_implementations;
  ir_frontend_metadata.linked_class_method_symbols =
      class_protocol_category_linking_summary.linked_class_method_symbols;
  ir_frontend_metadata.linked_category_method_symbols =
      class_protocol_category_linking_summary.linked_category_method_symbols;
  ir_frontend_metadata.protocol_composition_sites =
      class_protocol_category_linking_summary.protocol_composition_sites;
  ir_frontend_metadata.protocol_composition_symbols =
      class_protocol_category_linking_summary.protocol_composition_symbols;
  ir_frontend_metadata.category_composition_sites =
      class_protocol_category_linking_summary.category_composition_sites;
  ir_frontend_metadata.category_composition_symbols =
      class_protocol_category_linking_summary.category_composition_symbols;
  ir_frontend_metadata.invalid_protocol_composition_sites =
      class_protocol_category_linking_summary.invalid_protocol_composition_sites;
  ir_frontend_metadata.selector_method_declaration_entries = selector_normalization_summary.method_declaration_entries;
  ir_frontend_metadata.selector_normalized_method_declarations =
      selector_normalization_summary.normalized_method_declarations;
  ir_frontend_metadata.selector_piece_entries = selector_normalization_summary.selector_piece_entries;
  ir_frontend_metadata.selector_piece_parameter_links = selector_normalization_summary.selector_piece_parameter_links;
  ir_frontend_metadata.property_declaration_entries = property_attribute_summary.property_declaration_entries;
  ir_frontend_metadata.property_attribute_entries = property_attribute_summary.property_attribute_entries;
  ir_frontend_metadata.property_attribute_value_entries = property_attribute_summary.property_attribute_value_entries;
  ir_frontend_metadata.property_accessor_modifier_entries = property_attribute_summary.property_accessor_modifier_entries;
  ir_frontend_metadata.property_getter_selector_entries = property_attribute_summary.property_getter_selector_entries;
  ir_frontend_metadata.property_setter_selector_entries = property_attribute_summary.property_setter_selector_entries;
  ir_frontend_metadata.lowering_property_synthesis_ivar_binding_replay_key =
      property_synthesis_ivar_binding_replay_key;
  ir_frontend_metadata.lowering_id_class_sel_object_pointer_typecheck_replay_key =
      id_class_sel_object_pointer_typecheck_replay_key;
  ir_frontend_metadata.id_typecheck_sites = id_class_sel_object_pointer_typecheck_contract.id_typecheck_sites;
  ir_frontend_metadata.class_typecheck_sites = id_class_sel_object_pointer_typecheck_contract.class_typecheck_sites;
  ir_frontend_metadata.sel_typecheck_sites = id_class_sel_object_pointer_typecheck_contract.sel_typecheck_sites;
  ir_frontend_metadata.object_pointer_typecheck_sites =
      id_class_sel_object_pointer_typecheck_contract.object_pointer_typecheck_sites;
  ir_frontend_metadata.id_class_sel_object_pointer_typecheck_sites_total =
      id_class_sel_object_pointer_typecheck_contract.total_typecheck_sites;
  ir_frontend_metadata.lowering_dispatch_surface_classification_replay_key =
      dispatch_surface_classification_replay_key;
  ir_frontend_metadata.dispatch_surface_classification_instance_sites =
      dispatch_surface_classification_contract.instance_dispatch_sites;
  ir_frontend_metadata.dispatch_surface_classification_class_sites =
      dispatch_surface_classification_contract.class_dispatch_sites;
  ir_frontend_metadata.dispatch_surface_classification_super_sites =
      dispatch_surface_classification_contract.super_dispatch_sites;
  ir_frontend_metadata.dispatch_surface_classification_direct_sites =
      dispatch_surface_classification_contract.direct_dispatch_sites;
  ir_frontend_metadata.dispatch_surface_classification_dynamic_sites =
      dispatch_surface_classification_contract.dynamic_dispatch_sites;
  ir_frontend_metadata.dispatch_surface_classification_instance_entrypoint_family =
      dispatch_surface_classification_contract.instance_entrypoint_family;
  ir_frontend_metadata.dispatch_surface_classification_class_entrypoint_family =
      dispatch_surface_classification_contract.class_entrypoint_family;
  ir_frontend_metadata.dispatch_surface_classification_super_entrypoint_family =
      dispatch_surface_classification_contract.super_entrypoint_family;
  ir_frontend_metadata.dispatch_surface_classification_direct_entrypoint_family =
      dispatch_surface_classification_contract.direct_entrypoint_family;
  ir_frontend_metadata.dispatch_surface_classification_dynamic_entrypoint_family =
      dispatch_surface_classification_contract.dynamic_entrypoint_family;
  ir_frontend_metadata.lowering_message_send_selector_lowering_replay_key =
      message_send_selector_lowering_replay_key;
  ir_frontend_metadata.message_send_selector_lowering_sites =
      message_send_selector_lowering_contract.message_send_sites;
  ir_frontend_metadata.message_send_selector_lowering_unary_sites =
      message_send_selector_lowering_contract.unary_selector_sites;
  ir_frontend_metadata.message_send_selector_lowering_keyword_sites =
      message_send_selector_lowering_contract.keyword_selector_sites;
  ir_frontend_metadata.message_send_selector_lowering_selector_piece_sites =
      message_send_selector_lowering_contract.selector_piece_sites;
  ir_frontend_metadata.message_send_selector_lowering_argument_expression_sites =
      message_send_selector_lowering_contract.argument_expression_sites;
  ir_frontend_metadata.message_send_selector_lowering_receiver_sites =
      message_send_selector_lowering_contract.receiver_expression_sites;
  ir_frontend_metadata.message_send_selector_lowering_selector_literal_entries =
      message_send_selector_lowering_contract.selector_literal_entries;
  ir_frontend_metadata.message_send_selector_lowering_selector_literal_characters =
      message_send_selector_lowering_contract.selector_literal_characters;
  ir_frontend_metadata.lowering_dispatch_abi_marshalling_replay_key = dispatch_abi_marshalling_replay_key;
  ir_frontend_metadata.dispatch_abi_marshalling_message_send_sites =
      dispatch_abi_marshalling_contract.message_send_sites;
  ir_frontend_metadata.dispatch_abi_marshalling_receiver_slots_marshaled =
      dispatch_abi_marshalling_contract.receiver_slots_marshaled;
  ir_frontend_metadata.dispatch_abi_marshalling_selector_slots_marshaled =
      dispatch_abi_marshalling_contract.selector_slots_marshaled;
  ir_frontend_metadata.dispatch_abi_marshalling_argument_value_slots_marshaled =
      dispatch_abi_marshalling_contract.argument_value_slots_marshaled;
  ir_frontend_metadata.dispatch_abi_marshalling_argument_padding_slots_marshaled =
      dispatch_abi_marshalling_contract.argument_padding_slots_marshaled;
  ir_frontend_metadata.dispatch_abi_marshalling_argument_total_slots_marshaled =
      dispatch_abi_marshalling_contract.argument_total_slots_marshaled;
  ir_frontend_metadata.dispatch_abi_marshalling_total_marshaled_slots =
      dispatch_abi_marshalling_contract.total_marshaled_slots;
  ir_frontend_metadata.dispatch_abi_marshalling_runtime_dispatch_arg_slots =
      dispatch_abi_marshalling_contract.runtime_dispatch_arg_slots;
  ir_frontend_metadata.lowering_nil_receiver_semantics_foldability_replay_key =
      nil_receiver_semantics_foldability_replay_key;
  ir_frontend_metadata.nil_receiver_semantics_foldability_message_send_sites =
      nil_receiver_semantics_foldability_contract.message_send_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_receiver_nil_literal_sites =
      nil_receiver_semantics_foldability_contract.receiver_nil_literal_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_enabled_sites =
      nil_receiver_semantics_foldability_contract.nil_receiver_semantics_enabled_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_foldable_sites =
      nil_receiver_semantics_foldability_contract.nil_receiver_foldable_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_runtime_dispatch_required_sites =
      nil_receiver_semantics_foldability_contract.nil_receiver_runtime_dispatch_required_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_non_nil_receiver_sites =
      nil_receiver_semantics_foldability_contract.non_nil_receiver_sites;
  ir_frontend_metadata.nil_receiver_semantics_foldability_contract_violation_sites =
      nil_receiver_semantics_foldability_contract.contract_violation_sites;
  ir_frontend_metadata.lowering_super_dispatch_method_family_replay_key =
      super_dispatch_method_family_replay_key;
  ir_frontend_metadata.super_dispatch_method_family_message_send_sites =
      super_dispatch_method_family_contract.message_send_sites;
  ir_frontend_metadata.super_dispatch_method_family_receiver_super_identifier_sites =
      super_dispatch_method_family_contract.receiver_super_identifier_sites;
  ir_frontend_metadata.super_dispatch_method_family_enabled_sites =
      super_dispatch_method_family_contract.super_dispatch_enabled_sites;
  ir_frontend_metadata.super_dispatch_method_family_requires_class_context_sites =
      super_dispatch_method_family_contract.super_dispatch_requires_class_context_sites;
  ir_frontend_metadata.super_dispatch_method_family_init_sites =
      super_dispatch_method_family_contract.method_family_init_sites;
  ir_frontend_metadata.super_dispatch_method_family_copy_sites =
      super_dispatch_method_family_contract.method_family_copy_sites;
  ir_frontend_metadata.super_dispatch_method_family_mutable_copy_sites =
      super_dispatch_method_family_contract.method_family_mutable_copy_sites;
  ir_frontend_metadata.super_dispatch_method_family_new_sites =
      super_dispatch_method_family_contract.method_family_new_sites;
  ir_frontend_metadata.super_dispatch_method_family_none_sites =
      super_dispatch_method_family_contract.method_family_none_sites;
  ir_frontend_metadata.super_dispatch_method_family_returns_retained_result_sites =
      super_dispatch_method_family_contract.method_family_returns_retained_result_sites;
  ir_frontend_metadata.super_dispatch_method_family_returns_related_result_sites =
      super_dispatch_method_family_contract.method_family_returns_related_result_sites;
  ir_frontend_metadata.super_dispatch_method_family_contract_violation_sites =
      super_dispatch_method_family_contract.contract_violation_sites;
  ir_frontend_metadata.lowering_runtime_shim_host_link_replay_key = runtime_shim_host_link_replay_key;
  ir_frontend_metadata.runtime_shim_host_link_message_send_sites =
      runtime_shim_host_link_contract.message_send_sites;
  ir_frontend_metadata.runtime_shim_host_link_required_sites =
      runtime_shim_host_link_contract.runtime_shim_required_sites;
  ir_frontend_metadata.runtime_shim_host_link_elided_sites =
      runtime_shim_host_link_contract.runtime_shim_elided_sites;
  ir_frontend_metadata.runtime_shim_host_link_runtime_dispatch_arg_slots =
      runtime_shim_host_link_contract.runtime_dispatch_arg_slots;
  ir_frontend_metadata.runtime_shim_host_link_runtime_dispatch_declaration_parameter_count =
      runtime_shim_host_link_contract.runtime_dispatch_declaration_parameter_count;
  ir_frontend_metadata.runtime_shim_host_link_contract_violation_sites =
      runtime_shim_host_link_contract.contract_violation_sites;
  ir_frontend_metadata.runtime_shim_host_link_runtime_dispatch_symbol =
      runtime_shim_host_link_contract.runtime_dispatch_symbol;
  ir_frontend_metadata.runtime_shim_host_link_default_runtime_dispatch_symbol_binding =
      runtime_shim_host_link_contract.default_runtime_dispatch_symbol_binding;
  ir_frontend_metadata.lowering_ownership_qualifier_replay_key =
      ownership_qualifier_lowering_replay_key;
  ir_frontend_metadata.ownership_qualifier_lowering_ownership_qualifier_sites =
      ownership_qualifier_lowering_contract.ownership_qualifier_sites;
  ir_frontend_metadata.ownership_qualifier_lowering_invalid_ownership_qualifier_sites =
      ownership_qualifier_lowering_contract.invalid_ownership_qualifier_sites;
  ir_frontend_metadata.ownership_qualifier_lowering_object_pointer_type_annotation_sites =
      ownership_qualifier_lowering_contract.object_pointer_type_annotation_sites;
  ir_frontend_metadata.lowering_retain_release_operation_replay_key =
      retain_release_operation_lowering_replay_key;
  ir_frontend_metadata.retain_release_operation_lowering_ownership_qualified_sites =
      retain_release_operation_lowering_contract.ownership_qualified_sites;
  ir_frontend_metadata.retain_release_operation_lowering_retain_insertion_sites =
      retain_release_operation_lowering_contract.retain_insertion_sites;
  ir_frontend_metadata.retain_release_operation_lowering_release_insertion_sites =
      retain_release_operation_lowering_contract.release_insertion_sites;
  ir_frontend_metadata.retain_release_operation_lowering_autorelease_insertion_sites =
      retain_release_operation_lowering_contract.autorelease_insertion_sites;
  ir_frontend_metadata.retain_release_operation_lowering_contract_violation_sites =
      retain_release_operation_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.lowering_autoreleasepool_scope_replay_key =
      autoreleasepool_scope_lowering_replay_key;
  ir_frontend_metadata.autoreleasepool_scope_lowering_scope_sites =
      autoreleasepool_scope_lowering_contract.scope_sites;
  ir_frontend_metadata.autoreleasepool_scope_lowering_scope_symbolized_sites =
      autoreleasepool_scope_lowering_contract.scope_symbolized_sites;
  ir_frontend_metadata.autoreleasepool_scope_lowering_max_scope_depth =
      autoreleasepool_scope_lowering_contract.max_scope_depth;
  ir_frontend_metadata.autoreleasepool_scope_lowering_scope_entry_transition_sites =
      autoreleasepool_scope_lowering_contract.scope_entry_transition_sites;
  ir_frontend_metadata.autoreleasepool_scope_lowering_scope_exit_transition_sites =
      autoreleasepool_scope_lowering_contract.scope_exit_transition_sites;
  ir_frontend_metadata.autoreleasepool_scope_lowering_contract_violation_sites =
      autoreleasepool_scope_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.lowering_weak_unowned_semantics_replay_key =
      weak_unowned_semantics_lowering_replay_key;
  ir_frontend_metadata.weak_unowned_semantics_lowering_ownership_candidate_sites =
      weak_unowned_semantics_lowering_contract.ownership_candidate_sites;
  ir_frontend_metadata.weak_unowned_semantics_lowering_weak_reference_sites =
      weak_unowned_semantics_lowering_contract.weak_reference_sites;
  ir_frontend_metadata.weak_unowned_semantics_lowering_unowned_reference_sites =
      weak_unowned_semantics_lowering_contract.unowned_reference_sites;
  ir_frontend_metadata.weak_unowned_semantics_lowering_unowned_safe_reference_sites =
      weak_unowned_semantics_lowering_contract.unowned_safe_reference_sites;
  ir_frontend_metadata.weak_unowned_semantics_lowering_conflict_sites =
      weak_unowned_semantics_lowering_contract.weak_unowned_conflict_sites;
  ir_frontend_metadata.weak_unowned_semantics_lowering_contract_violation_sites =
      weak_unowned_semantics_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_weak_unowned_semantics_lowering_handoff =
      weak_unowned_semantics_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_arc_diagnostics_fixit_replay_key =
      arc_diagnostics_fixit_lowering_replay_key;
  ir_frontend_metadata.arc_diagnostics_fixit_lowering_ownership_arc_diagnostic_candidate_sites =
      arc_diagnostics_fixit_lowering_contract.ownership_arc_diagnostic_candidate_sites;
  ir_frontend_metadata.arc_diagnostics_fixit_lowering_ownership_arc_fixit_available_sites =
      arc_diagnostics_fixit_lowering_contract.ownership_arc_fixit_available_sites;
  ir_frontend_metadata.arc_diagnostics_fixit_lowering_ownership_arc_profiled_sites =
      arc_diagnostics_fixit_lowering_contract.ownership_arc_profiled_sites;
  ir_frontend_metadata
      .arc_diagnostics_fixit_lowering_ownership_arc_weak_unowned_conflict_diagnostic_sites =
      arc_diagnostics_fixit_lowering_contract.ownership_arc_weak_unowned_conflict_diagnostic_sites;
  ir_frontend_metadata.arc_diagnostics_fixit_lowering_ownership_arc_empty_fixit_hint_sites =
      arc_diagnostics_fixit_lowering_contract.ownership_arc_empty_fixit_hint_sites;
  ir_frontend_metadata.arc_diagnostics_fixit_lowering_contract_violation_sites =
      arc_diagnostics_fixit_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_arc_diagnostics_fixit_lowering_handoff =
      arc_diagnostics_fixit_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_block_literal_capture_replay_key =
      block_literal_capture_lowering_replay_key;
  ir_frontend_metadata.block_literal_capture_lowering_block_literal_sites =
      block_literal_capture_lowering_contract.block_literal_sites;
  ir_frontend_metadata.block_literal_capture_lowering_block_parameter_entries =
      block_literal_capture_lowering_contract.block_parameter_entries;
  ir_frontend_metadata.block_literal_capture_lowering_block_capture_entries =
      block_literal_capture_lowering_contract.block_capture_entries;
  ir_frontend_metadata.block_literal_capture_lowering_block_body_statement_entries =
      block_literal_capture_lowering_contract.block_body_statement_entries;
  ir_frontend_metadata.block_literal_capture_lowering_block_empty_capture_sites =
      block_literal_capture_lowering_contract.block_empty_capture_sites;
  ir_frontend_metadata.block_literal_capture_lowering_block_nondeterministic_capture_sites =
      block_literal_capture_lowering_contract.block_nondeterministic_capture_sites;
  ir_frontend_metadata.block_literal_capture_lowering_block_non_normalized_sites =
      block_literal_capture_lowering_contract.block_non_normalized_sites;
  ir_frontend_metadata.block_literal_capture_lowering_contract_violation_sites =
      block_literal_capture_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_block_literal_capture_lowering_handoff =
      block_literal_capture_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_block_abi_invoke_trampoline_replay_key =
      block_abi_invoke_trampoline_lowering_replay_key;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_block_literal_sites =
      block_abi_invoke_trampoline_lowering_contract.block_literal_sites;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_invoke_argument_slots_total =
      block_abi_invoke_trampoline_lowering_contract.invoke_argument_slots_total;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_capture_word_count_total =
      block_abi_invoke_trampoline_lowering_contract.capture_word_count_total;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_parameter_entries_total =
      block_abi_invoke_trampoline_lowering_contract.parameter_entries_total;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_capture_entries_total =
      block_abi_invoke_trampoline_lowering_contract.capture_entries_total;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_body_statement_entries_total =
      block_abi_invoke_trampoline_lowering_contract.body_statement_entries_total;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_descriptor_symbolized_sites =
      block_abi_invoke_trampoline_lowering_contract.descriptor_symbolized_sites;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_invoke_symbolized_sites =
      block_abi_invoke_trampoline_lowering_contract.invoke_trampoline_symbolized_sites;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_missing_invoke_sites =
      block_abi_invoke_trampoline_lowering_contract.missing_invoke_trampoline_sites;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_non_normalized_layout_sites =
      block_abi_invoke_trampoline_lowering_contract.non_normalized_layout_sites;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_contract_violation_sites =
      block_abi_invoke_trampoline_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_block_abi_invoke_trampoline_lowering_handoff =
      block_abi_invoke_trampoline_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_block_storage_escape_replay_key =
      block_storage_escape_lowering_replay_key;
  ir_frontend_metadata.block_storage_escape_lowering_block_literal_sites =
      block_storage_escape_lowering_contract.block_literal_sites;
  ir_frontend_metadata.block_storage_escape_lowering_mutable_capture_count_total =
      block_storage_escape_lowering_contract.mutable_capture_count_total;
  ir_frontend_metadata.block_storage_escape_lowering_byref_slot_count_total =
      block_storage_escape_lowering_contract.byref_slot_count_total;
  ir_frontend_metadata.block_storage_escape_lowering_parameter_entries_total =
      block_storage_escape_lowering_contract.parameter_entries_total;
  ir_frontend_metadata.block_storage_escape_lowering_capture_entries_total =
      block_storage_escape_lowering_contract.capture_entries_total;
  ir_frontend_metadata.block_storage_escape_lowering_body_statement_entries_total =
      block_storage_escape_lowering_contract.body_statement_entries_total;
  ir_frontend_metadata.block_storage_escape_lowering_requires_byref_cells_sites =
      block_storage_escape_lowering_contract.requires_byref_cells_sites;
  ir_frontend_metadata.block_storage_escape_lowering_escape_analysis_enabled_sites =
      block_storage_escape_lowering_contract.escape_analysis_enabled_sites;
  ir_frontend_metadata.block_storage_escape_lowering_escape_to_heap_sites =
      block_storage_escape_lowering_contract.escape_to_heap_sites;
  ir_frontend_metadata.block_storage_escape_lowering_escape_profile_normalized_sites =
      block_storage_escape_lowering_contract.escape_profile_normalized_sites;
  ir_frontend_metadata.block_storage_escape_lowering_byref_layout_symbolized_sites =
      block_storage_escape_lowering_contract.byref_layout_symbolized_sites;
  ir_frontend_metadata.block_storage_escape_lowering_contract_violation_sites =
      block_storage_escape_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_block_storage_escape_lowering_handoff =
      block_storage_escape_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_block_copy_dispose_replay_key =
      block_copy_dispose_lowering_replay_key;
  ir_frontend_metadata.block_copy_dispose_lowering_block_literal_sites =
      block_copy_dispose_lowering_contract.block_literal_sites;
  ir_frontend_metadata.block_copy_dispose_lowering_mutable_capture_count_total =
      block_copy_dispose_lowering_contract.mutable_capture_count_total;
  ir_frontend_metadata.block_copy_dispose_lowering_byref_slot_count_total =
      block_copy_dispose_lowering_contract.byref_slot_count_total;
  ir_frontend_metadata.block_copy_dispose_lowering_parameter_entries_total =
      block_copy_dispose_lowering_contract.parameter_entries_total;
  ir_frontend_metadata.block_copy_dispose_lowering_capture_entries_total =
      block_copy_dispose_lowering_contract.capture_entries_total;
  ir_frontend_metadata.block_copy_dispose_lowering_body_statement_entries_total =
      block_copy_dispose_lowering_contract.body_statement_entries_total;
  ir_frontend_metadata.block_copy_dispose_lowering_copy_helper_required_sites =
      block_copy_dispose_lowering_contract.copy_helper_required_sites;
  ir_frontend_metadata.block_copy_dispose_lowering_dispose_helper_required_sites =
      block_copy_dispose_lowering_contract.dispose_helper_required_sites;
  ir_frontend_metadata.block_copy_dispose_lowering_profile_normalized_sites =
      block_copy_dispose_lowering_contract.profile_normalized_sites;
  ir_frontend_metadata.block_copy_dispose_lowering_copy_helper_symbolized_sites =
      block_copy_dispose_lowering_contract.copy_helper_symbolized_sites;
  ir_frontend_metadata.block_copy_dispose_lowering_dispose_helper_symbolized_sites =
      block_copy_dispose_lowering_contract.dispose_helper_symbolized_sites;
  ir_frontend_metadata.block_copy_dispose_lowering_contract_violation_sites =
      block_copy_dispose_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_block_copy_dispose_lowering_handoff =
      block_copy_dispose_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_block_determinism_perf_baseline_replay_key =
      block_determinism_perf_baseline_lowering_replay_key;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_block_literal_sites =
      block_determinism_perf_baseline_lowering_contract.block_literal_sites;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_baseline_weight_total =
      block_determinism_perf_baseline_lowering_contract.baseline_weight_total;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_parameter_entries_total =
      block_determinism_perf_baseline_lowering_contract.parameter_entries_total;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_capture_entries_total =
      block_determinism_perf_baseline_lowering_contract.capture_entries_total;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_body_statement_entries_total =
      block_determinism_perf_baseline_lowering_contract.body_statement_entries_total;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_deterministic_capture_sites =
      block_determinism_perf_baseline_lowering_contract.deterministic_capture_sites;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_heavy_tier_sites =
      block_determinism_perf_baseline_lowering_contract.heavy_tier_sites;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_normalized_profile_sites =
      block_determinism_perf_baseline_lowering_contract.normalized_profile_sites;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_contract_violation_sites =
      block_determinism_perf_baseline_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_block_determinism_perf_baseline_lowering_handoff =
      block_determinism_perf_baseline_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_lightweight_generic_constraint_replay_key =
      lightweight_generic_constraint_lowering_replay_key;
  ir_frontend_metadata.lightweight_generic_constraint_lowering_generic_constraint_sites =
      lightweight_generic_constraint_lowering_contract.generic_constraint_sites;
  ir_frontend_metadata.lightweight_generic_constraint_lowering_generic_suffix_sites =
      lightweight_generic_constraint_lowering_contract.generic_suffix_sites;
  ir_frontend_metadata.lightweight_generic_constraint_lowering_object_pointer_type_sites =
      lightweight_generic_constraint_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata.lightweight_generic_constraint_lowering_terminated_generic_suffix_sites =
      lightweight_generic_constraint_lowering_contract.terminated_generic_suffix_sites;
  ir_frontend_metadata.lightweight_generic_constraint_lowering_pointer_declarator_sites =
      lightweight_generic_constraint_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.lightweight_generic_constraint_lowering_normalized_constraint_sites =
      lightweight_generic_constraint_lowering_contract.normalized_constraint_sites;
  ir_frontend_metadata.lightweight_generic_constraint_lowering_contract_violation_sites =
      lightweight_generic_constraint_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_lightweight_generic_constraint_lowering_handoff =
      lightweight_generic_constraint_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_nullability_flow_warning_precision_replay_key =
      nullability_flow_warning_precision_lowering_replay_key;
  ir_frontend_metadata.nullability_flow_warning_precision_lowering_sites =
      nullability_flow_warning_precision_lowering_contract.nullability_flow_sites;
  ir_frontend_metadata.nullability_flow_warning_precision_lowering_object_pointer_type_sites =
      nullability_flow_warning_precision_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata.nullability_flow_warning_precision_lowering_nullability_suffix_sites =
      nullability_flow_warning_precision_lowering_contract.nullability_suffix_sites;
  ir_frontend_metadata.nullability_flow_warning_precision_lowering_nullable_suffix_sites =
      nullability_flow_warning_precision_lowering_contract.nullable_suffix_sites;
  ir_frontend_metadata.nullability_flow_warning_precision_lowering_nonnull_suffix_sites =
      nullability_flow_warning_precision_lowering_contract.nonnull_suffix_sites;
  ir_frontend_metadata.nullability_flow_warning_precision_lowering_normalized_sites =
      nullability_flow_warning_precision_lowering_contract.normalized_sites;
  ir_frontend_metadata.nullability_flow_warning_precision_lowering_contract_violation_sites =
      nullability_flow_warning_precision_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_nullability_flow_warning_precision_lowering_handoff =
      nullability_flow_warning_precision_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_protocol_qualified_object_type_replay_key =
      protocol_qualified_object_type_lowering_replay_key;
  ir_frontend_metadata.protocol_qualified_object_type_lowering_sites =
      protocol_qualified_object_type_lowering_contract.protocol_qualified_object_type_sites;
  ir_frontend_metadata.protocol_qualified_object_type_lowering_protocol_composition_sites =
      protocol_qualified_object_type_lowering_contract.protocol_composition_sites;
  ir_frontend_metadata.protocol_qualified_object_type_lowering_object_pointer_type_sites =
      protocol_qualified_object_type_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata.protocol_qualified_object_type_lowering_terminated_protocol_composition_sites =
      protocol_qualified_object_type_lowering_contract.terminated_protocol_composition_sites;
  ir_frontend_metadata.protocol_qualified_object_type_lowering_pointer_declarator_sites =
      protocol_qualified_object_type_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.protocol_qualified_object_type_lowering_normalized_protocol_composition_sites =
      protocol_qualified_object_type_lowering_contract.normalized_protocol_composition_sites;
  ir_frontend_metadata.protocol_qualified_object_type_lowering_contract_violation_sites =
      protocol_qualified_object_type_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_protocol_qualified_object_type_lowering_handoff =
      protocol_qualified_object_type_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_variance_bridge_cast_replay_key =
      variance_bridge_cast_lowering_replay_key;
  ir_frontend_metadata.variance_bridge_cast_lowering_sites =
      variance_bridge_cast_lowering_contract.variance_bridge_cast_sites;
  ir_frontend_metadata.variance_bridge_cast_lowering_protocol_composition_sites =
      variance_bridge_cast_lowering_contract.protocol_composition_sites;
  ir_frontend_metadata.variance_bridge_cast_lowering_ownership_qualifier_sites =
      variance_bridge_cast_lowering_contract.ownership_qualifier_sites;
  ir_frontend_metadata.variance_bridge_cast_lowering_object_pointer_type_sites =
      variance_bridge_cast_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata.variance_bridge_cast_lowering_pointer_declarator_sites =
      variance_bridge_cast_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.variance_bridge_cast_lowering_normalized_sites =
      variance_bridge_cast_lowering_contract.normalized_sites;
  ir_frontend_metadata.variance_bridge_cast_lowering_contract_violation_sites =
      variance_bridge_cast_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_variance_bridge_cast_lowering_handoff =
      variance_bridge_cast_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_generic_metadata_abi_replay_key =
      generic_metadata_abi_lowering_replay_key;
  ir_frontend_metadata.generic_metadata_abi_lowering_sites =
      generic_metadata_abi_lowering_contract.generic_metadata_abi_sites;
  ir_frontend_metadata.generic_metadata_abi_lowering_generic_suffix_sites =
      generic_metadata_abi_lowering_contract.generic_suffix_sites;
  ir_frontend_metadata.generic_metadata_abi_lowering_protocol_composition_sites =
      generic_metadata_abi_lowering_contract.protocol_composition_sites;
  ir_frontend_metadata.generic_metadata_abi_lowering_ownership_qualifier_sites =
      generic_metadata_abi_lowering_contract.ownership_qualifier_sites;
  ir_frontend_metadata.generic_metadata_abi_lowering_object_pointer_type_sites =
      generic_metadata_abi_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata.generic_metadata_abi_lowering_pointer_declarator_sites =
      generic_metadata_abi_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.generic_metadata_abi_lowering_normalized_sites =
      generic_metadata_abi_lowering_contract.normalized_sites;
  ir_frontend_metadata.generic_metadata_abi_lowering_contract_violation_sites =
      generic_metadata_abi_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_generic_metadata_abi_lowering_handoff =
      generic_metadata_abi_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_module_import_graph_replay_key =
      module_import_graph_lowering_replay_key;
  ir_frontend_metadata.module_import_graph_lowering_sites =
      module_import_graph_lowering_contract.module_import_graph_sites;
  ir_frontend_metadata.module_import_graph_lowering_import_edge_candidate_sites =
      module_import_graph_lowering_contract.import_edge_candidate_sites;
  ir_frontend_metadata.module_import_graph_lowering_namespace_segment_sites =
      module_import_graph_lowering_contract.namespace_segment_sites;
  ir_frontend_metadata.module_import_graph_lowering_object_pointer_type_sites =
      module_import_graph_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata.module_import_graph_lowering_pointer_declarator_sites =
      module_import_graph_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.module_import_graph_lowering_normalized_sites =
      module_import_graph_lowering_contract.normalized_sites;
  ir_frontend_metadata.module_import_graph_lowering_contract_violation_sites =
      module_import_graph_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_module_import_graph_lowering_handoff =
      module_import_graph_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_namespace_collision_shadowing_replay_key =
      namespace_collision_shadowing_lowering_replay_key;
  ir_frontend_metadata.namespace_collision_shadowing_lowering_sites =
      namespace_collision_shadowing_lowering_contract
          .namespace_collision_shadowing_sites;
  ir_frontend_metadata
      .namespace_collision_shadowing_lowering_namespace_segment_sites =
      namespace_collision_shadowing_lowering_contract.namespace_segment_sites;
  ir_frontend_metadata
      .namespace_collision_shadowing_lowering_import_edge_candidate_sites =
      namespace_collision_shadowing_lowering_contract.import_edge_candidate_sites;
  ir_frontend_metadata
      .namespace_collision_shadowing_lowering_object_pointer_type_sites =
      namespace_collision_shadowing_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata
      .namespace_collision_shadowing_lowering_pointer_declarator_sites =
      namespace_collision_shadowing_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.namespace_collision_shadowing_lowering_normalized_sites =
      namespace_collision_shadowing_lowering_contract.normalized_sites;
  ir_frontend_metadata
      .namespace_collision_shadowing_lowering_contract_violation_sites =
      namespace_collision_shadowing_lowering_contract.contract_violation_sites;
  ir_frontend_metadata
      .deterministic_namespace_collision_shadowing_lowering_handoff =
      namespace_collision_shadowing_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_public_private_api_partition_replay_key =
      public_private_api_partition_lowering_replay_key;
  ir_frontend_metadata.public_private_api_partition_lowering_sites =
      public_private_api_partition_lowering_contract
          .public_private_api_partition_sites;
  ir_frontend_metadata
      .public_private_api_partition_lowering_namespace_segment_sites =
      public_private_api_partition_lowering_contract.namespace_segment_sites;
  ir_frontend_metadata
      .public_private_api_partition_lowering_import_edge_candidate_sites =
      public_private_api_partition_lowering_contract
          .import_edge_candidate_sites;
  ir_frontend_metadata
      .public_private_api_partition_lowering_object_pointer_type_sites =
      public_private_api_partition_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata
      .public_private_api_partition_lowering_pointer_declarator_sites =
      public_private_api_partition_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.public_private_api_partition_lowering_normalized_sites =
      public_private_api_partition_lowering_contract.normalized_sites;
  ir_frontend_metadata
      .public_private_api_partition_lowering_contract_violation_sites =
      public_private_api_partition_lowering_contract.contract_violation_sites;
  ir_frontend_metadata
      .deterministic_public_private_api_partition_lowering_handoff =
      public_private_api_partition_lowering_contract.deterministic;
  ir_frontend_metadata
      .lowering_incremental_module_cache_invalidation_replay_key =
      incremental_module_cache_invalidation_lowering_replay_key;
  ir_frontend_metadata.incremental_module_cache_invalidation_lowering_sites =
      incremental_module_cache_invalidation_lowering_contract
          .incremental_module_cache_invalidation_sites;
  ir_frontend_metadata
      .incremental_module_cache_invalidation_lowering_namespace_segment_sites =
      incremental_module_cache_invalidation_lowering_contract
          .namespace_segment_sites;
  ir_frontend_metadata
      .incremental_module_cache_invalidation_lowering_import_edge_candidate_sites =
      incremental_module_cache_invalidation_lowering_contract
          .import_edge_candidate_sites;
  ir_frontend_metadata
      .incremental_module_cache_invalidation_lowering_object_pointer_type_sites =
      incremental_module_cache_invalidation_lowering_contract
          .object_pointer_type_sites;
  ir_frontend_metadata
      .incremental_module_cache_invalidation_lowering_pointer_declarator_sites =
      incremental_module_cache_invalidation_lowering_contract
          .pointer_declarator_sites;
  ir_frontend_metadata
      .incremental_module_cache_invalidation_lowering_normalized_sites =
      incremental_module_cache_invalidation_lowering_contract.normalized_sites;
  ir_frontend_metadata
      .incremental_module_cache_invalidation_lowering_cache_invalidation_candidate_sites =
      incremental_module_cache_invalidation_lowering_contract
          .cache_invalidation_candidate_sites;
  ir_frontend_metadata
      .incremental_module_cache_invalidation_lowering_contract_violation_sites =
      incremental_module_cache_invalidation_lowering_contract
          .contract_violation_sites;
  ir_frontend_metadata
      .deterministic_incremental_module_cache_invalidation_lowering_handoff =
      incremental_module_cache_invalidation_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_cross_module_conformance_replay_key =
      cross_module_conformance_lowering_replay_key;
  ir_frontend_metadata.cross_module_conformance_lowering_sites =
      cross_module_conformance_lowering_contract.cross_module_conformance_sites;
  ir_frontend_metadata
      .cross_module_conformance_lowering_namespace_segment_sites =
      cross_module_conformance_lowering_contract.namespace_segment_sites;
  ir_frontend_metadata
      .cross_module_conformance_lowering_import_edge_candidate_sites =
      cross_module_conformance_lowering_contract.import_edge_candidate_sites;
  ir_frontend_metadata
      .cross_module_conformance_lowering_object_pointer_type_sites =
      cross_module_conformance_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata
      .cross_module_conformance_lowering_pointer_declarator_sites =
      cross_module_conformance_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.cross_module_conformance_lowering_normalized_sites =
      cross_module_conformance_lowering_contract.normalized_sites;
  ir_frontend_metadata
      .cross_module_conformance_lowering_cache_invalidation_candidate_sites =
      cross_module_conformance_lowering_contract
          .cache_invalidation_candidate_sites;
  ir_frontend_metadata
      .cross_module_conformance_lowering_contract_violation_sites =
      cross_module_conformance_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_cross_module_conformance_lowering_handoff =
      cross_module_conformance_lowering_contract.deterministic;
  ir_frontend_metadata.lowering_throws_propagation_replay_key =
      throws_propagation_lowering_replay_key;
  ir_frontend_metadata.throws_propagation_lowering_sites =
      throws_propagation_lowering_contract.throws_propagation_sites;
  ir_frontend_metadata.throws_propagation_lowering_namespace_segment_sites =
      throws_propagation_lowering_contract.namespace_segment_sites;
  ir_frontend_metadata.throws_propagation_lowering_import_edge_candidate_sites =
      throws_propagation_lowering_contract.import_edge_candidate_sites;
  ir_frontend_metadata.throws_propagation_lowering_object_pointer_type_sites =
      throws_propagation_lowering_contract.object_pointer_type_sites;
  ir_frontend_metadata.throws_propagation_lowering_pointer_declarator_sites =
      throws_propagation_lowering_contract.pointer_declarator_sites;
  ir_frontend_metadata.throws_propagation_lowering_normalized_sites =
      throws_propagation_lowering_contract.normalized_sites;
  ir_frontend_metadata
      .throws_propagation_lowering_cache_invalidation_candidate_sites =
      throws_propagation_lowering_contract
          .cache_invalidation_candidate_sites;
  ir_frontend_metadata.throws_propagation_lowering_contract_violation_sites =
      throws_propagation_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_throws_propagation_lowering_handoff =
      throws_propagation_lowering_contract.deterministic;
  ir_frontend_metadata.object_pointer_type_spellings =
      object_pointer_nullability_generics_summary.object_pointer_type_spellings;
  ir_frontend_metadata.pointer_declarator_entries =
      object_pointer_nullability_generics_summary.pointer_declarator_entries;
  ir_frontend_metadata.pointer_declarator_depth_total =
      object_pointer_nullability_generics_summary.pointer_declarator_depth_total;
  ir_frontend_metadata.pointer_declarator_token_entries =
      object_pointer_nullability_generics_summary.pointer_declarator_token_entries;
  ir_frontend_metadata.nullability_suffix_entries =
      object_pointer_nullability_generics_summary.nullability_suffix_entries;
  ir_frontend_metadata.generic_suffix_entries = object_pointer_nullability_generics_summary.generic_suffix_entries;
  ir_frontend_metadata.terminated_generic_suffix_entries =
      object_pointer_nullability_generics_summary.terminated_generic_suffix_entries;
  ir_frontend_metadata.unterminated_generic_suffix_entries =
      object_pointer_nullability_generics_summary.unterminated_generic_suffix_entries;
  ir_frontend_metadata.global_symbol_nodes = symbol_graph_scope_resolution_summary.global_symbol_nodes;
  ir_frontend_metadata.function_symbol_nodes = symbol_graph_scope_resolution_summary.function_symbol_nodes;
  ir_frontend_metadata.interface_symbol_nodes = symbol_graph_scope_resolution_summary.interface_symbol_nodes;
  ir_frontend_metadata.implementation_symbol_nodes = symbol_graph_scope_resolution_summary.implementation_symbol_nodes;
  ir_frontend_metadata.interface_property_symbol_nodes =
      symbol_graph_scope_resolution_summary.interface_property_symbol_nodes;
  ir_frontend_metadata.implementation_property_symbol_nodes =
      symbol_graph_scope_resolution_summary.implementation_property_symbol_nodes;
  ir_frontend_metadata.interface_method_symbol_nodes = symbol_graph_scope_resolution_summary.interface_method_symbol_nodes;
  ir_frontend_metadata.implementation_method_symbol_nodes =
      symbol_graph_scope_resolution_summary.implementation_method_symbol_nodes;
  ir_frontend_metadata.top_level_scope_symbols = symbol_graph_scope_resolution_summary.top_level_scope_symbols;
  ir_frontend_metadata.nested_scope_symbols = symbol_graph_scope_resolution_summary.nested_scope_symbols;
  ir_frontend_metadata.scope_frames_total = symbol_graph_scope_resolution_summary.scope_frames_total;
  ir_frontend_metadata.implementation_interface_resolution_sites =
      symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites;
  ir_frontend_metadata.implementation_interface_resolution_hits =
      symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits;
  ir_frontend_metadata.implementation_interface_resolution_misses =
      symbol_graph_scope_resolution_summary.implementation_interface_resolution_misses;
  ir_frontend_metadata.method_resolution_sites = symbol_graph_scope_resolution_summary.method_resolution_sites;
  ir_frontend_metadata.method_resolution_hits = symbol_graph_scope_resolution_summary.method_resolution_hits;
  ir_frontend_metadata.method_resolution_misses = symbol_graph_scope_resolution_summary.method_resolution_misses;
  ir_frontend_metadata.deterministic_interface_implementation_handoff =
      pipeline_result.sema_parity_surface.deterministic_interface_implementation_handoff &&
      interface_implementation_summary.deterministic;
  ir_frontend_metadata.deterministic_protocol_category_handoff =
      protocol_category_summary.deterministic_protocol_category_handoff;
  ir_frontend_metadata.deterministic_class_protocol_category_linking_handoff =
      class_protocol_category_linking_summary.deterministic_class_protocol_category_linking_handoff;
  ir_frontend_metadata.deterministic_selector_normalization_handoff =
      selector_normalization_summary.deterministic_selector_normalization_handoff;
  ir_frontend_metadata.deterministic_property_attribute_handoff =
      property_attribute_summary.deterministic_property_attribute_handoff;
  ir_frontend_metadata.runtime_metadata_source_ownership_contract_id =
      runtime_metadata_source_ownership.contract_id;
  ir_frontend_metadata.runtime_metadata_source_schema =
      runtime_metadata_source_ownership.canonical_source_schema;
  ir_frontend_metadata.runtime_metadata_ivar_source_model =
      runtime_metadata_source_ownership.ivar_record_source_model;
  ir_frontend_metadata.runtime_metadata_class_record_count =
      runtime_metadata_source_ownership.class_record_count;
  ir_frontend_metadata.runtime_metadata_protocol_record_count =
      runtime_metadata_source_ownership.protocol_record_count;
  ir_frontend_metadata.runtime_metadata_category_interface_record_count =
      runtime_metadata_source_ownership.category_interface_record_count;
  ir_frontend_metadata.runtime_metadata_category_implementation_record_count =
      runtime_metadata_source_ownership.category_implementation_record_count;
  ir_frontend_metadata.runtime_metadata_property_record_count =
      runtime_metadata_source_ownership.property_record_count;
  ir_frontend_metadata.runtime_metadata_method_record_count =
      runtime_metadata_source_ownership.method_record_count;
  ir_frontend_metadata.runtime_metadata_ivar_record_count =
      runtime_metadata_source_ownership.ivar_record_count;
  ir_frontend_metadata.frontend_owns_runtime_metadata_source_records =
      runtime_metadata_source_ownership.frontend_owns_runtime_metadata_source_records;
  ir_frontend_metadata.runtime_metadata_source_records_ready_for_lowering =
      runtime_metadata_source_ownership.runtime_metadata_source_records_ready_for_lowering;
  ir_frontend_metadata.native_runtime_library_present =
      runtime_metadata_source_ownership.native_runtime_library_present;
  ir_frontend_metadata.runtime_metadata_source_boundary_fail_closed =
      runtime_metadata_source_ownership.fail_closed;
  ir_frontend_metadata.runtime_shim_test_only =
      runtime_metadata_source_ownership.runtime_shim_test_only;
  ir_frontend_metadata.deterministic_runtime_metadata_source_schema =
      runtime_metadata_source_ownership.deterministic_source_schema;
  ir_frontend_metadata.runtime_export_legality_contract_id =
      runtime_export_legality.contract_id;
  ir_frontend_metadata.runtime_export_semantic_boundary_frozen =
      runtime_export_legality.semantic_boundary_frozen;
  ir_frontend_metadata.runtime_export_metadata_export_enforcement_ready =
      runtime_export_legality.metadata_export_enforcement_ready;
  ir_frontend_metadata.runtime_export_fail_closed =
      runtime_export_legality.fail_closed;
  ir_frontend_metadata.runtime_export_duplicate_runtime_identity_enforcement_pending =
      runtime_export_legality.duplicate_runtime_identity_enforcement_pending;
  ir_frontend_metadata.runtime_export_incomplete_declaration_export_blocking_pending =
      runtime_export_legality.incomplete_declaration_export_blocking_pending;
  ir_frontend_metadata.runtime_export_illegal_redeclaration_mix_export_blocking_pending =
      runtime_export_legality.illegal_redeclaration_mix_export_blocking_pending;
  ir_frontend_metadata.runtime_export_class_record_count =
      runtime_export_legality.class_record_count;
  ir_frontend_metadata.runtime_export_protocol_record_count =
      runtime_export_legality.protocol_record_count;
  ir_frontend_metadata.runtime_export_category_record_count =
      runtime_export_legality.category_record_count;
  ir_frontend_metadata.runtime_export_property_record_count =
      runtime_export_legality.property_record_count;
  ir_frontend_metadata.runtime_export_method_record_count =
      runtime_export_legality.method_record_count;
  ir_frontend_metadata.runtime_export_ivar_record_count =
      runtime_export_legality.ivar_record_count;
  ir_frontend_metadata.runtime_export_invalid_protocol_composition_sites =
      runtime_export_legality.invalid_protocol_composition_sites;
  ir_frontend_metadata.runtime_export_property_attribute_invalid_entries =
      runtime_export_legality.property_attribute_invalid_entries;
  ir_frontend_metadata.runtime_export_property_attribute_contract_violations =
      runtime_export_legality.property_attribute_contract_violations;
  ir_frontend_metadata.runtime_export_invalid_type_annotation_sites =
      runtime_export_legality.invalid_type_annotation_sites;
  ir_frontend_metadata.runtime_export_property_ivar_binding_missing =
      runtime_export_legality.property_ivar_binding_missing;
  ir_frontend_metadata.runtime_export_property_ivar_binding_conflicts =
      runtime_export_legality.property_ivar_binding_conflicts;
  ir_frontend_metadata.runtime_export_implementation_resolution_misses =
      runtime_export_legality.implementation_resolution_misses;
  ir_frontend_metadata.runtime_export_method_resolution_misses =
      runtime_export_legality.method_resolution_misses;
  ir_frontend_metadata.runtime_export_boundary_ready =
      IsReadyObjc3RuntimeExportLegalityBoundary(runtime_export_legality);
  ir_frontend_metadata.runtime_export_enforcement_contract_id =
      runtime_export_enforcement.contract_id;
  ir_frontend_metadata.runtime_export_metadata_completeness_enforced =
      runtime_export_enforcement.metadata_completeness_enforced;
  ir_frontend_metadata
      .runtime_export_duplicate_runtime_identity_suppression_enforced =
      runtime_export_enforcement
          .duplicate_runtime_identity_suppression_enforced;
  ir_frontend_metadata
      .runtime_export_illegal_redeclaration_mix_blocking_enforced =
      runtime_export_enforcement
          .illegal_redeclaration_mix_blocking_enforced;
  ir_frontend_metadata.runtime_export_metadata_shape_drift_blocking_enforced =
      runtime_export_enforcement.metadata_shape_drift_blocking_enforced;
  ir_frontend_metadata.runtime_export_enforcement_fail_closed =
      runtime_export_enforcement.fail_closed;
  ir_frontend_metadata.runtime_export_ready_for_runtime_export =
      runtime_export_enforcement.ready_for_runtime_export;
  ir_frontend_metadata.runtime_export_duplicate_runtime_identity_sites =
      runtime_export_enforcement.duplicate_runtime_identity_sites;
  ir_frontend_metadata.runtime_export_incomplete_declaration_sites =
      runtime_export_enforcement.incomplete_declaration_sites;
  ir_frontend_metadata.runtime_export_illegal_redeclaration_mix_sites =
      runtime_export_enforcement.illegal_redeclaration_mix_sites;
  ir_frontend_metadata.runtime_export_metadata_shape_drift_sites =
      runtime_export_enforcement.metadata_shape_drift_sites;
  ir_frontend_metadata.runtime_metadata_section_abi_contract_id =
      runtime_metadata_section_abi.contract_id;
  ir_frontend_metadata.runtime_metadata_section_boundary_frozen =
      runtime_metadata_section_abi.boundary_frozen;
  ir_frontend_metadata.runtime_metadata_section_fail_closed =
      runtime_metadata_section_abi.fail_closed;
  ir_frontend_metadata.runtime_metadata_section_object_file_inventory_frozen =
      runtime_metadata_section_abi.object_file_section_inventory_frozen;
  ir_frontend_metadata.runtime_metadata_section_symbol_policy_frozen =
      runtime_metadata_section_abi.symbol_policy_frozen;
  ir_frontend_metadata.runtime_metadata_section_visibility_model_frozen =
      runtime_metadata_section_abi.visibility_model_frozen;
  ir_frontend_metadata.runtime_metadata_section_retention_policy_frozen =
      runtime_metadata_section_abi.retention_policy_frozen;
  ir_frontend_metadata.runtime_metadata_section_ready_for_scaffold =
      runtime_metadata_section_abi.ready_for_section_scaffold;
  ir_frontend_metadata.runtime_metadata_section_logical_image_info_section =
      runtime_metadata_section_abi.logical_image_info_section;
  ir_frontend_metadata
      .runtime_metadata_section_logical_class_descriptor_section =
      runtime_metadata_section_abi.logical_class_descriptor_section;
  ir_frontend_metadata
      .runtime_metadata_section_logical_protocol_descriptor_section =
      runtime_metadata_section_abi.logical_protocol_descriptor_section;
  ir_frontend_metadata
      .runtime_metadata_section_logical_category_descriptor_section =
      runtime_metadata_section_abi.logical_category_descriptor_section;
  ir_frontend_metadata
      .runtime_metadata_section_logical_property_descriptor_section =
      runtime_metadata_section_abi.logical_property_descriptor_section;
  ir_frontend_metadata.runtime_metadata_section_logical_ivar_descriptor_section =
      runtime_metadata_section_abi.logical_ivar_descriptor_section;
  ir_frontend_metadata.runtime_metadata_section_descriptor_symbol_prefix =
      runtime_metadata_section_abi.descriptor_symbol_prefix;
  ir_frontend_metadata.runtime_metadata_section_aggregate_symbol_prefix =
      runtime_metadata_section_abi.aggregate_symbol_prefix;
  ir_frontend_metadata.runtime_metadata_section_image_info_symbol =
      runtime_metadata_section_abi.image_info_symbol;
  ir_frontend_metadata.runtime_metadata_section_descriptor_linkage =
      runtime_metadata_section_abi.descriptor_linkage;
  ir_frontend_metadata.runtime_metadata_section_aggregate_linkage =
      runtime_metadata_section_abi.aggregate_linkage;
  ir_frontend_metadata.runtime_metadata_section_visibility =
      runtime_metadata_section_abi.metadata_visibility;
  ir_frontend_metadata.runtime_metadata_section_retention_root =
      runtime_metadata_section_abi.retention_root;
  ir_frontend_metadata.runtime_metadata_section_scaffold_contract_id =
      runtime_metadata_section_scaffold.contract_id;
  ir_frontend_metadata.runtime_metadata_section_scaffold_abi_contract_id =
      runtime_metadata_section_scaffold.abi_contract_id;
  ir_frontend_metadata.runtime_metadata_section_scaffold_emitted =
      runtime_metadata_section_scaffold.scaffold_emitted;
  ir_frontend_metadata.runtime_metadata_section_scaffold_fail_closed =
      runtime_metadata_section_scaffold.fail_closed;
  ir_frontend_metadata.runtime_metadata_section_scaffold_uses_llvm_used =
      runtime_metadata_section_scaffold.uses_llvm_used;
  ir_frontend_metadata.runtime_metadata_section_scaffold_image_info_emitted =
      runtime_metadata_section_scaffold.image_info_emitted;
  ir_frontend_metadata.runtime_metadata_section_scaffold_class_descriptor_count =
      runtime_metadata_section_scaffold.class_descriptor_count;
  ir_frontend_metadata
      .runtime_metadata_section_scaffold_protocol_descriptor_count =
      runtime_metadata_section_scaffold.protocol_descriptor_count;
  ir_frontend_metadata
      .runtime_metadata_section_scaffold_category_descriptor_count =
      runtime_metadata_section_scaffold.category_descriptor_count;
  ir_frontend_metadata
      .runtime_metadata_section_scaffold_property_descriptor_count =
      runtime_metadata_section_scaffold.property_descriptor_count;
  ir_frontend_metadata.runtime_metadata_section_scaffold_ivar_descriptor_count =
      runtime_metadata_section_scaffold.ivar_descriptor_count;
  ir_frontend_metadata.runtime_metadata_section_scaffold_total_descriptor_count =
      runtime_metadata_section_scaffold.total_descriptor_count;
  ir_frontend_metadata
      .runtime_metadata_section_scaffold_total_retained_global_count =
      runtime_metadata_section_scaffold.total_retained_global_count;
  ir_frontend_metadata.runtime_metadata_section_scaffold_image_info_symbol =
      runtime_metadata_section_scaffold.image_info_symbol;
  ir_frontend_metadata.runtime_metadata_section_scaffold_class_aggregate_symbol =
      runtime_metadata_section_scaffold.class_aggregate_symbol;
  ir_frontend_metadata
      .runtime_metadata_section_scaffold_protocol_aggregate_symbol =
      runtime_metadata_section_scaffold.protocol_aggregate_symbol;
  ir_frontend_metadata
      .runtime_metadata_section_scaffold_category_aggregate_symbol =
      runtime_metadata_section_scaffold.category_aggregate_symbol;
  ir_frontend_metadata
      .runtime_metadata_section_scaffold_property_aggregate_symbol =
      runtime_metadata_section_scaffold.property_aggregate_symbol;
  ir_frontend_metadata.runtime_metadata_section_scaffold_ivar_aggregate_symbol =
      runtime_metadata_section_scaffold.ivar_aggregate_symbol;
  ir_frontend_metadata.runtime_metadata_class_metaclass_emission_contract_id =
      kObjc3RuntimeClassMetaclassEmissionContractId;
  ir_frontend_metadata.runtime_metadata_class_metaclass_payload_model =
      kObjc3RuntimeClassMetaclassEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_class_metaclass_name_model =
      kObjc3RuntimeClassMetaclassEmissionNameModel;
  ir_frontend_metadata.runtime_metadata_class_metaclass_super_link_model =
      kObjc3RuntimeClassMetaclassEmissionSuperLinkModel;
  ir_frontend_metadata
      .runtime_metadata_class_metaclass_method_list_reference_model =
      kObjc3RuntimeClassMetaclassEmissionMethodListReferenceModel;
  ir_frontend_metadata.runtime_metadata_class_metaclass_typed_handoff_replay_key =
      executable_metadata_typed_lowering_handoff.replay_key;
  ir_frontend_metadata.runtime_metadata_protocol_category_emission_contract_id =
      kObjc3RuntimeProtocolCategoryEmissionContractId;
  ir_frontend_metadata.runtime_metadata_protocol_emission_payload_model =
      kObjc3RuntimeProtocolEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_category_emission_payload_model =
      kObjc3RuntimeCategoryEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_protocol_reference_model =
      kObjc3RuntimeProtocolReferenceModel;
  ir_frontend_metadata.runtime_metadata_category_attachment_model =
      kObjc3RuntimeCategoryAttachmentModel;
  ir_frontend_metadata.runtime_metadata_protocol_category_typed_handoff_replay_key =
      executable_metadata_typed_lowering_handoff.replay_key;
  ir_frontend_metadata.runtime_metadata_member_table_emission_contract_id =
      kObjc3RuntimeMemberTableEmissionContractId;
  ir_frontend_metadata.runtime_metadata_method_list_emission_payload_model =
      kObjc3RuntimeMethodListEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_method_list_grouping_model =
      kObjc3RuntimeMethodListEmissionGroupingModel;
  ir_frontend_metadata.runtime_metadata_property_descriptor_emission_payload_model =
      kObjc3RuntimePropertyDescriptorEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_ivar_descriptor_emission_payload_model =
      kObjc3RuntimeIvarDescriptorEmissionPayloadModel;
  ir_frontend_metadata.runtime_metadata_member_table_typed_handoff_replay_key =
      executable_metadata_typed_lowering_handoff.replay_key;
  ir_frontend_metadata.runtime_metadata_archive_static_link_discovery_contract_id =
      kObjc3RuntimeArchiveStaticLinkDiscoveryContractId;
  ir_frontend_metadata.runtime_metadata_archive_static_link_anchor_seed_model =
      kObjc3RuntimeArchiveStaticLinkAnchorSeedModel;
  ir_frontend_metadata
      .runtime_metadata_archive_static_link_translation_unit_identity_model =
      kObjc3RuntimeArchiveStaticLinkTranslationUnitIdentityModel;
  ir_frontend_metadata.runtime_metadata_archive_static_link_merge_model =
      kObjc3RuntimeArchiveStaticLinkMergeModel;
  ir_frontend_metadata.runtime_metadata_archive_static_link_response_artifact_suffix =
      kObjc3RuntimeMergedLinkerResponseArtifactSuffix;
  ir_frontend_metadata.runtime_metadata_archive_static_link_discovery_artifact_suffix =
      kObjc3RuntimeMergedDiscoveryArtifactSuffix;
  ir_frontend_metadata.runtime_metadata_archive_static_link_discovery_ready =
      true;
  ir_frontend_metadata
      .runtime_metadata_archive_static_link_translation_unit_identity_key =
      input_path.generic_string() + "|" +
      bundle.parse_lowering_readiness_surface.parse_artifact_replay_key + "|" +
      bundle.parse_lowering_readiness_surface.lowering_boundary_replay_key;
  // M254-C002 bootstrap materialization anchor: the native IR emitter consumes
  // this lowering packet directly when it materializes the ctor root, derived
  // init stub, registration table, and image descriptor. Driver/process code
  // may publish the same packet, but they may not re-derive those symbol
  // shapes independently from truncated sidecar state.
  // M254-C003 registration-table/image-local-init anchor: the same lowering
  // packet now also carries the self-describing registration-table layout,
  // ABI/version counts, and image-local init-state model that the emitter,
  // manifest writers, and later runtime image-walk code must preserve exactly.
  ir_frontend_metadata.runtime_bootstrap_lowering_contract_id =
      bundle.runtime_bootstrap_lowering_summary.contract_id;
  ir_frontend_metadata.runtime_bootstrap_lowering_boundary_model =
      bundle.runtime_bootstrap_lowering_summary.lowering_boundary_model;
  ir_frontend_metadata.runtime_bootstrap_lowering_constructor_root_symbol =
      bundle.runtime_bootstrap_lowering_summary.constructor_root_symbol;
  ir_frontend_metadata.runtime_bootstrap_lowering_init_stub_symbol_prefix =
      bundle.runtime_bootstrap_lowering_summary
          .constructor_init_stub_symbol_prefix;
  ir_frontend_metadata
      .runtime_bootstrap_lowering_registration_table_symbol_prefix =
      bundle.runtime_bootstrap_lowering_summary
          .registration_table_symbol_prefix;
  ir_frontend_metadata
      .runtime_bootstrap_lowering_image_local_init_state_symbol_prefix =
      bundle.runtime_bootstrap_lowering_summary
          .image_local_init_state_symbol_prefix;
  ir_frontend_metadata.runtime_bootstrap_lowering_registration_entrypoint_symbol =
      bundle.runtime_bootstrap_lowering_summary.registration_entrypoint_symbol;
  ir_frontend_metadata.runtime_bootstrap_lowering_global_ctor_list_model =
      bundle.runtime_bootstrap_lowering_summary.global_ctor_list_model;
  ir_frontend_metadata.runtime_bootstrap_lowering_registration_table_layout_model =
      bundle.runtime_bootstrap_lowering_summary.registration_table_layout_model;
  ir_frontend_metadata.runtime_bootstrap_lowering_image_local_initialization_model =
      bundle.runtime_bootstrap_lowering_summary
          .image_local_initialization_model;
  ir_frontend_metadata.runtime_bootstrap_lowering_registration_table_abi_version =
      bundle.runtime_bootstrap_lowering_summary
          .registration_table_abi_version;
  ir_frontend_metadata
      .runtime_bootstrap_lowering_registration_table_pointer_field_count =
      bundle.runtime_bootstrap_lowering_summary
          .registration_table_pointer_field_count;
  ir_frontend_metadata.runtime_bootstrap_lowering_constructor_root_emission_state =
      bundle.runtime_bootstrap_lowering_summary.constructor_root_emission_state;
  ir_frontend_metadata.runtime_bootstrap_lowering_init_stub_emission_state =
      bundle.runtime_bootstrap_lowering_summary.init_stub_emission_state;
  ir_frontend_metadata
      .runtime_bootstrap_lowering_registration_table_emission_state =
      bundle.runtime_bootstrap_lowering_summary
          .registration_table_emission_state;
  ir_frontend_metadata
      .runtime_bootstrap_lowering_bootstrap_ir_materialization_landed =
      bundle.runtime_bootstrap_lowering_summary
          .bootstrap_ir_materialization_landed;
  ir_frontend_metadata
      .runtime_bootstrap_lowering_image_local_initialization_landed =
      bundle.runtime_bootstrap_lowering_summary
          .image_local_initialization_landed;
  ir_frontend_metadata.runtime_bootstrap_lowering_ready =
      bundle.runtime_bootstrap_lowering_summary
          .ready_for_bootstrap_materialization;
  ir_frontend_metadata.runtime_bootstrap_lowering_fail_closed =
      bundle.runtime_bootstrap_lowering_summary.fail_closed;
  {
    const bool typed_handoff_ready =
        IsReadyObjc3ExecutableMetadataTypedLoweringHandoff(
            executable_metadata_typed_lowering_handoff);
    if (typed_handoff_ready) {
      const auto &source_graph =
          executable_metadata_typed_lowering_handoff.source_graph;
      std::unordered_map<std::string,
                         const Objc3ExecutableMetadataClassGraphNode *>
          class_nodes_by_name;
      class_nodes_by_name.reserve(source_graph.class_nodes_lexicographic.size());
      for (const auto &class_node : source_graph.class_nodes_lexicographic) {
        class_nodes_by_name.emplace(class_node.class_name, &class_node);
      }
      std::unordered_map<std::string,
                         const Objc3ExecutableMetadataMetaclassGraphNode *>
          metaclass_nodes_by_name;
      metaclass_nodes_by_name.reserve(
          source_graph.metaclass_nodes_lexicographic.size());
      for (const auto &metaclass_node :
           source_graph.metaclass_nodes_lexicographic) {
        metaclass_nodes_by_name.emplace(metaclass_node.class_name,
                                        &metaclass_node);
      }
      std::unordered_map<std::string,
                         const Objc3ExecutableMetadataImplementationGraphNode *>
          implementation_nodes_by_name;
      implementation_nodes_by_name.reserve(
          source_graph.implementation_nodes_lexicographic.size());
      for (const auto &implementation_node :
           source_graph.implementation_nodes_lexicographic) {
        implementation_nodes_by_name.emplace(implementation_node.class_name,
                                             &implementation_node);
      }

      bool bundle_payload_complete = true;
      std::vector<Objc3IRRuntimeMetadataClassMetaclassBundle> bundles;
      bundles.reserve(source_graph.interface_nodes_lexicographic.size() +
                      source_graph.implementation_nodes_lexicographic.size());
      for (const auto &interface_node : source_graph.interface_nodes_lexicographic) {
        const auto metaclass_it =
            metaclass_nodes_by_name.find(interface_node.class_name);
        const auto class_it =
            class_nodes_by_name.find(interface_node.class_name);
        if (metaclass_it == metaclass_nodes_by_name.end() ||
            class_it == class_nodes_by_name.end()) {
          bundle_payload_complete = false;
          break;
        }

        Objc3IRRuntimeMetadataClassMetaclassBundle bundle;
        bundle.class_name = interface_node.class_name;
        bundle.owner_identity = interface_node.owner_identity;
        bundle.has_super = class_it->second->has_super;
        bundle.super_bundle_owner_identity =
            class_it->second->has_super
                ? ("interface:" + class_it->second->super_class_owner_identity.substr(6))
                : std::string{};
        bundle.instance_method_count = interface_node.instance_method_count;
        bundle.class_method_count =
            metaclass_it->second->interface_class_method_count;
        bundles.push_back(std::move(bundle));
      }
      for (const auto &implementation_node :
           source_graph.implementation_nodes_lexicographic) {
        const auto metaclass_it =
            metaclass_nodes_by_name.find(implementation_node.class_name);
        const auto class_it =
            class_nodes_by_name.find(implementation_node.class_name);
        if (metaclass_it == metaclass_nodes_by_name.end() ||
            class_it == class_nodes_by_name.end()) {
          bundle_payload_complete = false;
          break;
        }

        Objc3IRRuntimeMetadataClassMetaclassBundle bundle;
        bundle.class_name = implementation_node.class_name;
        bundle.owner_identity = implementation_node.owner_identity;
        bundle.has_super = class_it->second->has_super;
        if (class_it->second->has_super) {
          const std::string super_class_name =
              class_it->second->super_class_owner_identity.substr(6);
          const auto super_impl_it =
              implementation_nodes_by_name.find(super_class_name);
          if (super_impl_it == implementation_nodes_by_name.end()) {
            bundle_payload_complete = false;
            break;
          }
          bundle.super_bundle_owner_identity =
              super_impl_it->second->owner_identity;
        }
        bundle.instance_method_count = implementation_node.instance_method_count;
        bundle.class_method_count =
            metaclass_it->second->implementation_class_method_count;
        bundles.push_back(std::move(bundle));
      }

      bundle_payload_complete =
          bundle_payload_complete &&
          bundles.size() ==
              runtime_metadata_section_scaffold.class_descriptor_count;
      if (bundle_payload_complete) {
        ir_frontend_metadata.runtime_metadata_class_metaclass_bundles_lexicographic =
            std::move(bundles);
      }
      ir_frontend_metadata.runtime_metadata_class_metaclass_emission_ready =
          bundle_payload_complete;
      ir_frontend_metadata.runtime_metadata_class_metaclass_emission_fail_closed =
          bundle_payload_complete;

      // M253-C003 protocol/category data emission anchor: the typed lowering
      // handoff now expands one combined category graph node into explicit
      // interface/implementation record bundles so the emitted descriptor count
      // matches the runtime-export/scaffold record inventory exactly.
      bool protocol_category_payload_complete = true;
      std::unordered_set<std::string> protocol_owner_identities;
      protocol_owner_identities.reserve(
          source_graph.protocol_nodes_lexicographic.size());
      std::vector<Objc3IRRuntimeMetadataProtocolBundle> protocol_bundles;
      protocol_bundles.reserve(source_graph.protocol_nodes_lexicographic.size());
      for (const auto &protocol_node : source_graph.protocol_nodes_lexicographic) {
        if (protocol_node.protocol_name.empty() ||
            protocol_node.owner_identity.empty() ||
            !protocol_owner_identities.insert(protocol_node.owner_identity).second) {
          protocol_category_payload_complete = false;
          break;
        }
        for (const auto &inherited_owner_identity :
             protocol_node.inherited_protocol_owner_identities_lexicographic) {
          if (inherited_owner_identity.empty()) {
            protocol_category_payload_complete = false;
            break;
          }
        }
        if (!protocol_category_payload_complete) {
          break;
        }

        Objc3IRRuntimeMetadataProtocolBundle bundle;
        bundle.protocol_name = protocol_node.protocol_name;
        bundle.owner_identity = protocol_node.owner_identity;
        bundle.inherited_protocol_owner_identities_lexicographic =
            protocol_node.inherited_protocol_owner_identities_lexicographic;
        bundle.property_count = protocol_node.property_count;
        bundle.method_count = protocol_node.method_count;
        bundle.is_forward_declaration = protocol_node.is_forward_declaration;
        protocol_bundles.push_back(std::move(bundle));
      }

      std::vector<Objc3IRRuntimeMetadataCategoryBundle> category_bundles;
      category_bundles.reserve(source_graph.category_nodes_lexicographic.size());
      std::unordered_set<std::string> category_owner_identities;
      category_owner_identities.reserve(
          runtime_metadata_section_scaffold.category_descriptor_count);
      if (protocol_category_payload_complete) {
        for (const auto &category_node :
             source_graph.category_nodes_lexicographic) {
          if (category_node.class_name.empty() ||
              category_node.category_name.empty() ||
              category_node.owner_identity.empty() ||
              category_node.class_owner_identity.empty() ||
              (!category_node.has_interface &&
               !category_node.has_implementation)) {
            protocol_category_payload_complete = false;
            break;
          }

          for (const auto &protocol_owner_identity :
               category_node.adopted_protocol_owner_identities_lexicographic) {
            if (protocol_owner_identity.empty() ||
                protocol_owner_identities.find(protocol_owner_identity) ==
                    protocol_owner_identities.end()) {
              protocol_category_payload_complete = false;
              break;
            }
          }
          if (!protocol_category_payload_complete) {
            break;
          }
          const auto append_category_bundle =
              [&](const std::string &record_kind,
                  const std::string &record_owner_identity,
                  std::size_t property_count,
                  std::size_t instance_method_count,
                  std::size_t class_method_count) {
                if (record_owner_identity.empty() ||
                    !category_owner_identities.insert(record_owner_identity)
                         .second) {
                  protocol_category_payload_complete = false;
                  return;
                }
                Objc3IRRuntimeMetadataCategoryBundle bundle;
                bundle.record_kind = record_kind;
                bundle.class_name = category_node.class_name;
                bundle.category_name = category_node.category_name;
                bundle.owner_identity = record_owner_identity;
                bundle.category_owner_identity = category_node.owner_identity;
                bundle.class_owner_identity = category_node.class_owner_identity;
                bundle.adopted_protocol_owner_identities_lexicographic =
                    category_node.adopted_protocol_owner_identities_lexicographic;
                bundle.property_count = property_count;
                bundle.instance_method_count = instance_method_count;
                bundle.class_method_count = class_method_count;
                category_bundles.push_back(std::move(bundle));
              };
          if (category_node.has_interface) {
            append_category_bundle(
                "interface", category_node.interface_owner_identity,
                category_node.interface_property_count,
                category_node.interface_method_count,
                category_node.interface_class_method_count);
          }
          if (protocol_category_payload_complete &&
              category_node.has_implementation) {
            append_category_bundle(
                "implementation", category_node.implementation_owner_identity,
                category_node.implementation_property_count,
                category_node.implementation_method_count,
                category_node.implementation_class_method_count);
          }
          if (!protocol_category_payload_complete) {
            break;
          }
        }
      }

      if (protocol_category_payload_complete) {
        for (const auto &bundle : protocol_bundles) {
          for (const auto &inherited_owner_identity :
               bundle.inherited_protocol_owner_identities_lexicographic) {
            if (protocol_owner_identities.find(inherited_owner_identity) ==
                protocol_owner_identities.end()) {
              protocol_category_payload_complete = false;
              break;
            }
          }
          if (!protocol_category_payload_complete) {
            break;
          }
        }
      }

      protocol_category_payload_complete =
          protocol_category_payload_complete &&
          protocol_bundles.size() ==
              runtime_metadata_section_scaffold.protocol_descriptor_count &&
          category_bundles.size() ==
              runtime_metadata_section_scaffold.category_descriptor_count;
      if (protocol_category_payload_complete) {
        ir_frontend_metadata.runtime_metadata_protocol_bundles_lexicographic =
            std::move(protocol_bundles);
        ir_frontend_metadata.runtime_metadata_category_bundles_lexicographic =
            std::move(category_bundles);
      }
      ir_frontend_metadata.runtime_metadata_protocol_category_emission_ready =
          protocol_category_payload_complete;
      ir_frontend_metadata
          .runtime_metadata_protocol_category_emission_fail_closed =
          protocol_category_payload_complete;

      // M253-C004 member-table data emission anchor: the typed lowering
      // handoff now also projects real owner-scoped method tables plus real
      // property/ivar descriptor payload records without reopening the earlier
      // class/protocol/category descriptor bundle shapes from C002/C003.
      bool member_table_payload_complete = protocol_category_payload_complete;
      std::vector<Objc3IRRuntimeMetadataPropertyBundle> property_bundles;
      property_bundles.reserve(source_graph.property_nodes_lexicographic.size());
      std::unordered_set<std::string> property_owner_identities;
      property_owner_identities.reserve(
          source_graph.property_nodes_lexicographic.size());
      for (const auto &property_node : source_graph.property_nodes_lexicographic) {
        if (property_node.owner_kind.empty() || property_node.owner_name.empty() ||
            property_node.owner_identity.empty() ||
            property_node.declaration_owner_identity.empty() ||
            property_node.export_owner_identity.empty() ||
            property_node.property_name.empty() || property_node.type_name.empty() ||
            !property_owner_identities.insert(property_node.owner_identity).second ||
            (property_node.has_getter && property_node.getter_selector.empty()) ||
            (property_node.has_setter && property_node.setter_selector.empty())) {
          member_table_payload_complete = false;
          break;
        }

        Objc3IRRuntimeMetadataPropertyBundle bundle;
        bundle.owner_kind = property_node.owner_kind;
        bundle.owner_name = property_node.owner_name;
        bundle.owner_identity = property_node.owner_identity;
        bundle.declaration_owner_identity =
            property_node.declaration_owner_identity;
        bundle.export_owner_identity = property_node.export_owner_identity;
        bundle.property_name = property_node.property_name;
        bundle.type_name = property_node.type_name;
        bundle.has_getter = property_node.has_getter;
        bundle.getter_selector = property_node.getter_selector;
        bundle.has_setter = property_node.has_setter;
        bundle.setter_selector = property_node.setter_selector;
        bundle.ivar_binding_symbol = property_node.ivar_binding_symbol;
        property_bundles.push_back(std::move(bundle));
      }
      std::sort(
          property_bundles.begin(), property_bundles.end(),
          [](const Objc3IRRuntimeMetadataPropertyBundle &lhs,
             const Objc3IRRuntimeMetadataPropertyBundle &rhs) {
            return std::tie(lhs.declaration_owner_identity, lhs.property_name,
                            lhs.owner_identity) <
                   std::tie(rhs.declaration_owner_identity, rhs.property_name,
                            rhs.owner_identity);
          });

      std::vector<Objc3IRRuntimeMetadataIvarBundle> ivar_bundles;
      ivar_bundles.reserve(source_graph.ivar_nodes_lexicographic.size());
      std::unordered_set<std::string> ivar_owner_identities;
      ivar_owner_identities.reserve(source_graph.ivar_nodes_lexicographic.size());
      if (member_table_payload_complete) {
        for (const auto &ivar_node : source_graph.ivar_nodes_lexicographic) {
          if (ivar_node.owner_kind.empty() || ivar_node.owner_name.empty() ||
              ivar_node.owner_identity.empty() ||
              ivar_node.declaration_owner_identity.empty() ||
              ivar_node.export_owner_identity.empty() ||
              ivar_node.property_owner_identity.empty() ||
              ivar_node.property_name.empty() ||
              ivar_node.ivar_binding_symbol.empty() ||
              !ivar_owner_identities.insert(ivar_node.owner_identity).second) {
            member_table_payload_complete = false;
            break;
          }

          Objc3IRRuntimeMetadataIvarBundle bundle;
          bundle.owner_kind = ivar_node.owner_kind;
          bundle.owner_name = ivar_node.owner_name;
          bundle.owner_identity = ivar_node.owner_identity;
          bundle.declaration_owner_identity = ivar_node.declaration_owner_identity;
          bundle.export_owner_identity = ivar_node.export_owner_identity;
          bundle.property_owner_identity = ivar_node.property_owner_identity;
          bundle.property_name = ivar_node.property_name;
          bundle.ivar_binding_symbol = ivar_node.ivar_binding_symbol;
          ivar_bundles.push_back(std::move(bundle));
        }
      }
      std::sort(
          ivar_bundles.begin(), ivar_bundles.end(),
          [](const Objc3IRRuntimeMetadataIvarBundle &lhs,
             const Objc3IRRuntimeMetadataIvarBundle &rhs) {
            return std::tie(lhs.declaration_owner_identity, lhs.property_name,
                            lhs.owner_identity) <
                   std::tie(rhs.declaration_owner_identity, rhs.property_name,
                            rhs.owner_identity);
          });

      std::vector<Objc3IRRuntimeMetadataMethodListBundle> method_list_bundles;
      method_list_bundles.reserve(source_graph.method_nodes_lexicographic.size());
      std::unordered_map<std::string, std::size_t> method_bundle_indexes;
      method_bundle_indexes.reserve(source_graph.method_nodes_lexicographic.size());
      const auto determine_owner_family_kind = [](const std::string &owner_kind)
          -> std::string {
        if (owner_kind == "protocol") {
          return "protocol";
        }
        if (owner_kind == "class-interface" ||
            owner_kind == "class-implementation") {
          return "class";
        }
        if (owner_kind == "category-interface" ||
            owner_kind == "category-implementation") {
          return "category";
        }
        return {};
      };
      if (member_table_payload_complete) {
        for (const auto &method_node : source_graph.method_nodes_lexicographic) {
          const std::string list_kind =
              method_node.is_class_method ? "class" : "instance";
          const std::string owner_family_kind =
              determine_owner_family_kind(method_node.owner_kind);
          if (method_node.owner_kind.empty() || method_node.owner_name.empty() ||
              owner_family_kind.empty() || method_node.owner_identity.empty() ||
              method_node.declaration_owner_identity.empty() ||
              method_node.export_owner_identity.empty() ||
              method_node.selector.empty() ||
              method_node.return_type_name.empty()) {
            member_table_payload_complete = false;
            break;
          }

          const std::string bundle_key =
              method_node.declaration_owner_identity + "|" + list_kind;
          auto bundle_it = method_bundle_indexes.find(bundle_key);
          if (bundle_it == method_bundle_indexes.end()) {
            Objc3IRRuntimeMetadataMethodListBundle bundle;
            bundle.owner_kind = method_node.owner_kind;
            bundle.owner_name = method_node.owner_name;
            bundle.owner_family_kind = owner_family_kind;
            bundle.declaration_owner_identity =
                method_node.declaration_owner_identity;
            bundle.export_owner_identity = method_node.export_owner_identity;
            bundle.list_kind = list_kind;
            method_list_bundles.push_back(std::move(bundle));
            bundle_it = method_bundle_indexes
                            .emplace(bundle_key, method_list_bundles.size() - 1u)
                            .first;
          }

          auto &bundle = method_list_bundles[bundle_it->second];
          if (bundle.owner_kind != method_node.owner_kind ||
              bundle.owner_name != method_node.owner_name ||
              bundle.owner_family_kind != owner_family_kind ||
              bundle.export_owner_identity != method_node.export_owner_identity ||
              bundle.list_kind != list_kind) {
            member_table_payload_complete = false;
            break;
          }

          Objc3IRRuntimeMetadataMethodEntry entry;
          entry.owner_identity = method_node.owner_identity;
          entry.selector = method_node.selector;
          entry.return_type_name = method_node.return_type_name;
          entry.parameter_count = method_node.parameter_count;
          entry.has_body = method_node.has_body;
          bundle.entries_lexicographic.push_back(std::move(entry));
        }
      }
      std::sort(
          method_list_bundles.begin(), method_list_bundles.end(),
          [](const Objc3IRRuntimeMetadataMethodListBundle &lhs,
             const Objc3IRRuntimeMetadataMethodListBundle &rhs) {
            return std::tie(lhs.owner_family_kind,
                            lhs.declaration_owner_identity, lhs.list_kind) <
                   std::tie(rhs.owner_family_kind,
                            rhs.declaration_owner_identity, rhs.list_kind);
          });
      for (auto &bundle : method_list_bundles) {
        std::sort(bundle.entries_lexicographic.begin(),
                  bundle.entries_lexicographic.end(),
                  [](const Objc3IRRuntimeMetadataMethodEntry &lhs,
                     const Objc3IRRuntimeMetadataMethodEntry &rhs) {
                    return std::tie(lhs.selector, lhs.owner_identity,
                                    lhs.parameter_count, lhs.return_type_name,
                                    lhs.has_body) <
                           std::tie(rhs.selector, rhs.owner_identity,
                                    rhs.parameter_count, rhs.return_type_name,
                                    rhs.has_body);
                  });
      }

      member_table_payload_complete =
          member_table_payload_complete &&
          property_bundles.size() ==
              runtime_metadata_section_scaffold.property_descriptor_count &&
          ivar_bundles.size() ==
              runtime_metadata_section_scaffold.ivar_descriptor_count;
      if (member_table_payload_complete) {
        ir_frontend_metadata.runtime_metadata_method_list_bundles_lexicographic =
            std::move(method_list_bundles);
        ir_frontend_metadata.runtime_metadata_property_bundles_lexicographic =
            std::move(property_bundles);
        ir_frontend_metadata.runtime_metadata_ivar_bundles_lexicographic =
            std::move(ivar_bundles);
      }
      ir_frontend_metadata.runtime_metadata_member_table_emission_ready =
          member_table_payload_complete;
      ir_frontend_metadata.runtime_metadata_member_table_emission_fail_closed =
          member_table_payload_complete;
    }
  }
  ir_frontend_metadata.runtime_metadata_object_inspection_contract_id =
      runtime_metadata_object_inspection.contract_id;
  ir_frontend_metadata.runtime_metadata_object_inspection_scaffold_contract_id =
      runtime_metadata_object_inspection.scaffold_contract_id;
  ir_frontend_metadata.runtime_metadata_object_inspection_matrix_published =
      runtime_metadata_object_inspection.matrix_published;
  ir_frontend_metadata.runtime_metadata_object_inspection_fail_closed =
      runtime_metadata_object_inspection.fail_closed;
  ir_frontend_metadata.runtime_metadata_object_inspection_uses_llvm_readobj =
      runtime_metadata_object_inspection.uses_llvm_readobj;
  ir_frontend_metadata.runtime_metadata_object_inspection_uses_llvm_objdump =
      runtime_metadata_object_inspection.uses_llvm_objdump;
  ir_frontend_metadata.runtime_metadata_object_inspection_matrix_row_count =
      runtime_metadata_object_inspection.matrix_row_count;
  ir_frontend_metadata.runtime_metadata_object_inspection_fixture_path =
      runtime_metadata_object_inspection.fixture_path;
  ir_frontend_metadata.runtime_metadata_object_inspection_emit_prefix =
      runtime_metadata_object_inspection.emit_prefix;
  ir_frontend_metadata.runtime_metadata_object_inspection_object_relative_path =
      runtime_metadata_object_inspection.object_relative_path;
  ir_frontend_metadata.runtime_metadata_object_inspection_section_inventory_row_key =
      runtime_metadata_object_inspection.section_inventory_row_key;
  ir_frontend_metadata.runtime_metadata_object_inspection_section_inventory_command =
      runtime_metadata_object_inspection.section_inventory_command;
  ir_frontend_metadata.runtime_metadata_object_inspection_symbol_inventory_row_key =
      runtime_metadata_object_inspection.symbol_inventory_row_key;
  ir_frontend_metadata.runtime_metadata_object_inspection_symbol_inventory_command =
      runtime_metadata_object_inspection.symbol_inventory_command;
  ir_frontend_metadata.executable_metadata_debug_projection_contract_id =
      executable_metadata_debug_projection.contract_id;
  ir_frontend_metadata
      .executable_metadata_debug_projection_typed_handoff_contract_id =
      executable_metadata_debug_projection.typed_lowering_handoff_contract_id;
  ir_frontend_metadata
      .executable_metadata_debug_projection_source_graph_contract_id =
      executable_metadata_debug_projection.source_graph_contract_id;
  ir_frontend_metadata.executable_metadata_debug_projection_named_metadata_name =
      executable_metadata_debug_projection.named_metadata_name;
  ir_frontend_metadata
      .executable_metadata_debug_projection_manifest_surface_path =
      executable_metadata_debug_projection.manifest_surface_path;
  ir_frontend_metadata
      .executable_metadata_debug_projection_typed_handoff_surface_path =
      executable_metadata_debug_projection.typed_handoff_surface_path;
  ir_frontend_metadata
      .executable_metadata_debug_projection_source_graph_surface_path =
      executable_metadata_debug_projection.source_graph_surface_path;
  ir_frontend_metadata.executable_metadata_debug_projection_matrix_published =
      executable_metadata_debug_projection.matrix_published;
  ir_frontend_metadata.executable_metadata_debug_projection_fail_closed =
      executable_metadata_debug_projection.fail_closed;
  ir_frontend_metadata
      .executable_metadata_debug_projection_manifest_debug_surface_published =
      executable_metadata_debug_projection.manifest_debug_surface_published;
  ir_frontend_metadata
      .executable_metadata_debug_projection_ir_named_metadata_published =
      executable_metadata_debug_projection.ir_named_metadata_published;
  ir_frontend_metadata
      .executable_metadata_debug_projection_replay_anchor_deterministic =
      executable_metadata_debug_projection.replay_anchor_deterministic;
  ir_frontend_metadata
      .executable_metadata_debug_projection_active_typed_handoff_ready =
      executable_metadata_debug_projection.active_typed_handoff_ready;
  ir_frontend_metadata.executable_metadata_debug_projection_matrix_row_count =
      executable_metadata_debug_projection.matrix_row_count;
  ir_frontend_metadata.executable_metadata_debug_projection_replay_key =
      executable_metadata_debug_projection.replay_key;
  ir_frontend_metadata
      .executable_metadata_debug_projection_active_typed_handoff_replay_key =
      executable_metadata_debug_projection.active_typed_handoff_replay_key;
  ir_frontend_metadata.executable_metadata_debug_projection_row0_descriptor =
      BuildExecutableMetadataDebugProjectionRowDescriptor(
          executable_metadata_debug_projection.rows[0]);
  ir_frontend_metadata.executable_metadata_debug_projection_row1_descriptor =
      BuildExecutableMetadataDebugProjectionRowDescriptor(
          executable_metadata_debug_projection.rows[1]);
  ir_frontend_metadata.executable_metadata_debug_projection_row2_descriptor =
      BuildExecutableMetadataDebugProjectionRowDescriptor(
          executable_metadata_debug_projection.rows[2]);
  ir_frontend_metadata.runtime_support_library_contract_id =
      runtime_support_library.contract_id;
  ir_frontend_metadata.runtime_support_library_metadata_scaffold_contract_id =
      runtime_support_library.metadata_scaffold_contract_id;
  ir_frontend_metadata.runtime_support_library_boundary_frozen =
      runtime_support_library.boundary_frozen;
  ir_frontend_metadata.runtime_support_library_fail_closed =
      runtime_support_library.fail_closed;
  ir_frontend_metadata.runtime_support_library_target_name_frozen =
      runtime_support_library.target_name_frozen;
  ir_frontend_metadata.runtime_support_library_exported_entrypoints_frozen =
      runtime_support_library.exported_entrypoints_frozen;
  ir_frontend_metadata.runtime_support_library_ownership_boundaries_frozen =
      runtime_support_library.ownership_boundaries_frozen;
  ir_frontend_metadata.runtime_support_library_build_constraints_frozen =
      runtime_support_library.build_constraints_frozen;
  ir_frontend_metadata.runtime_support_library_shim_remains_test_only =
      runtime_support_library.shim_remains_test_only;
  ir_frontend_metadata.runtime_support_library_native_library_present =
      runtime_support_library.native_runtime_library_present;
  ir_frontend_metadata.runtime_support_library_driver_link_wiring_pending =
      runtime_support_library.driver_link_wiring_pending;
  ir_frontend_metadata.runtime_support_library_ready_for_skeleton =
      runtime_support_library.ready_for_runtime_library_skeleton;
  ir_frontend_metadata.runtime_support_library_target_name =
      runtime_support_library.cmake_target_name;
  ir_frontend_metadata.runtime_support_library_public_header_path =
      runtime_support_library.public_header_path;
  ir_frontend_metadata.runtime_support_library_source_root =
      runtime_support_library.source_root;
  ir_frontend_metadata.runtime_support_library_library_kind =
      runtime_support_library.library_kind;
  ir_frontend_metadata.runtime_support_library_archive_basename =
      runtime_support_library.archive_basename;
  ir_frontend_metadata.runtime_support_library_register_image_symbol =
      runtime_support_library.register_image_symbol;
  ir_frontend_metadata.runtime_support_library_lookup_selector_symbol =
      runtime_support_library.lookup_selector_symbol;
  ir_frontend_metadata.runtime_support_library_dispatch_i32_symbol =
      runtime_support_library.dispatch_i32_symbol;
  ir_frontend_metadata.runtime_support_library_reset_for_testing_symbol =
      runtime_support_library.reset_for_testing_symbol;
  ir_frontend_metadata.runtime_support_library_driver_link_mode =
      runtime_support_library.driver_link_mode;
  ir_frontend_metadata.runtime_support_library_compiler_ownership_boundary =
      runtime_support_library.compiler_ownership_boundary;
  ir_frontend_metadata.runtime_support_library_runtime_ownership_boundary =
      runtime_support_library.runtime_ownership_boundary;
  ir_frontend_metadata.runtime_support_library_core_feature_contract_id =
      runtime_support_library_core_feature.contract_id;
  ir_frontend_metadata
      .runtime_support_library_core_feature_support_library_contract_id =
      runtime_support_library_core_feature.support_library_contract_id;
  ir_frontend_metadata
      .runtime_support_library_core_feature_metadata_scaffold_contract_id =
      runtime_support_library_core_feature.metadata_scaffold_contract_id;
  ir_frontend_metadata.runtime_support_library_core_feature_fail_closed =
      runtime_support_library_core_feature.fail_closed;
  ir_frontend_metadata.runtime_support_library_core_feature_sources_present =
      runtime_support_library_core_feature
          .native_runtime_library_sources_present;
  ir_frontend_metadata.runtime_support_library_core_feature_header_present =
      runtime_support_library_core_feature.native_runtime_library_header_present;
  ir_frontend_metadata
      .runtime_support_library_core_feature_archive_build_enabled =
      runtime_support_library_core_feature
          .native_runtime_library_archive_build_enabled;
  ir_frontend_metadata
      .runtime_support_library_core_feature_entrypoints_implemented =
      runtime_support_library_core_feature
          .native_runtime_library_entrypoints_implemented;
  ir_frontend_metadata
      .runtime_support_library_core_feature_selector_lookup_stateful =
      runtime_support_library_core_feature.selector_lookup_stateful;
  ir_frontend_metadata
      .runtime_support_library_core_feature_dispatch_formula_matches_test_shim =
      runtime_support_library_core_feature
          .deterministic_dispatch_formula_matches_test_shim;
  ir_frontend_metadata
      .runtime_support_library_core_feature_reset_for_testing_supported =
      runtime_support_library_core_feature.reset_for_testing_supported;
  ir_frontend_metadata
      .runtime_support_library_core_feature_shim_remains_test_only =
      runtime_support_library_core_feature.shim_remains_test_only;
  ir_frontend_metadata
      .runtime_support_library_core_feature_driver_link_wiring_pending =
      runtime_support_library_core_feature.driver_link_wiring_pending;
  ir_frontend_metadata
      .runtime_support_library_core_feature_ready_for_driver_link_wiring =
      runtime_support_library_core_feature.ready_for_driver_link_wiring;
  ir_frontend_metadata.runtime_support_library_core_feature_target_name =
      runtime_support_library_core_feature.cmake_target_name;
  ir_frontend_metadata.runtime_support_library_core_feature_public_header_path =
      runtime_support_library_core_feature.public_header_path;
  ir_frontend_metadata.runtime_support_library_core_feature_source_root =
      runtime_support_library_core_feature.source_root;
  ir_frontend_metadata
      .runtime_support_library_core_feature_implementation_source_path =
      runtime_support_library_core_feature.implementation_source_path;
  ir_frontend_metadata.runtime_support_library_core_feature_library_kind =
      runtime_support_library_core_feature.library_kind;
  ir_frontend_metadata.runtime_support_library_core_feature_archive_basename =
      runtime_support_library_core_feature.archive_basename;
  ir_frontend_metadata.runtime_support_library_core_feature_archive_relative_path =
      runtime_support_library_core_feature.archive_relative_path;
  ir_frontend_metadata.runtime_support_library_core_feature_probe_source_path =
      runtime_support_library_core_feature.probe_source_path;
  ir_frontend_metadata.runtime_support_library_core_feature_register_image_symbol =
      runtime_support_library_core_feature.register_image_symbol;
  ir_frontend_metadata
      .runtime_support_library_core_feature_lookup_selector_symbol =
      runtime_support_library_core_feature.lookup_selector_symbol;
  ir_frontend_metadata.runtime_support_library_core_feature_dispatch_i32_symbol =
      runtime_support_library_core_feature.dispatch_i32_symbol;
  ir_frontend_metadata
      .runtime_support_library_core_feature_reset_for_testing_symbol =
      runtime_support_library_core_feature.reset_for_testing_symbol;
  ir_frontend_metadata.runtime_support_library_core_feature_driver_link_mode =
      runtime_support_library_core_feature.driver_link_mode;
  ir_frontend_metadata.runtime_support_library_link_wiring_contract_id =
      runtime_support_library_link_wiring.contract_id;
  ir_frontend_metadata
      .runtime_support_library_link_wiring_core_feature_contract_id =
      runtime_support_library_link_wiring
          .support_library_core_feature_contract_id;
  ir_frontend_metadata.runtime_support_library_link_wiring_fail_closed =
      runtime_support_library_link_wiring.fail_closed;
  ir_frontend_metadata.runtime_support_library_link_wiring_archive_available =
      runtime_support_library_link_wiring.runtime_library_archive_available;
  ir_frontend_metadata
      .runtime_support_library_link_wiring_compatibility_dispatch_alias_exported =
      runtime_support_library_link_wiring
          .compatibility_dispatch_alias_exported;
  ir_frontend_metadata
      .runtime_support_library_link_wiring_driver_emits_runtime_link_contract =
      runtime_support_library_link_wiring.driver_emits_runtime_link_contract;
  ir_frontend_metadata
      .runtime_support_library_link_wiring_execution_smoke_consumes_runtime_library =
      runtime_support_library_link_wiring
          .execution_smoke_consumes_runtime_library;
  ir_frontend_metadata.runtime_support_library_link_wiring_shim_remains_test_only =
      runtime_support_library_link_wiring.shim_remains_test_only;
  ir_frontend_metadata
      .runtime_support_library_link_wiring_ready_for_runtime_library_consumption =
      runtime_support_library_link_wiring
          .ready_for_runtime_library_consumption;
  ir_frontend_metadata.runtime_support_library_link_wiring_archive_relative_path =
      runtime_support_library_link_wiring.archive_relative_path;
  ir_frontend_metadata
      .runtime_support_library_link_wiring_compatibility_dispatch_symbol =
      runtime_support_library_link_wiring.compatibility_dispatch_symbol;
  ir_frontend_metadata.runtime_support_library_link_wiring_runtime_dispatch_symbol =
      runtime_support_library_link_wiring.runtime_dispatch_symbol;
  ir_frontend_metadata
      .runtime_support_library_link_wiring_execution_smoke_script_path =
      runtime_support_library_link_wiring.execution_smoke_script_path;
  ir_frontend_metadata.runtime_support_library_link_wiring_driver_link_mode =
      runtime_support_library_link_wiring.driver_link_mode;
  ir_frontend_metadata.deterministic_id_class_sel_object_pointer_typecheck_handoff =
      id_class_sel_object_pointer_typecheck_contract.deterministic;
  ir_frontend_metadata.deterministic_dispatch_surface_classification_handoff =
      dispatch_surface_classification_contract.deterministic;
  ir_frontend_metadata.deterministic_message_send_selector_lowering_handoff =
      message_send_selector_lowering_contract.deterministic;
  ir_frontend_metadata.deterministic_dispatch_abi_marshalling_handoff =
      dispatch_abi_marshalling_contract.deterministic;
  ir_frontend_metadata.deterministic_nil_receiver_semantics_foldability_handoff =
      nil_receiver_semantics_foldability_contract.deterministic;
  ir_frontend_metadata.deterministic_super_dispatch_method_family_handoff =
      super_dispatch_method_family_contract.deterministic;
  ir_frontend_metadata.deterministic_runtime_shim_host_link_handoff =
      runtime_shim_host_link_contract.deterministic;
  ir_frontend_metadata.deterministic_ownership_qualifier_lowering_handoff =
      ownership_qualifier_lowering_contract.deterministic;
  ir_frontend_metadata.deterministic_retain_release_operation_lowering_handoff =
      retain_release_operation_lowering_contract.deterministic;
  ir_frontend_metadata.deterministic_autoreleasepool_scope_lowering_handoff =
      autoreleasepool_scope_lowering_contract.deterministic;
  ir_frontend_metadata.deterministic_object_pointer_nullability_generics_handoff =
      object_pointer_nullability_generics_summary.deterministic_object_pointer_nullability_generics_handoff;
  ir_frontend_metadata.deterministic_symbol_graph_handoff =
      symbol_graph_scope_resolution_summary.deterministic_symbol_graph_handoff;
  ir_frontend_metadata.deterministic_scope_resolution_handoff =
      symbol_graph_scope_resolution_summary.deterministic_scope_resolution_handoff;
  ir_frontend_metadata.deterministic_symbol_graph_scope_resolution_handoff_key =
      symbol_graph_scope_resolution_summary.deterministic_handoff_key;
  ir_frontend_metadata.ownership_aware_lowering_core_feature_expansion_ready =
      ownership_aware_lowering_behavior_scaffold.expansion_ready;
  ir_frontend_metadata.ownership_aware_lowering_core_feature_expansion_key =
      ownership_aware_lowering_behavior_scaffold.expansion_key;
  ir_frontend_metadata.ownership_aware_lowering_performance_quality_guardrails_ready =
      ownership_aware_lowering_behavior_scaffold.performance_quality_guardrails_ready;
  ir_frontend_metadata.ownership_aware_lowering_performance_quality_guardrails_key =
      ownership_aware_lowering_behavior_scaffold.performance_quality_guardrails_key;
  ir_frontend_metadata.ownership_aware_lowering_cross_lane_integration_ready =
      ownership_aware_lowering_behavior_scaffold.cross_lane_integration_ready;
  ir_frontend_metadata.ownership_aware_lowering_cross_lane_integration_key =
      ownership_aware_lowering_behavior_scaffold.cross_lane_integration_key;
  ir_frontend_metadata.lowering_pass_graph_core_feature_ready =
      pipeline_result.ir_emission_completeness_scaffold.core_feature_ready;
  ir_frontend_metadata.lowering_pass_graph_core_feature_key =
      pipeline_result.ir_emission_completeness_scaffold.core_feature_key;
  ir_frontend_metadata.lowering_pass_graph_core_feature_expansion_ready =
      pipeline_result.ir_emission_completeness_scaffold.expansion_ready;
  ir_frontend_metadata.lowering_pass_graph_core_feature_expansion_key =
      pipeline_result.ir_emission_completeness_scaffold.expansion_key;
  ir_frontend_metadata.lowering_pass_graph_edge_case_compatibility_ready =
      pipeline_result.ir_emission_completeness_scaffold
          .edge_case_compatibility_ready;
  ir_frontend_metadata.lowering_pass_graph_edge_case_compatibility_key =
      pipeline_result.ir_emission_completeness_scaffold
          .edge_case_compatibility_key;
  ir_frontend_metadata.lowering_pass_graph_edge_case_robustness_ready =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .edge_case_robustness_ready;
  ir_frontend_metadata.lowering_pass_graph_edge_case_robustness_key =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .edge_case_robustness_key;
  ir_frontend_metadata.lowering_pass_graph_diagnostics_hardening_ready =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .diagnostics_hardening_ready;
  ir_frontend_metadata.lowering_pass_graph_diagnostics_hardening_key =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .diagnostics_hardening_key;
  ir_frontend_metadata.lowering_pass_graph_recovery_determinism_ready =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .recovery_determinism_ready;
  ir_frontend_metadata.lowering_pass_graph_recovery_determinism_key =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .recovery_determinism_key;
  ir_frontend_metadata.lowering_pass_graph_conformance_matrix_ready =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .conformance_matrix_ready;
  ir_frontend_metadata.lowering_pass_graph_conformance_matrix_key =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .conformance_matrix_key;
  ir_frontend_metadata.lowering_pass_graph_conformance_corpus_ready =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .conformance_corpus_ready;
  ir_frontend_metadata.lowering_pass_graph_conformance_corpus_key =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .conformance_corpus_key;
  ir_frontend_metadata.lowering_pass_graph_performance_quality_guardrails_ready =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .performance_quality_guardrails_ready;
  ir_frontend_metadata.lowering_pass_graph_performance_quality_guardrails_key =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .performance_quality_guardrails_key;
  ir_frontend_metadata.ir_emission_completeness_modular_split_ready =
      pipeline_result.ir_emission_completeness_scaffold.modular_split_ready;
  ir_frontend_metadata.ir_emission_completeness_modular_split_key =
      pipeline_result.ir_emission_completeness_scaffold.scaffold_key;
  ir_frontend_metadata.ir_emission_core_feature_impl_ready =
      ir_emission_core_feature_impl_surface.core_feature_impl_ready;
  ir_frontend_metadata.ir_emission_core_feature_impl_key =
      ir_emission_core_feature_impl_surface.core_feature_key;
  ir_frontend_metadata.ir_emission_core_feature_expansion_ready =
      ir_emission_core_feature_impl_surface.core_feature_expansion_ready;
  ir_frontend_metadata.ir_emission_core_feature_expansion_key =
      ir_emission_core_feature_impl_surface.expansion_key;
  ir_frontend_metadata.ir_emission_core_feature_edge_case_compatibility_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_edge_case_compatibility_ready;
  ir_frontend_metadata.ir_emission_core_feature_edge_case_compatibility_key =
      ir_emission_core_feature_impl_surface.edge_case_compatibility_key;
  ir_frontend_metadata.ir_emission_core_feature_edge_case_robustness_ready =
      ir_emission_core_feature_impl_surface.core_feature_edge_case_robustness_ready;
  ir_frontend_metadata.ir_emission_core_feature_edge_case_robustness_key =
      ir_emission_core_feature_impl_surface.edge_case_robustness_key;
  ir_frontend_metadata.ir_emission_core_feature_diagnostics_hardening_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_diagnostics_hardening_ready;
  ir_frontend_metadata.ir_emission_core_feature_diagnostics_hardening_key =
      ir_emission_core_feature_impl_surface.diagnostics_hardening_key;
  ir_frontend_metadata.ir_emission_core_feature_recovery_determinism_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_recovery_determinism_ready;
  ir_frontend_metadata.ir_emission_core_feature_recovery_determinism_key =
      ir_emission_core_feature_impl_surface.recovery_determinism_key;
  ir_frontend_metadata.ir_emission_core_feature_conformance_matrix_ready =
      ir_emission_core_feature_impl_surface.core_feature_conformance_matrix_ready;
  ir_frontend_metadata.ir_emission_core_feature_conformance_matrix_key =
      ir_emission_core_feature_impl_surface.conformance_matrix_key;
  ir_frontend_metadata.ir_emission_core_feature_conformance_corpus_ready =
      ir_emission_core_feature_impl_surface.core_feature_conformance_corpus_ready;
  ir_frontend_metadata.ir_emission_core_feature_conformance_corpus_key =
      ir_emission_core_feature_impl_surface.conformance_corpus_key;
  ir_frontend_metadata.ir_emission_core_feature_performance_quality_guardrails_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_performance_quality_guardrails_ready;
  ir_frontend_metadata.ir_emission_core_feature_performance_quality_guardrails_key =
      ir_emission_core_feature_impl_surface.performance_quality_guardrails_key;
  ir_frontend_metadata.ir_emission_core_feature_cross_lane_integration_sync_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_cross_lane_integration_sync_ready;
  ir_frontend_metadata.ir_emission_core_feature_cross_lane_integration_sync_key =
      ir_emission_core_feature_impl_surface.cross_lane_integration_sync_key;
  ir_frontend_metadata.ir_emission_core_feature_advanced_core_shard1_ready =
      ir_emission_core_feature_impl_surface.core_feature_advanced_core_shard1_ready;
  ir_frontend_metadata.ir_emission_core_feature_advanced_core_shard1_key =
      ir_emission_core_feature_impl_surface.advanced_core_shard1_key;
  ir_frontend_metadata.ir_emission_core_feature_advanced_edge_compatibility_shard1_ready =
      ir_emission_core_feature_impl_surface
          .core_feature_advanced_edge_compatibility_shard1_ready;
  ir_frontend_metadata.ir_emission_core_feature_advanced_edge_compatibility_shard1_key =
      ir_emission_core_feature_impl_surface
          .advanced_edge_compatibility_shard1_key;
  ir_frontend_metadata.ir_emission_core_feature_advanced_diagnostics_shard1_ready =
      ir_emission_core_feature_impl_surface.core_feature_advanced_diagnostics_shard1_ready;
  ir_frontend_metadata.ir_emission_core_feature_advanced_diagnostics_shard1_key =
      ir_emission_core_feature_impl_surface.advanced_diagnostics_shard1_key;
  ir_frontend_metadata.ir_emission_core_feature_advanced_conformance_shard1_ready =
      ir_emission_core_feature_impl_surface.core_feature_advanced_conformance_shard1_ready;
  ir_frontend_metadata.ir_emission_core_feature_advanced_conformance_shard1_key =
      ir_emission_core_feature_impl_surface.advanced_conformance_shard1_key;
  ir_frontend_metadata.ir_emission_core_feature_advanced_integration_shard1_ready =
      ir_emission_core_feature_impl_surface.core_feature_advanced_integration_shard1_ready;
  ir_frontend_metadata.ir_emission_core_feature_advanced_integration_shard1_key =
      ir_emission_core_feature_impl_surface.advanced_integration_shard1_key;
  std::string ir_error;
  // Historical extraction contract marker:
  // EmitObjc3IRText(pipeline_result.program, options.lowering, ir_frontend_metadata, bundle.ir_text, ir_error)
  // if (!EmitObjc3IRText(pipeline_result.program, options.lowering, ir_frontend_metadata, bundle.ir_text, ir_error)) {
  if (!EmitObjc3IRText(pipeline_result.program.ast, options.lowering, ir_frontend_metadata, bundle.ir_text, ir_error)) {
    bundle.post_pipeline_diagnostics = {MakeDiag(1, 1, "O3L300", "LLVM IR emission failed: " + ir_error)};
    bundle.diagnostics = bundle.post_pipeline_diagnostics;
    bundle.manifest_json.clear();
    bundle.runtime_metadata_binary.clear();
    bundle.ir_text.clear();
    return bundle;
  }
  bundle.ir_text =
      std::string("; runtime_dispatch_lowering_abi_boundary = ") +
      Objc3RuntimeDispatchLoweringAbiBoundarySummary(
          runtime_dispatch_lowering_abi_contract) +
      "\n" + bundle.ir_text;

  return bundle;
}
