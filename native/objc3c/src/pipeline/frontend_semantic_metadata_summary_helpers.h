#pragma once

#include <cstddef>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "ast/objc3_ast_declarations.h"
#include "sema/model/frontend_linkage_summaries.h"
#include "sema/objc3_sema_contract_type_handoff.h"
#include "support/objc3_type_profile_helpers.h"
#include "token/objc3_sema_token_metadata.h"

namespace objc3c::pipeline::orchestration {
namespace detail {

template <typename T, typename = void>
struct HasProtocolsMember : std::false_type {};

template <typename T>
struct HasProtocolsMember<T, std::void_t<decltype(std::declval<const T &>().protocols)>>
    : std::true_type {};

template <typename T, typename = void>
struct HasCategoriesMember : std::false_type {};

template <typename T>
struct HasCategoriesMember<T, std::void_t<decltype(std::declval<const T &>().categories)>>
    : std::true_type {};

template <typename T, typename = void>
struct HasMethodsMember : std::false_type {};

template <typename T>
struct HasMethodsMember<T, std::void_t<decltype(std::declval<const T &>().methods)>>
    : std::true_type {};

template <typename T, typename = void>
struct HasMethodsLexicographicMember : std::false_type {};

template <typename T>
struct HasMethodsLexicographicMember<
    T,
    std::void_t<decltype(std::declval<const T &>().methods_lexicographic)>>
    : std::true_type {};

template <typename T, typename = void>
struct HasHasMatchingInterfaceMember : std::false_type {};

template <typename T>
struct HasHasMatchingInterfaceMember<
    T,
    std::void_t<decltype(std::declval<const T &>().has_matching_interface)>>
    : std::true_type {};

template <typename T, typename = void>
struct HasProtocolsLexicographicMember : std::false_type {};

template <typename T>
struct HasProtocolsLexicographicMember<
    T,
    std::void_t<decltype(std::declval<const T &>().protocols_lexicographic)>>
    : std::true_type {};

template <typename T, typename = void>
struct HasCategoriesLexicographicMember : std::false_type {};

template <typename T>
struct HasCategoriesLexicographicMember<
    T,
    std::void_t<decltype(std::declval<const T &>().categories_lexicographic)>>
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

template <typename Container>
void AccumulateSelectorNormalizationSummary(
    const Container &declarations,
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

template <typename Container>
void AccumulatePropertyAttributeSummary(
    const Container &declarations,
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
      if (property.is_retain) {
        ++accessor_modifier_entries;
      }
      if (property.is_strong) {
        ++accessor_modifier_entries;
      }
      if (property.is_weak) {
        ++accessor_modifier_entries;
      }
      if (property.is_unowned) {
        ++accessor_modifier_entries;
      }
      if (property.is_unsafe_unretained) {
        ++accessor_modifier_entries;
      }
      if (property.is_assign) {
        ++accessor_modifier_entries;
      }
      if (property.is_nullable) {
        ++accessor_modifier_entries;
      }
      if (property.is_nonnull) {
        ++accessor_modifier_entries;
      }
      if (property.is_null_resettable) {
        ++accessor_modifier_entries;
      }
      if (property.is_class) {
        ++accessor_modifier_entries;
      }
      if (property.is_direct) {
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
          (!property.is_weak || (!property.is_unowned && !property.is_unsafe_unretained)) &&
          (!property.is_nullable || !property.is_nonnull) &&
          (!property.is_null_resettable || (!property.is_nullable && !property.is_nonnull)) &&
          (!property.has_getter || !property.getter_selector.empty()) &&
          (!property.has_setter || !property.setter_selector.empty()) &&
          attribute_names_complete &&
          attribute_values_complete &&
          summary.property_getter_selector_entries <= summary.property_declaration_entries &&
          summary.property_setter_selector_entries <= summary.property_declaration_entries;
    }
  }
}

inline void AccumulateObjectPointerNullabilityGenericsTypeAnnotation(
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
  }

  summary.deterministic_object_pointer_nullability_generics_handoff =
      summary.deterministic_object_pointer_nullability_generics_handoff &&
      objc3c::support::IsPointerDeclaratorDepthConsistent(
          has_pointer_declarator, pointer_declarator_depth) &&
      objc3c::support::IsObjectPointerTypeNameConsistent(
          object_pointer_type_spelling, object_pointer_type_name) &&
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

inline void AccumulateObjectPointerNullabilityGenericsForMethod(
    const Objc3MethodDecl &method,
    Objc3FrontendObjectPointerNullabilityGenericsSummary &summary) {
  AccumulateObjectPointerNullabilityGenericsTypeAnnotation(
      method.return_object_pointer_type_spelling,
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
    AccumulateObjectPointerNullabilityGenericsTypeAnnotation(
        param.object_pointer_type_spelling,
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
      AccumulateObjectPointerNullabilityGenericsTypeAnnotation(
          property.object_pointer_type_spelling,
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

}  // namespace detail

inline Objc3FrontendProtocolCategorySummary BuildProtocolCategorySummary(
    const Objc3Program &program,
    const Objc3SemanticIntegrationSurface &integration_surface,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff) {
  Objc3FrontendProtocolCategorySummary summary;
  summary.declared_protocols = detail::CountProtocols(program);
  summary.declared_categories = detail::CountCategories(program);
  summary.resolved_protocol_symbols = detail::CountProtocols(integration_surface);
  summary.resolved_category_symbols = detail::CountCategories(integration_surface);
  summary.protocol_method_symbols =
      detail::CountProtocolMethodsFromSymbolTable(integration_surface);
  summary.category_method_symbols =
      detail::CountCategoryMethodsFromSymbolTable(integration_surface);
  summary.linked_category_symbols =
      detail::CountLinkedCategorySymbolsFromSymbolTable(integration_surface);

  if (summary.protocol_method_symbols == 0) {
    summary.protocol_method_symbols =
        detail::CountProtocolMethodsFromTypeMetadata(type_metadata_handoff);
  }
  if (summary.category_method_symbols == 0) {
    summary.category_method_symbols =
        detail::CountCategoryMethodsFromTypeMetadata(type_metadata_handoff);
  }
  if (summary.linked_category_symbols == 0) {
    summary.linked_category_symbols =
        detail::CountLinkedCategorySymbolsFromTypeMetadata(type_metadata_handoff);
  }

  summary.deterministic_protocol_category_handoff =
      summary.linked_category_symbols <= summary.category_method_symbols &&
      summary.resolved_protocol_symbols <= summary.declared_protocols &&
      summary.resolved_category_symbols <= summary.declared_categories;
  return summary;
}

inline Objc3FrontendClassProtocolCategoryLinkingSummary
BuildClassProtocolCategoryLinkingSummary(
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
  const auto select_composition_value = [&](std::size_t integration_value,
                                            std::size_t type_metadata_value) {
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
      summary.linked_class_method_symbols <=
          interface_implementation_summary.interface_method_symbols &&
      summary.linked_class_method_symbols <=
          interface_implementation_summary.implementation_method_symbols &&
      summary.linked_category_method_symbols <= protocol_category_summary.category_method_symbols &&
      summary.category_composition_sites <= summary.protocol_composition_sites &&
      summary.category_composition_symbols <= summary.protocol_composition_symbols &&
      summary.invalid_protocol_composition_sites <=
          summary.protocol_composition_sites + summary.category_composition_sites;
  return summary;
}

inline Objc3FrontendSelectorNormalizationSummary BuildSelectorNormalizationSummary(
    const Objc3Program &program) {
  Objc3FrontendSelectorNormalizationSummary summary;
  detail::AccumulateSelectorNormalizationSummary(program.protocols, summary);
  detail::AccumulateSelectorNormalizationSummary(program.interfaces, summary);
  detail::AccumulateSelectorNormalizationSummary(program.implementations, summary);
  summary.deterministic_selector_normalization_handoff =
      summary.deterministic_selector_normalization_handoff &&
      summary.normalized_method_declarations <= summary.method_declaration_entries &&
      summary.selector_piece_parameter_links <= summary.selector_piece_entries;
  return summary;
}

inline Objc3FrontendPropertyAttributeSummary BuildPropertyAttributeSummary(
    const Objc3Program &program) {
  Objc3FrontendPropertyAttributeSummary summary;
  detail::AccumulatePropertyAttributeSummary(program.protocols, summary);
  detail::AccumulatePropertyAttributeSummary(program.interfaces, summary);
  detail::AccumulatePropertyAttributeSummary(program.implementations, summary);
  summary.deterministic_property_attribute_handoff =
      summary.deterministic_property_attribute_handoff &&
      summary.property_attribute_value_entries <= summary.property_attribute_entries &&
      summary.property_accessor_modifier_entries >= summary.property_getter_selector_entries &&
      summary.property_accessor_modifier_entries >= summary.property_setter_selector_entries;
  return summary;
}

inline Objc3FrontendObjectPointerNullabilityGenericsSummary
BuildObjectPointerNullabilityGenericsSummary(const Objc3Program &program) {
  Objc3FrontendObjectPointerNullabilityGenericsSummary summary;
  for (const auto &fn : program.functions) {
    detail::AccumulateObjectPointerNullabilityGenericsTypeAnnotation(
        fn.return_object_pointer_type_spelling,
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
      detail::AccumulateObjectPointerNullabilityGenericsTypeAnnotation(
          param.object_pointer_type_spelling,
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
  detail::AccumulateObjectPointerNullabilityGenericsForObjcDeclarations(
      program.protocols, summary);
  detail::AccumulateObjectPointerNullabilityGenericsForObjcDeclarations(
      program.interfaces, summary);
  detail::AccumulateObjectPointerNullabilityGenericsForObjcDeclarations(
      program.implementations, summary);

  summary.deterministic_object_pointer_nullability_generics_handoff =
      summary.deterministic_object_pointer_nullability_generics_handoff &&
      summary.terminated_generic_suffix_entries + summary.unterminated_generic_suffix_entries ==
          summary.generic_suffix_entries &&
      summary.pointer_declarator_entries <= summary.pointer_declarator_depth_total &&
      summary.pointer_declarator_entries <= summary.pointer_declarator_token_entries;
  return summary;
}

}  // namespace objc3c::pipeline::orchestration
