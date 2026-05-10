#include "tools/objc3c_frontend_c_api_runner_observability_json_status.h"

#include <ostream>

#include "io/objc3_json.h"
#include "tools/objc3c_frontend_c_api_runner_status_mapping.h"

using objc3::io::EscapeJsonString;

void WriteFrontendCApiRunnerObservabilityStatusStageJsonRows(
    std::ostream &out,
    const std::string &child_indent,
    objc3c_frontend_c_status_t status,
    const std::string &last_attempted_stage,
    const std::string &blocking_stage) {
  out << child_indent << "\"status_name\": \"" << FrontendCApiStatusName(status)
      << "\",\n";
  out << child_indent << "\"last_attempted_stage\": \""
      << EscapeJsonString(last_attempted_stage) << "\",\n";
  out << child_indent << "\"blocking_stage\": \""
      << EscapeJsonString(blocking_stage) << "\",\n";
}
