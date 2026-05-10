#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json_runtime_inspector.h"

#include <ostream>

#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json_runtime_inspector_rows.h"

void WriteFrontendCApiRunnerBonusExperiencesRuntimeInspectorJsonRows(
    std::ostream &out,
    const std::string &child_indent,
    const std::string &grandchild_indent,
    const FrontendCApiRunnerArtifactPathView &paths,
    bool runtime_inspector_ready) {
  out << child_indent
      << "\"runtime_inspector_and_capability_explorer\": {\n";
  WriteFrontendCApiRunnerBonusRuntimeInspectorAvailabilityRows(
      out, grandchild_indent, runtime_inspector_ready);
  WriteFrontendCApiRunnerBonusRuntimeInspectorSectionFields(out,
                                                           grandchild_indent,
                                                           paths);
  WriteFrontendCApiRunnerBonusRuntimeInspectorArtifactCommandRows(
      out, grandchild_indent, paths);
  out << child_indent << "},\n";
}
