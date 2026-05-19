#include "pipeline/runtime_import_json_helpers.h"

namespace objc3c::pipeline {

const RuntimeImportJsonValue::Object *AsObject(
    const RuntimeImportJsonValue &value) {
  return std::get_if<RuntimeImportJsonValue::Object>(&value.value);
}

const RuntimeImportJsonValue::Array *AsArray(
    const RuntimeImportJsonValue &value) {
  return std::get_if<RuntimeImportJsonValue::Array>(&value.value);
}

const std::string *AsString(const RuntimeImportJsonValue &value) {
  return std::get_if<std::string>(&value.value);
}

const bool *AsBool(const RuntimeImportJsonValue &value) {
  return std::get_if<bool>(&value.value);
}

const std::int64_t *AsInteger(const RuntimeImportJsonValue &value) {
  return std::get_if<std::int64_t>(&value.value);
}

const RuntimeImportJsonValue *FindMember(
    const RuntimeImportJsonValue::Object &object,
    const std::string &name) {
  const auto it = object.find(name);
  return it == object.end() ? nullptr : &it->second;
}

}  // namespace objc3c::pipeline
