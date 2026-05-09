#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json.h"

#include <sstream>

#include "ast/objc3_ast.h"
#include "io/objc3_json.h"
#include "tools/objc3c_frontend_c_api_runner_commands.h"
#include "tools/objc3c_frontend_c_api_runner_result.h"

using objc3::io::EscapeJsonString;

namespace {

constexpr const char *kObjc3RuntimeArcDebugStateSnapshotSymbol =
    "objc3_runtime_copy_arc_debug_state_for_testing";

}  // namespace

void WriteFrontendCApiRunnerRuntimeInspectorJson(
    std::ostream &out,
    const std::string &indent,
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result) {
  const std::string object_path_text =
      FrontendCApiResultArtifactPath(result, OBJC3C_FRONTEND_ARTIFACT_OBJECT);
  const std::string child_indent = indent + "  ";
  const std::string grandchild_indent = child_indent + "  ";
  const bool available = FrontendCApiRunnerPathExists(object_path_text);
  const std::string availability_reason =
      available ? std::string() : "object artifact missing or not emitted";
  out << "{\n";
  out << child_indent << "\"contract_id\": \""
      << kObjc3RuntimeMetadataObjectInspectionContractId << "\",\n";
  out << child_indent << "\"publication_contract_id\": \""
      << kObjc3RuntimeMetadataSectionPublicationContractId << "\",\n";
  out << child_indent << "\"available\": " << (available ? "true" : "false")
      << ",\n";
  out << child_indent << "\"active_emit_prefix\": \""
      << EscapeJsonString(options.emit_prefix) << "\",\n";
  out << child_indent << "\"fixture_path\": \""
      << EscapeJsonString(kObjc3RuntimeMetadataObjectInspectionFixturePath)
      << "\",\n";
  out << child_indent << "\"object_path\": \""
      << EscapeJsonString(object_path_text) << "\",\n";
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
             object_path_text))
      << "\",\n";
  out << child_indent << "\"symbol_inventory_command\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerObjectInspectionCommand(
             kObjc3RuntimeMetadataObjectInspectionSymbolCommand,
             object_path_text))
      << "\",\n";
  out << child_indent << "\"arc_debug_state_snapshot_symbol\": \""
      << kObjc3RuntimeArcDebugStateSnapshotSymbol << "\",\n";
  out << child_indent << "\"runtime_abi_boundary_model\": \""
      << EscapeJsonString(kObjc3RuntimeBlockArcRuntimeAbiBoundaryModel)
      << "\",\n";
  out << child_indent << "\"block_runtime_model\": \""
      << EscapeJsonString(kObjc3RuntimeBlockArcRuntimeAbiBlockModel)
      << "\",\n";
  out << child_indent << "\"arc_runtime_model\": \""
      << EscapeJsonString(kObjc3RuntimeBlockArcRuntimeAbiArcModel) << "\",\n";
  out << child_indent << "\"fail_closed_model\": \""
      << EscapeJsonString(kObjc3RuntimeBlockArcRuntimeAbiFailClosedModel)
      << "\",\n";
  out << child_indent << "\"dump_commands\": {\n";
  out << grandchild_indent << "\"object_sections\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerObjectInspectionCommand(
             kObjc3RuntimeMetadataObjectInspectionSectionCommand,
             object_path_text))
      << "\",\n";
  out << grandchild_indent << "\"object_symbols\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerObjectInspectionCommand(
             kObjc3RuntimeMetadataObjectInspectionSymbolCommand,
             object_path_text))
      << "\"\n";
  out << child_indent << "},\n";
  out << child_indent << "\"availability_reason\": \""
      << EscapeJsonString(availability_reason) << "\"\n";
  out << indent << "}";
}

std::string BuildFrontendCApiRunnerRuntimeInspectorJson(
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result) {
  std::ostringstream dump;
  WriteFrontendCApiRunnerRuntimeInspectorJson(dump, "", options, result);
  dump << "\n";
  return dump.str();
}
