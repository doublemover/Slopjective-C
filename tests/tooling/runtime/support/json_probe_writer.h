#ifndef OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_JSON_PROBE_WRITER_H_
#define OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_JSON_PROBE_WRITER_H_

#include <cstdint>
#include <cstdio>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>

namespace objc3c::runtime::probe {

inline std::string JsonEscapedString(std::string_view value) {
  std::string escaped;
  escaped.reserve(value.size());
  for (const unsigned char byte : value) {
    switch (byte) {
      case '"':
        escaped += "\\\"";
        break;
      case '\\':
        escaped += "\\\\";
        break;
      case '\b':
        escaped += "\\b";
        break;
      case '\f':
        escaped += "\\f";
        break;
      case '\n':
        escaped += "\\n";
        break;
      case '\r':
        escaped += "\\r";
        break;
      case '\t':
        escaped += "\\t";
        break;
      default:
        if (byte < 0x20U) {
          std::ostringstream unicode_escape;
          unicode_escape << "\\u" << std::hex << std::nouppercase
                         << std::setw(4) << std::setfill('0')
                         << static_cast<unsigned int>(byte);
          escaped += unicode_escape.str();
        } else {
          // Probe output is byte-preserving for non-ASCII bytes. Runtime probes
          // emit UTF-8 today, and invalid byte validation belongs at the caller.
          escaped.push_back(static_cast<char>(byte));
        }
        break;
    }
  }
  return escaped;
}

inline std::string JsonEscape(const char *value) {
  return value == nullptr ? std::string() : JsonEscapedString(value);
}

inline std::string JsonString(std::string_view value) {
  std::string quoted;
  quoted.reserve(value.size() + 2U);
  quoted.push_back('"');
  quoted += JsonEscapedString(value);
  quoted.push_back('"');
  return quoted;
}

inline std::string JsonStringOrNull(const char *value) {
  return value == nullptr ? std::string("null") : JsonString(value);
}

inline void WriteJsonString(std::ostream &out, std::string_view value) {
  out << JsonString(value);
}

inline void WriteJsonStringOrNull(std::ostream &out, const char *value) {
  out << JsonStringOrNull(value);
}

inline std::string CopyJsonString(const char *value) {
  return value == nullptr ? std::string() : std::string(value);
}

inline const char *NullableCString(const std::string &value) {
  return value.empty() ? nullptr : value.c_str();
}

class JsonFieldSeparator {
 public:
  void BeforeField(std::ostream &out) {
    if (first_) {
      first_ = false;
      return;
    }
    out << ',';
  }

 private:
  bool first_ = true;
};

inline void WriteJsonFieldName(std::ostream &out, std::string_view name) {
  WriteJsonString(out, name);
  out << ':';
}

inline void WriteJsonIntField(std::ostream &out, JsonFieldSeparator &separator,
                              std::string_view name, int value) {
  separator.BeforeField(out);
  WriteJsonFieldName(out, name);
  out << value;
}

inline void WriteJsonUInt64Field(std::ostream &out,
                                 JsonFieldSeparator &separator,
                                 std::string_view name,
                                 unsigned long long value) {
  separator.BeforeField(out);
  WriteJsonFieldName(out, name);
  out << value;
}

inline void WriteJsonBoolField(std::ostream &out, JsonFieldSeparator &separator,
                               std::string_view name, bool value) {
  separator.BeforeField(out);
  WriteJsonFieldName(out, name);
  out << (value ? "true" : "false");
}

inline void WriteJsonStringField(std::ostream &out,
                                 JsonFieldSeparator &separator,
                                 std::string_view name, const char *value) {
  separator.BeforeField(out);
  WriteJsonFieldName(out, name);
  WriteJsonStringOrNull(out, value);
}

inline void PrintJsonStringOrNull(const char *value) {
  const std::string rendered = JsonStringOrNull(value);
  std::printf("%s", rendered.c_str());
}

inline void PrintUint64Field(const char *name, unsigned long long value,
                             bool trailing_comma = true) {
  const std::string rendered_name = JsonString(name);
  std::printf("%s:%llu%s", rendered_name.c_str(), value,
              trailing_comma ? "," : "");
}

inline void PrintIntField(const char *name, int value,
                          bool trailing_comma = true) {
  const std::string rendered_name = JsonString(name);
  std::printf("%s:%d%s", rendered_name.c_str(), value,
              trailing_comma ? "," : "");
}

inline void PrintStringField(const char *name, const char *value,
                             bool trailing_comma = true) {
  const std::string rendered_name = JsonString(name);
  std::printf("%s:", rendered_name.c_str());
  PrintJsonStringOrNull(value);
  if (trailing_comma) {
    std::printf(",");
  }
}

}  // namespace objc3c::runtime::probe

#endif  // OBJC3C_TESTS_TOOLING_RUNTIME_SUPPORT_JSON_PROBE_WRITER_H_
