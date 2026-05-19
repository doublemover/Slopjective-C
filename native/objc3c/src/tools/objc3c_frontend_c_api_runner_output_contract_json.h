#pragma once

#include <ostream>
#include <string>

#include "tools/objc3c_frontend_c_api_runner_output_contract.h"

void WriteFrontendCApiRunnerOutputContractJson(
    std::ostream &out,
    const std::string &indent,
    const FrontendCApiRunnerOutputContract &output_contract);
