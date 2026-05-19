#pragma once

#include <string>
#include <vector>

#include "driver/objc3_cli_options.h"

struct Objc3DriverConformanceProfileSelection {
  std::string selected_profile;
  bool selected_profile_supported = false;
  std::vector<std::string> supported_profile_ids;
  std::vector<std::string> rejected_profile_ids;
  std::vector<std::string> release_targeted_profile_ids;
};

bool ValidateObjc3DriverConformanceSelection(
    const Objc3CliOptions &cli_options,
    std::string &error);

Objc3DriverConformanceProfileSelection
BuildObjc3DriverConformanceProfileSelection(
    const Objc3CliOptions &cli_options);
