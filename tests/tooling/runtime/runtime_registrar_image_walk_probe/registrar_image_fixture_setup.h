#pragma once

#include <string>

namespace objc3c::runtime::probe::runtime_registrar_image_walk {

inline constexpr const char *kKnownSelectorName = "tokenValue";
inline constexpr const char *kUnknownSelectorName =
    "__objc3_unknown_probe_selector";

inline std::string CopyRuntimeStringForReport(const char *value) {
  return value != nullptr ? value : "";
}

inline const char *ReportString(const std::string &value) {
  return value.c_str();
}

}  // namespace objc3c::runtime::probe::runtime_registrar_image_walk
