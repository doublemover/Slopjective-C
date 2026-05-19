#include "io/objc3_process_json_helpers.h"

#include <cstdint>
#include <iomanip>
#include <optional>
#include <utility>

#include "io/json/json_parser.h"

using objc3::io::json::JsonObjectWriter;
using objc3::io::json::JsonValue;

std::vector<std::string> BuildFixedStringVector(
    std::initializer_list<const char *> values) {
  return std::vector<std::string>(values.begin(), values.end());
}

std::string BuildIndentedStringArrayJson(
    const std::vector<std::string> &values,
    const std::string &indent) {
  (void)indent;
  JsonValue::Array array;
  array.reserve(values.size());
  for (const std::string &value : values) {
    array.push_back(JsonValue::String(value));
  }
  return objc3::io::json::RenderJson(JsonValue::ArrayValue(std::move(array)));
}

bool TryParseJsonObjectText(const std::string &text,
                            const std::string &label,
                            JsonValue &value,
                            std::string &error) {
  if (!TryParseJsonValueText(text, label, value, error)) {
    return false;
  }
  if (!value.IsObject()) {
    error = label + " is not a JSON object";
    return false;
  }
  return true;
}

bool TryParseJsonValueText(const std::string &text,
                           const std::string &label,
                           JsonValue &value,
                           std::string &error) {
  objc3::io::json::JsonParseResult parsed = objc3::io::json::ParseJson(text);
  if (!parsed.ok()) {
    error = label + " is not valid JSON: " + parsed.error->Format();
    return false;
  }
  value = std::move(parsed.value);
  return true;
}

bool TryGetJsonStringField(const JsonValue &object,
                           std::string_view field,
                           std::string &value) {
  const std::optional<std::string> field_value = object.GetString(field);
  if (!field_value.has_value()) {
    return false;
  }
  value = *field_value;
  return true;
}

bool TryGetJsonBoolField(const JsonValue &object,
                         std::string_view field,
                         bool &value) {
  const std::optional<bool> field_value = object.GetBool(field);
  if (!field_value.has_value()) {
    return false;
  }
  value = *field_value;
  return true;
}

bool TryGetJsonStringArrayField(const JsonValue &object,
                                std::string_view field,
                                std::vector<std::string> &values) {
  const JsonValue *field_value = object.Find(field);
  if (field_value == nullptr || !field_value->IsArray()) {
    return false;
  }
  std::vector<std::string> array_values;
  array_values.reserve(field_value->AsArray().size());
  for (const JsonValue &entry : field_value->AsArray()) {
    if (!entry.IsString()) {
      return false;
    }
    array_values.push_back(entry.AsString());
  }
  values = std::move(array_values);
  return true;
}

const JsonValue *FindJsonPath(
    const JsonValue &root,
    std::initializer_list<std::string_view> field_path) {
  const JsonValue *cursor = &root;
  for (std::string_view field : field_path) {
    if (cursor == nullptr) {
      return nullptr;
    }
    cursor = cursor->Find(field);
  }
  return cursor;
}

bool HasJsonField(const JsonValue &root,
                  std::initializer_list<std::string_view> field_path) {
  return FindJsonPath(root, field_path) != nullptr;
}

bool JsonStringFieldEquals(const JsonValue &root,
                           std::initializer_list<std::string_view> field_path,
                           std::string_view expected) {
  const JsonValue *field = FindJsonPath(root, field_path);
  return field != nullptr && field->IsString() && field->AsString() == expected;
}

std::string FinishJsonObject(JsonObjectWriter &object, std::ostringstream &out) {
  object.End();
  out << '\n';
  return out.str();
}

std::string ComputeFnv1a64Hex(const std::string &text) {
  std::uint64_t hash = 14695981039346656037ull;
  for (unsigned char c : text) {
    hash ^= static_cast<std::uint64_t>(c);
    hash *= 1099511628211ull;
  }
  std::ostringstream out;
  out << std::hex;
  out.width(16);
  out.fill('0');
  out << hash;
  return out.str();
}

std::string ComputeSha256ShapedContentDigest(const std::string &text) {
  return ComputeFnv1a64Hex("objc3c-dashboard-digest-1:" + text) +
         ComputeFnv1a64Hex("objc3c-dashboard-digest-2:" + text) +
         ComputeFnv1a64Hex("objc3c-dashboard-digest-3:" + text) +
         ComputeFnv1a64Hex("objc3c-dashboard-digest-4:" + text);
}

bool TryExtractJsonStringField(const std::string &text,
                               const std::string &field,
                               std::string &value) {
  JsonValue parsed;
  std::string parse_error;
  if (!TryParseJsonObjectText(text, "JSON object", parsed, parse_error)) {
    return false;
  }
  return TryGetJsonStringField(parsed, field, value);
}

bool TryExtractJsonBoolField(const std::string &text,
                             const std::string &field,
                             bool &value) {
  JsonValue parsed;
  std::string parse_error;
  if (!TryParseJsonObjectText(text, "JSON object", parsed, parse_error)) {
    return false;
  }
  return TryGetJsonBoolField(parsed, field, value);
}

bool TryExtractJsonStringArrayField(const std::string &text,
                                    const std::string &field,
                                    std::vector<std::string> &values) {
  JsonValue parsed;
  std::string parse_error;
  if (!TryParseJsonObjectText(text, "JSON object", parsed, parse_error)) {
    return false;
  }
  return TryGetJsonStringArrayField(parsed, field, values);
}
