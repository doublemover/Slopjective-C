#pragma once

#include <string_view>

#include "diag/objc3_diag_category.h"
#include "diag/objc3_diag_code.h"

struct Objc3DiagnosticRoute {
  Objc3DiagnosticSubsystem subsystem = Objc3DiagnosticSubsystem::kUnknown;
  Objc3DiagnosticCategory category = Objc3DiagnosticCategory::kUnknown;
  bool frontend_compiler = false;
};

Objc3DiagnosticRoute RouteDiagnosticCode(std::string_view code);
