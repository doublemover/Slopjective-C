#pragma once

#include <cstdint>
#include <string_view>

#include "diag/objc3_diag_code.h"

enum class Objc3DiagnosticCategory : std::uint8_t {
  kUnknown,
  kConfiguration,
  kLexical,
  kParsing,
  kSemanticAnalysis,
  kRuntime,
  kFrontendApi,
  kArtifact,
  kTooling,
};

const char *DiagnosticCategoryName(Objc3DiagnosticCategory category);
Objc3DiagnosticCategory DiagnosticCategoryForSubsystem(
    Objc3DiagnosticSubsystem subsystem);
Objc3DiagnosticCategory DiagnosticCategoryForCode(std::string_view code);
bool DiagnosticCategoryIsFrontendCompiler(Objc3DiagnosticCategory category);
