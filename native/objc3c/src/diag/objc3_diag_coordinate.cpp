#include "diag/objc3_diag_coordinate.h"

bool IsValidDiagnosticCoordinate(
    const Objc3DiagnosticCoordinate &coordinate) {
  return coordinate.line != std::numeric_limits<unsigned>::max() &&
         coordinate.column != std::numeric_limits<unsigned>::max();
}
