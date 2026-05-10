#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json_playground_rows.h"

#include <ostream>

void WriteFrontendCApiRunnerBonusExperiencesPlaygroundArrayRows(
    std::ostream &out,
    const std::string &grandchild_indent) {
  out << grandchild_indent << "\"artifact_roots\": [\n";
  out << grandchild_indent << "  \"tmp/artifacts/playground\",\n";
  out << grandchild_indent << "  \"tmp/reports/playground\",\n";
  out << grandchild_indent << "  \"tmp/artifacts/showcase\"\n";
  out << grandchild_indent << "],\n";
  out << grandchild_indent << "\"public_actions\": [\n";
  out << grandchild_indent << "  \"materialize-playground-workspace\",\n";
  out << grandchild_indent << "  \"compile-objc3c\",\n";
  out << grandchild_indent << "  \"inspect-playground-repro\",\n";
  out << grandchild_indent << "  \"inspect-compile-observability\",\n";
  out << grandchild_indent << "  \"trace-compile-stages\"\n";
  out << grandchild_indent << "],\n";
}
