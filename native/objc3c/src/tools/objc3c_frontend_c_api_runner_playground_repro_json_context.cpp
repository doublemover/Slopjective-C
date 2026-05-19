#include "tools/objc3c_frontend_c_api_runner_playground_repro_json_context.h"

FrontendCApiRunnerPlaygroundReproContext
BuildFrontendCApiRunnerPlaygroundReproContext(
    const std::string &indent,
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &summary_path_text) {
  FrontendCApiRunnerPlaygroundReproContext context;
  context.paths =
      BuildFrontendCApiRunnerArtifactPathView(result, summary_path_text);
  context.child_indent = indent + "  ";
  context.grandchild_indent = context.child_indent + "  ";
  return context;
}
