#include "diag/objc3_diag_record.h"

#include "diag/objc3_diag_code.h"

bool IsValidDiagnosticCoordinate(
    const Objc3DiagnosticCoordinate &coordinate) {
  return coordinate.line != std::numeric_limits<unsigned>::max() &&
         coordinate.column != std::numeric_limits<unsigned>::max();
}

bool IsValidDiagnosticPayload(const Objc3DiagnosticPayload &payload,
                              std::string *reason) {
  if (payload.severity == Objc3DiagnosticSeverity::kUnknown) {
    if (reason != nullptr) {
      *reason = "diagnostic severity is unknown";
    }
    return false;
  }
  if (!IsValidDiagnosticCoordinate(payload.coordinate)) {
    if (reason != nullptr) {
      *reason = "diagnostic coordinate is missing";
    }
    return false;
  }
  if (!IsNativeDiagCode(payload.code)) {
    if (reason != nullptr) {
      *reason = "diagnostic code is not an Objective-C 3 native code";
    }
    return false;
  }
  if (payload.message.empty()) {
    if (reason != nullptr) {
      *reason = "diagnostic message is empty";
    }
    return false;
  }
  if (reason != nullptr) {
    reason->clear();
  }
  return true;
}

Objc3DiagnosticPayload MakeErrorDiagnosticPayload(
    unsigned line,
    unsigned column,
    const std::string &code,
    const std::string &message) {
  Objc3DiagnosticPayload payload;
  payload.severity = Objc3DiagnosticSeverity::kError;
  payload.coordinate = Objc3DiagnosticCoordinate{line, column};
  payload.code = code;
  payload.message = message;
  return payload;
}
