#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace objc3c::pipeline {

struct RuntimeImportJsonValue {
  using Array = std::vector<RuntimeImportJsonValue>;
  using Object = std::unordered_map<std::string, RuntimeImportJsonValue>;

  std::variant<std::monostate, bool, std::int64_t, std::string, Array, Object>
      value;
};

class RuntimeImportJsonParser {
 public:
  explicit RuntimeImportJsonParser(const std::string &input);

  bool Parse(RuntimeImportJsonValue &value, std::string &error);

 private:
  bool AtEnd() const;
  char Peek() const;
  char Consume();
  void SkipWhitespace();
  bool ParseValue(RuntimeImportJsonValue &value, std::string &error);
  bool ParseObject(RuntimeImportJsonValue &value, std::string &error);
  bool ParseArray(RuntimeImportJsonValue &value, std::string &error);
  bool ParseString(std::string &value, std::string &error);
  bool ParseInteger(std::int64_t &value, std::string &error);
  bool ConsumeKeyword(const char *keyword);

  const std::string &input_;
  std::size_t offset_ = 0;
};

const RuntimeImportJsonValue::Object *AsObject(
    const RuntimeImportJsonValue &value);
const RuntimeImportJsonValue::Array *AsArray(
    const RuntimeImportJsonValue &value);
const std::string *AsString(const RuntimeImportJsonValue &value);
const bool *AsBool(const RuntimeImportJsonValue &value);
const std::int64_t *AsInteger(const RuntimeImportJsonValue &value);
const RuntimeImportJsonValue *FindMember(
    const RuntimeImportJsonValue::Object &object,
    const std::string &name);

bool ReadStringMember(const RuntimeImportJsonValue::Object &object,
                      const std::string &name,
                      std::string &value,
                      std::string &error);
bool ReadBoolMember(const RuntimeImportJsonValue::Object &object,
                    const std::string &name,
                    bool &value,
                    std::string &error);
bool ReadSizeMember(const RuntimeImportJsonValue::Object &object,
                    const std::string &name,
                    std::size_t &value,
                    std::string &error);
bool ReadOptionalStringMember(const RuntimeImportJsonValue::Object &object,
                              const std::string &name,
                              std::string &value,
                              std::string &error);
bool ReadOptionalBoolMember(const RuntimeImportJsonValue::Object &object,
                            const std::string &name,
                            bool &value,
                            std::string &error);
bool ReadOptionalSizeMember(const RuntimeImportJsonValue::Object &object,
                            const std::string &name,
                            std::size_t &value,
                            std::string &error);

template <typename UIntT>
bool ReadUnsignedMember(const RuntimeImportJsonValue::Object &object,
                        const std::string &name,
                        UIntT &value,
                        std::string &error) {
  static_assert(std::numeric_limits<UIntT>::is_integer,
                "ReadUnsignedMember requires an integer destination");
  static_assert(!std::numeric_limits<UIntT>::is_signed,
                "ReadUnsignedMember requires an unsigned destination");
  std::size_t parsed_value = 0;
  if (!ReadSizeMember(object, name, parsed_value, error)) {
    return false;
  }
  if (parsed_value >
      static_cast<std::size_t>(std::numeric_limits<UIntT>::max())) {
    error = "JSON member '" + name + "' exceeds destination range";
    return false;
  }
  value = static_cast<UIntT>(parsed_value);
  return true;
}

bool ReadStringArrayMember(const RuntimeImportJsonValue::Object &object,
                           const std::string &name,
                           std::vector<std::string> &values,
                           std::string &error);

}  // namespace objc3c::pipeline
