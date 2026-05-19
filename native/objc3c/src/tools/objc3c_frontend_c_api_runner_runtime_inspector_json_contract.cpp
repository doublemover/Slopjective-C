#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_contract.h"

#include <ostream>

#include "ast/objc3_ast.h"
#include "io/objc3_json.h"

using objc3::io::EscapeJsonString;

void WriteFrontendCApiRunnerRuntimeInspectorContractAvailabilityJsonRows(
    std::ostream &out,
    const std::string &child_indent,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerArtifactPathView &paths,
    bool available) {
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
      << EscapeJsonString(paths.object) << "\",\n";
}
