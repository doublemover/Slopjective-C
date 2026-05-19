#include "pipeline/frontend_semantic_metadata_summary_helpers.h"

#include <cstddef>

namespace objc3c::pipeline::orchestration {
namespace detail {

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
          !property.name.empty() && (!property.is_readonly || !property.is_readwrite) &&
          (!property.is_atomic || !property.is_nonatomic) &&
          (!property.is_weak ||
           (!property.is_unowned && !property.is_unsafe_unretained)) &&
          (!property.is_nullable || !property.is_nonnull) &&
          (!property.is_null_resettable ||
           (!property.is_nullable && !property.is_nonnull)) &&
          (!property.has_getter || !property.getter_selector.empty()) &&
          (!property.has_setter || !property.setter_selector.empty()) &&
          attribute_names_complete && attribute_values_complete &&
          summary.property_getter_selector_entries <=
              summary.property_declaration_entries &&
          summary.property_setter_selector_entries <=
              summary.property_declaration_entries;
    }
  }
}

}  // namespace detail

Objc3FrontendSelectorNormalizationSummary BuildSelectorNormalizationSummary(
    const Objc3Program &program) {
  Objc3FrontendSelectorNormalizationSummary summary;
  detail::AccumulateSelectorNormalizationSummary(program.protocols, summary);
  detail::AccumulateSelectorNormalizationSummary(program.interfaces, summary);
  detail::AccumulateSelectorNormalizationSummary(program.implementations,
                                                 summary);
  summary.deterministic_selector_normalization_handoff =
      summary.deterministic_selector_normalization_handoff &&
      summary.normalized_method_declarations <=
          summary.method_declaration_entries &&
      summary.selector_piece_parameter_links <= summary.selector_piece_entries;
  return summary;
}

Objc3FrontendPropertyAttributeSummary BuildPropertyAttributeSummary(
    const Objc3Program &program) {
  Objc3FrontendPropertyAttributeSummary summary;
  detail::AccumulatePropertyAttributeSummary(program.protocols, summary);
  detail::AccumulatePropertyAttributeSummary(program.interfaces, summary);
  detail::AccumulatePropertyAttributeSummary(program.implementations, summary);
  summary.deterministic_property_attribute_handoff =
      summary.deterministic_property_attribute_handoff &&
      summary.property_attribute_value_entries <=
          summary.property_attribute_entries &&
      summary.property_accessor_modifier_entries >=
          summary.property_getter_selector_entries &&
      summary.property_accessor_modifier_entries >=
          summary.property_setter_selector_entries;
  return summary;
}

}  // namespace objc3c::pipeline::orchestration
