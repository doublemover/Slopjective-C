#include "tools/objc3c_frontend_c_api_runner_public_result_json.h"

#include <ostream>

#include "tools/objc3c_frontend_c_api_runner_public_result_json_artifacts.h"
#include "tools/objc3c_frontend_c_api_runner_public_result_json_diagnostics.h"
#include "tools/objc3c_frontend_c_api_runner_public_result_json_ownership.h"
#include "tools/objc3c_frontend_c_api_runner_public_result_json_status.h"

void WriteFrontendCApiRunnerPublicResultSummaryFields(
    std::ostream &out,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerPublicResultView &public_result) {
  WriteFrontendCApiRunnerPublicResultStatusJsonRows(out, options, public_result);
  WriteFrontendCApiRunnerPublicResultArtifactJsonRows(out, public_result);
  WriteFrontendCApiRunnerPublicResultDiagnosticJsonRows(out, public_result);
  WriteFrontendCApiRunnerPublicResultOwnershipJsonRows(out, public_result);
}
