#include "io/json/json_schema.h"

#include <sstream>

namespace objc3::io::json {
namespace {

std::string TypeName(const JsonValue &value) {
  switch (value.kind()) {
    case JsonValue::Kind::kNull:
      return "null";
    case JsonValue::Kind::kBool:
      return "boolean";
    case JsonValue::Kind::kNumber:
      return "number";
    case JsonValue::Kind::kString:
      return "string";
    case JsonValue::Kind::kArray:
      return "array";
    case JsonValue::Kind::kObject:
      return "object";
  }
  return "unknown";
}

bool MatchesType(const JsonValue &payload, const std::string &type) {
  if (type == "null") {
    return payload.IsNull();
  }
  if (type == "boolean") {
    return payload.IsBool();
  }
  if (type == "number" || type == "integer") {
    return payload.IsNumber();
  }
  if (type == "string") {
    return payload.IsString();
  }
  if (type == "array") {
    return payload.IsArray();
  }
  if (type == "object") {
    return payload.IsObject();
  }
  return false;
}

void ValidateAt(const JsonValue &schema, const JsonValue &payload, std::string path, JsonSchemaResult &result) {
  if (!schema.IsObject()) {
    return;
  }
  if (const auto type = schema.GetString("type")) {
    if (!MatchesType(payload, *type)) {
      std::ostringstream out;
      out << path << " expected " << *type << " but found " << TypeName(payload);
      result.errors.push_back(out.str());
      result.ok = false;
      return;
    }
  }
  if (const JsonValue *required = schema.Find("required"); required != nullptr && required->IsArray() && payload.IsObject()) {
    for (const JsonValue &entry : required->AsArray()) {
      if (!entry.IsString()) {
        continue;
      }
      if (payload.Find(entry.AsString()) == nullptr) {
        result.errors.push_back(path + " missing required property " + entry.AsString());
        result.ok = false;
      }
    }
  }
  const JsonValue *properties = schema.Find("properties");
  if (properties != nullptr && properties->IsObject() && payload.IsObject()) {
    for (const auto &[key, property_schema] : properties->AsObject()) {
      const JsonValue *property = payload.Find(key);
      if (property != nullptr) {
        ValidateAt(property_schema, *property, path + "." + key, result);
      }
    }
  }
  const JsonValue *items = schema.Find("items");
  if (items != nullptr && payload.IsArray()) {
    const auto &array = payload.AsArray();
    for (std::size_t i = 0; i < array.size(); ++i) {
      ValidateAt(*items, array[i], path + "[" + std::to_string(i) + "]", result);
    }
  }
}

}  // namespace

JsonSchemaResult ValidateJsonSchema(const JsonValue &schema, const JsonValue &payload) {
  JsonSchemaResult result;
  ValidateAt(schema, payload, "$", result);
  return result;
}

}  // namespace objc3::io::json
