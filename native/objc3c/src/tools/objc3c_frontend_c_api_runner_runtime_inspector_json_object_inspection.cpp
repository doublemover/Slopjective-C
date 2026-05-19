#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_object_inspection.h"

#include <ostream>

#include "ast/objc3_ast.h"
#include "io/objc3_json.h"
#include "tools/objc3c_frontend_c_api_runner_commands.h"

using objc3::io::EscapeJsonString;

void WriteFrontendCApiRunnerRuntimeInspectorObjectInspectionJsonRows(
    std::ostream &out,
    const std::string &child_indent,
    const FrontendCApiRunnerArtifactPathView &paths) {
  out << child_indent << "\"section_inventory_row_key\": \""
      << EscapeJsonString(
             kObjc3RuntimeMetadataObjectInspectionSectionInventoryRowKey)
      << "\",\n";
  out << child_indent << "\"symbol_inventory_row_key\": \""
      << EscapeJsonString(
             kObjc3RuntimeMetadataObjectInspectionSymbolInventoryRowKey)
      << "\",\n";
  out << child_indent << "\"section_inventory_command\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerObjectInspectionCommand(
             kObjc3RuntimeMetadataObjectInspectionSectionCommand,
             paths.object))
      << "\",\n";
  out << child_indent << "\"symbol_inventory_command\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerObjectInspectionCommand(
             kObjc3RuntimeMetadataObjectInspectionSymbolCommand,
             paths.object))
      << "\",\n";
  out << child_indent
      << "\"realized_class_graph_snapshot_symbol\": "
         "\"objc3_runtime_copy_realized_class_graph_state_for_testing\",\n";
  out << child_indent
      << "\"realized_class_entry_snapshot_symbol\": "
         "\"objc3_runtime_copy_realized_class_entry_for_testing\",\n";
  out << child_indent
      << "\"runtime_instance_entry_snapshot_symbol\": "
         "\"objc3_runtime_copy_instance_entry_for_testing\",\n";
  out << child_indent
      << "\"property_registry_state_snapshot_symbol\": "
         "\"objc3_runtime_copy_property_registry_state_for_testing\",\n";
  out << child_indent
      << "\"property_entry_snapshot_symbol\": "
         "\"objc3_runtime_copy_property_entry_for_testing\",\n";
  out << child_indent
      << "\"storage_accessor_snapshot_symbol\": "
         "\"objc3_runtime_copy_storage_accessor_implementation_snapshot_for_testing\",\n";
  out << child_indent
      << "\"protocol_conformance_query_symbol\": "
         "\"objc3_runtime_copy_protocol_conformance_query_for_testing\",\n";
  out << child_indent
      << "\"object_model_query_state_snapshot_symbol\": "
         "\"objc3_runtime_copy_object_model_query_state_for_testing\",\n";
}
