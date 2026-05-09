#pragma once

#include <string>

#include "diag/objc3_diag_record.h"

std::string RenderDiagnosticPayloadUnchecked(
    const Objc3DiagnosticPayload &payload);
Objc3DiagnosticPayload MakeInvalidDiagnosticPayloadDiagnostic(
    const std::string &reason);
