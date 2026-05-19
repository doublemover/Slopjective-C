#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json_runtime_inspector_rows.h"

#include <ostream>

#include "io/objc3_json.h"
#include "tools/objc3c_frontend_c_api_runner_commands.h"

using objc3::io::EscapeJsonString;

void WriteFrontendCApiRunnerBonusRuntimeInspectorArtifactCommandRows(
    std::ostream &out,
    const std::string &grandchild_indent,
    const FrontendCApiRunnerArtifactPathView &paths) {
  out << grandchild_indent << "\"dump_commands\": {\n";
  out << grandchild_indent << "  \"ir\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerReadCommand(paths.ir))
      << "\",\n";
  out << grandchild_indent << "  \"object\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerReadCommand(paths.object))
      << "\"\n";
  out << grandchild_indent << "}\n";
}
