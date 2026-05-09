#pragma once

#include "lower/contracts/lowering_ownership_contracts.h"

#include <string>

enum class Objc3LoweringDiagnosticSeverity {
  Note,
  Warning,
  Error,
};

struct Objc3LoweringDiagnostic {
  std::string code;
  Objc3LoweringDiagnosticSeverity severity =
      Objc3LoweringDiagnosticSeverity::Error;
  std::string message;
  unsigned line = 1;
  unsigned column = 1;
  std::string diagnostic_handoff_owner = kObjc3LoweringDiagnosticHandoffOwner;
  std::string diagnostic_owner_model = kObjc3LoweringNoFallbackOwnerModel;
  bool strict_no_fallback = true;
  bool strict_no_compatibility = true;
  std::string replay_key;
};

const char *Objc3LoweringDiagnosticSeverityName(
    Objc3LoweringDiagnosticSeverity severity);
Objc3LoweringDiagnostic Objc3MakeLoweringDiagnostic(
    const std::string &code, Objc3LoweringDiagnosticSeverity severity,
    const std::string &message, unsigned line, unsigned column);
Objc3LoweringDiagnostic Objc3MakeUnsupportedLoweringDiagnostic(
    const std::string &surface, unsigned line, unsigned column);
bool Objc3LoweringDiagnosticIsBlocking(
    const Objc3LoweringDiagnostic &diagnostic);
std::string Objc3LoweringDiagnosticReplayKey(
    const Objc3LoweringDiagnostic &diagnostic);
