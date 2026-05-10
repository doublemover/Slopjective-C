#include "tools/objc3c_frontend_c_api_runner_public_result_json_ownership.h"

#include <ostream>

#include "tools/objc3c_frontend_c_api_runner_public_result_json_ownership_aggregate.h"

void WriteFrontendCApiRunnerPublicResultOwnershipJsonRows(
    std::ostream &out,
    const FrontendCApiRunnerPublicResultView &public_result) {
  out << "  \"c_api_ownership\": "
      << RenderFrontendCApiRunnerCOwnershipJson(public_result.c_api_ownership)
      << ",\n";
}
