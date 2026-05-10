#include "tools/objc3c_frontend_c_api_runner_playground_repro_json_artifacts.h"

#include <ostream>

#include "io/objc3_json.h"

using objc3::io::EscapeJsonString;

void WriteFrontendCApiRunnerPlaygroundReproArtifactPathJsonRows(
    std::ostream &out,
    const std::string &child_indent,
    const std::string &grandchild_indent,
    const FrontendCApiRunnerArtifactPathView &paths) {
  out << child_indent << "\"artifact_paths\": {\n";
  out << grandchild_indent << "\"diagnostics\": \""
      << EscapeJsonString(paths.diagnostics) << "\",\n";
  out << grandchild_indent << "\"manifest\": \""
      << EscapeJsonString(paths.manifest) << "\",\n";
  out << grandchild_indent << "\"ir\": \"" << EscapeJsonString(paths.ir)
      << "\",\n";
  out << grandchild_indent << "\"object\": \""
      << EscapeJsonString(paths.object) << "\"\n";
  out << child_indent << "},\n";
}
