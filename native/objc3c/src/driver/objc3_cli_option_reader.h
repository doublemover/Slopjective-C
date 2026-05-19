#pragma once

#include <string>

bool ReadObjc3CliRequiredValue(const std::string &flag,
                               int &index,
                               int argc,
                               char **argv,
                               std::string &value,
                               std::string &error);
