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
}
