#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_context.h"

#include "tools/objc3c_frontend_c_api_runner_read_command.h"

FrontendCApiRunnerRuntimeInspectorContext
BuildFrontendCApiRunnerRuntimeInspectorContext(
    const std::string &indent,
    const objc3c_frontend_c_compile_result_t &result) {
  FrontendCApiRunnerRuntimeInspectorContext context;
  context.paths = BuildFrontendCApiRunnerArtifactPathView(result,
                                                          std::string());
  context.child_indent = indent + "  ";
  context.grandchild_indent = context.child_indent + "  ";
  context.available = FrontendCApiRunnerPathExists(context.paths.object);
  context.availability_reason =
      context.available ? std::string()
                        : "object artifact missing or not emitted";
  return context;
}
