#pragma once

#include <string>

#include "diag/objc3_diag_utils.h"

namespace objc3c::parse::support {

bool ParseIntegerLiteralValue(const std::string &text, int &value);

using ::MakeDiag;

}  // namespace objc3c::parse::support
