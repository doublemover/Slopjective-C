#pragma once

#include <initializer_list>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "io/json/json_value.h"
#include "io/json/json_writer.h"

std::vector<std::string> BuildFixedStringVector(
    std::initializer_list<const char *> values);
std::string BuildIndentedStringArrayJson(
    const std::vector<std::string> &values,
    const std::string &indent);

bool TryParseJsonObjectText(const std::string &text,
                            const std::string &label,
                            objc3::io::json::JsonValue &value,
                            std::string &error);
bool TryParseJsonValueText(const std::string &text,
                           const std::string &label,
                           objc3::io::json::JsonValue &value,
                           std::string &error);
bool TryGetJsonStringField(const objc3::io::json::JsonValue &object,
                           std::string_view field,
                           std::string &value);
bool TryGetJsonBoolField(const objc3::io::json::JsonValue &object,
                         std::string_view field,
                         bool &value);
bool TryGetJsonStringArrayField(const objc3::io::json::JsonValue &object,
                                std::string_view field,
                                std::vector<std::string> &values);

const objc3::io::json::JsonValue *FindJsonPath(
    const objc3::io::json::JsonValue &root,
    std::initializer_list<std::string_view> field_path);
bool HasJsonField(const objc3::io::json::JsonValue &root,
                  std::initializer_list<std::string_view> field_path);
bool JsonStringFieldEquals(const objc3::io::json::JsonValue &root,
                           std::initializer_list<std::string_view> field_path,
                           std::string_view expected);

std::string FinishJsonObject(objc3::io::json::JsonObjectWriter &object,
                             std::ostringstream &out);
std::string ComputeSha256ShapedContentDigest(const std::string &text);

bool TryExtractJsonStringField(const std::string &text,
                               const std::string &field,
                               std::string &value);
bool TryExtractJsonBoolField(const std::string &text,
                             const std::string &field,
                             bool &value);
bool TryExtractJsonStringArrayField(const std::string &text,
                                    const std::string &field,
                                    std::vector<std::string> &values);
