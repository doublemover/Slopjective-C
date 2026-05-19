#pragma once

#include <cstdint>
#include <string_view>

enum class Objc3DiagnosticSeverity : std::uint8_t {
  kFatal,
  kError,
  kWarning,
  kNote,
  kIgnored,
  kUnknown,
};

Objc3DiagnosticSeverity ParseDiagnosticSeverity(std::string_view severity);
std::string_view DiagnosticSeveritySpelling(Objc3DiagnosticSeverity severity);
unsigned DiagnosticSeverityRank(Objc3DiagnosticSeverity severity);
unsigned DiagSeverityRank(std::string_view severity);
bool IsFailureDiagnosticSeverity(Objc3DiagnosticSeverity severity);
