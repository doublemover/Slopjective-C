#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json_runtime_inspector.h"

#include <ostream>

#include "io/objc3_json.h"
#include "tools/objc3c_frontend_c_api_runner_commands.h"

using objc3::io::EscapeJsonString;

void WriteFrontendCApiRunnerBonusExperiencesRuntimeInspectorJsonRows(
    std::ostream &out,
    const std::string &child_indent,
    const std::string &grandchild_indent,
    const FrontendCApiRunnerArtifactPathView &paths,
    bool runtime_inspector_ready) {
  out << child_indent
      << "\"runtime_inspector_and_capability_explorer\": {\n";
  out << grandchild_indent << "\"available\": "
      << (runtime_inspector_ready ? "true" : "false") << ",\n";
  out << grandchild_indent << "\"object_path\": \""
      << EscapeJsonString(paths.object) << "\",\n";
  out << grandchild_indent << "\"runtime_metadata_binary_path\": \""
      << EscapeJsonString(paths.runtime_metadata_binary) << "\",\n";
  out << grandchild_indent << "\"public_actions\": [\n";
  out << grandchild_indent << "  \"inspect-runtime-inspector\",\n";
  out << grandchild_indent << "  \"inspect-capability-explorer\",\n";
  out << grandchild_indent << "  \"benchmark-runtime-inspector\",\n";
  out << grandchild_indent << "  \"trace-compile-stages\",\n";
  out << grandchild_indent << "  \"validate-developer-tooling\"\n";
  out << grandchild_indent << "],\n";
  out << grandchild_indent << "\"capability_probe_action\": "
      << "\"npm run objc3c -- inspect-capability-explorer\",\n";
  out << grandchild_indent << "\"capability_summary_report_path\": "
      << "\"tmp/reports/objc3c-public-workflow/capability-explorer.json\",\n";
  out << grandchild_indent << "\"dump_commands\": {\n";
  out << grandchild_indent << "  \"ir\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerReadCommand(paths.ir))
      << "\",\n";
  out << grandchild_indent << "  \"object\": \""
      << EscapeJsonString(BuildFrontendCApiRunnerReadCommand(paths.object))
      << "\"\n";
  out << grandchild_indent << "}\n";
  out << child_indent << "},\n";
}
