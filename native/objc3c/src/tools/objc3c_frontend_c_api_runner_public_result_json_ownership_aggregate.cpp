#include "tools/objc3c_frontend_c_api_runner_public_result_json_ownership_aggregate.h"

#include <sstream>

#include "io/json/json_writer.h"
#include "tools/objc3c_frontend_c_api_runner_public_result_json_ownership_aggregate_rows.h"

using objc3::io::json::JsonObjectWriter;

std::string RenderFrontendCApiRunnerCOwnershipJson(
    const FrontendCApiRunnerCOwnershipView &ownership) {
  std::ostringstream out;
  JsonObjectWriter object(out);
  WriteFrontendCApiRunnerCOwnershipAggregateLifetimeRows(object, ownership);
  WriteFrontendCApiRunnerCOwnershipStringNullRows(object, ownership);
  WriteFrontendCApiRunnerCOwnershipArtifactSummaryRows(object, ownership);
  object.End();
  return out.str();
}
