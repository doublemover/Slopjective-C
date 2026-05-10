#include "tools/objc3c_frontend_c_api_runner_observability_json_writer.h"

FrontendCApiRunnerObservabilityContext
BuildFrontendCApiRunnerObservabilityWriterContext(
    const std::string &indent,
    const FrontendCApiRunnerObservabilityPublication &publication) {
  return BuildFrontendCApiRunnerObservabilityContext(
      indent,
      publication);
}
