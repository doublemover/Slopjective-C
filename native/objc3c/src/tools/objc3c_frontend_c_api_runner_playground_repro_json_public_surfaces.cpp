#include "tools/objc3c_frontend_c_api_runner_playground_repro_json_public_surfaces.h"

#include <ostream>

void WriteFrontendCApiRunnerPlaygroundReproPublicSurfaceJsonRows(
    std::ostream &out,
    const std::string &child_indent) {
  out << child_indent << "\"public_actions\": [\n";
  out << child_indent << "  \"materialize-playground-workspace\",\n";
  out << child_indent << "  \"compile-objc3c\",\n";
  out << child_indent << "  \"inspect-playground-repro\",\n";
  out << child_indent << "  \"inspect-compile-observability\",\n";
  out << child_indent << "  \"trace-compile-stages\"\n";
  out << child_indent << "],\n";
  out << child_indent << "\"showcase_examples\": [\n";
  out << child_indent << "  \"showcase/auroraBoard/main.objc3\",\n";
  out << child_indent << "  \"showcase/signalMesh/main.objc3\",\n";
  out << child_indent << "  \"showcase/patchKit/main.objc3\",\n";
  out << child_indent << "  \"tests/tooling/fixtures/native/hello.objc3\"\n";
  out << child_indent << "],\n";
}
