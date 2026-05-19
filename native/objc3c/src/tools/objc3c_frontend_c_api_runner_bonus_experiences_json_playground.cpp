#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json_playground.h"

#include <ostream>

#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json_playground_rows.h"

void WriteFrontendCApiRunnerBonusExperiencesPlaygroundJsonRows(
    std::ostream &out,
    const std::string &child_indent,
    const std::string &grandchild_indent,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerArtifactPathView &paths,
    bool compile_surface_ready) {
  out << child_indent << "\"playground\": {\n";
  WriteFrontendCApiRunnerBonusExperiencesPlaygroundAvailabilitySourceRows(
      out,
      grandchild_indent,
      options,
      paths,
      compile_surface_ready);
  WriteFrontendCApiRunnerBonusExperiencesPlaygroundArrayRows(
      out,
      grandchild_indent);
  WriteFrontendCApiRunnerBonusExperiencesPlaygroundDumpCommandRows(
      out,
      grandchild_indent,
      options,
      paths);
  out << child_indent << "},\n";
}
