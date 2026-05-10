#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_sections.h"

#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_availability.h"
#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_contract.h"
#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_dump_commands.h"
#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_object_inspection.h"
#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_runtime_abi.h"

void WriteFrontendCApiRunnerRuntimeInspectorSections(
    std::ostream &out,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerRuntimeInspectorContext &context) {
  WriteFrontendCApiRunnerRuntimeInspectorContractAvailabilityJsonRows(
      out,
      context.child_indent,
      options,
      context.paths,
      context.available);
  WriteFrontendCApiRunnerRuntimeInspectorObjectInspectionJsonRows(
      out,
      context.child_indent,
      context.paths);
  WriteFrontendCApiRunnerRuntimeInspectorRuntimeAbiJsonRows(
      out,
      context.child_indent);
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
