#pragma once

#include <string>

#include "libobjc3c_frontend/c_api.h"

struct FrontendCApiRunnerCOwnershipView {
  bool result_owned_error_message = false;
  bool diagnostics_path_borrowed = false;
  bool manifest_path_borrowed = false;
  bool ir_path_borrowed = false;
  bool object_path_borrowed = false;
  bool runtime_metadata_path_borrowed = false;
};

FrontendCApiRunnerCOwnershipView BuildFrontendCApiRunnerCOwnershipView(
    const objc3c_frontend_c_compile_result_t &result,
    const std::string &result_error_message);
