#include "io/json/json_schema_type.h"

#include <sstream>

namespace objc3::io::json {

namespace {

bool MatchesTypeName(const JsonValue &payload, const std::string &type) {
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

}  // namespace

std::string JsonSchemaValueTypeName(const JsonValue &value) {
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

std::string DescribeExpectedJsonSchemaType(const JsonValue &schema_type) {
  if (schema_type.IsString()) {
    return schema_type.AsString();
  }
  if (!schema_type.IsArray()) {
    return "any";
  }
  std::ostringstream out;
  bool first = true;
  for (const JsonValue &entry : schema_type.AsArray()) {
    if (!entry.IsString()) {
      continue;
    }
    if (!first) {
      out << "|";
    }
    first = false;
    out << entry.AsString();
  }
  return first ? "any" : out.str();
}

bool JsonSchemaMatchesType(const JsonValue &schema_type,
                           const JsonValue &payload) {
  if (schema_type.IsString()) {
    return MatchesTypeName(payload, schema_type.AsString());
  }
  if (!schema_type.IsArray()) {
    return true;
  }
  for (const JsonValue &entry : schema_type.AsArray()) {
    if (entry.IsString() && MatchesTypeName(payload, entry.AsString())) {
      return true;
    }
  }
  return false;
}

}  // namespace objc3::io::json
