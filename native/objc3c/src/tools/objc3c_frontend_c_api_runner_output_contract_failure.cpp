#include "tools/objc3c_frontend_c_api_runner_output_contract_failure.h"

bool FailFrontendCApiRunnerOutputContract(const char *boundary,
                                          const std::string &reason,
                                          std::string &error) {
  error = "cli/reporting output ";
  error += boundary;
  error += " fail-closed: ";
  error += reason;
  return false;
}
