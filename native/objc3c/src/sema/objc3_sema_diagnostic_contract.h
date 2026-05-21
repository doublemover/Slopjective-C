#pragma once

#include <string>

bool IsObjc3SemaOwnedDiagnosticCode(const char *code);
std::string BuildObjc3SemaDiagnostic(
    unsigned line,
    unsigned column,
    const char *code,
    const std::string &message);
std::string BuildObjc3SemaDiagnosticWithRecovery(
    unsigned line,
    unsigned column,
    const char *code,
    const std::string &message,
    const std::string &strategy,
    const std::string &boundary);
std::string BuildObjc3SemaMissingReturnDiagnostic(
    unsigned line,
    unsigned column,
    const std::string &callable_context);
