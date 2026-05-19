#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_sections.h"

#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_sections_internal.h"

void WriteFrontendCApiRunnerRuntimeInspectorSections(
    std::ostream &out,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerRuntimeInspectorContext &context) {
  WriteFrontendCApiRunnerRuntimeInspectorContractAvailabilitySection(
      out,
      options,
      context);
  WriteFrontendCApiRunnerRuntimeInspectorObjectAbiSections(out, context);
  WriteFrontendCApiRunnerRuntimeInspectorDumpTailSections(out, context);
}
