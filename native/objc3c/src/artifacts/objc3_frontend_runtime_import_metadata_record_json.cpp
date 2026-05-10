#include "artifacts/objc3_frontend_runtime_import_artifacts.h"

#include <sstream>
#include <string>
#include <vector>

#include "io/objc3_json.h"
#include "support/objc3_runtime_metadata_record_set.h"

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

}  // namespace

std::string RenderRuntimeOwnedDeclarationsJson(
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records) {
  std::ostringstream out;
  out << "{\n"
      << "    \"classes\": [\n";
  for (std::size_t i = 0;
       i < runtime_metadata_source_records.classes_lexicographic.size(); ++i) {
    const auto &class_record =
        runtime_metadata_source_records.classes_lexicographic[i];
    out << "      {\"record_kind\":\""
        << EscapeJsonString(class_record.record_kind)
        << "\",\"name\":\"" << EscapeJsonString(class_record.name)
        << "\",\"super_name\":\"" << EscapeJsonString(class_record.super_name)
        << "\",\"has_super\":" << (class_record.has_super ? "true" : "false")
        << ",\"objc_final_declared\":"
        << (class_record.objc_final_declared ? "true" : "false")
        << ",\"objc_sealed_declared\":"
        << (class_record.objc_sealed_declared ? "true" : "false")
        << ",\"adopted_protocols\":"
        << BuildStringArrayJson(class_record.adopted_protocols_lexicographic)
        << ",\"property_count\":" << class_record.property_count
        << ",\"method_count\":" << class_record.method_count
        << ",\"line\":" << class_record.line
        << ",\"column\":" << class_record.column << "}";
    if (i + 1 != runtime_metadata_source_records.classes_lexicographic.size()) {
      out << ",";
    }
    out << "\n";
  }
  out << "    ],\n"
      << "    \"protocols\": [\n";
  for (std::size_t i = 0;
       i < runtime_metadata_source_records.protocols_lexicographic.size();
       ++i) {
    const auto &protocol_record =
        runtime_metadata_source_records.protocols_lexicographic[i];
    out << "      {\"name\":\"" << EscapeJsonString(protocol_record.name)
        << "\",\"inherited_protocols\":"
        << BuildStringArrayJson(
               protocol_record.inherited_protocols_lexicographic)
        << ",\"is_forward_declaration\":"
        << (protocol_record.is_forward_declaration ? "true" : "false")
        << ",\"property_count\":" << protocol_record.property_count
        << ",\"method_count\":" << protocol_record.method_count
        << ",\"line\":" << protocol_record.line
        << ",\"column\":" << protocol_record.column << "}";
    if (i + 1 !=
        runtime_metadata_source_records.protocols_lexicographic.size()) {
      out << ",";
    }
    out << "\n";
  }
  out << "    ],\n"
      << "    \"categories\": [\n";
  for (std::size_t i = 0;
       i < runtime_metadata_source_records.categories_lexicographic.size();
       ++i) {
    const auto &category_record =
        runtime_metadata_source_records.categories_lexicographic[i];
    out << "      {\"record_kind\":\""
        << EscapeJsonString(category_record.record_kind)
        << "\",\"class_name\":\""
        << EscapeJsonString(category_record.class_name)
        << "\",\"category_name\":\""
        << EscapeJsonString(category_record.category_name)
        << "\",\"adopted_protocols\":"
        << BuildStringArrayJson(category_record.adopted_protocols_lexicographic)
        << ",\"property_count\":" << category_record.property_count
        << ",\"method_count\":" << category_record.method_count
        << ",\"line\":" << category_record.line
        << ",\"column\":" << category_record.column << "}";
    if (i + 1 !=
        runtime_metadata_source_records.categories_lexicographic.size()) {
      out << ",";
    }
    out << "\n";
  }
  out << "    ],\n"
      << "    \"properties\": [\n";
  for (std::size_t i = 0;
       i < runtime_metadata_source_records.properties_lexicographic.size();
       ++i) {
    const auto &property_record =
        runtime_metadata_source_records.properties_lexicographic[i];
    out << "      {\"owner_kind\":\""
        << EscapeJsonString(property_record.owner_kind)
        << "\",\"owner_name\":\"" << EscapeJsonString(property_record.owner_name)
        << "\",\"property_name\":\""
        << EscapeJsonString(property_record.property_name)
        << "\",\"type\":\"" << EscapeJsonString(property_record.type_name)
        << "\",\"effective_getter_selector\":\""
        << EscapeJsonString(property_record.effective_getter_selector)
        << "\",\"effective_setter_available\":"
        << (property_record.effective_setter_available ? "true" : "false")
        << ",\"effective_setter_selector\":\""
        << EscapeJsonString(property_record.effective_setter_selector)
        << "\",\"ivar_binding_symbol\":\""
        << EscapeJsonString(property_record.ivar_binding_symbol)
        << "\",\"executable_synthesized_binding_kind\":\""
        << EscapeJsonString(property_record.executable_synthesized_binding_kind)
        << "\",\"executable_synthesized_binding_symbol\":\""
        << EscapeJsonString(
               property_record.executable_synthesized_binding_symbol)
        << "\",\"property_attribute_profile\":\""
        << EscapeJsonString(property_record.property_attribute_profile)
        << "\",\"ownership_lifetime_profile\":\""
        << EscapeJsonString(property_record.ownership_lifetime_profile)
        << "\",\"ownership_runtime_hook_profile\":\""
        << EscapeJsonString(property_record.ownership_runtime_hook_profile)
        << "\",\"accessor_ownership_profile\":\""
        << EscapeJsonString(property_record.accessor_ownership_profile)
        << "\",\"synthesizes_executable_accessors\":"
        << (property_record.synthesizes_executable_accessors ? "true" : "false")
        << ",\"getter_storage_runtime_helper_symbol\":\""
        << EscapeJsonString(property_record.getter_storage_runtime_helper_symbol)
        << "\",\"setter_storage_runtime_helper_symbol\":\""
        << EscapeJsonString(property_record.setter_storage_runtime_helper_symbol)
        << "\",\"executable_ivar_layout_symbol\":\""
        << EscapeJsonString(property_record.executable_ivar_layout_symbol)
        << "\",\"executable_ivar_layout_slot_index\":"
        << property_record.executable_ivar_layout_slot_index
        << ",\"executable_ivar_layout_size_bytes\":"
        << property_record.executable_ivar_layout_size_bytes
        << ",\"executable_ivar_layout_alignment_bytes\":"
        << property_record.executable_ivar_layout_alignment_bytes
        << ",\"executable_ivar_layout_offset_bytes\":"
        << property_record.executable_ivar_layout_offset_bytes
        << ",\"executable_ivar_layout_padding_bytes\":"
        << property_record.executable_ivar_layout_padding_bytes
        << ",\"executable_ivar_layout_inherited_slot_count\":"
        << property_record.executable_ivar_layout_inherited_slot_count
        << ",\"executable_ivar_layout_inherited_size_bytes\":"
        << property_record.executable_ivar_layout_inherited_size_bytes
        << ",\"executable_ivar_layout_owner_size_bytes\":"
        << property_record.executable_ivar_layout_owner_size_bytes
        << ",\"executable_ivar_init_order_index\":"
        << property_record.executable_ivar_init_order_index
        << ",\"executable_ivar_destroy_order_index\":"
        << property_record.executable_ivar_destroy_order_index
        << ",\"executable_ivar_layout_valid\":"
        << (property_record.executable_ivar_layout_valid ? "true" : "false")
        << ",\"executable_ivar_layout_replay_key\":\""
        << EscapeJsonString(property_record.executable_ivar_layout_replay_key)
        << "\""
        << ",\"line\":" << property_record.line
        << ",\"column\":" << property_record.column << "}";
    if (i + 1 !=
        runtime_metadata_source_records.properties_lexicographic.size()) {
      out << ",";
    }
    out << "\n";
  }
  out << "    ],\n"
      << "    \"methods\": [\n";
  for (std::size_t i = 0;
       i < runtime_metadata_source_records.methods_lexicographic.size(); ++i) {
    const auto &method_record =
        runtime_metadata_source_records.methods_lexicographic[i];
    out << "      {\"owner_kind\":\""
        << EscapeJsonString(method_record.owner_kind)
        << "\",\"owner_name\":\"" << EscapeJsonString(method_record.owner_name)
        << "\",\"selector\":\"" << EscapeJsonString(method_record.selector)
        << "\",\"is_class_method\":"
        << (method_record.is_class_method ? "true" : "false")
        << ",\"has_body\":" << (method_record.has_body ? "true" : "false")
        << ",\"effective_direct_dispatch\":"
        << (method_record.effective_direct_dispatch ? "true" : "false")
        << ",\"objc_final_declared\":"
        << (method_record.objc_final_declared ? "true" : "false")
        << ",\"parameter_count\":" << method_record.parameter_count
        << ",\"return_type\":\""
        << EscapeJsonString(method_record.return_type_name)
        << "\",\"line\":" << method_record.line
        << ",\"column\":" << method_record.column << "}";
    if (i + 1 != runtime_metadata_source_records.methods_lexicographic.size()) {
      out << ",";
    }
    out << "\n";
  }
  out << "    ],\n"
      << "    \"ivars\": [\n";
  for (std::size_t i = 0;
       i < runtime_metadata_source_records.ivars_lexicographic.size(); ++i) {
    const auto &ivar_record =
        runtime_metadata_source_records.ivars_lexicographic[i];
    out << "      {\"owner_kind\":\""
        << EscapeJsonString(ivar_record.owner_kind)
        << "\",\"owner_name\":\"" << EscapeJsonString(ivar_record.owner_name)
        << "\",\"property_name\":\""
        << EscapeJsonString(ivar_record.property_name)
        << "\",\"ivar_binding_symbol\":\""
        << EscapeJsonString(ivar_record.ivar_binding_symbol)
        << "\",\"executable_synthesized_binding_kind\":\""
        << EscapeJsonString(ivar_record.executable_synthesized_binding_kind)
        << "\",\"executable_synthesized_binding_symbol\":\""
        << EscapeJsonString(ivar_record.executable_synthesized_binding_symbol)
        << "\",\"executable_ivar_layout_symbol\":\""
        << EscapeJsonString(ivar_record.executable_ivar_layout_symbol)
        << "\",\"executable_ivar_layout_slot_index\":"
        << ivar_record.executable_ivar_layout_slot_index
        << ",\"executable_ivar_layout_size_bytes\":"
        << ivar_record.executable_ivar_layout_size_bytes
        << ",\"executable_ivar_layout_alignment_bytes\":"
        << ivar_record.executable_ivar_layout_alignment_bytes
        << ",\"executable_ivar_layout_offset_bytes\":"
        << ivar_record.executable_ivar_layout_offset_bytes
        << ",\"executable_ivar_layout_padding_bytes\":"
        << ivar_record.executable_ivar_layout_padding_bytes
        << ",\"executable_ivar_layout_inherited_slot_count\":"
        << ivar_record.executable_ivar_layout_inherited_slot_count
        << ",\"executable_ivar_layout_inherited_size_bytes\":"
        << ivar_record.executable_ivar_layout_inherited_size_bytes
        << ",\"executable_ivar_layout_owner_size_bytes\":"
        << ivar_record.executable_ivar_layout_owner_size_bytes
        << ",\"executable_ivar_init_order_index\":"
        << ivar_record.executable_ivar_init_order_index
        << ",\"executable_ivar_destroy_order_index\":"
        << ivar_record.executable_ivar_destroy_order_index
        << ",\"executable_ivar_layout_valid\":"
        << (ivar_record.executable_ivar_layout_valid ? "true" : "false")
        << ",\"executable_ivar_layout_replay_key\":\""
        << EscapeJsonString(ivar_record.executable_ivar_layout_replay_key)
        << "\""
        << ",\"source_model\":\"" << EscapeJsonString(ivar_record.source_model)
        << "\",\"line\":" << ivar_record.line
        << ",\"column\":" << ivar_record.column << "}";
    if (i + 1 != runtime_metadata_source_records.ivars_lexicographic.size()) {
      out << ",";
    }
    out << "\n";
  }
  out << "    ]\n"
      << "  }";
  return out.str();
}

std::string RenderRuntimeMetadataReferencesJson(
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records) {
  std::ostringstream out;
  out << "[\n";
  bool first_reference = true;
  auto emit_reference = [&](const std::string &reference_kind,
                            const std::string &owner_kind,
                            const std::string &owner_name,
                            const std::string &target_kind,
                            const std::string &target_name, unsigned line,
                            unsigned column) {
    if (!first_reference) {
      out << ",\n";
    }
    first_reference = false;
    out << "    {\"reference_kind\":\"" << EscapeJsonString(reference_kind)
        << "\",\"owner_kind\":\"" << EscapeJsonString(owner_kind)
        << "\",\"owner_name\":\"" << EscapeJsonString(owner_name)
        << "\",\"target_kind\":\"" << EscapeJsonString(target_kind)
        << "\",\"target_name\":\"" << EscapeJsonString(target_name)
        << "\",\"line\":" << line << ",\"column\":" << column << "}";
  };
  for (const auto &class_record :
       runtime_metadata_source_records.classes_lexicographic) {
    if (class_record.has_super && !class_record.super_name.empty()) {
      emit_reference("class-superclass", class_record.record_kind,
                     class_record.name, "class", class_record.super_name,
                     class_record.line, class_record.column);
    }
    for (const auto &protocol_name :
         class_record.adopted_protocols_lexicographic) {
      emit_reference("class-adopted-protocol", class_record.record_kind,
                     class_record.name, "protocol", protocol_name,
                     class_record.line, class_record.column);
    }
  }
  for (const auto &protocol_record :
       runtime_metadata_source_records.protocols_lexicographic) {
    for (const auto &inherited_name :
         protocol_record.inherited_protocols_lexicographic) {
      emit_reference("protocol-inherited-protocol", "protocol",
                     protocol_record.name, "protocol", inherited_name,
                     protocol_record.line, protocol_record.column);
    }
  }
  for (const auto &category_record :
       runtime_metadata_source_records.categories_lexicographic) {
    for (const auto &protocol_name :
         category_record.adopted_protocols_lexicographic) {
      emit_reference("category-adopted-protocol", category_record.record_kind,
                     category_record.category_name, "protocol", protocol_name,
                     category_record.line, category_record.column);
    }
  }
  for (const auto &property_record :
       runtime_metadata_source_records.properties_lexicographic) {
    if (!property_record.effective_getter_selector.empty()) {
      emit_reference("property-getter-selector", property_record.owner_kind,
                     property_record.owner_name, "selector",
                     property_record.effective_getter_selector,
                     property_record.line, property_record.column);
    }
    if (property_record.effective_setter_available &&
        !property_record.effective_setter_selector.empty()) {
      emit_reference("property-setter-selector", property_record.owner_kind,
                     property_record.owner_name, "selector",
                     property_record.effective_setter_selector,
                     property_record.line, property_record.column);
    }
    if (!property_record.ivar_binding_symbol.empty()) {
      emit_reference("property-ivar-binding", property_record.owner_kind,
                     property_record.owner_name, "ivar-binding-symbol",
                     property_record.ivar_binding_symbol, property_record.line,
                     property_record.column);
    }
  }
  for (const auto &method_record :
       runtime_metadata_source_records.methods_lexicographic) {
    emit_reference("method-selector", method_record.owner_kind,
                   method_record.owner_name, "selector", method_record.selector,
                   method_record.line, method_record.column);
  }
  if (!first_reference) {
    out << "\n";
  }
  out << "  ]";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
