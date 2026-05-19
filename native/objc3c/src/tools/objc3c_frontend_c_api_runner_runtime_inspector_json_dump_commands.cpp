#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_dump_commands.h"

#include <ostream>

#include "ast/objc3_ast.h"
#include "io/objc3_json.h"
#include "tools/objc3c_frontend_c_api_runner_commands.h"

using objc3::io::EscapeJsonString;

void WriteFrontendCApiRunnerRuntimeInspectorDumpCommandJsonRows(
    std::ostream &out,
    const std::string &child_indent,
    const std::string &grandchild_indent,
    const FrontendCApiRunnerArtifactPathView &paths) {
  out << child_indent << "\"dump_commands\": {\n";
  out << grandchild_indent << "\"object_sections\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerObjectInspectionCommand(
             kObjc3RuntimeMetadataObjectInspectionSectionCommand,
             paths.object))
      << "\",\n";
  out << grandchild_indent << "\"object_symbols\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerObjectInspectionCommand(
             kObjc3RuntimeMetadataObjectInspectionSymbolCommand,
             paths.object))
      << "\"\n";
  out << child_indent << "},\n";
}
