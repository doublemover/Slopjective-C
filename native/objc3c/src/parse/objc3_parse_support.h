#pragma once

#include <string>

namespace objc3c::parse::support {

bool ParseIntegerLiteralValue(const std::string &text, int &value);

std::string MakeDiag(unsigned line, unsigned column, const std::string &code, const std::string &message);

}  // namespace objc3c::parse::support
