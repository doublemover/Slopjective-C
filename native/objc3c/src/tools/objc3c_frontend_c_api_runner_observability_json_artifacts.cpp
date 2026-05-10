#include "tools/objc3c_frontend_c_api_runner_observability_json_artifacts.h"

#include <ostream>

void WriteFrontendCApiRunnerObservabilityArtifactPresenceJsonRows(
    std::ostream &out,
    const std::string &child_indent,
    const std::string &grandchild_indent,
    const FrontendCApiRunnerArtifactPathView &paths) {
  out << child_indent << "\"artifact_presence\": {\n";
  out << grandchild_indent << "\"summary\": true,\n";
  out << grandchild_indent << "\"diagnostics\": "
      << (FrontendCApiRunnerPathExists(paths.diagnostics) ? "true"
                                                          : "false")
      << ",\n";
  out << grandchild_indent << "\"manifest\": "
      << (FrontendCApiRunnerPathExists(paths.manifest) ? "true" : "false")
      << ",\n";
  out << grandchild_indent << "\"ir\": "
      << (FrontendCApiRunnerPathExists(paths.ir) ? "true" : "false")
      << ",\n";
  out << grandchild_indent << "\"object\": "
      << (FrontendCApiRunnerPathExists(paths.object) ? "true" : "false")
      << ",\n";
  out << grandchild_indent << "\"runtime_metadata_binary\": "
      << (!paths.runtime_metadata_binary.empty() ? "true" : "false")
      << "\n";
  out << child_indent << "},\n";
}
