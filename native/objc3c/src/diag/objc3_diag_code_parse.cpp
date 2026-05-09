#include "diag/objc3_diag_code.h"

#include <cctype>
#include <cstddef>

bool TryParseNativeDiagCode(std::string_view candidate,
                            Objc3DiagnosticCode &code) {
  code = Objc3DiagnosticCode{};
  if (candidate.size() != 6) {
    return false;
  }
  if (candidate[0] != 'O' || candidate[1] != '3') {
    return false;
  }
  if (std::isupper(static_cast<unsigned char>(candidate[2])) == 0) {
    return false;
  }

  unsigned ordinal = 0;
  for (std::size_t i = 3; i < candidate.size(); ++i) {
    if (std::isdigit(static_cast<unsigned char>(candidate[i])) == 0) {
      return false;
    }
    ordinal = ordinal * 10u + static_cast<unsigned>(candidate[i] - '0');
  }

  const Objc3DiagnosticSubsystem subsystem =
      DiagnosticSubsystemForPrefix(candidate[2]);
  if (subsystem == Objc3DiagnosticSubsystem::kUnknown) {
    return false;
  }

  code.text = std::string(candidate);
  code.subsystem = subsystem;
  code.ordinal = ordinal;
  code.valid = true;
  return true;
}
