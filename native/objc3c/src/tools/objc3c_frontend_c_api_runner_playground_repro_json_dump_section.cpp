#include "tools/objc3c_frontend_c_api_runner_playground_repro_json_sections.h"

#include "tools/objc3c_frontend_c_api_runner_playground_repro_json_dump_commands.h"

void WriteFrontendCApiRunnerPlaygroundReproDumpCommandSection(
    std::ostream &out,
    const FrontendCApiRunnerPlaygroundReproContext &context,
    const FrontendCApiRunnerOptions &options) {
  WriteFrontendCApiRunnerPlaygroundReproDumpCommandJsonRows(
      out,
      context.child_indent,
      context.grandchild_indent,
      options,
      context.paths);
}
