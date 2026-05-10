#include "tools/objc3c_frontend_c_api_runner_observability_json.h"

#include <sstream>

#include "tools/objc3c_frontend_c_api_runner_observability_json_writer.h"

void WriteFrontendCApiRunnerObservabilityJson(
    std::ostream &out,
    const std::string &indent,
    const FrontendCApiRunnerObservabilityPublication &publication) {
  const FrontendCApiRunnerObservabilityContext context =
      BuildFrontendCApiRunnerObservabilityWriterContext(
          indent,
          publication);
  out << "{\n";
  WriteFrontendCApiRunnerObservabilityJsonSectionRows(out, context);
  out << indent << "}";
}

std::string BuildFrontendCApiRunnerObservabilityJson(
    const FrontendCApiRunnerObservabilityPublication &publication) {
  std::ostringstream dump;
  WriteFrontendCApiRunnerObservabilityJson(
      dump,
      "",
      publication);
  dump << "\n";
  return dump.str();
}
