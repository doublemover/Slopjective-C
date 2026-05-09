#pragma once

#include <string>

bool IsObjc3SemaOwnedDiagnosticCode(const char *code);
std::string BuildObjc3SemaDiagnostic(
    unsigned line,
    unsigned column,
    const char *code,
    const std::string &message);
