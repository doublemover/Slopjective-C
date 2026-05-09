#include "io/json/json_schema.h"

#include <cstddef>
#include <regex>
#include <sstream>
#include <utility>

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

bool JsonEquals(const JsonValue &left, const JsonValue &right) {
  if (left.kind() != right.kind()) {
    return false;
  }
  switch (left.kind()) {
    case JsonValue::Kind::kNull:
      return true;
    case JsonValue::Kind::kBool:
      return left.AsBool() == right.AsBool();
    case JsonValue::Kind::kNumber:
      return left.AsNumber() == right.AsNumber();
    case JsonValue::Kind::kString:
      return left.AsString() == right.AsString();
    case JsonValue::Kind::kArray: {
      const JsonValue::Array &left_array = left.AsArray();
      const JsonValue::Array &right_array = right.AsArray();
      if (left_array.size() != right_array.size()) {
        return false;
      }
      for (std::size_t i = 0; i < left_array.size(); ++i) {
        if (!JsonEquals(left_array[i], right_array[i])) {
          return false;
        }
      }
      return true;
    }
    case JsonValue::Kind::kObject: {
      const JsonValue::Object &left_object = left.AsObject();
      const JsonValue::Object &right_object = right.AsObject();
      if (left_object.size() != right_object.size()) {
        return false;
      }
      for (const auto &[key, left_value] : left_object) {
        const JsonValue *right_value = right.Find(key);
        if (right_value == nullptr || !JsonEquals(left_value, *right_value)) {
          return false;
        }
      }
      return true;
    }
  }
  return false;
}

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

void AddError(JsonSchemaResult &result, std::string message) {
  result.errors.push_back(std::move(message));
  result.ok = false;
}

std::string DecodeJsonPointerToken(std::string_view token) {
  std::string decoded;
  decoded.reserve(token.size());
  for (std::size_t i = 0; i < token.size(); ++i) {
    if (token[i] == '~' && i + 1 < token.size()) {
      const char escaped = token[++i];
      if (escaped == '0') {
        decoded.push_back('~');
        continue;
      }
      if (escaped == '1') {
        decoded.push_back('/');
        continue;
      }
      decoded.push_back('~');
      decoded.push_back(escaped);
      continue;
    }
    decoded.push_back(token[i]);
  }
  return decoded;
}

std::string DescribeExpectedType(const JsonValue &schema_type) {
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

const JsonValue *ResolveLocalRef(const JsonValue &schema_root,
                                 std::string_view ref) {
  if (ref.empty() || ref[0] != '#') {
    return nullptr;
  }
  const JsonValue *cursor = &schema_root;
  std::size_t offset = 1;
  while (offset < ref.size()) {
    if (ref[offset] != '/') {
      return nullptr;
    }
    const std::size_t next = ref.find('/', offset + 1);
    const std::size_t token_end = next == std::string_view::npos ? ref.size() : next;
    const std::string token = DecodeJsonPointerToken(ref.substr(offset + 1, token_end - offset - 1));
    cursor = cursor->Find(token);
    if (cursor == nullptr) {
      return nullptr;
    }
    offset = token_end;
  }
  return cursor;
}

bool MatchesType(const JsonValue &schema_type, const JsonValue &payload) {
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

void ValidateAt(const JsonValue &schema_root, const JsonValue &schema,
                const JsonValue &payload, std::string path,
                JsonSchemaResult &result);

bool SubschemaPasses(const JsonValue &schema_root, const JsonValue &schema,
                     const JsonValue &payload, const std::string &path) {
  JsonSchemaResult probe;
  ValidateAt(schema_root, schema, payload, path, probe);
  return probe.ok;
}

void ValidateAt(const JsonValue &schema_root, const JsonValue &schema,
                const JsonValue &payload, std::string path,
                JsonSchemaResult &result) {
  if (!schema.IsObject()) {
    return;
  }
  if (const auto ref = schema.GetString("$ref")) {
    const JsonValue *resolved = ResolveLocalRef(schema_root, *ref);
    if (resolved == nullptr) {
      AddError(result, path + " unresolved schema reference " + *ref);
      return;
    }
    ValidateAt(schema_root, *resolved, payload, path, result);
    return;
  }
  const JsonValue *all_of = schema.Find("allOf");
  if (all_of != nullptr && all_of->IsArray()) {
    for (const JsonValue &candidate : all_of->AsArray()) {
      ValidateAt(schema_root, candidate, payload, path, result);
    }
  }
  const JsonValue *any_of = schema.Find("anyOf");
  if (any_of != nullptr && any_of->IsArray()) {
    bool matched = false;
    for (const JsonValue &candidate : any_of->AsArray()) {
      if (SubschemaPasses(schema_root, candidate, payload, path)) {
        matched = true;
        break;
      }
    }
    if (!matched) {
      AddError(result, path + " did not match any allowed schema");
    }
  }
  if (const JsonValue *schema_type = schema.Find("type"); schema_type != nullptr) {
    if (!MatchesType(*schema_type, payload)) {
      std::ostringstream out;
      out << path << " expected " << DescribeExpectedType(*schema_type) << " but found " << TypeName(payload);
      AddError(result, out.str());
      return;
    }
  }
  const JsonValue *const_value = schema.Find("const");
  if (const_value != nullptr && !JsonEquals(*const_value, payload)) {
    AddError(result, path + " did not match const value");
  }
  const JsonValue *enum_values = schema.Find("enum");
  if (enum_values != nullptr && enum_values->IsArray()) {
    bool matched = false;
    for (const JsonValue &candidate : enum_values->AsArray()) {
      if (JsonEquals(candidate, payload)) {
        matched = true;
        break;
      }
    }
    if (!matched) {
      AddError(result, path + " did not match enum values");
    }
  }
  if (const JsonValue *required = schema.Find("required"); required != nullptr && required->IsArray() && payload.IsObject()) {
    for (const JsonValue &entry : required->AsArray()) {
      if (!entry.IsString()) {
        continue;
      }
      if (payload.Find(entry.AsString()) == nullptr) {
        AddError(result, path + " missing required property " + entry.AsString());
      }
    }
  }
  const JsonValue *properties = schema.Find("properties");
  if (properties != nullptr && properties->IsObject() && payload.IsObject()) {
    for (const auto &[key, property_schema] : properties->AsObject()) {
      const JsonValue *property = payload.Find(key);
      if (property != nullptr) {
        ValidateAt(schema_root, property_schema, *property, path + "." + key, result);
      }
    }
  }
  const JsonValue *additional_properties = schema.Find("additionalProperties");
  if (additional_properties != nullptr && payload.IsObject()) {
    for (const auto &[key, value] : payload.AsObject()) {
      const bool declared_property =
          properties != nullptr && properties->IsObject() &&
          properties->Find(key) != nullptr;
      if (declared_property) {
        continue;
      }
      if (additional_properties->IsBool() && !additional_properties->AsBool()) {
        AddError(result, path + " unexpected property " + key);
        continue;
      }
      if (additional_properties->IsObject()) {
        ValidateAt(schema_root, *additional_properties, value, path + "." + key, result);
      }
    }
  }
  const JsonValue *items = schema.Find("items");
  if (items != nullptr && payload.IsArray()) {
    const auto &array = payload.AsArray();
    for (std::size_t i = 0; i < array.size(); ++i) {
      ValidateAt(schema_root, *items, array[i], path + "[" + std::to_string(i) + "]", result);
    }
  }
  const JsonValue *contains = schema.Find("contains");
  if (contains != nullptr && payload.IsArray()) {
    bool matched = false;
    for (const JsonValue &item : payload.AsArray()) {
      if (SubschemaPasses(schema_root, *contains, item, path + "[]")) {
        matched = true;
        break;
      }
    }
    if (!matched) {
      AddError(result, path + " did not contain a matching item");
    }
  }
  const JsonValue *minimum = schema.Find("minimum");
  if (minimum != nullptr && minimum->IsNumber() && payload.IsNumber() &&
      payload.AsNumber() < minimum->AsNumber()) {
    AddError(result, path + " below minimum");
  }
  const JsonValue *maximum = schema.Find("maximum");
  if (maximum != nullptr && maximum->IsNumber() && payload.IsNumber() &&
      payload.AsNumber() > maximum->AsNumber()) {
    AddError(result, path + " above maximum");
  }
  const JsonValue *min_items = schema.Find("minItems");
  if (min_items != nullptr && min_items->IsNumber() && payload.IsArray() &&
      payload.AsArray().size() < static_cast<std::size_t>(min_items->AsNumber())) {
    AddError(result, path + " has too few items");
  }
  const JsonValue *max_items = schema.Find("maxItems");
  if (max_items != nullptr && max_items->IsNumber() && payload.IsArray() &&
      payload.AsArray().size() > static_cast<std::size_t>(max_items->AsNumber())) {
    AddError(result, path + " has too many items");
  }
  const JsonValue *unique_items = schema.Find("uniqueItems");
  if (unique_items != nullptr && unique_items->IsBool() &&
      unique_items->AsBool() && payload.IsArray()) {
    const JsonValue::Array &array = payload.AsArray();
    bool duplicate = false;
    for (std::size_t i = 0; i < array.size(); ++i) {
      for (std::size_t j = i + 1; j < array.size(); ++j) {
        if (JsonEquals(array[i], array[j])) {
          duplicate = true;
          break;
        }
      }
      if (duplicate) {
        AddError(result, path + " contains duplicate items");
        break;
      }
    }
  }
  const JsonValue *min_length = schema.Find("minLength");
  if (min_length != nullptr && min_length->IsNumber() && payload.IsString() &&
      payload.AsString().size() < static_cast<std::size_t>(min_length->AsNumber())) {
    AddError(result, path + " is shorter than minLength");
  }
  const JsonValue *max_length = schema.Find("maxLength");
  if (max_length != nullptr && max_length->IsNumber() && payload.IsString() &&
      payload.AsString().size() > static_cast<std::size_t>(max_length->AsNumber())) {
    AddError(result, path + " is longer than maxLength");
  }
  if (const auto pattern = schema.GetString("pattern"); pattern.has_value() && payload.IsString()) {
    try {
      if (!std::regex_search(payload.AsString(), std::regex(*pattern))) {
        AddError(result, path + " did not match pattern");
      }
    } catch (const std::regex_error &) {
      AddError(result, path + " contains an invalid schema pattern");
    }
  }
}

}  // namespace

JsonSchemaResult ValidateJsonSchema(const JsonValue &schema, const JsonValue &payload) {
  JsonSchemaResult result;
  ValidateAt(schema, schema, payload, "$", result);
  return result;
}

}  // namespace objc3::io::json
