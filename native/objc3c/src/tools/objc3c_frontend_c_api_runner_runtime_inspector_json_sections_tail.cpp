#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_sections_internal.h"

#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_availability.h"
#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_dump_commands.h"

void WriteFrontendCApiRunnerRuntimeInspectorDumpTailSections(
    std::ostream &out,
    const FrontendCApiRunnerRuntimeInspectorContext &context) {
  WriteFrontendCApiRunnerRuntimeInspectorDumpCommandJsonRows(
      out,
      context.child_indent,
      context.grandchild_indent,
      context.paths);
  WriteFrontendCApiRunnerRuntimeInspectorAvailabilityReasonJsonRows(
      out,
      context.child_indent,
      context.availability_reason);
}
