#include "driver/objc3_cli_option_reader.h"

bool ReadObjc3CliRequiredValue(const std::string &flag,
                               int &index,
                               int argc,
                               char **argv,
                               std::string &value,
                               std::string &error) {
  if (index + 1 >= argc) {
    error = "missing value for " + flag;
    return false;
  }
  value = argv[++index];
  return true;
}
