#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_sections_internal.h"

#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_object_inspection.h"
#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_runtime_abi.h"

void WriteFrontendCApiRunnerRuntimeInspectorObjectAbiSections(
    std::ostream &out,
    const FrontendCApiRunnerRuntimeInspectorContext &context) {
  WriteFrontendCApiRunnerRuntimeInspectorObjectInspectionJsonRows(
      out,
      context.child_indent,
      context.paths);
  WriteFrontendCApiRunnerRuntimeInspectorRuntimeAbiJsonRows(
      out,
      context.child_indent);
}
