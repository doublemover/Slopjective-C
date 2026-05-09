#pragma once

#include <limits>
#include <string>

#include "diag/objc3_diag_severity.h"

struct Objc3DiagnosticCoordinate {
  unsigned line = std::numeric_limits<unsigned>::max();
  unsigned column = std::numeric_limits<unsigned>::max();
};

struct Objc3DiagnosticPayload {
  Objc3DiagnosticSeverity severity = Objc3DiagnosticSeverity::kError;
  Objc3DiagnosticCoordinate coordinate;
  std::string code;
  std::string message;
};

bool IsValidDiagnosticCoordinate(const Objc3DiagnosticCoordinate &coordinate);
bool IsValidDiagnosticPayload(const Objc3DiagnosticPayload &payload,
                              std::string *reason = nullptr);
Objc3DiagnosticPayload MakeErrorDiagnosticPayload(unsigned line,
                                                  unsigned column,
                                                  const std::string &code,
                                                  const std::string &message);
