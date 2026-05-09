#pragma once

#include <filesystem>
#include <string>

#include "driver/objc3_cli_options.h"
#include "io/objc3_toolchain_runtime_ga_operations_scaffold.h"

struct Objc3DriverObjectBackendResult {
  int status_code = 0;
  int compile_status = 0;
  std::filesystem::path ir_out;
  std::filesystem::path object_out;
  std::filesystem::path backend_out;
  bool backend_output_recorded = false;
  std::string backend_output_payload;
  Objc3ToolchainRuntimeGaOperationsScaffold scaffold;
};

Objc3DriverObjectBackendResult EmitObjc3DriverObjectBackend(
    const Objc3CliOptions &cli_options,
    const std::string &ir_text);
