#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_sections_internal.h"

#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_contract.h"

void WriteFrontendCApiRunnerRuntimeInspectorContractAvailabilitySection(
    std::ostream &out,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerRuntimeInspectorContext &context) {
  WriteFrontendCApiRunnerRuntimeInspectorContractAvailabilityJsonRows(
      out,
      context.child_indent,
      options,
      context.paths,
      context.available);
}
