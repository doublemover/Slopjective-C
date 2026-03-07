#include "pipeline/objc3_frontend_pipeline.h"

#include <algorithm>
#include <sstream>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "lex/objc3_lexer.h"
#include "parse/objc3_ast_builder_contract.h"
#include "parse/objc3_parse_support.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_edge_case_compatibility_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface.h"
#include "pipeline/objc3_lowering_runtime_stability_core_feature_implementation_surface.h"
#include "pipeline/objc3_lowering_runtime_stability_invariant_scaffold.h"
#include "pipeline/objc3_ir_emission_completeness_scaffold.h"
#include "pipeline/objc3_lowering_runtime_diagnostics_surfacing_scaffold.h"
#include "pipeline/objc3_lowering_pipeline_pass_graph_core_feature_surface.h"
#include "pipeline/objc3_lowering_pipeline_pass_graph_scaffold.h"
#include "pipeline/objc3_parse_lowering_readiness_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_scaffold.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_implementation_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_core_feature_expansion_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_compatibility_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_expansion_and_robustness_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_diagnostics_hardening_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_recovery_determinism_hardening_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_matrix_implementation_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_conformance_corpus_expansion_surface.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_performance_quality_guardrails_surface.h"
#include "pipeline/objc3_semantic_stability_core_feature_implementation_surface.h"
#include "pipeline/objc3_semantic_stability_spec_delta_closure_scaffold.h"
#include "pipeline/objc3_typed_sema_to_lowering_contract_surface.h"
#include "sema/objc3_sema_pass_manager.h"

using objc3c::parse::support::MakeDiag;

namespace {

template <typename T, typename = void>
struct HasProtocolsMember : std::false_type {};

template <typename T>
struct HasProtocolsMember<T, std::void_t<decltype(std::declval<const T &>().protocols)>> : std::true_type {};

template <typename T, typename = void>
struct HasCategoriesMember : std::false_type {};

template <typename T>
struct HasCategoriesMember<T, std::void_t<decltype(std::declval<const T &>().categories)>> : std::true_type {};

template <typename T, typename = void>
struct HasMethodsMember : std::false_type {};

template <typename T>
struct HasMethodsMember<T, std::void_t<decltype(std::declval<const T &>().methods)>> : std::true_type {};

template <typename T, typename = void>
struct HasMethodsLexicographicMember : std::false_type {};

template <typename T>
struct HasMethodsLexicographicMember<T, std::void_t<decltype(std::declval<const T &>().methods_lexicographic)>>
    : std::true_type {};

template <typename T, typename = void>
struct HasHasMatchingInterfaceMember : std::false_type {};

template <typename T>
struct HasHasMatchingInterfaceMember<T, std::void_t<decltype(std::declval<const T &>().has_matching_interface)>>
    : std::true_type {};

template <typename T, typename = void>
struct HasProtocolsLexicographicMember : std::false_type {};

template <typename T>
struct HasProtocolsLexicographicMember<T, std::void_t<decltype(std::declval<const T &>().protocols_lexicographic)>>
    : std::true_type {};

template <typename T, typename = void>
struct HasCategoriesLexicographicMember : std::false_type {};

template <typename T>
struct HasCategoriesLexicographicMember<T, std::void_t<decltype(std::declval<const T &>().categories_lexicographic)>>
    : std::true_type {};

template <typename T>
std::size_t CountProtocols(const T &value) {
  if constexpr (HasProtocolsMember<T>::value) {
    return value.protocols.size();
  }
  return 0;
}

template <typename T>
std::size_t CountCategories(const T &value) {
  if constexpr (HasCategoriesMember<T>::value) {
    return value.categories.size();
  }
  return 0;
}

template <typename Surface>
std::size_t CountProtocolMethodsFromSymbolTable(const Surface &surface) {
  if constexpr (HasProtocolsMember<Surface>::value) {
    std::size_t total = 0;
    for (const auto &entry : surface.protocols) {
      const auto &metadata = entry.second;
      if constexpr (HasMethodsMember<std::decay_t<decltype(metadata)>>::value) {
        total += metadata.methods.size();
      }
    }
    return total;
  }
  return 0;
}

template <typename Surface>
std::size_t CountCategoryMethodsFromSymbolTable(const Surface &surface) {
  if constexpr (HasCategoriesMember<Surface>::value) {
    std::size_t total = 0;
    for (const auto &entry : surface.categories) {
      const auto &metadata = entry.second;
      if constexpr (HasMethodsMember<std::decay_t<decltype(metadata)>>::value) {
        total += metadata.methods.size();
      }
    }
    return total;
  }
  return 0;
}

template <typename Surface>
std::size_t CountLinkedCategorySymbolsFromSymbolTable(const Surface &surface) {
  if constexpr (HasCategoriesMember<Surface>::value) {
    std::size_t total = 0;
    for (const auto &entry : surface.categories) {
      const auto &metadata = entry.second;
      if constexpr (HasHasMatchingInterfaceMember<std::decay_t<decltype(metadata)>>::value &&
                    HasMethodsMember<std::decay_t<decltype(metadata)>>::value) {
        if (metadata.has_matching_interface) {
          total += metadata.methods.size();
        }
      }
    }
    return total;
  }
  return 0;
}

template <typename Handoff>
std::size_t CountProtocolMethodsFromTypeMetadata(const Handoff &handoff) {
  if constexpr (HasProtocolsLexicographicMember<Handoff>::value) {
    std::size_t total = 0;
    for (const auto &metadata : handoff.protocols_lexicographic) {
      if constexpr (HasMethodsLexicographicMember<std::decay_t<decltype(metadata)>>::value) {
        total += metadata.methods_lexicographic.size();
      }
    }
    return total;
  }
  return 0;
}

template <typename Handoff>
std::size_t CountCategoryMethodsFromTypeMetadata(const Handoff &handoff) {
  if constexpr (HasCategoriesLexicographicMember<Handoff>::value) {
    std::size_t total = 0;
    for (const auto &metadata : handoff.categories_lexicographic) {
      if constexpr (HasMethodsLexicographicMember<std::decay_t<decltype(metadata)>>::value) {
        total += metadata.methods_lexicographic.size();
      }
    }
    return total;
  }
  return 0;
}

template <typename Handoff>
std::size_t CountLinkedCategorySymbolsFromTypeMetadata(const Handoff &handoff) {
  if constexpr (HasCategoriesLexicographicMember<Handoff>::value) {
    std::size_t total = 0;
    for (const auto &metadata : handoff.categories_lexicographic) {
      if constexpr (HasHasMatchingInterfaceMember<std::decay_t<decltype(metadata)>>::value &&
                    HasMethodsLexicographicMember<std::decay_t<decltype(metadata)>>::value) {
        if (metadata.has_matching_interface) {
          total += metadata.methods_lexicographic.size();
        }
      }
    }
    return total;
  }
  return 0;
}

const char *RuntimeMetadataTypeName(ValueType type) {
  switch (type) {
    case ValueType::I32:
      return "i32";
    case ValueType::Bool:
      return "bool";
    case ValueType::Void:
      return "void";
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

std::string BuildCategoryOwnerName(const std::string &class_name,
                                   const std::string &category_name) {
  return class_name + "(" + category_name + ")";
}

bool IsClassSourceRecordLess(const Objc3RuntimeMetadataClassSourceRecord &lhs,
                             const Objc3RuntimeMetadataClassSourceRecord &rhs) {
  return std::tie(lhs.name, lhs.record_kind, lhs.line, lhs.column) <
         std::tie(rhs.name, rhs.record_kind, rhs.line, rhs.column);
}

bool IsProtocolSourceRecordLess(const Objc3RuntimeMetadataProtocolSourceRecord &lhs,
                                const Objc3RuntimeMetadataProtocolSourceRecord &rhs) {
  return std::tie(lhs.name, lhs.line, lhs.column) <
         std::tie(rhs.name, rhs.line, rhs.column);
}

bool IsCategorySourceRecordLess(const Objc3RuntimeMetadataCategorySourceRecord &lhs,
                                const Objc3RuntimeMetadataCategorySourceRecord &rhs) {
  return std::tie(lhs.class_name, lhs.category_name, lhs.record_kind, lhs.line, lhs.column) <
         std::tie(rhs.class_name, rhs.category_name, rhs.record_kind, rhs.line, rhs.column);
}

bool IsPropertySourceRecordLess(const Objc3RuntimeMetadataPropertySourceRecord &lhs,
                                const Objc3RuntimeMetadataPropertySourceRecord &rhs) {
  return std::tie(lhs.owner_kind, lhs.owner_name, lhs.property_name, lhs.line, lhs.column) <
         std::tie(rhs.owner_kind, rhs.owner_name, rhs.property_name, rhs.line, rhs.column);
}

bool IsMethodSourceRecordLess(const Objc3RuntimeMetadataMethodSourceRecord &lhs,
                              const Objc3RuntimeMetadataMethodSourceRecord &rhs) {
  return std::tie(lhs.owner_kind, lhs.owner_name, lhs.selector, lhs.line, lhs.column) <
         std::tie(rhs.owner_kind, rhs.owner_name, rhs.selector, rhs.line, rhs.column);
}

bool IsIvarSourceRecordLess(const Objc3RuntimeMetadataIvarSourceRecord &lhs,
                            const Objc3RuntimeMetadataIvarSourceRecord &rhs) {
  return std::tie(lhs.owner_kind, lhs.owner_name, lhs.property_name, lhs.ivar_binding_symbol, lhs.line, lhs.column) <
         std::tie(rhs.owner_kind, rhs.owner_name, rhs.property_name, rhs.ivar_binding_symbol, rhs.line, rhs.column);
}

Objc3RuntimeMetadataSourceRecordSet BuildRuntimeMetadataSourceRecordSet(
    const Objc3Program &program) {
  Objc3RuntimeMetadataSourceRecordSet records;

  const auto append_property_records =
      [&records](const auto &properties, const std::string &owner_kind, const std::string &owner_name) {
        for (const auto &property : properties) {
          Objc3RuntimeMetadataPropertySourceRecord property_record;
          property_record.owner_kind = owner_kind;
          property_record.owner_name = owner_name;
          property_record.property_name = property.name;
          property_record.type_name = RuntimeMetadataTypeName(property.type);
          property_record.has_getter = property.has_getter;
          property_record.getter_selector = property.getter_selector;
          property_record.has_setter = property.has_setter;
          property_record.setter_selector = property.setter_selector;
          property_record.ivar_binding_symbol = property.ivar_binding_symbol;
          property_record.line = property.line;
          property_record.column = property.column;
          records.properties_lexicographic.push_back(std::move(property_record));

          if (!property.ivar_binding_symbol.empty()) {
            Objc3RuntimeMetadataIvarSourceRecord ivar_record;
            ivar_record.owner_kind = owner_kind;
            ivar_record.owner_name = owner_name;
            ivar_record.property_name = property.name;
            ivar_record.ivar_binding_symbol = property.ivar_binding_symbol;
            ivar_record.line = property.line;
            ivar_record.column = property.column;
            records.ivars_lexicographic.push_back(std::move(ivar_record));
          }
        }
      };

  const auto append_method_records =
      [&records](const auto &methods, const std::string &owner_kind, const std::string &owner_name) {
        for (const auto &method : methods) {
          Objc3RuntimeMetadataMethodSourceRecord method_record;
          method_record.owner_kind = owner_kind;
          method_record.owner_name = owner_name;
          method_record.selector = method.selector;
          method_record.is_class_method = method.is_class_method;
          method_record.has_body = method.has_body;
          method_record.parameter_count = method.params.size();
          method_record.return_type_name = RuntimeMetadataTypeName(method.return_type);
          method_record.line = method.line;
          method_record.column = method.column;
          records.methods_lexicographic.push_back(std::move(method_record));
        }
      };

  for (const auto &protocol : program.protocols) {
    Objc3RuntimeMetadataProtocolSourceRecord record;
    record.name = protocol.name;
    record.inherited_protocols_lexicographic = protocol.inherited_protocols_lexicographic;
    record.is_forward_declaration = protocol.is_forward_declaration;
    record.property_count = protocol.properties.size();
    record.method_count = protocol.methods.size();
    record.line = protocol.line;
    record.column = protocol.column;
    records.protocols_lexicographic.push_back(record);
    append_property_records(protocol.properties, "protocol", protocol.name);
    append_method_records(protocol.methods, "protocol", protocol.name);
  }

  for (const auto &interface_decl : program.interfaces) {
    if (interface_decl.has_category) {
      Objc3RuntimeMetadataCategorySourceRecord record;
      record.record_kind = "interface";
      record.class_name = interface_decl.name;
      record.category_name = interface_decl.category_name;
      record.adopted_protocols_lexicographic =
          interface_decl.adopted_protocols_lexicographic;
      record.property_count = interface_decl.properties.size();
      record.method_count = interface_decl.methods.size();
      record.line = interface_decl.line;
      record.column = interface_decl.column;
      records.categories_lexicographic.push_back(record);
      const std::string owner_name =
          BuildCategoryOwnerName(interface_decl.name, interface_decl.category_name);
      append_property_records(interface_decl.properties, "category-interface", owner_name);
      append_method_records(interface_decl.methods, "category-interface", owner_name);
      continue;
    }

    Objc3RuntimeMetadataClassSourceRecord record;
    record.record_kind = "interface";
    record.name = interface_decl.name;
    record.super_name = interface_decl.super_name;
    record.has_super = !interface_decl.super_name.empty();
    record.property_count = interface_decl.properties.size();
    record.method_count = interface_decl.methods.size();
    record.line = interface_decl.line;
    record.column = interface_decl.column;
    records.classes_lexicographic.push_back(record);
    append_property_records(interface_decl.properties, "class-interface", interface_decl.name);
    append_method_records(interface_decl.methods, "class-interface", interface_decl.name);
  }

  for (const auto &implementation : program.implementations) {
    if (implementation.has_category) {
      Objc3RuntimeMetadataCategorySourceRecord record;
      record.record_kind = "implementation";
      record.class_name = implementation.name;
      record.category_name = implementation.category_name;
      record.property_count = implementation.properties.size();
      record.method_count = implementation.methods.size();
      record.line = implementation.line;
      record.column = implementation.column;
      records.categories_lexicographic.push_back(record);
      const std::string owner_name =
          BuildCategoryOwnerName(implementation.name, implementation.category_name);
      append_property_records(implementation.properties, "category-implementation", owner_name);
      append_method_records(implementation.methods, "category-implementation", owner_name);
      continue;
    }

    Objc3RuntimeMetadataClassSourceRecord record;
    record.record_kind = "implementation";
    record.name = implementation.name;
    record.property_count = implementation.properties.size();
    record.method_count = implementation.methods.size();
    record.line = implementation.line;
    record.column = implementation.column;
    records.classes_lexicographic.push_back(record);
    append_property_records(implementation.properties, "class-implementation", implementation.name);
    append_method_records(implementation.methods, "class-implementation", implementation.name);
  }

  std::sort(records.classes_lexicographic.begin(),
            records.classes_lexicographic.end(),
            IsClassSourceRecordLess);
  std::sort(records.protocols_lexicographic.begin(),
            records.protocols_lexicographic.end(),
            IsProtocolSourceRecordLess);
  std::sort(records.categories_lexicographic.begin(),
            records.categories_lexicographic.end(),
            IsCategorySourceRecordLess);
  std::sort(records.properties_lexicographic.begin(),
            records.properties_lexicographic.end(),
            IsPropertySourceRecordLess);
  std::sort(records.methods_lexicographic.begin(),
            records.methods_lexicographic.end(),
            IsMethodSourceRecordLess);
  std::sort(records.ivars_lexicographic.begin(),
            records.ivars_lexicographic.end(),
            IsIvarSourceRecordLess);

  records.deterministic = std::is_sorted(records.classes_lexicographic.begin(),
                                         records.classes_lexicographic.end(),
                                         IsClassSourceRecordLess) &&
                          std::is_sorted(records.protocols_lexicographic.begin(),
                                         records.protocols_lexicographic.end(),
                                         IsProtocolSourceRecordLess) &&
                          std::is_sorted(records.categories_lexicographic.begin(),
                                         records.categories_lexicographic.end(),
                                         IsCategorySourceRecordLess) &&
                          std::is_sorted(records.properties_lexicographic.begin(),
                                         records.properties_lexicographic.end(),
                                         IsPropertySourceRecordLess) &&
                          std::is_sorted(records.methods_lexicographic.begin(),
                                         records.methods_lexicographic.end(),
                                         IsMethodSourceRecordLess) &&
                          std::is_sorted(records.ivars_lexicographic.begin(),
                                         records.ivars_lexicographic.end(),
                                         IsIvarSourceRecordLess);
  return records;
}

Objc3RuntimeMetadataSourceOwnershipBoundary BuildRuntimeMetadataSourceOwnershipBoundary(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff) {
  Objc3RuntimeMetadataSourceOwnershipBoundary boundary;
  const std::size_t sema_interface_implementation_record_count =
      type_metadata_handoff.interfaces_lexicographic.size() +
      type_metadata_handoff.implementations_lexicographic.size();
  const bool sema_interface_implementation_record_count_present =
      sema_interface_implementation_record_count > 0u ||
      type_metadata_handoff.interface_implementation_summary.declared_interfaces > 0u ||
      type_metadata_handoff.interface_implementation_summary.declared_implementations > 0u;

  boundary.frontend_owns_runtime_metadata_source_records = true;
  boundary.runtime_metadata_source_records_ready_for_lowering = false;
  boundary.native_runtime_library_present = false;
  boundary.runtime_shim_test_only = true;
  boundary.class_record_count = records.classes_lexicographic.size();
  boundary.protocol_record_count = records.protocols_lexicographic.size();
  boundary.category_interface_record_count =
      static_cast<std::size_t>(std::count_if(records.categories_lexicographic.begin(),
                                             records.categories_lexicographic.end(),
                                             [](const Objc3RuntimeMetadataCategorySourceRecord &record) {
                                               return record.record_kind == "interface";
                                             }));
  boundary.category_implementation_record_count =
      static_cast<std::size_t>(std::count_if(records.categories_lexicographic.begin(),
                                             records.categories_lexicographic.end(),
                                             [](const Objc3RuntimeMetadataCategorySourceRecord &record) {
                                               return record.record_kind == "implementation";
                                             }));
  boundary.property_record_count = records.properties_lexicographic.size();
  boundary.method_record_count = records.methods_lexicographic.size();
  boundary.ivar_record_count = records.ivars_lexicographic.size();

  // The current semantic type-metadata handoff normalizes class interfaces,
  // class implementations, category interfaces, and category implementations
  // into a single interface/implementation declaration surface. The runtime
  // metadata source ownership boundary therefore has to validate that combined
  // declaration count against the combined class/category source-record
  // surface until a later milestone splits class/category sema metadata lanes.
  const std::size_t source_interface_implementation_record_count =
      boundary.class_record_count + boundary.category_record_count();
  const bool class_alignment_consistent =
      !sema_interface_implementation_record_count_present ||
      sema_interface_implementation_record_count ==
          source_interface_implementation_record_count;
  boundary.deterministic_source_schema =
      IsReadyObjc3RuntimeMetadataSourceRecordSet(records) &&
      class_alignment_consistent &&
      boundary.ivar_record_count <= boundary.property_record_count &&
      !boundary.contract_id.empty() &&
      !boundary.canonical_source_schema.empty() &&
      !boundary.class_record_ast_anchor.empty() &&
      !boundary.protocol_record_ast_anchor.empty() &&
      !boundary.category_record_ast_anchor.empty() &&
      !boundary.property_record_ast_anchor.empty() &&
      !boundary.method_record_ast_anchor.empty() &&
      !boundary.ivar_record_ast_anchor.empty() &&
      !boundary.ivar_record_source_model.empty();
  boundary.fail_closed =
      boundary.frontend_owns_runtime_metadata_source_records &&
      !boundary.runtime_metadata_source_records_ready_for_lowering &&
      !boundary.native_runtime_library_present &&
      boundary.runtime_shim_test_only;

  if (!class_alignment_consistent) {
    boundary.failure_reason = "AST/sema class metadata source counts diverged";
  } else if (boundary.ivar_record_count > boundary.property_record_count) {
    boundary.failure_reason = "ivar source records exceed property source records";
  } else if (!boundary.deterministic_source_schema) {
    boundary.failure_reason = "runtime metadata source schema anchors are incomplete";
  } else if (!boundary.fail_closed) {
    boundary.failure_reason = "runtime metadata source ownership boundary is not fail-closed";
  }

  return boundary;
}

Objc3RuntimeExportLegalityBoundary BuildRuntimeExportLegalityBoundary(
    const Objc3RuntimeMetadataSourceOwnershipBoundary &runtime_metadata_source_ownership,
    const Objc3TypedSemaToLoweringContractSurface &typed_surface,
    const Objc3SemanticIntegrationSurface &integration_surface,
    const Objc3FrontendProtocolCategorySummary &protocol_category_summary,
    const Objc3FrontendClassProtocolCategoryLinkingSummary &class_protocol_category_linking_summary,
    const Objc3FrontendSelectorNormalizationSummary &selector_normalization_summary,
    const Objc3FrontendPropertyAttributeSummary &property_attribute_summary,
    const Objc3FrontendObjectPointerNullabilityGenericsSummary &object_pointer_summary,
    const Objc3FrontendSymbolGraphScopeResolutionSummary &symbol_graph_scope_resolution_summary,
    const Objc3SemaParityContractSurface &sema_parity_surface) {
  Objc3RuntimeExportLegalityBoundary boundary;
  boundary.semantic_integration_surface_built = integration_surface.built;
  boundary.sema_type_metadata_handoff_deterministic =
      typed_surface.semantic_type_metadata_handoff_deterministic &&
      sema_parity_surface.deterministic_type_metadata_handoff;
  boundary.typed_sema_surface_ready =
      typed_surface.semantic_integration_surface_built;
  boundary.typed_sema_surface_deterministic =
      typed_surface.semantic_type_metadata_handoff_deterministic &&
      typed_surface.protocol_category_handoff_deterministic &&
      typed_surface.class_protocol_category_linking_handoff_deterministic &&
      typed_surface.selector_normalization_handoff_deterministic &&
      typed_surface.property_attribute_handoff_deterministic &&
      typed_surface.object_pointer_type_handoff_deterministic &&
      typed_surface.symbol_graph_handoff_deterministic &&
      typed_surface.scope_resolution_handoff_deterministic;
  boundary.runtime_metadata_source_boundary_ready =
      IsReadyObjc3RuntimeMetadataSourceOwnershipBoundary(
          runtime_metadata_source_ownership);
  boundary.protocol_category_deterministic =
      protocol_category_summary.deterministic_protocol_category_handoff;
  boundary.class_protocol_category_linking_deterministic =
      class_protocol_category_linking_summary
          .deterministic_class_protocol_category_linking_handoff;
  boundary.selector_normalization_deterministic =
      selector_normalization_summary.deterministic_selector_normalization_handoff;
  boundary.property_attribute_deterministic =
      property_attribute_summary.deterministic_property_attribute_handoff;
  boundary.object_pointer_surface_deterministic =
      object_pointer_summary
          .deterministic_object_pointer_nullability_generics_handoff;
  boundary.symbol_graph_scope_resolution_deterministic =
      symbol_graph_scope_resolution_summary.deterministic_symbol_graph_handoff &&
      symbol_graph_scope_resolution_summary.deterministic_scope_resolution_handoff;
  boundary.property_synthesis_ivar_binding_deterministic =
      sema_parity_surface.property_synthesis_ivar_binding_summary.deterministic &&
      sema_parity_surface.deterministic_property_synthesis_ivar_binding_handoff;

  boundary.class_record_count = runtime_metadata_source_ownership.class_record_count;
  boundary.protocol_record_count =
      runtime_metadata_source_ownership.protocol_record_count;
  boundary.category_record_count =
      runtime_metadata_source_ownership.category_record_count();
  boundary.property_record_count =
      runtime_metadata_source_ownership.property_record_count;
  boundary.method_record_count = runtime_metadata_source_ownership.method_record_count;
  boundary.ivar_record_count = runtime_metadata_source_ownership.ivar_record_count;
  boundary.invalid_protocol_composition_sites =
      class_protocol_category_linking_summary.invalid_protocol_composition_sites;
  boundary.property_attribute_invalid_entries =
      sema_parity_surface.property_attribute_invalid_attribute_entries_total;
  boundary.property_attribute_contract_violations =
      sema_parity_surface.property_attribute_contract_violations_total;
  boundary.invalid_type_annotation_sites =
      sema_parity_surface.type_annotation_invalid_generic_suffix_sites_total +
      sema_parity_surface.type_annotation_invalid_pointer_declarator_sites_total +
      sema_parity_surface.type_annotation_invalid_nullability_suffix_sites_total +
      sema_parity_surface.type_annotation_invalid_ownership_qualifier_sites_total;
  boundary.property_ivar_binding_missing =
      sema_parity_surface.property_synthesis_ivar_binding_summary
          .ivar_binding_missing;
  boundary.property_ivar_binding_conflicts =
      sema_parity_surface.property_synthesis_ivar_binding_summary
          .ivar_binding_conflicts;
  boundary.implementation_resolution_misses =
      symbol_graph_scope_resolution_summary.implementation_interface_resolution_misses;
  boundary.method_resolution_misses =
      symbol_graph_scope_resolution_summary.method_resolution_misses;

  if (boundary.contract_id.empty()) {
    boundary.failure_reason = "runtime export legality contract id is empty";
  } else if (!boundary.sema_type_metadata_handoff_deterministic) {
    boundary.failure_reason =
        "semantic type-metadata handoff is not deterministic";
  } else if (!boundary.typed_sema_surface_ready) {
    boundary.failure_reason =
        "typed sema runtime-export handoff is not ready";
  } else if (!boundary.typed_sema_surface_deterministic) {
    boundary.failure_reason =
        "typed sema runtime-export handoff is not deterministic";
  } else if (!boundary.runtime_metadata_source_boundary_ready) {
    boundary.failure_reason =
        "runtime metadata source ownership boundary is not ready";
  } else if (!boundary.protocol_category_deterministic) {
    boundary.failure_reason =
        "protocol/category semantic handoff is not deterministic";
  } else if (!boundary.class_protocol_category_linking_deterministic) {
    boundary.failure_reason =
        "class/protocol/category linking handoff is not deterministic";
  } else if (!boundary.selector_normalization_deterministic) {
    boundary.failure_reason = "selector normalization handoff is not deterministic";
  } else if (!boundary.property_attribute_deterministic) {
    boundary.failure_reason = "property attribute handoff is not deterministic";
  } else if (!boundary.object_pointer_surface_deterministic) {
    boundary.failure_reason =
        "object-pointer/nullability/generics handoff is not deterministic";
  } else if (!boundary.symbol_graph_scope_resolution_deterministic) {
    boundary.failure_reason =
        "symbol-graph/scope-resolution handoff is not deterministic";
  } else if (!boundary.property_synthesis_ivar_binding_deterministic) {
    boundary.failure_reason =
        "property synthesis/ivar binding handoff is not deterministic";
  } else if (boundary.invalid_protocol_composition_sites >
             boundary.protocol_record_count + boundary.category_record_count) {
    boundary.failure_reason =
        "invalid protocol composition sites exceed export-bearing records";
  } else if (boundary.ivar_record_count > boundary.property_record_count) {
    boundary.failure_reason =
        "ivar export records exceed property export records";
  }

  boundary.semantic_boundary_frozen = boundary.failure_reason.empty();
  boundary.metadata_export_enforcement_ready = false;
  boundary.fail_closed =
      boundary.semantic_boundary_frozen &&
      !boundary.metadata_export_enforcement_ready &&
      boundary.duplicate_runtime_identity_enforcement_pending &&
      boundary.incomplete_declaration_export_blocking_pending &&
      boundary.illegal_redeclaration_mix_export_blocking_pending;
  if (boundary.failure_reason.empty() && !boundary.fail_closed) {
    boundary.failure_reason =
        "runtime export legality freeze is not fail-closed";
  }
  return boundary;
}

struct Objc3RuntimeExportViolationAccumulator {
  std::size_t count = 0;
  unsigned first_line = 1;
  unsigned first_column = 1;
  bool has_location = false;

  void Add(std::size_t increment, unsigned line, unsigned column) {
    if (increment == 0) {
      return;
    }
    if (!has_location) {
      first_line = line;
      first_column = column;
      has_location = true;
    }
    count += increment;
  }

  void Merge(const Objc3RuntimeExportViolationAccumulator &other) {
    if (other.count == 0) {
      return;
    }
    Add(other.count, other.first_line, other.first_column);
  }
};

template <typename Record, typename KeyBuilder>
Objc3RuntimeExportViolationAccumulator CountDuplicateRuntimeExportIdentitySites(
    const std::vector<Record> &records,
    KeyBuilder build_key) {
  Objc3RuntimeExportViolationAccumulator violations;
  std::unordered_map<std::string, std::size_t> seen;
  seen.reserve(records.size());
  for (const auto &record : records) {
    std::size_t &count = seen[build_key(record)];
    if (count > 0u) {
      violations.Add(1u, record.line, record.column);
    }
    ++count;
  }
  return violations;
}

struct Objc3RuntimeExportPairPresence {
  std::size_t interface_records = 0;
  std::size_t implementation_records = 0;
  unsigned line = 1;
  unsigned column = 1;
  bool has_location = false;
};

bool IsInterfaceRuntimePropertyOwnerKind(const std::string &owner_kind) {
  return owner_kind == "class-interface" || owner_kind == "category-interface";
}

bool IsImplementationRuntimePropertyOwnerKind(const std::string &owner_kind) {
  return owner_kind == "class-implementation" || owner_kind == "category-implementation";
}

bool IsInterfaceRuntimeMethodOwnerKind(const std::string &owner_kind) {
  return owner_kind == "class-interface" || owner_kind == "category-interface";
}

bool IsImplementationRuntimeMethodOwnerKind(const std::string &owner_kind) {
  return owner_kind == "class-implementation" || owner_kind == "category-implementation";
}

bool AreCompatibleRuntimePropertyRedeclarations(
    const Objc3RuntimeMetadataPropertySourceRecord &interface_record,
    const Objc3RuntimeMetadataPropertySourceRecord &implementation_record) {
  return interface_record.type_name == implementation_record.type_name &&
         interface_record.has_getter == implementation_record.has_getter &&
         interface_record.getter_selector == implementation_record.getter_selector &&
         interface_record.has_setter == implementation_record.has_setter &&
         interface_record.setter_selector == implementation_record.setter_selector;
}

bool AreCompatibleRuntimeMethodRedeclarations(
    const Objc3RuntimeMetadataMethodSourceRecord &interface_record,
    const Objc3RuntimeMetadataMethodSourceRecord &implementation_record) {
  return interface_record.is_class_method == implementation_record.is_class_method &&
         interface_record.selector == implementation_record.selector &&
         interface_record.parameter_count == implementation_record.parameter_count &&
         interface_record.return_type_name == implementation_record.return_type_name &&
         !interface_record.has_body && implementation_record.has_body;
}

bool HasRuntimeMetadataSourceRecords(const Objc3RuntimeMetadataSourceRecordSet &records) {
  return !records.classes_lexicographic.empty() ||
         !records.protocols_lexicographic.empty() ||
         !records.categories_lexicographic.empty() ||
         !records.properties_lexicographic.empty() ||
         !records.methods_lexicographic.empty() ||
         !records.ivars_lexicographic.empty();
}

Objc3RuntimeExportEnforcementSummary BuildRuntimeExportEnforcementSummary(
    const Objc3RuntimeMetadataSourceRecordSet &records,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality) {
  Objc3RuntimeExportEnforcementSummary summary;
  summary.metadata_completeness_enforced = true;
  summary.duplicate_runtime_identity_suppression_enforced = true;
  summary.illegal_redeclaration_mix_blocking_enforced = true;
  summary.metadata_shape_drift_blocking_enforced = true;
  summary.fail_closed = true;

  Objc3RuntimeExportViolationAccumulator duplicate_violations;
  duplicate_violations.Merge(CountDuplicateRuntimeExportIdentitySites(
      records.classes_lexicographic,
      [](const Objc3RuntimeMetadataClassSourceRecord &record) {
        return record.record_kind + "\n" + record.name;
      }));
  duplicate_violations.Merge(CountDuplicateRuntimeExportIdentitySites(
      records.categories_lexicographic,
      [](const Objc3RuntimeMetadataCategorySourceRecord &record) {
        return record.record_kind + "\n" + record.class_name + "\n" +
               record.category_name;
      }));
  duplicate_violations.Merge(CountDuplicateRuntimeExportIdentitySites(
      records.properties_lexicographic,
      [](const Objc3RuntimeMetadataPropertySourceRecord &record) {
        return record.owner_kind + "\n" + record.owner_name + "\n" +
               record.property_name;
      }));
  duplicate_violations.Merge(CountDuplicateRuntimeExportIdentitySites(
      records.methods_lexicographic,
      [](const Objc3RuntimeMetadataMethodSourceRecord &record) {
        return record.owner_kind + "\n" + record.owner_name + "\n" +
               (record.is_class_method ? "+" : "-") + "\n" + record.selector;
      }));
  duplicate_violations.Merge(CountDuplicateRuntimeExportIdentitySites(
      records.ivars_lexicographic,
      [](const Objc3RuntimeMetadataIvarSourceRecord &record) {
        return record.owner_kind + "\n" + record.owner_name + "\n" +
               record.ivar_binding_symbol;
      }));
  {
    std::unordered_map<std::string, std::size_t> seen_protocols;
    seen_protocols.reserve(records.protocols_lexicographic.size());
    for (const auto &record : records.protocols_lexicographic) {
      if (record.is_forward_declaration) {
        continue;
      }
      std::size_t &count = seen_protocols[record.name];
      if (count > 0u) {
        duplicate_violations.Add(1u, record.line, record.column);
      }
      ++count;
    }
  }
  summary.duplicate_runtime_identity_sites = duplicate_violations.count;

  Objc3RuntimeExportViolationAccumulator incomplete_violations;
  // Forward protocol declarations are dependency hints for later complete
  // protocol records or composition spelling; they are not themselves
  // exportable runtime metadata units and must not block the runnable path.
  {
    std::unordered_map<std::string, Objc3RuntimeExportPairPresence> class_presence;
    class_presence.reserve(records.classes_lexicographic.size());
    for (const auto &record : records.classes_lexicographic) {
      Objc3RuntimeExportPairPresence &presence = class_presence[record.name];
      if (!presence.has_location) {
        presence.line = record.line;
        presence.column = record.column;
        presence.has_location = true;
      }
      if (record.record_kind == "interface") {
        ++presence.interface_records;
      } else if (record.record_kind == "implementation") {
        ++presence.implementation_records;
      }
    }
    for (const auto &entry : class_presence) {
      const Objc3RuntimeExportPairPresence &presence = entry.second;
      if (presence.interface_records == 0u || presence.implementation_records == 0u) {
        incomplete_violations.Add(1u, presence.line, presence.column);
      }
    }
  }
  {
    std::unordered_map<std::string, Objc3RuntimeExportPairPresence> category_presence;
    category_presence.reserve(records.categories_lexicographic.size());
    for (const auto &record : records.categories_lexicographic) {
      const std::string key = record.class_name + "\n" + record.category_name;
      Objc3RuntimeExportPairPresence &presence = category_presence[key];
      if (!presence.has_location) {
        presence.line = record.line;
        presence.column = record.column;
        presence.has_location = true;
      }
      if (record.record_kind == "interface") {
        ++presence.interface_records;
      } else if (record.record_kind == "implementation") {
        ++presence.implementation_records;
      }
    }
    for (const auto &entry : category_presence) {
      const Objc3RuntimeExportPairPresence &presence = entry.second;
      if (presence.interface_records == 0u || presence.implementation_records == 0u) {
        incomplete_violations.Add(1u, presence.line, presence.column);
      }
    }
  }
  incomplete_violations.count +=
      runtime_export_legality.implementation_resolution_misses +
      runtime_export_legality.method_resolution_misses +
      runtime_export_legality.property_ivar_binding_missing;
  summary.incomplete_declaration_sites = incomplete_violations.count;

  Objc3RuntimeExportViolationAccumulator illegal_redeclaration_violations;
  {
    struct RuntimePropertyRedeclarationPair {
      const Objc3RuntimeMetadataPropertySourceRecord *interface_record = nullptr;
      const Objc3RuntimeMetadataPropertySourceRecord *implementation_record =
          nullptr;
    };
    std::unordered_map<std::string, RuntimePropertyRedeclarationPair>
        property_pairs;
    property_pairs.reserve(records.properties_lexicographic.size());
    for (const auto &record : records.properties_lexicographic) {
      if (!IsInterfaceRuntimePropertyOwnerKind(record.owner_kind) &&
          !IsImplementationRuntimePropertyOwnerKind(record.owner_kind)) {
        continue;
      }
      const std::string key = record.owner_name + "\n" + record.property_name;
      RuntimePropertyRedeclarationPair &pair = property_pairs[key];
      if (IsInterfaceRuntimePropertyOwnerKind(record.owner_kind)) {
        pair.interface_record = &record;
      } else if (IsImplementationRuntimePropertyOwnerKind(record.owner_kind)) {
        pair.implementation_record = &record;
      }
    }
    for (const auto &entry : property_pairs) {
      const RuntimePropertyRedeclarationPair &pair = entry.second;
      if (pair.interface_record == nullptr || pair.implementation_record == nullptr) {
        continue;
      }
      if (!AreCompatibleRuntimePropertyRedeclarations(*pair.interface_record,
                                                      *pair.implementation_record)) {
        illegal_redeclaration_violations.Add(
            1u, pair.implementation_record->line,
            pair.implementation_record->column);
      }
    }
  }
  {
    struct RuntimeMethodRedeclarationPair {
      const Objc3RuntimeMetadataMethodSourceRecord *interface_record = nullptr;
      const Objc3RuntimeMetadataMethodSourceRecord *implementation_record =
          nullptr;
    };
    std::unordered_map<std::string, RuntimeMethodRedeclarationPair> method_pairs;
    method_pairs.reserve(records.methods_lexicographic.size());
    for (const auto &record : records.methods_lexicographic) {
      if (!IsInterfaceRuntimeMethodOwnerKind(record.owner_kind) &&
          !IsImplementationRuntimeMethodOwnerKind(record.owner_kind)) {
        continue;
      }
      const std::string key = record.owner_name + "\n" +
                              (record.is_class_method ? "+" : "-") + "\n" +
                              record.selector;
      RuntimeMethodRedeclarationPair &pair = method_pairs[key];
      if (IsInterfaceRuntimeMethodOwnerKind(record.owner_kind)) {
        pair.interface_record = &record;
      } else if (IsImplementationRuntimeMethodOwnerKind(record.owner_kind)) {
        pair.implementation_record = &record;
      }
    }
    for (const auto &entry : method_pairs) {
      const RuntimeMethodRedeclarationPair &pair = entry.second;
      if (pair.interface_record == nullptr || pair.implementation_record == nullptr) {
        continue;
      }
      if (!AreCompatibleRuntimeMethodRedeclarations(*pair.interface_record,
                                                    *pair.implementation_record)) {
        illegal_redeclaration_violations.Add(
            1u, pair.implementation_record->line,
            pair.implementation_record->column);
      }
    }
  }
  summary.illegal_redeclaration_mix_sites =
      illegal_redeclaration_violations.count +
      runtime_export_legality.invalid_protocol_composition_sites +
      runtime_export_legality.property_attribute_invalid_entries +
      runtime_export_legality.property_attribute_contract_violations +
      runtime_export_legality.invalid_type_annotation_sites +
      runtime_export_legality.property_ivar_binding_conflicts;

  summary.metadata_shape_drift_sites =
      (runtime_export_legality.semantic_integration_surface_built ? 0u : 1u) +
      (runtime_export_legality.sema_type_metadata_handoff_deterministic ? 0u : 1u) +
      (runtime_export_legality.typed_sema_surface_ready ? 0u : 1u) +
      (runtime_export_legality.typed_sema_surface_deterministic ? 0u : 1u) +
      (runtime_export_legality.runtime_metadata_source_boundary_ready ? 0u : 1u) +
      (runtime_export_legality.protocol_category_deterministic ? 0u : 1u) +
      (runtime_export_legality.class_protocol_category_linking_deterministic ? 0u : 1u) +
      (runtime_export_legality.selector_normalization_deterministic ? 0u : 1u) +
      (runtime_export_legality.property_attribute_deterministic ? 0u : 1u) +
      (runtime_export_legality.object_pointer_surface_deterministic ? 0u : 1u) +
      (runtime_export_legality.symbol_graph_scope_resolution_deterministic ? 0u : 1u) +
      (runtime_export_legality.property_synthesis_ivar_binding_deterministic ? 0u : 1u) +
      (runtime_export_legality.invalid_protocol_composition_sites <=
               runtime_export_legality.protocol_record_count +
                   runtime_export_legality.category_record_count
           ? 0u
           : 1u) +
      (runtime_export_legality.ivar_record_count <=
               runtime_export_legality.property_record_count
           ? 0u
           : 1u);

  summary.ready_for_runtime_export =
      summary.duplicate_runtime_identity_sites == 0u &&
      summary.incomplete_declaration_sites == 0u &&
      summary.illegal_redeclaration_mix_sites == 0u &&
      summary.metadata_shape_drift_sites == 0u;

  if (duplicate_violations.has_location) {
    summary.first_failure_line = duplicate_violations.first_line;
    summary.first_failure_column = duplicate_violations.first_column;
  } else if (incomplete_violations.has_location) {
    summary.first_failure_line = incomplete_violations.first_line;
    summary.first_failure_column = incomplete_violations.first_column;
  } else if (illegal_redeclaration_violations.has_location) {
    summary.first_failure_line = illegal_redeclaration_violations.first_line;
    summary.first_failure_column = illegal_redeclaration_violations.first_column;
  }

  if (summary.duplicate_runtime_identity_sites > 0u) {
    summary.failure_reason =
        "duplicate runtime metadata identities are not exportable";
  } else if (summary.incomplete_declaration_sites > 0u) {
    summary.failure_reason =
        "incomplete runtime metadata declarations are not exportable";
  } else if (summary.illegal_redeclaration_mix_sites > 0u) {
    summary.failure_reason =
        "illegal runtime metadata redeclaration mixes are not exportable";
  } else if (summary.metadata_shape_drift_sites > 0u) {
    summary.failure_reason =
        "runtime metadata export shape drift detected before lowering";
  }

  return summary;
}

Objc3FrontendProtocolCategorySummary BuildProtocolCategorySummary(
    const Objc3Program &program,
    const Objc3SemanticIntegrationSurface &integration_surface,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff) {
  Objc3FrontendProtocolCategorySummary summary;
  summary.declared_protocols = CountProtocols(program);
  summary.declared_categories = CountCategories(program);
  summary.resolved_protocol_symbols = CountProtocols(integration_surface);
  summary.resolved_category_symbols = CountCategories(integration_surface);
  summary.protocol_method_symbols = CountProtocolMethodsFromSymbolTable(integration_surface);
  summary.category_method_symbols = CountCategoryMethodsFromSymbolTable(integration_surface);
  summary.linked_category_symbols = CountLinkedCategorySymbolsFromSymbolTable(integration_surface);

  if (summary.protocol_method_symbols == 0) {
    summary.protocol_method_symbols = CountProtocolMethodsFromTypeMetadata(type_metadata_handoff);
  }
  if (summary.category_method_symbols == 0) {
    summary.category_method_symbols = CountCategoryMethodsFromTypeMetadata(type_metadata_handoff);
  }
  if (summary.linked_category_symbols == 0) {
    summary.linked_category_symbols = CountLinkedCategorySymbolsFromTypeMetadata(type_metadata_handoff);
  }

  summary.deterministic_protocol_category_handoff =
      summary.linked_category_symbols <= summary.category_method_symbols &&
      summary.resolved_protocol_symbols <= summary.declared_protocols &&
      summary.resolved_category_symbols <= summary.declared_categories;
  return summary;
}

Objc3FrontendClassProtocolCategoryLinkingSummary BuildClassProtocolCategoryLinkingSummary(
    const Objc3InterfaceImplementationSummary &interface_implementation_summary,
    const Objc3FrontendProtocolCategorySummary &protocol_category_summary,
    const Objc3SemanticIntegrationSurface &integration_surface,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff) {
  Objc3FrontendClassProtocolCategoryLinkingSummary summary;
  summary.declared_class_interfaces = interface_implementation_summary.declared_interfaces;
  summary.declared_class_implementations = interface_implementation_summary.declared_implementations;
  summary.resolved_class_interfaces = interface_implementation_summary.resolved_interfaces;
  summary.resolved_class_implementations = interface_implementation_summary.resolved_implementations;
  summary.linked_class_method_symbols = interface_implementation_summary.linked_implementation_symbols;
  summary.linked_category_method_symbols = protocol_category_summary.linked_category_symbols;

  const Objc3ProtocolCategoryCompositionSummary &integration_composition_summary =
      integration_surface.protocol_category_composition_summary;
  const Objc3ProtocolCategoryCompositionSummary &type_metadata_composition_summary =
      type_metadata_handoff.protocol_category_composition_summary;
  const auto select_composition_value = [&](std::size_t integration_value, std::size_t type_metadata_value) {
    if (integration_surface.built) {
      return integration_value;
    }
    return type_metadata_value;
  };

  summary.protocol_composition_sites =
      select_composition_value(integration_composition_summary.protocol_composition_sites,
                               type_metadata_composition_summary.protocol_composition_sites);
  summary.protocol_composition_symbols =
      select_composition_value(integration_composition_summary.protocol_composition_symbols,
                               type_metadata_composition_summary.protocol_composition_symbols);
  summary.category_composition_sites =
      select_composition_value(integration_composition_summary.category_composition_sites,
                               type_metadata_composition_summary.category_composition_sites);
  summary.category_composition_symbols =
      select_composition_value(integration_composition_summary.category_composition_symbols,
                               type_metadata_composition_summary.category_composition_symbols);
  summary.invalid_protocol_composition_sites =
      select_composition_value(integration_composition_summary.invalid_protocol_composition_sites,
                               type_metadata_composition_summary.invalid_protocol_composition_sites);

  const bool composition_fields_match =
      integration_composition_summary.protocol_composition_sites ==
          type_metadata_composition_summary.protocol_composition_sites &&
      integration_composition_summary.protocol_composition_symbols ==
          type_metadata_composition_summary.protocol_composition_symbols &&
      integration_composition_summary.category_composition_sites ==
          type_metadata_composition_summary.category_composition_sites &&
      integration_composition_summary.category_composition_symbols ==
          type_metadata_composition_summary.category_composition_symbols &&
      integration_composition_summary.invalid_protocol_composition_sites ==
          type_metadata_composition_summary.invalid_protocol_composition_sites;

  summary.deterministic_class_protocol_category_linking_handoff =
      interface_implementation_summary.deterministic &&
      protocol_category_summary.deterministic_protocol_category_handoff &&
      integration_composition_summary.deterministic &&
      type_metadata_composition_summary.deterministic &&
      composition_fields_match &&
      summary.resolved_class_interfaces <= summary.declared_class_interfaces &&
      summary.resolved_class_implementations <= summary.declared_class_implementations &&
      summary.linked_class_method_symbols <= interface_implementation_summary.interface_method_symbols &&
      summary.linked_class_method_symbols <= interface_implementation_summary.implementation_method_symbols &&
      summary.linked_category_method_symbols <= protocol_category_summary.category_method_symbols &&
      summary.category_composition_sites <= summary.protocol_composition_sites &&
      summary.category_composition_symbols <= summary.protocol_composition_symbols &&
      summary.invalid_protocol_composition_sites <=
          summary.protocol_composition_sites + summary.category_composition_sites;
  return summary;
}

template <typename Container>
void AccumulateSelectorNormalizationSummary(const Container &declarations,
                                           Objc3FrontendSelectorNormalizationSummary &summary) {
  for (const auto &declaration : declarations) {
    for (const auto &method : declaration.methods) {
      ++summary.method_declaration_entries;
      summary.selector_piece_entries += method.selector_pieces.size();

      std::size_t method_parameter_links = 0;
      bool method_parameter_names_complete = true;
      for (const auto &piece : method.selector_pieces) {
        if (!piece.has_parameter) {
          continue;
        }
        ++method_parameter_links;
        ++summary.selector_piece_parameter_links;
        if (piece.parameter_name.empty()) {
          method_parameter_names_complete = false;
        }
      }

      if (method.selector_is_normalized) {
        ++summary.normalized_method_declarations;
      }

      summary.deterministic_selector_normalization_handoff =
          summary.deterministic_selector_normalization_handoff &&
          (!method.selector_pieces.empty() || method.selector.empty()) &&
          (method.selector_is_normalized || method.selector_pieces.empty()) &&
          method_parameter_names_complete &&
          method_parameter_links <= method.params.size() &&
          method.params.size() <= method.selector_pieces.size();
    }
  }
}

Objc3FrontendSelectorNormalizationSummary BuildSelectorNormalizationSummary(const Objc3Program &program) {
  Objc3FrontendSelectorNormalizationSummary summary;
  AccumulateSelectorNormalizationSummary(program.protocols, summary);
  AccumulateSelectorNormalizationSummary(program.interfaces, summary);
  AccumulateSelectorNormalizationSummary(program.implementations, summary);
  summary.deterministic_selector_normalization_handoff =
      summary.deterministic_selector_normalization_handoff &&
      summary.normalized_method_declarations <= summary.method_declaration_entries &&
      summary.selector_piece_parameter_links <= summary.selector_piece_entries;
  return summary;
}

template <typename Container>
void AccumulatePropertyAttributeSummary(const Container &declarations,
                                        Objc3FrontendPropertyAttributeSummary &summary) {
  for (const auto &declaration : declarations) {
    for (const auto &property : declaration.properties) {
      ++summary.property_declaration_entries;
      summary.property_attribute_entries += property.attributes.size();

      std::size_t accessor_modifier_entries = 0;
      if (property.is_readonly) {
        ++accessor_modifier_entries;
      }
      if (property.is_readwrite) {
        ++accessor_modifier_entries;
      }
      if (property.is_atomic) {
        ++accessor_modifier_entries;
      }
      if (property.is_nonatomic) {
        ++accessor_modifier_entries;
      }
      if (property.is_copy) {
        ++accessor_modifier_entries;
      }
      if (property.is_strong) {
        ++accessor_modifier_entries;
      }
      if (property.is_weak) {
        ++accessor_modifier_entries;
      }
      if (property.is_assign) {
        ++accessor_modifier_entries;
      }
      if (property.has_getter) {
        ++accessor_modifier_entries;
        ++summary.property_getter_selector_entries;
      }
      if (property.has_setter) {
        ++accessor_modifier_entries;
        ++summary.property_setter_selector_entries;
      }
      summary.property_accessor_modifier_entries += accessor_modifier_entries;

      bool attribute_names_complete = true;
      bool attribute_values_complete = true;
      for (const auto &attribute : property.attributes) {
        if (attribute.name.empty()) {
          attribute_names_complete = false;
        }
        if (attribute.has_value) {
          ++summary.property_attribute_value_entries;
          if (attribute.value.empty()) {
            attribute_values_complete = false;
          }
        }
      }

      summary.deterministic_property_attribute_handoff =
          summary.deterministic_property_attribute_handoff &&
          !property.name.empty() &&
          (!property.is_readonly || !property.is_readwrite) &&
          (!property.is_atomic || !property.is_nonatomic) &&
          (!property.has_getter || !property.getter_selector.empty()) &&
          (!property.has_setter || !property.setter_selector.empty()) &&
          attribute_names_complete &&
          attribute_values_complete &&
          summary.property_getter_selector_entries <= summary.property_declaration_entries &&
          summary.property_setter_selector_entries <= summary.property_declaration_entries;
    }
  }
}

Objc3FrontendPropertyAttributeSummary BuildPropertyAttributeSummary(const Objc3Program &program) {
  Objc3FrontendPropertyAttributeSummary summary;
  AccumulatePropertyAttributeSummary(program.protocols, summary);
  AccumulatePropertyAttributeSummary(program.interfaces, summary);
  AccumulatePropertyAttributeSummary(program.implementations, summary);
  summary.deterministic_property_attribute_handoff =
      summary.deterministic_property_attribute_handoff &&
      summary.property_attribute_value_entries <= summary.property_attribute_entries &&
      summary.property_accessor_modifier_entries >= summary.property_getter_selector_entries &&
      summary.property_accessor_modifier_entries >= summary.property_setter_selector_entries;
  return summary;
}

void AccumulateObjectPointerNullabilityGenericsTypeAnnotation(
    bool object_pointer_type_spelling,
    const std::string &object_pointer_type_name,
    bool has_pointer_declarator,
    unsigned pointer_declarator_depth,
    const std::vector<Objc3SemaTokenMetadata> &pointer_declarator_tokens,
    const std::vector<Objc3SemaTokenMetadata> &nullability_suffix_tokens,
    bool has_generic_suffix,
    bool generic_suffix_terminated,
    const std::string &generic_suffix_text,
    Objc3FrontendObjectPointerNullabilityGenericsSummary &summary) {
  if (object_pointer_type_spelling) {
    ++summary.object_pointer_type_spellings;
  }
  summary.pointer_declarator_depth_total += pointer_declarator_depth;
  summary.pointer_declarator_token_entries += pointer_declarator_tokens.size();
  summary.nullability_suffix_entries += nullability_suffix_tokens.size();

  if (has_pointer_declarator) {
    ++summary.pointer_declarator_entries;
    summary.deterministic_object_pointer_nullability_generics_handoff =
        summary.deterministic_object_pointer_nullability_generics_handoff && pointer_declarator_depth > 0;
  } else {
    summary.deterministic_object_pointer_nullability_generics_handoff =
        summary.deterministic_object_pointer_nullability_generics_handoff && pointer_declarator_depth == 0;
  }

  summary.deterministic_object_pointer_nullability_generics_handoff =
      summary.deterministic_object_pointer_nullability_generics_handoff &&
      (!object_pointer_type_spelling || !object_pointer_type_name.empty()) &&
      pointer_declarator_tokens.size() == static_cast<std::size_t>(pointer_declarator_depth);

  for (const auto &token : pointer_declarator_tokens) {
    summary.deterministic_object_pointer_nullability_generics_handoff =
        summary.deterministic_object_pointer_nullability_generics_handoff &&
        token.kind == Objc3SemaTokenKind::PointerDeclarator && !token.text.empty();
  }
  for (const auto &token : nullability_suffix_tokens) {
    summary.deterministic_object_pointer_nullability_generics_handoff =
        summary.deterministic_object_pointer_nullability_generics_handoff &&
        token.kind == Objc3SemaTokenKind::NullabilitySuffix && !token.text.empty();
  }

  if (has_generic_suffix) {
    ++summary.generic_suffix_entries;
    if (generic_suffix_terminated) {
      ++summary.terminated_generic_suffix_entries;
    } else {
      ++summary.unterminated_generic_suffix_entries;
    }
    summary.deterministic_object_pointer_nullability_generics_handoff =
        summary.deterministic_object_pointer_nullability_generics_handoff &&
        !generic_suffix_text.empty() && generic_suffix_text.front() == '<' &&
        (!generic_suffix_terminated || generic_suffix_text.back() == '>');
    return;
  }

  summary.deterministic_object_pointer_nullability_generics_handoff =
      summary.deterministic_object_pointer_nullability_generics_handoff &&
      generic_suffix_terminated && generic_suffix_text.empty();
}

void AccumulateObjectPointerNullabilityGenericsForMethod(
    const Objc3MethodDecl &method,
    Objc3FrontendObjectPointerNullabilityGenericsSummary &summary) {
  AccumulateObjectPointerNullabilityGenericsTypeAnnotation(method.return_object_pointer_type_spelling,
                                                           method.return_object_pointer_type_name,
                                                           method.has_return_pointer_declarator,
                                                           method.return_pointer_declarator_depth,
                                                           method.return_pointer_declarator_tokens,
                                                           method.return_nullability_suffix_tokens,
                                                           method.has_return_generic_suffix,
                                                           method.return_generic_suffix_terminated,
                                                           method.return_generic_suffix_text,
                                                           summary);
  for (const auto &param : method.params) {
    AccumulateObjectPointerNullabilityGenericsTypeAnnotation(param.object_pointer_type_spelling,
                                                             param.object_pointer_type_name,
                                                             param.has_pointer_declarator,
                                                             param.pointer_declarator_depth,
                                                             param.pointer_declarator_tokens,
                                                             param.nullability_suffix_tokens,
                                                             param.has_generic_suffix,
                                                             param.generic_suffix_terminated,
                                                             param.generic_suffix_text,
                                                             summary);
  }
}

template <typename Container>
void AccumulateObjectPointerNullabilityGenericsForObjcDeclarations(
    const Container &declarations,
    Objc3FrontendObjectPointerNullabilityGenericsSummary &summary) {
  for (const auto &declaration : declarations) {
    for (const auto &property : declaration.properties) {
      AccumulateObjectPointerNullabilityGenericsTypeAnnotation(property.object_pointer_type_spelling,
                                                               property.object_pointer_type_name,
                                                               property.has_pointer_declarator,
                                                               property.pointer_declarator_depth,
                                                               property.pointer_declarator_tokens,
                                                               property.nullability_suffix_tokens,
                                                               property.has_generic_suffix,
                                                               property.generic_suffix_terminated,
                                                               property.generic_suffix_text,
                                                               summary);
    }
    for (const auto &method : declaration.methods) {
      AccumulateObjectPointerNullabilityGenericsForMethod(method, summary);
    }
  }
}

Objc3FrontendObjectPointerNullabilityGenericsSummary BuildObjectPointerNullabilityGenericsSummary(
    const Objc3Program &program) {
  Objc3FrontendObjectPointerNullabilityGenericsSummary summary;
  for (const auto &fn : program.functions) {
    AccumulateObjectPointerNullabilityGenericsTypeAnnotation(fn.return_object_pointer_type_spelling,
                                                             fn.return_object_pointer_type_name,
                                                             fn.has_return_pointer_declarator,
                                                             fn.return_pointer_declarator_depth,
                                                             fn.return_pointer_declarator_tokens,
                                                             fn.return_nullability_suffix_tokens,
                                                             fn.has_return_generic_suffix,
                                                             fn.return_generic_suffix_terminated,
                                                             fn.return_generic_suffix_text,
                                                             summary);
    for (const auto &param : fn.params) {
      AccumulateObjectPointerNullabilityGenericsTypeAnnotation(param.object_pointer_type_spelling,
                                                               param.object_pointer_type_name,
                                                               param.has_pointer_declarator,
                                                               param.pointer_declarator_depth,
                                                               param.pointer_declarator_tokens,
                                                               param.nullability_suffix_tokens,
                                                               param.has_generic_suffix,
                                                               param.generic_suffix_terminated,
                                                               param.generic_suffix_text,
                                                               summary);
    }
  }
  AccumulateObjectPointerNullabilityGenericsForObjcDeclarations(program.protocols, summary);
  AccumulateObjectPointerNullabilityGenericsForObjcDeclarations(program.interfaces, summary);
  AccumulateObjectPointerNullabilityGenericsForObjcDeclarations(program.implementations, summary);

  summary.deterministic_object_pointer_nullability_generics_handoff =
      summary.deterministic_object_pointer_nullability_generics_handoff &&
      summary.terminated_generic_suffix_entries + summary.unterminated_generic_suffix_entries ==
          summary.generic_suffix_entries &&
      summary.pointer_declarator_entries <= summary.pointer_declarator_depth_total &&
      summary.pointer_declarator_entries <= summary.pointer_declarator_token_entries;
  return summary;
}

std::string BuildSymbolGraphScopeResolutionHandoffKey(
    const Objc3FrontendSymbolGraphScopeResolutionSummary &summary) {
  std::ostringstream out;
  out << "symbol_graph_nodes="
      << summary.global_symbol_nodes << ":" << summary.function_symbol_nodes << ":" << summary.interface_symbol_nodes
      << ":" << summary.implementation_symbol_nodes << ":" << summary.interface_property_symbol_nodes << ":"
      << summary.implementation_property_symbol_nodes << ":" << summary.interface_method_symbol_nodes << ":"
      << summary.implementation_method_symbol_nodes
      << ";scope_surface="
      << summary.top_level_scope_symbols << ":" << summary.nested_scope_symbols << ":"
      << summary.scope_frames_total
      << ";resolution_surface="
      << summary.implementation_interface_resolution_sites << ":" << summary.implementation_interface_resolution_hits
      << ":" << summary.implementation_interface_resolution_misses << ":" << summary.method_resolution_sites << ":"
      << summary.method_resolution_hits << ":" << summary.method_resolution_misses
      << ";deterministic="
      << (summary.deterministic_symbol_graph_handoff ? "true" : "false") << ":"
      << (summary.deterministic_scope_resolution_handoff ? "true" : "false");
  return out.str();
}

Objc3FrontendSymbolGraphScopeResolutionSummary BuildSymbolGraphScopeResolutionSummary(
    const Objc3SemanticIntegrationSurface &integration_surface,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff) {
  Objc3FrontendSymbolGraphScopeResolutionSummary summary;
  const Objc3SymbolGraphScopeResolutionSummary &integration_summary =
      integration_surface.symbol_graph_scope_resolution_summary;
  const Objc3SymbolGraphScopeResolutionSummary &type_metadata_summary =
      type_metadata_handoff.symbol_graph_scope_resolution_summary;

  const auto select_value = [&](std::size_t integration_value, std::size_t type_metadata_value) {
    if (integration_surface.built) {
      return integration_value;
    }
    return type_metadata_value;
  };

  summary.global_symbol_nodes = select_value(integration_summary.global_symbol_nodes,
                                             type_metadata_summary.global_symbol_nodes);
  summary.function_symbol_nodes = select_value(integration_summary.function_symbol_nodes,
                                               type_metadata_summary.function_symbol_nodes);
  summary.interface_symbol_nodes = select_value(integration_summary.interface_symbol_nodes,
                                                type_metadata_summary.interface_symbol_nodes);
  summary.implementation_symbol_nodes = select_value(integration_summary.implementation_symbol_nodes,
                                                     type_metadata_summary.implementation_symbol_nodes);
  summary.interface_property_symbol_nodes = select_value(integration_summary.interface_property_symbol_nodes,
                                                         type_metadata_summary.interface_property_symbol_nodes);
  summary.implementation_property_symbol_nodes = select_value(integration_summary.implementation_property_symbol_nodes,
                                                              type_metadata_summary.implementation_property_symbol_nodes);
  summary.interface_method_symbol_nodes = select_value(integration_summary.interface_method_symbol_nodes,
                                                       type_metadata_summary.interface_method_symbol_nodes);
  summary.implementation_method_symbol_nodes = select_value(integration_summary.implementation_method_symbol_nodes,
                                                            type_metadata_summary.implementation_method_symbol_nodes);
  summary.top_level_scope_symbols = select_value(integration_summary.top_level_scope_symbols,
                                                 type_metadata_summary.top_level_scope_symbols);
  summary.nested_scope_symbols = select_value(integration_summary.nested_scope_symbols,
                                              type_metadata_summary.nested_scope_symbols);
  summary.scope_frames_total = select_value(integration_summary.scope_frames_total,
                                            type_metadata_summary.scope_frames_total);
  summary.implementation_interface_resolution_sites =
      select_value(integration_summary.implementation_interface_resolution_sites,
                   type_metadata_summary.implementation_interface_resolution_sites);
  summary.implementation_interface_resolution_hits =
      select_value(integration_summary.implementation_interface_resolution_hits,
                   type_metadata_summary.implementation_interface_resolution_hits);
  summary.implementation_interface_resolution_misses =
      select_value(integration_summary.implementation_interface_resolution_misses,
                   type_metadata_summary.implementation_interface_resolution_misses);
  summary.method_resolution_sites = select_value(integration_summary.method_resolution_sites,
                                                 type_metadata_summary.method_resolution_sites);
  summary.method_resolution_hits = select_value(integration_summary.method_resolution_hits,
                                                type_metadata_summary.method_resolution_hits);
  summary.method_resolution_misses = select_value(integration_summary.method_resolution_misses,
                                                  type_metadata_summary.method_resolution_misses);

  const bool symbol_graph_fields_match =
      integration_summary.global_symbol_nodes == type_metadata_summary.global_symbol_nodes &&
      integration_summary.function_symbol_nodes == type_metadata_summary.function_symbol_nodes &&
      integration_summary.interface_symbol_nodes == type_metadata_summary.interface_symbol_nodes &&
      integration_summary.implementation_symbol_nodes == type_metadata_summary.implementation_symbol_nodes &&
      integration_summary.interface_property_symbol_nodes == type_metadata_summary.interface_property_symbol_nodes &&
      integration_summary.implementation_property_symbol_nodes ==
          type_metadata_summary.implementation_property_symbol_nodes &&
      integration_summary.interface_method_symbol_nodes == type_metadata_summary.interface_method_symbol_nodes &&
      integration_summary.implementation_method_symbol_nodes ==
          type_metadata_summary.implementation_method_symbol_nodes;
  const bool scope_resolution_fields_match =
      integration_summary.top_level_scope_symbols == type_metadata_summary.top_level_scope_symbols &&
      integration_summary.nested_scope_symbols == type_metadata_summary.nested_scope_symbols &&
      integration_summary.scope_frames_total == type_metadata_summary.scope_frames_total &&
      integration_summary.implementation_interface_resolution_sites ==
          type_metadata_summary.implementation_interface_resolution_sites &&
      integration_summary.implementation_interface_resolution_hits ==
          type_metadata_summary.implementation_interface_resolution_hits &&
      integration_summary.implementation_interface_resolution_misses ==
          type_metadata_summary.implementation_interface_resolution_misses &&
      integration_summary.method_resolution_sites == type_metadata_summary.method_resolution_sites &&
      integration_summary.method_resolution_hits == type_metadata_summary.method_resolution_hits &&
      integration_summary.method_resolution_misses == type_metadata_summary.method_resolution_misses;

  summary.deterministic_symbol_graph_handoff =
      integration_summary.deterministic &&
      type_metadata_summary.deterministic &&
      symbol_graph_fields_match &&
      summary.symbol_nodes_total() == summary.top_level_scope_symbols + summary.nested_scope_symbols;
  summary.deterministic_scope_resolution_handoff =
      integration_summary.deterministic &&
      type_metadata_summary.deterministic &&
      scope_resolution_fields_match &&
      summary.resolution_hits_total() <= summary.resolution_sites_total() &&
      summary.resolution_hits_total() + summary.resolution_misses_total() == summary.resolution_sites_total();
  summary.deterministic_handoff_key = BuildSymbolGraphScopeResolutionHandoffKey(summary);
  return summary;
}

}  // namespace

Objc3FrontendPipelineResult RunObjc3FrontendPipeline(const std::string &source,
                                                     const Objc3FrontendOptions &options) {
  Objc3FrontendPipelineResult result;

  Objc3LexerOptions lexer_options;
  lexer_options.language_version = options.language_version;
  lexer_options.compatibility_mode = options.compatibility_mode == Objc3FrontendCompatibilityMode::kLegacy
                                         ? Objc3LexerCompatibilityMode::kLegacy
                                         : Objc3LexerCompatibilityMode::kCanonical;
  lexer_options.migration_assist = options.migration_assist;
  Objc3Lexer lexer(source, lexer_options);
  std::vector<Objc3LexToken> tokens = lexer.Run(result.stage_diagnostics.lexer);
  const Objc3LexerMigrationHints &lexer_hints = lexer.MigrationHints();
  const Objc3LexerLanguageVersionPragmaContract &pragma_contract = lexer.LanguageVersionPragmaContract();
  result.migration_hints.legacy_yes_count = lexer_hints.legacy_yes_count;
  result.migration_hints.legacy_no_count = lexer_hints.legacy_no_count;
  result.migration_hints.legacy_null_count = lexer_hints.legacy_null_count;
  result.language_version_pragma_contract.seen = pragma_contract.seen;
  result.language_version_pragma_contract.directive_count = pragma_contract.directive_count;
  result.language_version_pragma_contract.duplicate = pragma_contract.duplicate;
  result.language_version_pragma_contract.non_leading = pragma_contract.non_leading;
  result.language_version_pragma_contract.first_line = pragma_contract.first_line;
  result.language_version_pragma_contract.first_column = pragma_contract.first_column;
  result.language_version_pragma_contract.last_line = pragma_contract.last_line;
  result.language_version_pragma_contract.last_column = pragma_contract.last_column;

  if (result.stage_diagnostics.lexer.empty()) {
    Objc3AstBuilderResult parse_result = BuildObjc3AstFromTokens(tokens);
    result.program = std::move(parse_result.program);
    result.stage_diagnostics.parser = std::move(parse_result.diagnostics);
    result.parser_contract_snapshot = parse_result.contract_snapshot;
  }
  result.selector_normalization_summary =
      BuildSelectorNormalizationSummary(Objc3ParsedProgramAst(result.program));
  result.property_attribute_summary =
      BuildPropertyAttributeSummary(Objc3ParsedProgramAst(result.program));
  result.object_pointer_nullability_generics_summary =
      BuildObjectPointerNullabilityGenericsSummary(Objc3ParsedProgramAst(result.program));
  result.protocol_category_summary =
      BuildProtocolCategorySummary(Objc3ParsedProgramAst(result.program),
                                   result.integration_surface,
                                   result.sema_type_metadata_handoff);
  result.class_protocol_category_linking_summary =
      BuildClassProtocolCategoryLinkingSummary(result.sema_type_metadata_handoff.interface_implementation_summary,
                                               result.protocol_category_summary,
                                               result.integration_surface,
                                               result.sema_type_metadata_handoff);
  result.symbol_graph_scope_resolution_summary =
      BuildSymbolGraphScopeResolutionSummary(result.integration_surface,
                                             result.sema_type_metadata_handoff);

  if (result.stage_diagnostics.lexer.empty() && result.stage_diagnostics.parser.empty()) {
    Objc3SemanticValidationOptions semantic_options;
    semantic_options.max_message_send_args = options.lowering.max_message_send_args;

    Objc3SemaPassManagerInput sema_input;
    sema_input.program = &result.program;
    sema_input.validation_options = semantic_options;
    sema_input.compatibility_mode = options.compatibility_mode == Objc3FrontendCompatibilityMode::kLegacy
                                        ? Objc3SemaCompatibilityMode::Legacy
                                        : Objc3SemaCompatibilityMode::Canonical;
    sema_input.migration_assist = options.migration_assist;
    sema_input.migration_hints.legacy_yes_count = result.migration_hints.legacy_yes_count;
    sema_input.migration_hints.legacy_no_count = result.migration_hints.legacy_no_count;
    sema_input.migration_hints.legacy_null_count = result.migration_hints.legacy_null_count;
    sema_input.diagnostics_bus.diagnostics = &result.stage_diagnostics.semantic;

    Objc3SemaPassManagerResult sema_result = RunObjc3SemaPassManager(sema_input);
    result.integration_surface = std::move(sema_result.integration_surface);
    result.sema_type_metadata_handoff = std::move(sema_result.type_metadata_handoff);
    result.protocol_category_summary =
        BuildProtocolCategorySummary(Objc3ParsedProgramAst(result.program),
                                     result.integration_surface,
                                     result.sema_type_metadata_handoff);
    result.class_protocol_category_linking_summary =
        BuildClassProtocolCategoryLinkingSummary(result.sema_type_metadata_handoff.interface_implementation_summary,
                                                 result.protocol_category_summary,
                                                 result.integration_surface,
                                                 result.sema_type_metadata_handoff);
    result.symbol_graph_scope_resolution_summary =
        BuildSymbolGraphScopeResolutionSummary(result.integration_surface,
                                               result.sema_type_metadata_handoff);
    result.sema_diagnostics_after_pass = sema_result.diagnostics_after_pass;
    result.sema_pass_flow_summary = sema_result.sema_pass_flow_summary;
    result.sema_parity_surface = sema_result.parity_surface;
    if (result.stage_diagnostics.semantic.empty() && !sema_result.diagnostics.empty()) {
      result.stage_diagnostics.semantic = std::move(sema_result.diagnostics);
    }
  }

  result.runtime_metadata_source_records =
      BuildRuntimeMetadataSourceRecordSet(Objc3ParsedProgramAst(result.program));
  result.runtime_metadata_source_ownership_boundary =
      BuildRuntimeMetadataSourceOwnershipBoundary(result.runtime_metadata_source_records,
                                                 result.sema_type_metadata_handoff);
  result.typed_sema_to_lowering_contract_surface =
      BuildObjc3TypedSemaToLoweringContractSurface(result, options);
  result.runtime_export_legality_boundary = BuildRuntimeExportLegalityBoundary(
      result.runtime_metadata_source_ownership_boundary,
      result.typed_sema_to_lowering_contract_surface,
      result.integration_surface,
      result.protocol_category_summary,
      result.class_protocol_category_linking_summary,
      result.selector_normalization_summary,
      result.property_attribute_summary,
      result.object_pointer_nullability_generics_summary,
      result.symbol_graph_scope_resolution_summary,
      result.sema_parity_surface);
  result.runtime_export_enforcement_summary =
      BuildRuntimeExportEnforcementSummary(
          result.runtime_metadata_source_records,
          result.runtime_export_legality_boundary);
  if (result.stage_diagnostics.semantic.empty() &&
      HasRuntimeMetadataSourceRecords(result.runtime_metadata_source_records) &&
      !IsReadyObjc3RuntimeExportEnforcementSummary(
          result.runtime_export_enforcement_summary)) {
    result.stage_diagnostics.semantic.push_back(MakeDiag(
        result.runtime_export_enforcement_summary.first_failure_line,
        result.runtime_export_enforcement_summary.first_failure_column,
        "O3S260",
        "runtime metadata export blocked: " +
            result.runtime_export_enforcement_summary.failure_reason));
  }
  result.semantic_diagnostic_taxonomy_and_fixit_synthesis_scaffold =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisScaffold(
          result.sema_pass_flow_summary,
          result.sema_parity_surface,
          result.typed_sema_to_lowering_contract_surface);
  result.semantic_diagnostic_taxonomy_and_fixit_core_feature_implementation_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureImplementationSurface(
          result.sema_pass_flow_summary,
          result.sema_parity_surface,
          result.typed_sema_to_lowering_contract_surface,
          result.semantic_diagnostic_taxonomy_and_fixit_synthesis_scaffold);
  result.semantic_diagnostic_taxonomy_and_fixit_core_feature_expansion_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisCoreFeatureExpansionSurface(
          result.semantic_diagnostic_taxonomy_and_fixit_core_feature_implementation_surface,
          result.sema_parity_surface,
          result.typed_sema_to_lowering_contract_surface);
  result.parse_lowering_readiness_surface = BuildObjc3ParseLoweringReadinessSurface(result, options);
  result.semantic_diagnostic_taxonomy_and_fixit_edge_case_compatibility_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseCompatibilitySurface(
          result.semantic_diagnostic_taxonomy_and_fixit_core_feature_expansion_surface,
          result.parse_lowering_readiness_surface,
          result.typed_sema_to_lowering_contract_surface);
  result.semantic_diagnostic_taxonomy_and_fixit_edge_case_expansion_and_robustness_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseExpansionAndRobustnessSurface(
          result.semantic_diagnostic_taxonomy_and_fixit_edge_case_compatibility_surface,
          result.parse_lowering_readiness_surface);
  result.semantic_diagnostic_taxonomy_and_fixit_diagnostics_hardening_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisDiagnosticsHardeningSurface(
          result.semantic_diagnostic_taxonomy_and_fixit_edge_case_expansion_and_robustness_surface,
          result.parse_lowering_readiness_surface);
  result.semantic_diagnostic_taxonomy_and_fixit_recovery_determinism_hardening_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisRecoveryDeterminismHardeningSurface(
          result.semantic_diagnostic_taxonomy_and_fixit_diagnostics_hardening_surface,
          result.parse_lowering_readiness_surface);
  result.semantic_diagnostic_taxonomy_and_fixit_conformance_matrix_implementation_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceMatrixImplementationSurface(
          result.semantic_diagnostic_taxonomy_and_fixit_recovery_determinism_hardening_surface,
          result.parse_lowering_readiness_surface);
  result.semantic_diagnostic_taxonomy_and_fixit_conformance_corpus_expansion_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceCorpusExpansionSurface(
          result.semantic_diagnostic_taxonomy_and_fixit_conformance_matrix_implementation_surface,
          result.parse_lowering_readiness_surface);
  result.semantic_diagnostic_taxonomy_and_fixit_performance_quality_guardrails_surface =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisPerformanceQualityGuardrailsSurface(
          result.semantic_diagnostic_taxonomy_and_fixit_conformance_corpus_expansion_surface,
          result.parse_lowering_readiness_surface);
  result.semantic_stability_spec_delta_closure_scaffold =
      BuildObjc3SemanticStabilitySpecDeltaClosureScaffold(
          result.typed_sema_to_lowering_contract_surface,
          result.parse_lowering_readiness_surface);
  result.semantic_stability_core_feature_implementation_surface =
      BuildObjc3SemanticStabilityCoreFeatureImplementationSurface(
          result.typed_sema_to_lowering_contract_surface,
          result.parse_lowering_readiness_surface,
          result.semantic_stability_spec_delta_closure_scaffold);
  result.lowering_runtime_stability_invariant_scaffold =
      BuildObjc3LoweringRuntimeStabilityInvariantScaffold(
          result.typed_sema_to_lowering_contract_surface,
          result.parse_lowering_readiness_surface);
  result.lowering_pipeline_pass_graph_scaffold =
      BuildObjc3LoweringPipelinePassGraphScaffold(result, options);
  result.lowering_pipeline_pass_graph_core_feature_surface =
      BuildObjc3LoweringPipelinePassGraphCoreFeatureSurface(result, options);
  result.ir_emission_completeness_scaffold =
      BuildObjc3IREmissionCompletenessScaffold(result);
  result.lowering_runtime_diagnostics_surfacing_scaffold =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingScaffold(result);
  result.lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface(
          result);
  result.lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface(
          result);
  result.lowering_runtime_diagnostics_surfacing_edge_case_compatibility_surface =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseCompatibilitySurface(
          result);
  result
      .lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_surface =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurface(
          result);
  result.lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface(
          result);
  result
      .lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface(
          result);
  result
      .lowering_runtime_diagnostics_surfacing_conformance_matrix_implementation_surface =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface(
          result);
  result.lowering_runtime_stability_core_feature_implementation_surface =
      BuildObjc3LoweringRuntimeStabilityCoreFeatureImplementationSurface(
          result.typed_sema_to_lowering_contract_surface,
          result.parse_lowering_readiness_surface,
          result.lowering_runtime_stability_invariant_scaffold);
  TransportObjc3DiagnosticsToParsedProgram(result.stage_diagnostics, result.program);
  return result;
}
