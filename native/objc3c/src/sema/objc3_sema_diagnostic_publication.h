#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "sema/objc3_sema_pass_manager_contract.h"

bool Objc3SemaDiagnosticsBusHasSink(const Objc3SemaDiagnosticsBus &bus);
bool Objc3SemaDiagnosticsBusHasHardCutoverOwner(
    const Objc3SemaDiagnosticsBus &bus);
std::size_t Objc3SemaDiagnosticsBusCount(const Objc3SemaDiagnosticsBus &bus);
void PublishObjc3SemaDiagnostic(
    const Objc3SemaDiagnosticsBus &bus,
    const std::string &diagnostic);
void PublishObjc3SemaDiagnostics(
    const Objc3SemaDiagnosticsBus &bus,
    const std::vector<std::string> &diagnostics);
