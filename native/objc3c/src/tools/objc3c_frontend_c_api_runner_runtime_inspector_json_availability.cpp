#include "tools/objc3c_frontend_c_api_runner_runtime_inspector_json_availability.h"

#include <ostream>

#include "io/objc3_json.h"

using objc3::io::EscapeJsonString;

void WriteFrontendCApiRunnerRuntimeInspectorAvailabilityReasonJsonRows(
    std::ostream &out,
    const std::string &child_indent,
    const std::string &availability_reason) {
  out << child_indent << "\"availability_reason\": \""
      << EscapeJsonString(availability_reason) << "\"\n";
}
