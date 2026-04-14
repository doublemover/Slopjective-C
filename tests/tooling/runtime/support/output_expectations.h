#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_OUTPUT_EXPECTATIONS_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_OUTPUT_EXPECTATIONS_H_

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <sstream>
#include <string>
#include <string_view>

namespace objc3c::runtime::probe {

inline std::size_t FirstMismatch(std::string_view actual,
                                 std::string_view expected) {
  const std::size_t compared = std::min(actual.size(), expected.size());
  for (std::size_t index = 0; index < compared; ++index) {
    if (actual[index] != expected[index]) {
      return index;
    }
  }
  return compared;
}

inline std::string DebugSnippet(std::string_view value, std::size_t focus) {
  constexpr std::size_t kRadius = 48;
  const std::size_t start = focus > kRadius ? focus - kRadius : 0;
  const std::size_t end = std::min(value.size(), focus + kRadius);
  std::string rendered;
  if (start > 0) {
    rendered += "...";
  }
  for (std::size_t index = start; index < end; ++index) {
    const unsigned char byte = static_cast<unsigned char>(value[index]);
    switch (byte) {
      case '\n':
        rendered += "\\n";
        break;
      case '\r':
        rendered += "\\r";
        break;
      case '\t':
        rendered += "\\t";
        break;
      case '\\':
        rendered += "\\\\";
        break;
      default:
        if (byte < 0x20U) {
          char escaped[5] = {};
          std::snprintf(escaped, sizeof(escaped), "\\x%02X",
                        static_cast<unsigned int>(byte));
          rendered += escaped;
        } else {
          rendered.push_back(static_cast<char>(byte));
        }
        break;
    }
  }
  if (end < value.size()) {
    rendered += "...";
  }
  return rendered;
}

inline int ExpectTextEqual(std::string_view actual, std::string_view expected,
                           const char *label, int exit_code) {
  if (actual == expected) {
    return 0;
  }
  const std::size_t mismatch = FirstMismatch(actual, expected);
  const std::string actual_context = DebugSnippet(actual, mismatch);
  const std::string expected_context = DebugSnippet(expected, mismatch);
  std::fprintf(stderr,
               "%s mismatch at byte %zu\n"
               "  actual_size=%zu expected_size=%zu\n"
               "  actual_context=%s\n"
               "  expected_context=%s\n",
               label, mismatch, actual.size(), expected.size(),
               actual_context.c_str(), expected_context.c_str());
  return exit_code;
}

template <typename Actual, typename Expected>
inline int ExpectValueEqual(const Actual &actual, const Expected &expected,
                            const char *label, int exit_code) {
  if (actual == expected) {
    return 0;
  }
  std::ostringstream actual_stream;
  std::ostringstream expected_stream;
  actual_stream << actual;
  expected_stream << expected;
  const std::string actual_text = actual_stream.str();
  const std::string expected_text = expected_stream.str();
  std::fprintf(stderr, "%s mismatch: actual=%s expected=%s\n", label,
               actual_text.c_str(), expected_text.c_str());
  return exit_code;
}

inline int ExpectTrue(bool condition, const char *label, int exit_code) {
  if (condition) {
    return 0;
  }
  std::fprintf(stderr, "%s failed\n", label);
  return exit_code;
}

}  // namespace objc3c::runtime::probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_OUTPUT_EXPECTATIONS_H_
