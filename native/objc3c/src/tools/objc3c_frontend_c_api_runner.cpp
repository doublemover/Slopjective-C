#include <iostream>
#include <string>

#include "tools/objc3c_frontend_c_api_runner_command_parser.h"
#include "tools/objc3c_frontend_c_api_runner_options.h"
#include "tools/objc3c_frontend_c_api_runner_session.h"

int main(int argc, char **argv) {
  FrontendCApiRunnerOptions options;
  std::string parse_error;
  if (!ParseFrontendCApiRunnerOptions(argc, argv, options, parse_error)) {
    std::cerr << parse_error << "\n";
    return 2;
  }

  return RunFrontendCApiRunnerSession(options);
}
