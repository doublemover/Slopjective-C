#pragma once

#include <string>
#include <string_view>

#include "diagnostics/modes/objc3_mode_option_diagnostic.h"

namespace objc3c::diagnostics::modes {

using CanonicalRejectionDiagnostic = ModeOptionDiagnostic;

CanonicalRejectionDiagnostic ClassifyCanonicalModeRejection(std::string_view flag);
bool BuildCanonicalModeRejectionDiagnostic(const std::string &flag,
                                           std::string &diagnostic);

}  // namespace objc3c::diagnostics::modes
