#include "ir/objc3_ir_frontend_metadata_publication_core_core_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRFrontendCoreCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!1 = !{i64 "
      << static_cast<unsigned long long>(metadata.declared_interfaces)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.declared_implementations)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.resolved_interface_symbols)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.resolved_implementation_symbols)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.interface_method_symbols)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.implementation_method_symbols)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.linked_implementation_symbols)
      << ", i1 "
      << (metadata.deterministic_interface_implementation_handoff ? 1 : 0)
      << "}\n";
  out << "!2 = !{i64 "
      << static_cast<unsigned long long>(metadata.declared_protocols)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.declared_categories)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.resolved_protocol_symbols)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.resolved_category_symbols)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.protocol_method_symbols)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.category_method_symbols)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.linked_category_symbols)
      << ", i1 "
      << (metadata.deterministic_protocol_category_handoff ? 1 : 0) << "}\n";
  out << "!3 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.selector_method_declaration_entries)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.selector_normalized_method_declarations)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.selector_piece_entries)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.selector_piece_parameter_links)
      << ", i1 "
      << (metadata.deterministic_selector_normalization_handoff ? 1 : 0)
      << "}\n";
  out << "!4 = !{i64 "
      << static_cast<unsigned long long>(metadata.property_declaration_entries)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.property_attribute_entries)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.property_attribute_value_entries)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.property_accessor_modifier_entries)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.property_getter_selector_entries)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.property_setter_selector_entries)
      << ", i1 "
      << (metadata.deterministic_property_attribute_handoff ? 1 : 0)
      << "}\n\n";
}
