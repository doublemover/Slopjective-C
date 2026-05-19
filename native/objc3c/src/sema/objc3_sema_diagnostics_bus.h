#pragma once

#include <string>
#include <vector>

#include "sema/objc3_sema_pass_manager_contract.h"

using Objc3SemanticDiagnosticsBus = Objc3SemaDiagnosticsBus;

void CanonicalizeObjc3SemaPassDiagnostics(std::vector<std::string> &diagnostics);
bool AreObjc3SemaPassDiagnosticsCanonical(const std::vector<std::string> &diagnostics);
bool AreObjc3SemaPassDiagnosticsHardCutoverOwned(
    const std::vector<std::string> &diagnostics);
