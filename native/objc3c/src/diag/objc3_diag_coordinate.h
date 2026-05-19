#pragma once

#include <limits>

struct Objc3DiagnosticCoordinate {
  unsigned line = std::numeric_limits<unsigned>::max();
  unsigned column = std::numeric_limits<unsigned>::max();
};

bool IsValidDiagnosticCoordinate(const Objc3DiagnosticCoordinate &coordinate);
