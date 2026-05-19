#include "parse/objc3_parse_support.h"

#include <cerrno>
#include <cctype>
#include <cstdlib>
#include <limits>

#include "support/objc3_ascii_predicates.h"

namespace objc3c::parse::support {

namespace {

using objc3c::support::IsBinaryDigit;
using objc3c::support::IsDigitSeparator;
using objc3c::support::IsHexDigit;
using objc3c::support::IsOctalDigit;

#include "parse/objc3_parse_support_integer_digit_helpers.inc"

}  // namespace

#include "parse/objc3_parse_support_integer_literals.inc"

}  // namespace objc3c::parse::support
