bool IsDigitForBase(char c, int base) {
  switch (base) {
  case 2:
    return IsBinaryDigit(c);
  case 8:
    return IsOctalDigit(c);
  case 10:
    return std::isdigit(static_cast<unsigned char>(c)) != 0;
  case 16:
    return IsHexDigit(c);
  default:
    return false;
  }
}

bool NormalizeIntegerDigits(const std::string &digits, int base, std::string &normalized) {
  normalized.clear();
  if (digits.empty()) {
    return false;
  }

  bool previous_was_digit = false;
  for (std::size_t i = 0; i < digits.size(); ++i) {
    const char c = digits[i];
    if (IsDigitSeparator(c)) {
      if (!previous_was_digit || i + 1 >= digits.size() || !IsDigitForBase(digits[i + 1], base)) {
        return false;
      }
      previous_was_digit = false;
      continue;
    }

    if (!IsDigitForBase(c, base)) {
      return false;
    }
    normalized.push_back(c);
    previous_was_digit = true;
  }

  return !normalized.empty() && previous_was_digit;
}
