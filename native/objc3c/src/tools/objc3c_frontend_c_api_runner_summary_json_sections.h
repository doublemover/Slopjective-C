#pragma once

#include <ostream>
#include <string>

#include "libobjc3c_frontend/c_api.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"
#include "tools/objc3c_frontend_c_api_runner_output_contract.h"
#include "tools/objc3c_frontend_c_api_runner_public_result.h"

void WriteFrontendCApiRunnerSummaryPublicResultFields(
    std::ostream &out,
    const FrontendCApiRunnerOptions &options,
    const FrontendCApiRunnerPublicResultView &public_result);

void WriteFrontendCApiRunnerSummaryStageBlock(
    std::ostream &out,
    const objc3c_frontend_c_compile_result_t &result);

void WriteFrontendCApiRunnerSummaryObservabilityRuntimeBonusSections(
    std::ostream &out,
    const FrontendCApiRunnerOptions &options,
    const objc3c_frontend_c_compile_result_t &result,
    objc3c_frontend_c_status_t status,
    const FrontendCApiRunnerPublicResultView &public_result);

void WriteFrontendCApiRunnerSummaryOutputContractSection(
    std::ostream &out,
    const FrontendCApiRunnerOutputContract &output_contract);
