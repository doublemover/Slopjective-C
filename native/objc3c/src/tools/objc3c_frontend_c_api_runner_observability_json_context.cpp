#include "tools/objc3c_frontend_c_api_runner_observability_json_context.h"

FrontendCApiRunnerObservabilityContext
BuildFrontendCApiRunnerObservabilityContext(
    const std::string &indent,
    const FrontendCApiRunnerObservabilityPublication &publication) {
  FrontendCApiRunnerObservabilityContext context;
  context.publication = publication;
  context.child_indent = indent + "  ";
  context.grandchild_indent = context.child_indent + "  ";
  return context;
}
