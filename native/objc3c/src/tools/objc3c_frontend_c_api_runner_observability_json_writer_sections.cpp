#include "tools/objc3c_frontend_c_api_runner_observability_json_writer.h"

#include "tools/objc3c_frontend_c_api_runner_observability_json_sections.h"

void WriteFrontendCApiRunnerObservabilityJsonSectionRows(
    std::ostream &out,
    objc3c_frontend_c_status_t status,
    const FrontendCApiRunnerObservabilityContext &context) {
  WriteFrontendCApiRunnerObservabilitySections(out, status, context);
}
