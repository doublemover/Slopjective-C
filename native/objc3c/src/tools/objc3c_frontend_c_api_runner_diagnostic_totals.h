#pragma once

#include <cstdint>

#include "libobjc3c_frontend/c_api.h"

struct FrontendCApiDiagnosticTotals {
  std::uint64_t total = 0;
  std::uint64_t notes = 0;
  std::uint64_t warnings = 0;
  std::uint64_t errors = 0;
  std::uint64_t fatals = 0;
};

FrontendCApiDiagnosticTotals BuildFrontendCApiDiagnosticTotals(
    const objc3c_frontend_c_compile_result_t &result);
const char *HighestFrontendCApiDiagnosticSeverity(
    const FrontendCApiDiagnosticTotals &totals);
