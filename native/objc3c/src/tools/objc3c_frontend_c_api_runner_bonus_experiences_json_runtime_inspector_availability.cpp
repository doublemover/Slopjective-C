#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json_runtime_inspector_rows.h"

#include <ostream>

void WriteFrontendCApiRunnerBonusRuntimeInspectorAvailabilityRows(
    std::ostream &out,
    const std::string &grandchild_indent,
    bool runtime_inspector_ready) {
  out << grandchild_indent << "\"available\": "
      << (runtime_inspector_ready ? "true" : "false") << ",\n";
}
