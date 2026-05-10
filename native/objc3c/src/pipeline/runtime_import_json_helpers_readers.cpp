#include "pipeline/runtime_import_json_helpers.h"

namespace objc3c::pipeline {

bool ReadStringMember(const RuntimeImportJsonValue::Object &object,
                      const std::string &name,
                      std::string &value,
                      std::string &error) {
  const RuntimeImportJsonValue *member = FindMember(object, name);
  if (member == nullptr) {
    error = "missing JSON string member '" + name + "'";
    return false;
  }
  const std::string *string_value = AsString(*member);
  if (string_value == nullptr) {
    error = "JSON member '" + name + "' must be a string";
    return false;
  }
  value = *string_value;
  return true;
}

bool ReadBoolMember(const RuntimeImportJsonValue::Object &object,
                    const std::string &name,
                    bool &value,
                    std::string &error) {
  const RuntimeImportJsonValue *member = FindMember(object, name);
  if (member == nullptr) {
    error = "missing JSON bool member '" + name + "'";
    return false;
  }
  const bool *bool_value = AsBool(*member);
  if (bool_value == nullptr) {
    error = "JSON member '" + name + "' must be a bool";
    return false;
  }
  value = *bool_value;
  return true;
}

bool ReadSizeMember(const RuntimeImportJsonValue::Object &object,
                    const std::string &name,
                    std::size_t &value,
                    std::string &error) {
  const RuntimeImportJsonValue *member = FindMember(object, name);
  if (member == nullptr) {
    error = "missing JSON integer member '" + name + "'";
    return false;
  }
  const std::int64_t *integer_value = AsInteger(*member);
  if (integer_value == nullptr || *integer_value < 0) {
    error = "JSON member '" + name + "' must be a non-negative integer";
    return false;
  }
  value = static_cast<std::size_t>(*integer_value);
  return true;
}

bool ReadOptionalStringMember(const RuntimeImportJsonValue::Object &object,
                              const std::string &name,
                              std::string &value,
                              std::string &error) {
  const RuntimeImportJsonValue *member = FindMember(object, name);
  if (member == nullptr) {
    value.clear();
    return true;
  }
  const std::string *string_value = AsString(*member);
  if (string_value == nullptr) {
    error = "JSON member '" + name + "' must be a string";
    return false;
  }
  value = *string_value;
  return true;
}

bool ReadOptionalBoolMember(const RuntimeImportJsonValue::Object &object,
                            const std::string &name,
                            bool &value,
                            std::string &error) {
  const RuntimeImportJsonValue *member = FindMember(object, name);
  if (member == nullptr) {
    value = false;
    return true;
  }
  const bool *bool_value = AsBool(*member);
  if (bool_value == nullptr) {
    error = "JSON member '" + name + "' must be a bool";
    return false;
  }
  value = *bool_value;
  return true;
}

bool ReadOptionalSizeMember(const RuntimeImportJsonValue::Object &object,
                            const std::string &name,
                            std::size_t &value,
                            std::string &error) {
  const RuntimeImportJsonValue *member = FindMember(object, name);
  if (member == nullptr) {
    value = 0;
    return true;
  }
  const std::int64_t *integer_value = AsInteger(*member);
  if (integer_value == nullptr || *integer_value < 0) {
    error = "JSON member '" + name + "' must be a non-negative integer";
    return false;
  }
  value = static_cast<std::size_t>(*integer_value);
  return true;
}

bool ReadStringArrayMember(const RuntimeImportJsonValue::Object &object,
                           const std::string &name,
                           std::vector<std::string> &values,
                           std::string &error) {
  const RuntimeImportJsonValue *member = FindMember(object, name);
  if (member == nullptr) {
    error = "missing JSON string-array member '" + name + "'";
    return false;
  }
  const RuntimeImportJsonValue::Array *array_value = AsArray(*member);
  if (array_value == nullptr) {
    error = "JSON member '" + name + "' must be an array";
    return false;
  }
  values.clear();
  values.reserve(array_value->size());
  for (const RuntimeImportJsonValue &element : *array_value) {
    const std::string *string_value = AsString(element);
    if (string_value == nullptr) {
      error =
          "JSON string-array member '" + name + "' contains a non-string value";
      return false;
    }
    values.push_back(*string_value);
  }
  return true;
}

}  // namespace objc3c::pipeline
