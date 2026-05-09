#include "diag/objc3_diag_severity.h"

#include <algorithm>
#include <cctype>
#include <string>

namespace {

std::string LowercaseCopy(std::string_view value) {
  std::string normalized(value);
  std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                 [](unsigned char c) {
                   return static_cast<char>(std::tolower(c));
                 });
  return normalized;
}

}  // namespace

Objc3DiagnosticSeverity ParseDiagnosticSeverity(std::string_view severity) {
  const std::string normalized = LowercaseCopy(severity);
  if (normalized == "fatal") {
    return Objc3DiagnosticSeverity::kFatal;
  }
  if (normalized == "error") {
    return Objc3DiagnosticSeverity::kError;
  }
  if (normalized == "warning") {
    return Objc3DiagnosticSeverity::kWarning;
  }
  if (normalized == "note") {
    return Objc3DiagnosticSeverity::kNote;
  }
  if (normalized == "ignored") {
    return Objc3DiagnosticSeverity::kIgnored;
  }
  return Objc3DiagnosticSeverity::kUnknown;
}

std::string_view DiagnosticSeveritySpelling(
    Objc3DiagnosticSeverity severity) {
  switch (severity) {
    case Objc3DiagnosticSeverity::kFatal:
      return "fatal";
    case Objc3DiagnosticSeverity::kError:
      return "error";
    case Objc3DiagnosticSeverity::kWarning:
      return "warning";
    case Objc3DiagnosticSeverity::kNote:
      return "note";
    case Objc3DiagnosticSeverity::kIgnored:
      return "ignored";
    case Objc3DiagnosticSeverity::kUnknown:
      break;
  }
  return "unknown";
}

unsigned DiagnosticSeverityRank(Objc3DiagnosticSeverity severity) {
  switch (severity) {
    case Objc3DiagnosticSeverity::kFatal:
      return 0;
    case Objc3DiagnosticSeverity::kError:
      return 1;
    case Objc3DiagnosticSeverity::kWarning:
      return 2;
    case Objc3DiagnosticSeverity::kNote:
      return 3;
    case Objc3DiagnosticSeverity::kIgnored:
      return 4;
    case Objc3DiagnosticSeverity::kUnknown:
      break;
  }
  return 5;
}

unsigned DiagSeverityRank(std::string_view severity) {
  return DiagnosticSeverityRank(ParseDiagnosticSeverity(severity));
}

bool IsFailureDiagnosticSeverity(Objc3DiagnosticSeverity severity) {
  return severity == Objc3DiagnosticSeverity::kFatal ||
         severity == Objc3DiagnosticSeverity::kError;
}
