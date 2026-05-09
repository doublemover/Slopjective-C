#include "diag/objc3_diag_route.h"

Objc3DiagnosticRoute RouteDiagnosticCode(std::string_view code) {
  Objc3DiagnosticCode parsed;
  Objc3DiagnosticRoute route;
  if (!TryParseDiagnosticCode(code, parsed)) {
    return route;
  }

  route.subsystem = parsed.subsystem;
  route.category = DiagnosticCategoryForSubsystem(parsed.subsystem);
  route.frontend_compiler = DiagnosticCategoryIsFrontendCompiler(route.category);
  return route;
}
