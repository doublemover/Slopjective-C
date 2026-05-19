#include "tools/objc3c_frontend_c_api_runner_playground_repro_json_sections.h"

#include "tools/objc3c_frontend_c_api_runner_playground_repro_json_public_surfaces.h"

void WriteFrontendCApiRunnerPlaygroundReproPublicSurfaceSection(
    std::ostream &out,
    const FrontendCApiRunnerPlaygroundReproContext &context) {
  WriteFrontendCApiRunnerPlaygroundReproPublicSurfaceJsonRows(
      out,
      context.child_indent);
}
