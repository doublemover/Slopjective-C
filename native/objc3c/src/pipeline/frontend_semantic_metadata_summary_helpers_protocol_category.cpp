#include "pipeline/frontend_semantic_metadata_summary_helpers.h"

#include <cstddef>
#include <type_traits>
#include <utility>

namespace objc3c::pipeline::orchestration {
namespace detail {

template <typename T, typename = void>
struct HasProtocolsMember : std::false_type {};

template <typename T>
struct HasProtocolsMember<
    T,
    std::void_t<decltype(std::declval<const T &>().protocols)>>
    : std::true_type {};

template <typename T, typename = void>
struct HasCategoriesMember : std::false_type {};

template <typename T>
struct HasCategoriesMember<
    T,
    std::void_t<decltype(std::declval<const T &>().categories)>>
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
      if constexpr (HasMethodsMember<
                        std::decay_t<decltype(metadata)>>::value) {
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
      if constexpr (HasMethodsMember<
                        std::decay_t<decltype(metadata)>>::value) {
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
      if constexpr (HasHasMatchingInterfaceMember<
                        std::decay_t<decltype(metadata)>>::value &&
                    HasMethodsMember<
                        std::decay_t<decltype(metadata)>>::value) {
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
      if constexpr (HasMethodsLexicographicMember<
                        std::decay_t<decltype(metadata)>>::value) {
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
      if constexpr (HasMethodsLexicographicMember<
                        std::decay_t<decltype(metadata)>>::value) {
        total += metadata.methods_lexicographic.size();
      }
    }
    return total;
  }
  return 0;
}

template <typename Handoff>
std::size_t CountLinkedCategorySymbolsFromTypeMetadata(
    const Handoff &handoff) {
  if constexpr (HasCategoriesLexicographicMember<Handoff>::value) {
    std::size_t total = 0;
    for (const auto &metadata : handoff.categories_lexicographic) {
      if constexpr (HasHasMatchingInterfaceMember<
                        std::decay_t<decltype(metadata)>>::value &&
                    HasMethodsLexicographicMember<
                        std::decay_t<decltype(metadata)>>::value) {
        if (metadata.has_matching_interface) {
          total += metadata.methods_lexicographic.size();
        }
      }
    }
    return total;
  }
  return 0;
}

}  // namespace detail

Objc3FrontendProtocolCategorySummary BuildProtocolCategorySummary(
    const Objc3Program &program,
    const Objc3SemanticIntegrationSurface &integration_surface,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff) {
  Objc3FrontendProtocolCategorySummary summary;
  summary.declared_protocols = detail::CountProtocols(program);
  summary.declared_categories = detail::CountCategories(program);
  summary.resolved_protocol_symbols =
      detail::CountProtocols(integration_surface);
  summary.resolved_category_symbols =
      detail::CountCategories(integration_surface);
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

Objc3FrontendClassProtocolCategoryLinkingSummary
BuildClassProtocolCategoryLinkingSummary(
    const Objc3InterfaceImplementationSummary &interface_implementation_summary,
    const Objc3FrontendProtocolCategorySummary &protocol_category_summary,
    const Objc3SemanticIntegrationSurface &integration_surface,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff) {
  Objc3FrontendClassProtocolCategoryLinkingSummary summary;
  summary.declared_class_interfaces =
      interface_implementation_summary.declared_interfaces;
  summary.declared_class_implementations =
      interface_implementation_summary.declared_implementations;
  summary.resolved_class_interfaces =
      interface_implementation_summary.resolved_interfaces;
  summary.resolved_class_implementations =
      interface_implementation_summary.resolved_implementations;
  summary.linked_class_method_symbols =
      interface_implementation_summary.linked_implementation_symbols;
  summary.linked_category_method_symbols =
      protocol_category_summary.linked_category_symbols;

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
      select_composition_value(
          integration_composition_summary.protocol_composition_sites,
          type_metadata_composition_summary.protocol_composition_sites);
  summary.protocol_composition_symbols =
      select_composition_value(
          integration_composition_summary.protocol_composition_symbols,
          type_metadata_composition_summary.protocol_composition_symbols);
  summary.category_composition_sites =
      select_composition_value(
          integration_composition_summary.category_composition_sites,
          type_metadata_composition_summary.category_composition_sites);
  summary.category_composition_symbols =
      select_composition_value(
          integration_composition_summary.category_composition_symbols,
          type_metadata_composition_summary.category_composition_symbols);
  summary.invalid_protocol_composition_sites =
      select_composition_value(
          integration_composition_summary.invalid_protocol_composition_sites,
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
      summary.resolved_class_interfaces <=
          summary.declared_class_interfaces &&
      summary.resolved_class_implementations <=
          summary.declared_class_implementations &&
      summary.linked_class_method_symbols <=
          interface_implementation_summary.interface_method_symbols &&
      summary.linked_class_method_symbols <=
          interface_implementation_summary.implementation_method_symbols &&
      summary.linked_category_method_symbols <=
          protocol_category_summary.category_method_symbols &&
      summary.category_composition_sites <= summary.protocol_composition_sites &&
      summary.category_composition_symbols <=
          summary.protocol_composition_symbols &&
      summary.invalid_protocol_composition_sites <=
          summary.protocol_composition_sites + summary.category_composition_sites;
  return summary;
}

}  // namespace objc3c::pipeline::orchestration
