#include "tools/objc3c_frontend_c_api_runner_observability_json_dump_commands.h"

#include <ostream>

#include "io/objc3_json.h"
#include "tools/objc3c_frontend_c_api_runner_commands.h"

using objc3::io::EscapeJsonString;

void WriteFrontendCApiRunnerObservabilityDumpCommandJsonRows(
    std::ostream &out,
    const std::string &child_indent,
    const std::string &grandchild_indent,
    const FrontendCApiRunnerArtifactPathView &paths) {
  out << child_indent << "\"dump_commands\": {\n";
  out << grandchild_indent << "\"summary\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerReadCommand(paths.summary))
      << "\",\n";
  out << grandchild_indent << "\"diagnostics\": \""
      << EscapeJsonString(
             BuildFrontendCApiRunnerReadCommand(paths.diagnostics))
      << "\",\n";
  out << grandchild_indent << "\"manifest\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerReadCommand(paths.manifest))
      << "\",\n";
  out << grandchild_indent << "\"ir\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerReadCommand(paths.ir))
      << "\",\n";
  out << grandchild_indent << "\"object\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerReadCommand(paths.object))
      << "\"\n";
  out << child_indent << "}\n";
}
