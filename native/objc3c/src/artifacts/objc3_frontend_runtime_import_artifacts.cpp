#include "artifacts/objc3_frontend_runtime_import_artifacts.h"

#include <algorithm>
#include <set>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <vector>

#include "io/json/json_writer.h"
#include "io/objc3_json.h"
#include "pipeline/results/evidence_record.h"
#include "sema/model/semantic_type.h"
#include "artifacts/objc3_runtime_import_preservation_artifact_builders.h"
#include "support/objc3_runtime_metadata_record_set.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;
using objc3::io::json::JsonObjectWriter;
using objc3c::support::CountRuntimeMetadataSourceRecordSetDeclarations;
using objc3c::support::CountRuntimeMetadataSourceRecordSetReferences;

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

std::string BuildRuntimeAwareImportModuleSurfaceReplayKey(
    const Objc3Program &program,
    const Objc3ParserContractSnapshot &parser_contract_snapshot,
    const Objc3ModuleImportGraphLoweringContract
        &module_import_graph_lowering_contract) {
  return runtime_import_preservation::
      RuntimeAwareImportModuleSurfaceArtifactBuilder::BuildReplayKey(
          program,
          parser_contract_snapshot,
          module_import_graph_lowering_contract);
}

std::string BuildRuntimeAwareImportModuleSurfaceSummaryJson(
    const Objc3Program &program,
    const Objc3ParserContractSnapshot &parser_contract_snapshot,
    const Objc3ModuleImportGraphLoweringContract
        &module_import_graph_lowering_contract) {
  return runtime_import_preservation::
      RuntimeAwareImportModuleSurfaceArtifactBuilder::BuildSummaryJson(
          program,
          parser_contract_snapshot,
          module_import_graph_lowering_contract);
}

std::string BuildRuntimeAwareImportModuleFrontendClosureReplayKey(
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary) {
  return runtime_import_preservation::
      RuntimeAwareImportModuleFrontendClosureArtifactBuilder::BuildReplayKey(
          summary);
}

Objc3RuntimeAwareImportModuleFrontendClosureSummary
BuildRuntimeAwareImportModuleFrontendClosureSummary(
    const Objc3Program &program,
    const Objc3ParserContractSnapshot &parser_contract_snapshot,
    const Objc3ModuleImportGraphLoweringContract
        &module_import_graph_lowering_contract,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records) {
  return runtime_import_preservation::
      RuntimeAwareImportModuleFrontendClosureArtifactBuilder::BuildSummary(
          program,
          parser_contract_snapshot,
          module_import_graph_lowering_contract,
          runtime_metadata_source_records);
}

std::string BuildRuntimeAwareImportModuleFrontendClosureSummaryJson(
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary) {
  std::ostringstream out;
  JsonObjectWriter object(out);
  object.StringField("contract_id", summary.contract_id);
  object.StringField("source_surface_contract_id",
                     summary.source_surface_contract_id);
  object.StringField("frontend_surface_path", summary.frontend_surface_path);
  object.StringField("payload_model", summary.payload_model);
  object.StringField("artifact_relative_path", summary.artifact_relative_path);
  object.StringField("authority_model", summary.authority_model);
  object.StringField("payload_ownership_model",
                     summary.payload_ownership_model);
  object.StringField("module_name", summary.module_name);
  object.SizeField("protocol_decl_count", summary.protocol_decl_count);
  object.SizeField("interface_decl_count", summary.interface_decl_count);
  object.SizeField("implementation_decl_count",
                   summary.implementation_decl_count);
  object.SizeField("interface_category_decl_count",
                   summary.interface_category_decl_count);
  object.SizeField("implementation_category_decl_count",
                   summary.implementation_category_decl_count);
  object.SizeField("function_decl_count", summary.function_decl_count);
  object.SizeField("module_import_graph_sites",
                   summary.module_import_graph_sites);
  object.SizeField("import_edge_candidate_sites",
                   summary.import_edge_candidate_sites);
  object.SizeField("namespace_segment_sites", summary.namespace_segment_sites);
  object.SizeField("object_pointer_type_sites",
                   summary.object_pointer_type_sites);
  object.SizeField("pointer_declarator_sites",
                   summary.pointer_declarator_sites);
  object.SizeField("normalized_sites", summary.normalized_sites);
  object.SizeField("contract_violation_sites",
                   summary.contract_violation_sites);
  object.SizeField("class_record_count", summary.class_record_count);
  object.SizeField("protocol_record_count", summary.protocol_record_count);
  object.SizeField("category_record_count", summary.category_record_count);
  object.SizeField("property_record_count", summary.property_record_count);
  object.SizeField("method_record_count", summary.method_record_count);
  object.SizeField("ivar_record_count", summary.ivar_record_count);
  object.SizeField("runtime_owned_declaration_count",
                   summary.runtime_owned_declaration_count);
  object.SizeField("superclass_reference_count",
                   summary.superclass_reference_count);
  object.SizeField("protocol_reference_count",
                   summary.protocol_reference_count);
  object.SizeField("property_accessor_reference_count",
                   summary.property_accessor_reference_count);
  object.SizeField("property_ivar_binding_reference_count",
                   summary.property_ivar_binding_reference_count);
  object.SizeField("method_selector_reference_count",
                   summary.method_selector_reference_count);
  object.SizeField("metadata_reference_count", summary.metadata_reference_count);
  object.BoolField("ready",
                   IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(
                       summary));
  object.BoolField("fail_closed", summary.fail_closed);
  object.BoolField("source_surface_contract_ready",
                   summary.source_surface_contract_ready);
  object.BoolField("runtime_metadata_source_records_ready",
                   summary.runtime_metadata_source_records_ready);
  object.BoolField("frontend_surface_published",
                   summary.frontend_surface_published);
  object.BoolField("import_artifact_template_published",
                   summary.import_artifact_template_published);
  object.BoolField("runtime_aware_import_declarations_landed",
                   summary.runtime_aware_import_declarations_landed);
  object.BoolField("module_metadata_import_surface_landed",
                   summary.module_metadata_import_surface_landed);
  object.BoolField("runtime_owned_declaration_import_landed",
                   summary.runtime_owned_declaration_import_landed);
  object.BoolField("runtime_metadata_reference_import_landed",
                   summary.runtime_metadata_reference_import_landed);
  object.BoolField("public_frontend_api_module_surface_landed",
                   summary.public_frontend_api_module_surface_landed);
  object.BoolField("ready_for_import_artifact_emission",
                   summary.ready_for_import_artifact_emission);
  object.BoolField("ready_for_frontend_module_consumption",
                   summary.ready_for_frontend_module_consumption);
  object.StringField("source_surface_replay_key",
                     summary.source_surface_replay_key);
  object.StringField("replay_key", summary.replay_key);
  object.StringField("failure_reason", summary.failure_reason);
  object.End();
  return out.str();
}

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

std::string RenderSerializedRuntimeMetadataReusePayloadJson(
    const Objc3SerializedRuntimeMetadataArtifactReuseSummary &summary,
    const std::string &module_name,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records) {
  std::ostringstream out;
  out << "{\n"
      << "    \"contract_id\": \"" << EscapeJsonString(summary.contract_id)
      << "\",\n"
      << "    \"module_name\": \"" << EscapeJsonString(module_name) << "\",\n"
      << "    \"reused_module_names_lexicographic\": "
      << BuildStringArrayJson(summary.reused_module_names_lexicographic)
      << ",\n"
      << "    \"runtime_owned_declaration_count\": "
      << summary.runtime_owned_declaration_count << ",\n"
      << "    \"metadata_reference_count\": " << summary.metadata_reference_count
      << ",\n"
      << "    \"runtime_owned_declarations\": "
      << RenderRuntimeOwnedDeclarationsJson(runtime_metadata_source_records)
      << ",\n"
      << "    \"metadata_references\": "
      << RenderRuntimeMetadataReferencesJson(runtime_metadata_source_records)
      << ",\n"
      << "    \"ready\": "
      << (IsReadyObjc3SerializedRuntimeMetadataArtifactReuseSummary(summary)
              ? "true"
              : "false")
      << ",\n"
      << "    \"replay_key\": \"" << EscapeJsonString(summary.replay_key)
      << "\"\n"
      << "  }";
  return out.str();
}

std::string RenderRuntimeAwareImportModuleArtifactJson(
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records,
    const std::string &type_system_optional_keypath_lowering_contract_json,
    const std::string &type_system_optional_keypath_runtime_helper_contract_json,
    const std::string &type_system_generic_contract_preservation_json,
    const std::string &type_system_nullability_contract_preservation_json,
    const std::string &type_system_protocol_contract_preservation_json,
    const std::string &error_handling_result_and_bridging_artifact_replay_json,
    const std::string &concurrency_actor_mailbox_runtime_import_json,
    const std::string &interop_foreign_surface_interface_preservation_json,
    const std::string &interop_header_module_bridge_generation_json,
    const std::string &interop_ffi_metadata_interface_preservation_json,
    const std::string &metaprogramming_module_interface_replay_preservation_json,
    const std::string
        &metaprogramming_macro_host_process_cache_runtime_integration_json,
    const std::string &dispatch_dispatch_metadata_interface_preservation_json,
    const std::string &runtime_block_ownership_artifact_preservation_json,
    const std::string &runtime_storage_reflection_artifact_preservation_json,
    const Objc3SerializedRuntimeMetadataArtifactReuseSummary
        &serialized_runtime_metadata_artifact_reuse,
    const Objc3RuntimeMetadataSourceRecordSet
        &serialized_runtime_metadata_reuse_records) {
  std::ostringstream out;
  out << "{\n"
      << "  \"contract_id\": \"" << EscapeJsonString(summary.contract_id)
      << "\",\n"
      << "  \"source_surface_contract_id\": \""
      << EscapeJsonString(summary.source_surface_contract_id) << "\",\n"
      << "  \"frontend_surface_path\": \""
      << EscapeJsonString(summary.frontend_surface_path) << "\",\n"
      << "  \"artifact\": \"" << EscapeJsonString(summary.artifact_relative_path)
      << "\",\n"
      << "  \"payload_model\": \"" << EscapeJsonString(summary.payload_model)
      << "\",\n"
      << "  \"authority_model\": \"" << EscapeJsonString(summary.authority_model)
      << "\",\n"
      << "  \"payload_ownership_model\": \""
      << EscapeJsonString(summary.payload_ownership_model) << "\",\n"
      << "  \"module_name\": \"" << EscapeJsonString(summary.module_name)
      << "\",\n"
      << "  \"protocol_decl_count\": " << summary.protocol_decl_count << ",\n"
      << "  \"interface_decl_count\": " << summary.interface_decl_count
      << ",\n"
      << "  \"implementation_decl_count\": " << summary.implementation_decl_count
      << ",\n"
      << "  \"interface_category_decl_count\": "
      << summary.interface_category_decl_count << ",\n"
      << "  \"implementation_category_decl_count\": "
      << summary.implementation_category_decl_count << ",\n"
      << "  \"function_decl_count\": " << summary.function_decl_count << ",\n"
      << "  \"module_import_graph_sites\": " << summary.module_import_graph_sites
      << ",\n"
      << "  \"import_edge_candidate_sites\": "
      << summary.import_edge_candidate_sites << ",\n"
      << "  \"namespace_segment_sites\": " << summary.namespace_segment_sites
      << ",\n"
      << "  \"object_pointer_type_sites\": " << summary.object_pointer_type_sites
      << ",\n"
      << "  \"pointer_declarator_sites\": " << summary.pointer_declarator_sites
      << ",\n"
      << "  \"normalized_sites\": " << summary.normalized_sites << ",\n"
      << "  \"contract_violation_sites\": " << summary.contract_violation_sites
      << ",\n"
      << "  \"runtime_owned_declaration_count\": "
      << summary.runtime_owned_declaration_count << ",\n"
      << "  \"metadata_reference_count\": " << summary.metadata_reference_count
      << ",\n"
      << "  \"runtime_aware_import_declarations_landed\": "
      << (summary.runtime_aware_import_declarations_landed ? "true" : "false")
      << ",\n"
      << "  \"module_metadata_import_surface_landed\": "
      << (summary.module_metadata_import_surface_landed ? "true" : "false")
      << ",\n"
      << "  \"runtime_owned_declaration_import_landed\": "
      << (summary.runtime_owned_declaration_import_landed ? "true" : "false")
      << ",\n"
      << "  \"runtime_metadata_reference_import_landed\": "
      << (summary.runtime_metadata_reference_import_landed ? "true" : "false")
      << ",\n"
      << "  \"public_frontend_api_module_surface_landed\": "
      << (summary.public_frontend_api_module_surface_landed ? "true" : "false")
      << ",\n"
      << "  \"ready_for_import_artifact_emission\": "
      << (summary.ready_for_import_artifact_emission ? "true" : "false")
      << ",\n"
      << "  \"ready_for_frontend_module_consumption\": "
      << (summary.ready_for_frontend_module_consumption ? "true" : "false")
      << ",\n"
      << "  \"runtime_owned_declarations\": "
      << RenderRuntimeOwnedDeclarationsJson(runtime_metadata_source_records)
      << ",\n"
      << "  \"metadata_references\": "
      << RenderRuntimeMetadataReferencesJson(runtime_metadata_source_records)
      << ",\n"
      << "  \"objc_type_system_optional_keypath_lowering_contract\": "
      << type_system_optional_keypath_lowering_contract_json << ",\n"
      << "  \"objc_type_system_optional_keypath_runtime_helper_contract\": "
      << type_system_optional_keypath_runtime_helper_contract_json << ",\n"
      << "  \"objc_type_system_generic_contract_preservation\": "
      << type_system_generic_contract_preservation_json << ",\n"
      << "  \"objc_type_system_nullability_contract_preservation\": "
      << type_system_nullability_contract_preservation_json << ",\n"
      << "  \"objc_type_system_protocol_contract_preservation\": "
      << type_system_protocol_contract_preservation_json << ",\n"
      << "  \"objc_error_handling_result_and_bridging_artifact_replay\": "
      << error_handling_result_and_bridging_artifact_replay_json << ",\n"
      << "  \"objc_concurrency_actor_mailbox_and_isolation_runtime_import_surface\": "
      << concurrency_actor_mailbox_runtime_import_json << ",\n"
      << "  \"objc_interop_foreign_surface_interface_and_module_preservation\": "
      << interop_foreign_surface_interface_preservation_json << ",\n"
      << "  \"objc_interop_header_module_and_bridge_generation\": "
      << interop_header_module_bridge_generation_json << ",\n"
      << "  \"objc_interop_ffi_metadata_and_interface_preservation\": "
      << interop_ffi_metadata_interface_preservation_json << ",\n"
      << "  \"objc_metaprogramming_module_interface_and_replay_preservation\": "
      << metaprogramming_module_interface_replay_preservation_json << ",\n"
      << "  \"objc_metaprogramming_macro_host_process_and_cache_runtime_integration\": "
      << metaprogramming_macro_host_process_cache_runtime_integration_json
      << ",\n"
      << "  \"objc_dispatch_dispatch_metadata_and_interface_preservation\": "
      << dispatch_dispatch_metadata_interface_preservation_json << ",\n"
      << "  \"objc_runtime_block_ownership_artifact_preservation\": "
      << runtime_block_ownership_artifact_preservation_json << ",\n"
      << "  \"objc_runtime_storage_reflection_artifact_preservation\": "
      << runtime_storage_reflection_artifact_preservation_json << ",\n"
      << "  \""
      << kObjc3SerializedRuntimeMetadataArtifactReusePayloadMemberName
      << "\": "
      << RenderSerializedRuntimeMetadataReusePayloadJson(
             serialized_runtime_metadata_artifact_reuse, summary.module_name,
             serialized_runtime_metadata_reuse_records)
      << ",\n"
      << "  \"source_surface_replay_key\": \""
      << EscapeJsonString(summary.source_surface_replay_key) << "\",\n"
      << "  \"replay_key\": \"" << EscapeJsonString(summary.replay_key)
      << "\"\n"
      << "}\n";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
