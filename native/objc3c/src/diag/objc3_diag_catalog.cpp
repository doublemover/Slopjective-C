#include "diag/objc3_diag_catalog.h"

#include <cctype>
#include <string>

#include "diag/objc3_diag_text.h"

unsigned DiagSeverityRank(std::string_view severity) {
  const std::string normalized = ToLower(std::string(severity));
  if (normalized == "fatal") {
    return 0;
  }
  if (normalized == "error") {
    return 1;
  }
  if (normalized == "warning") {
    return 2;
  }
  if (normalized == "note") {
    return 3;
  }
  if (normalized == "ignored") {
    return 4;
  }
  return 5;
}

bool IsNativeDiagCode(std::string_view candidate) {
  if (candidate.size() != 6) {
    return false;
  }
  if (candidate[0] != 'O' || candidate[1] != '3') {
    return false;
  }
  if (std::isupper(static_cast<unsigned char>(candidate[2])) == 0) {
    return false;
  }
  return std::isdigit(static_cast<unsigned char>(candidate[3])) != 0 &&
         std::isdigit(static_cast<unsigned char>(candidate[4])) != 0 &&
         std::isdigit(static_cast<unsigned char>(candidate[5])) != 0;
}
