#include "driver/objc3_cli_parse.h"

#include <string>

#include "driver/objc3_cli_environment.h"
#include "driver/objc3_cli_option_application.h"
#include "driver/objc3_cli_option_validation.h"
#include "driver/objc3_cli_usage.h"

bool ParseObjc3CliOptions(int argc,
                          char **argv,
                          Objc3CliOptions &options,
                          std::string &error) {
  if (argc < 2) {
    error = Objc3CliUsage();
    return false;
  }

  options = Objc3CliOptions{};
  ApplyObjc3CliEnvironmentDefaults(options);
  int index = 1;
  if (argv[1][0] != '-') {
    options.input = argv[1];
    index = 2;
  }

  for (int i = index; i < argc; ++i) {
    if (!ApplyObjc3CliOption(i, argc, argv, options, error)) {
      return false;
    }
  }

  return ValidateObjc3CliOptions(options, error);
}
