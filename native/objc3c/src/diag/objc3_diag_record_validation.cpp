#include "diag/objc3_diag_record.h"

#include "diag/objc3_diag_catalog.h"
#include "diag/objc3_diag_code.h"

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
  if (!NativeDiagCodeIsWithinCatalog(payload.code)) {
    if (reason != nullptr) {
      *reason = "diagnostic code is outside the Objective-C 3 catalog";
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
