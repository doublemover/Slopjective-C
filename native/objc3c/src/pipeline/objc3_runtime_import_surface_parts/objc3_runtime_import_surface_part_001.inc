#include "pipeline/objc3_runtime_import_surface.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "io/objc3_manifest_artifacts.h"
#include "lower/objc3_lowering_contract.h"
#include "support/objc3_file_reading.h"
#include "support/objc3_runtime_metadata_record_set.h"

namespace {

using objc3c::support::CountRuntimeMetadataSourceRecordSetDeclarations;
using objc3c::support::CountRuntimeMetadataSourceRecordSetReferences;

struct JsonValue {
  using Array = std::vector<JsonValue>;
  using Object = std::unordered_map<std::string, JsonValue>;

  std::variant<std::monostate, bool, std::int64_t, std::string, Array, Object>
      value;
};

class JsonParser {
 public:
  explicit JsonParser(const std::string &input) : input_(input) {}

  bool Parse(JsonValue &value, std::string &error) {
    SkipWhitespace();
    if (!ParseValue(value, error)) {
      return false;
    }
    SkipWhitespace();
    if (!AtEnd()) {
      error = "unexpected trailing JSON content";
      return false;
    }
    return true;
  }

 private:
  bool AtEnd() const { return offset_ >= input_.size(); }

  char Peek() const { return AtEnd() ? '\0' : input_[offset_]; }

  char Consume() { return AtEnd() ? '\0' : input_[offset_++]; }

  void SkipWhitespace() {
    while (!AtEnd() &&
           std::isspace(static_cast<unsigned char>(input_[offset_])) != 0) {
      ++offset_;
    }
  }

  bool ParseValue(JsonValue &value, std::string &error) {
    SkipWhitespace();
    if (AtEnd()) {
      error = "unexpected end of JSON input";
      return false;
    }
    switch (Peek()) {
      case '{':
        return ParseObject(value, error);
      case '[':
        return ParseArray(value, error);
      case '"': {
        std::string string_value;
        if (!ParseString(string_value, error)) {
          return false;
        }
        value.value = std::move(string_value);
        return true;
      }
      case 't':
        if (!ConsumeKeyword("true")) {
          error = "invalid JSON literal";
          return false;
        }
        value.value = true;
        return true;
      case 'f':
        if (!ConsumeKeyword("false")) {
          error = "invalid JSON literal";
          return false;
        }
        value.value = false;
        return true;
      case 'n':
        if (!ConsumeKeyword("null")) {
          error = "invalid JSON literal";
          return false;
        }
        value.value = std::monostate{};
        return true;
      default:
        if (Peek() == '-' || std::isdigit(static_cast<unsigned char>(Peek())) != 0) {
          std::int64_t number_value = 0;
          if (!ParseInteger(number_value, error)) {
            return false;
          }
          value.value = number_value;
          return true;
        }
        error = "unsupported JSON token";
        return false;
    }
  }

  bool ParseObject(JsonValue &value, std::string &error) {
    if (Consume() != '{') {
      error = "expected '{'";
      return false;
    }
    JsonValue::Object object;
    SkipWhitespace();
    if (Peek() == '}') {
      Consume();
      value.value = std::move(object);
      return true;
    }
    while (true) {
      SkipWhitespace();
      std::string key;
      if (!ParseString(key, error)) {
        return false;
      }
      SkipWhitespace();
      if (Consume() != ':') {
        error = "expected ':' in JSON object";
        return false;
      }
      JsonValue member;
      if (!ParseValue(member, error)) {
        return false;
      }
      object.emplace(std::move(key), std::move(member));
      SkipWhitespace();
      const char separator = Consume();
      if (separator == '}') {
        break;
      }
      if (separator != ',') {
        error = "expected ',' or '}' in JSON object";
        return false;
      }
    }
    value.value = std::move(object);
    return true;
  }

  bool ParseArray(JsonValue &value, std::string &error) {
    if (Consume() != '[') {
      error = "expected '['";
      return false;
    }
    JsonValue::Array array;
    SkipWhitespace();
    if (Peek() == ']') {
      Consume();
      value.value = std::move(array);
      return true;
    }
    while (true) {
      JsonValue element;
      if (!ParseValue(element, error)) {
        return false;
      }
      array.push_back(std::move(element));
      SkipWhitespace();
      const char separator = Consume();
      if (separator == ']') {
        break;
      }
      if (separator != ',') {
        error = "expected ',' or ']' in JSON array";
        return false;
      }
    }
    value.value = std::move(array);
    return true;
  }

  bool ParseString(std::string &value, std::string &error) {
    if (Consume() != '"') {
      error = "expected string literal";
      return false;
    }
    std::ostringstream out;
    while (!AtEnd()) {
      const char ch = Consume();
      if (ch == '"') {
        value = out.str();
        return true;
      }
      if (ch == '\\') {
        if (AtEnd()) {
          error = "unterminated JSON escape sequence";
          return false;
        }
        const char escaped = Consume();
        switch (escaped) {
          case '"':
          case '\\':
          case '/':
            out << escaped;
            break;
          case 'b':
            out << '\b';
            break;
          case 'f':
            out << '\f';
            break;
          case 'n':
            out << '\n';
            break;
          case 'r':
            out << '\r';
            break;
          case 't':
            out << '\t';
            break;
          default:
            error = "unsupported JSON escape sequence";
            return false;
        }
      } else {
        out << ch;
      }
    }
    error = "unterminated JSON string literal";
    return false;
  }

  bool ParseInteger(std::int64_t &value, std::string &error) {
    const std::size_t start = offset_;
    if (Peek() == '-') {
      Consume();
    }
    if (AtEnd() || std::isdigit(static_cast<unsigned char>(Peek())) == 0) {
      error = "invalid JSON number";
      return false;
    }
    if (Peek() == '0') {
      Consume();
    } else {
      while (!AtEnd() &&
             std::isdigit(static_cast<unsigned char>(Peek())) != 0) {
        Consume();
      }
    }
    if (!AtEnd() && (Peek() == '.' || Peek() == 'e' || Peek() == 'E')) {
      error = "floating-point JSON numbers are unsupported";
      return false;
    }
    const std::string token = input_.substr(start, offset_ - start);
    try {
      value = std::stoll(token);
    } catch (const std::exception &) {
      error = "invalid JSON integer range";
      return false;
    }
    return true;
  }

  bool ConsumeKeyword(const char *keyword) {
    const std::size_t start = offset_;
    while (*keyword != '\0') {
      if (AtEnd() || Consume() != *keyword) {
        offset_ = start;
        return false;
      }
      ++keyword;
    }
    return true;
  }

  const std::string &input_;
  std::size_t offset_ = 0;
};

const JsonValue::Object *AsObject(const JsonValue &value) {
  return std::get_if<JsonValue::Object>(&value.value);
}

const JsonValue::Array *AsArray(const JsonValue &value) {
  return std::get_if<JsonValue::Array>(&value.value);
}

const std::string *AsString(const JsonValue &value) {
  return std::get_if<std::string>(&value.value);
}

const bool *AsBool(const JsonValue &value) {
  return std::get_if<bool>(&value.value);
}

const std::int64_t *AsInteger(const JsonValue &value) {
  return std::get_if<std::int64_t>(&value.value);
}

const JsonValue *FindMember(const JsonValue::Object &object,
                            const std::string &name) {
  const auto it = object.find(name);
  return it == object.end() ? nullptr : &it->second;
}

bool ReadStringMember(const JsonValue::Object &object,
                      const std::string &name,
                      std::string &value,
                      std::string &error) {
  const JsonValue *member = FindMember(object, name);
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

bool ReadBoolMember(const JsonValue::Object &object,
                    const std::string &name,
                    bool &value,
                    std::string &error) {
  const JsonValue *member = FindMember(object, name);
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

bool ReadSizeMember(const JsonValue::Object &object,
                    const std::string &name,
                    std::size_t &value,
                    std::string &error) {
  const JsonValue *member = FindMember(object, name);
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

bool ReadOptionalStringMember(const JsonValue::Object &object,
                              const std::string &name,
                              std::string &value,
                              std::string &error) {
  const JsonValue *member = FindMember(object, name);
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

bool ReadOptionalBoolMember(const JsonValue::Object &object,
                            const std::string &name,
                            bool &value,
                            std::string &error) {
  const JsonValue *member = FindMember(object, name);
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

bool ReadOptionalSizeMember(const JsonValue::Object &object,
                            const std::string &name,
                            std::size_t &value,
                            std::string &error) {
  const JsonValue *member = FindMember(object, name);
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

template <typename UIntT>
bool ReadUnsignedMember(const JsonValue::Object &object,
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

bool ReadStringArrayMember(const JsonValue::Object &object,
                           const std::string &name,
                           std::vector<std::string> &values,
                           std::string &error) {
  const JsonValue *member = FindMember(object, name);
  if (member == nullptr) {
    error = "missing JSON string-array member '" + name + "'";
    return false;
  }
  const JsonValue::Array *array_value = AsArray(*member);
  if (array_value == nullptr) {
    error = "JSON member '" + name + "' must be an array";
    return false;
  }
  values.clear();
  values.reserve(array_value->size());
  for (const JsonValue &element : *array_value) {
    const std::string *string_value = AsString(element);
    if (string_value == nullptr) {
      error = "JSON string-array member '" + name + "' contains a non-string value";
      return false;
    }
    values.push_back(*string_value);
  }
  return true;
}

bool EndsWith(const std::string &text, const std::string &suffix) {
  return text.size() >= suffix.size() &&
         text.compare(text.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool TryResolveEmitPrefixFromImportSurfacePath(const std::filesystem::path &path,
                                               std::string &emit_prefix,
                                               std::string &error) {
  const std::string filename = path.filename().generic_string();
  const std::string suffix =
      kObjc3RuntimeAwareImportModuleFrontendClosureArtifactSuffix;
  if (!EndsWith(filename, suffix) || filename.size() <= suffix.size()) {
    error = "import surface path does not end with the canonical artifact suffix";
    return false;
  }
  emit_prefix = filename.substr(0, filename.size() - suffix.size());
  if (emit_prefix.empty()) {
    error = "import surface path does not contain an emit prefix";
    return false;
  }
  return true;
}

std::vector<std::string> SplitNonEmptyLines(const std::string &text) {
  std::vector<std::string> lines;
  std::istringstream input(text);
  for (std::string line; std::getline(input, line);) {
    if (!line.empty()) {
      lines.push_back(std::move(line));
    }
  }
  return lines;
}

bool ParseClassRecord(const JsonValue::Object &object,
                      Objc3RuntimeMetadataClassSourceRecord &record,
                      std::string &error) {
  return ReadStringMember(object, "record_kind", record.record_kind, error) &&
         ReadStringMember(object, "name", record.name, error) &&
         ReadOptionalStringMember(object, "super_name", record.super_name, error) &&
         ReadBoolMember(object, "has_super", record.has_super, error) &&
         ReadOptionalBoolMember(object, "objc_final_declared",
                                record.objc_final_declared, error) &&
         ReadOptionalBoolMember(object, "objc_sealed_declared",
                                record.objc_sealed_declared, error) &&
         ReadStringArrayMember(object, "adopted_protocols", record.adopted_protocols_lexicographic, error) &&
         ReadSizeMember(object, "property_count", record.property_count, error) &&
         ReadSizeMember(object, "method_count", record.method_count, error) &&
         ReadUnsignedMember(object, "line", record.line, error) &&
         ReadUnsignedMember(object, "column", record.column, error);
}

bool ParseProtocolRecord(const JsonValue::Object &object,
                         Objc3RuntimeMetadataProtocolSourceRecord &record,
                         std::string &error) {
  return ReadStringMember(object, "name", record.name, error) &&
         ReadStringArrayMember(object, "inherited_protocols", record.inherited_protocols_lexicographic, error) &&
         ReadBoolMember(object, "is_forward_declaration", record.is_forward_declaration, error) &&
         ReadSizeMember(object, "property_count", record.property_count, error) &&
         ReadSizeMember(object, "method_count", record.method_count, error) &&
         ReadUnsignedMember(object, "line", record.line, error) &&
         ReadUnsignedMember(object, "column", record.column, error);
}

bool ParseCategoryRecord(const JsonValue::Object &object,
                         Objc3RuntimeMetadataCategorySourceRecord &record,
                         std::string &error) {
  return ReadStringMember(object, "record_kind", record.record_kind, error) &&
         ReadStringMember(object, "class_name", record.class_name, error) &&
         ReadStringMember(object, "category_name", record.category_name, error) &&
         ReadStringArrayMember(object, "adopted_protocols", record.adopted_protocols_lexicographic, error) &&
         ReadSizeMember(object, "property_count", record.property_count, error) &&
         ReadSizeMember(object, "method_count", record.method_count, error) &&
         ReadUnsignedMember(object, "line", record.line, error) &&
         ReadUnsignedMember(object, "column", record.column, error);
}

bool ParsePropertyRecord(const JsonValue::Object &object,
                         Objc3RuntimeMetadataPropertySourceRecord &record,
                         std::string &error) {
  return ReadStringMember(object, "owner_kind", record.owner_kind, error) &&
         ReadStringMember(object, "owner_name", record.owner_name, error) &&
         ReadStringMember(object, "property_name", record.property_name, error) &&
         ReadStringMember(object, "type", record.type_name, error) &&
         ReadOptionalStringMember(object, "effective_getter_selector", record.effective_getter_selector, error) &&
         ReadBoolMember(object, "effective_setter_available", record.effective_setter_available, error) &&
         ReadOptionalStringMember(object, "effective_setter_selector", record.effective_setter_selector, error) &&
         ReadOptionalBoolMember(object, "synthesizes_executable_accessors", record.synthesizes_executable_accessors, error) &&
         ReadOptionalStringMember(object, "getter_storage_runtime_helper_symbol", record.getter_storage_runtime_helper_symbol, error) &&
         ReadOptionalStringMember(object, "setter_storage_runtime_helper_symbol", record.setter_storage_runtime_helper_symbol, error) &&
         ReadOptionalStringMember(object, "ivar_binding_symbol", record.ivar_binding_symbol, error) &&
         ReadOptionalStringMember(object, "executable_synthesized_binding_kind", record.executable_synthesized_binding_kind, error) &&
         ReadOptionalStringMember(object, "executable_synthesized_binding_symbol", record.executable_synthesized_binding_symbol, error) &&
         ReadOptionalStringMember(object, "property_attribute_profile", record.property_attribute_profile, error) &&
         ReadOptionalStringMember(object, "ownership_lifetime_profile", record.ownership_lifetime_profile, error) &&
         ReadOptionalStringMember(object, "ownership_runtime_hook_profile", record.ownership_runtime_hook_profile, error) &&
         ReadOptionalStringMember(object, "accessor_ownership_profile", record.accessor_ownership_profile, error) &&
         ReadOptionalStringMember(object, "executable_ivar_layout_symbol", record.executable_ivar_layout_symbol, error) &&
         ReadOptionalSizeMember(object, "executable_ivar_layout_slot_index", record.executable_ivar_layout_slot_index, error) &&
         ReadOptionalSizeMember(object, "executable_ivar_layout_size_bytes", record.executable_ivar_layout_size_bytes, error) &&
         ReadOptionalSizeMember(object, "executable_ivar_layout_alignment_bytes", record.executable_ivar_layout_alignment_bytes, error) &&
         ReadOptionalSizeMember(object, "executable_ivar_layout_offset_bytes", record.executable_ivar_layout_offset_bytes, error) &&
         ReadOptionalSizeMember(object, "executable_ivar_layout_padding_bytes", record.executable_ivar_layout_padding_bytes, error) &&
         ReadOptionalSizeMember(object, "executable_ivar_layout_inherited_slot_count", record.executable_ivar_layout_inherited_slot_count, error) &&
         ReadOptionalSizeMember(object, "executable_ivar_layout_inherited_size_bytes", record.executable_ivar_layout_inherited_size_bytes, error) &&
         ReadOptionalSizeMember(object, "executable_ivar_layout_owner_size_bytes", record.executable_ivar_layout_owner_size_bytes, error) &&
         ReadOptionalSizeMember(object, "executable_ivar_init_order_index", record.executable_ivar_init_order_index, error) &&
         ReadOptionalSizeMember(object, "executable_ivar_destroy_order_index", record.executable_ivar_destroy_order_index, error) &&
         ReadOptionalBoolMember(object, "executable_ivar_layout_valid", record.executable_ivar_layout_valid, error) &&
         ReadOptionalStringMember(object, "executable_ivar_layout_replay_key", record.executable_ivar_layout_replay_key, error) &&
         ReadUnsignedMember(object, "line", record.line, error) &&
         ReadUnsignedMember(object, "column", record.column, error);
}

bool ParseMethodRecord(const JsonValue::Object &object,
                       Objc3RuntimeMetadataMethodSourceRecord &record,
                       std::string &error) {
  return ReadStringMember(object, "owner_kind", record.owner_kind, error) &&
         ReadStringMember(object, "owner_name", record.owner_name, error) &&
         ReadStringMember(object, "selector", record.selector, error) &&
         ReadBoolMember(object, "is_class_method", record.is_class_method, error) &&
         ReadBoolMember(object, "has_body", record.has_body, error) &&
         ReadOptionalBoolMember(object, "effective_direct_dispatch",
                                record.effective_direct_dispatch, error) &&
         ReadOptionalBoolMember(object, "objc_final_declared",
                                record.objc_final_declared, error) &&
         ReadSizeMember(object, "parameter_count", record.parameter_count, error) &&
         ReadStringMember(object, "return_type", record.return_type_name, error) &&
         ReadUnsignedMember(object, "line", record.line, error) &&
         ReadUnsignedMember(object, "column", record.column, error);
}

bool ParseIvarRecord(const JsonValue::Object &object,
                     Objc3RuntimeMetadataIvarSourceRecord &record,
                     std::string &error) {
  return ReadStringMember(object, "owner_kind", record.owner_kind, error) &&
         ReadStringMember(object, "owner_name", record.owner_name, error) &&
         ReadStringMember(object, "property_name", record.property_name, error) &&
         ReadOptionalStringMember(object, "ivar_binding_symbol", record.ivar_binding_symbol, error) &&
         ReadOptionalStringMember(object, "executable_synthesized_binding_kind", record.executable_synthesized_binding_kind, error) &&
         ReadOptionalStringMember(object, "executable_synthesized_binding_symbol", record.executable_synthesized_binding_symbol, error) &&
         ReadOptionalStringMember(object, "executable_ivar_layout_symbol", record.executable_ivar_layout_symbol, error) &&
         ReadOptionalSizeMember(object, "executable_ivar_layout_slot_index", record.executable_ivar_layout_slot_index, error) &&
         ReadOptionalSizeMember(object, "executable_ivar_layout_size_bytes", record.executable_ivar_layout_size_bytes, error) &&
         ReadOptionalSizeMember(object, "executable_ivar_layout_alignment_bytes", record.executable_ivar_layout_alignment_bytes, error) &&
         ReadOptionalSizeMember(object, "executable_ivar_layout_offset_bytes", record.executable_ivar_layout_offset_bytes, error) &&
         ReadOptionalSizeMember(object, "executable_ivar_layout_padding_bytes", record.executable_ivar_layout_padding_bytes, error) &&
         ReadOptionalSizeMember(object, "executable_ivar_layout_inherited_slot_count", record.executable_ivar_layout_inherited_slot_count, error) &&
         ReadOptionalSizeMember(object, "executable_ivar_layout_inherited_size_bytes", record.executable_ivar_layout_inherited_size_bytes, error) &&
         ReadOptionalSizeMember(object, "executable_ivar_layout_owner_size_bytes", record.executable_ivar_layout_owner_size_bytes, error) &&
         ReadOptionalSizeMember(object, "executable_ivar_init_order_index", record.executable_ivar_init_order_index, error) &&
         ReadOptionalSizeMember(object, "executable_ivar_destroy_order_index", record.executable_ivar_destroy_order_index, error) &&
         ReadOptionalBoolMember(object, "executable_ivar_layout_valid", record.executable_ivar_layout_valid, error) &&
         ReadOptionalStringMember(object, "executable_ivar_layout_replay_key", record.executable_ivar_layout_replay_key, error) &&
         ReadOptionalStringMember(object, "source_model", record.source_model, error) &&
         ReadUnsignedMember(object, "line", record.line, error) &&
         ReadUnsignedMember(object, "column", record.column, error);
}

template <typename RecordT>
bool ParseRecordArray(const JsonValue::Object &root,
                      const std::string &declarations_name,
                      const std::string &record_name,
                      std::vector<RecordT> &records,
                      bool (*parser)(const JsonValue::Object &, RecordT &,
                                     std::string &),
                      std::string &error) {
  const JsonValue *declarations_value = FindMember(root, declarations_name);
  if (declarations_value == nullptr) {
    error = "missing JSON object member '" + declarations_name + "'";
    return false;
  }
  const JsonValue::Object *declarations_object = AsObject(*declarations_value);
  if (declarations_object == nullptr) {
    error = "JSON member '" + declarations_name + "' must be an object";
    return false;
  }
  const JsonValue *records_value = FindMember(*declarations_object, record_name);
  if (records_value == nullptr) {
    error = "missing JSON array member '" + record_name + "'";
    return false;
  }
  const JsonValue::Array *records_array = AsArray(*records_value);
  if (records_array == nullptr) {
    error = "JSON member '" + record_name + "' must be an array";
    return false;
  }
  records.clear();
  records.reserve(records_array->size());
  for (const JsonValue &element : *records_array) {
    const JsonValue::Object *record_object = AsObject(element);
    if (record_object == nullptr) {
      error = "JSON array member '" + record_name + "' must contain objects";
      return false;
    }
    RecordT record;
    if (!parser(*record_object, record, error)) {
      return false;
    }
    records.push_back(std::move(record));
  }
  return true;
}

bool PopulateFrontendClosureSummary(const JsonValue::Object &root,
                                    Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary,
                                    std::string &error) {
  summary = Objc3RuntimeAwareImportModuleFrontendClosureSummary{};
  if (!ReadStringMember(root, "contract_id", summary.contract_id, error) ||
      !ReadStringMember(root, "source_surface_contract_id", summary.source_surface_contract_id, error) ||
      !ReadStringMember(root, "frontend_surface_path", summary.frontend_surface_path, error) ||
      !ReadStringMember(root, "payload_model", summary.payload_model, error) ||
      !ReadStringMember(root, "artifact", summary.artifact_relative_path, error) ||
      !ReadStringMember(root, "authority_model", summary.authority_model, error) ||
      !ReadStringMember(root, "payload_ownership_model", summary.payload_ownership_model, error) ||
      !ReadStringMember(root, "module_name", summary.module_name, error) ||
      !ReadSizeMember(root, "protocol_decl_count", summary.protocol_decl_count, error) ||
      !ReadSizeMember(root, "interface_decl_count", summary.interface_decl_count, error) ||
      !ReadSizeMember(root, "implementation_decl_count", summary.implementation_decl_count, error) ||
      !ReadSizeMember(root, "interface_category_decl_count", summary.interface_category_decl_count, error) ||
      !ReadSizeMember(root, "implementation_category_decl_count", summary.implementation_category_decl_count, error) ||
      !ReadSizeMember(root, "function_decl_count", summary.function_decl_count, error) ||
      !ReadSizeMember(root, "module_import_graph_sites", summary.module_import_graph_sites, error) ||
      !ReadSizeMember(root, "import_edge_candidate_sites", summary.import_edge_candidate_sites, error) ||
      !ReadSizeMember(root, "namespace_segment_sites", summary.namespace_segment_sites, error) ||
      !ReadSizeMember(root, "object_pointer_type_sites", summary.object_pointer_type_sites, error) ||
      !ReadSizeMember(root, "pointer_declarator_sites", summary.pointer_declarator_sites, error) ||
      !ReadSizeMember(root, "normalized_sites", summary.normalized_sites, error) ||
      !ReadSizeMember(root, "contract_violation_sites", summary.contract_violation_sites, error) ||
      !ReadSizeMember(root, "runtime_owned_declaration_count", summary.runtime_owned_declaration_count, error) ||
      !ReadSizeMember(root, "metadata_reference_count", summary.metadata_reference_count, error) ||
      !ReadBoolMember(root, "runtime_aware_import_declarations_landed", summary.runtime_aware_import_declarations_landed, error) ||
      !ReadBoolMember(root, "module_metadata_import_surface_landed", summary.module_metadata_import_surface_landed, error) ||
      !ReadBoolMember(root, "runtime_owned_declaration_import_landed", summary.runtime_owned_declaration_import_landed, error) ||
      !ReadBoolMember(root, "runtime_metadata_reference_import_landed", summary.runtime_metadata_reference_import_landed, error) ||
      !ReadBoolMember(root, "public_frontend_api_module_surface_landed", summary.public_frontend_api_module_surface_landed, error) ||
      !ReadBoolMember(root, "ready_for_import_artifact_emission", summary.ready_for_import_artifact_emission, error) ||
      !ReadBoolMember(root, "ready_for_frontend_module_consumption", summary.ready_for_frontend_module_consumption, error) ||
      !ReadStringMember(root, "source_surface_replay_key", summary.source_surface_replay_key, error) ||
      !ReadStringMember(root, "replay_key", summary.replay_key, error)) {
    return false;
  }
  summary.fail_closed = true;
  summary.source_surface_contract_ready =
      !summary.source_surface_replay_key.empty();
  summary.runtime_metadata_source_records_ready = false;
  summary.frontend_surface_published = true;
  summary.import_artifact_template_published = true;
  summary.failure_reason.clear();
  return true;
}

bool PopulateImportedTypeSystemOptionalKeypathSurface(
    const JsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *lowering_value =
      FindMember(root, "objc_type_system_optional_keypath_lowering_contract");
  const JsonValue *runtime_value =
      FindMember(root, "objc_type_system_optional_keypath_runtime_helper_contract");
  if (lowering_value == nullptr && runtime_value == nullptr) {
    return true;
  }
  if (lowering_value == nullptr || runtime_value == nullptr) {
    error =
        "runtime import surface must publish both optional/keypath lowering and runtime helper contracts together";
    return false;
  }

  const JsonValue::Object *lowering_object = AsObject(*lowering_value);
  const JsonValue::Object *runtime_object = AsObject(*runtime_value);
  if (lowering_object == nullptr || runtime_object == nullptr) {
    error =
        "optional/keypath imported contract members must both be JSON objects";
    return false;
  }

  std::string lowering_contract_id;
  std::string lowering_replay_key;
  std::string runtime_contract_id;
  std::string runtime_replay_key;
  if (!ReadStringMember(*lowering_object, "contract_id", lowering_contract_id,
                        error) ||
      !ReadSizeMember(*lowering_object, "optional_send_sites",
                      surface.type_system_optional_send_sites, error) ||
      !ReadSizeMember(*lowering_object, "typed_keypath_literal_sites",
                      surface.type_system_typed_keypath_literal_sites, error) ||
      !ReadSizeMember(*lowering_object, "live_optional_lowering_sites",
                      surface.type_system_live_optional_lowering_sites, error) ||
      !ReadSizeMember(*lowering_object, "live_typed_keypath_artifact_sites",
                      surface.type_system_live_typed_keypath_artifact_sites, error) ||
      !ReadBoolMember(*lowering_object, "ready_for_native_optional_lowering",
                      surface.type_system_ready_for_native_optional_lowering, error) ||
      !ReadStringMember(*lowering_object, "replay_key", lowering_replay_key,
                        error) ||
      !ReadStringMember(*runtime_object, "contract_id", runtime_contract_id,
                        error) ||
      !ReadBoolMember(*runtime_object, "optional_send_runtime_ready",
                      surface.type_system_optional_send_runtime_ready, error) ||
      !ReadBoolMember(*runtime_object, "typed_keypath_descriptor_handles_ready",
                      surface.type_system_typed_keypath_descriptor_handles_ready,
                      error) ||
      !ReadBoolMember(*runtime_object,
                      "typed_keypath_runtime_execution_helper_landed",
                      surface.type_system_typed_keypath_runtime_execution_helper_landed,
                      error) ||
      !ReadStringMember(*runtime_object, "replay_key", runtime_replay_key,
                        error)) {
    return false;
  }

  if (lowering_contract_id != kObjc3TypeSystemOptionalKeypathLoweringContractId) {
    error = "unexpected optional/keypath lowering contract id in import surface";
    return false;
  }
  if (runtime_contract_id != kObjc3TypeSystemOptionalKeypathRuntimeHelperContractId) {
    error =
        "unexpected optional/keypath runtime helper contract id in import surface";
    return false;
  }

  surface.type_system_optional_keypath_lowering_contract_present = true;
  surface.type_system_optional_keypath_lowering_replay_key =
      std::move(lowering_replay_key);
  surface.type_system_optional_keypath_runtime_helper_replay_key =
      std::move(runtime_replay_key);
  return true;
}

bool PopulateImportedTypeSystemGenericContractPreservation(
    const JsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *preservation_value =
      FindMember(root, "objc_type_system_generic_contract_preservation");
  if (preservation_value == nullptr) {
    return true;
  }

  const JsonValue::Object *preservation_object = AsObject(*preservation_value);
  if (preservation_object == nullptr) {
    error = "type-system generic contract preservation must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  if (!ReadStringMember(*preservation_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*preservation_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadSizeMember(*preservation_object, "generic_interface_count",
                      surface.type_system_generic_interface_count, error) ||
      !ReadSizeMember(*preservation_object, "generic_parameter_count",
                      surface.type_system_generic_parameter_count, error) ||
      !ReadSizeMember(*preservation_object,
                      "generic_variance_annotation_count",
                      surface.type_system_generic_variance_annotation_count,
                      error) ||
      !ReadSizeMember(*preservation_object,
                      "generic_argument_reference_count",
                      surface.type_system_generic_argument_reference_count,
                      error) ||
      !ReadSizeMember(*preservation_object,
                      "protocol_qualified_generic_argument_count",
                      surface
                          .type_system_protocol_qualified_generic_argument_count,
                      error) ||
      !ReadBoolMember(*preservation_object, "ready",
                      surface.type_system_generic_contract_ready, error) ||
      !ReadBoolMember(*preservation_object, "deterministic",
                      surface.type_system_generic_contract_deterministic,
                      error) ||
      !ReadStringMember(*preservation_object, "type_semantic_replay_key",
                        surface.type_system_generic_type_semantic_replay_key,
                        error) ||
      !ReadStringMember(*preservation_object, "replay_key",
                        surface.type_system_generic_contract_replay_key,
                        error)) {
    return false;
  }

  if (contract_id != kObjc3TypeSystemGenericContractPreservationContractId) {
    error =
        "unexpected type-system generic contract preservation id in import surface";
    return false;
  }
  if (source_contract_id != kObjc3TypeSystemTypeSemanticModelContractId) {
    error =
        "unexpected type-system generic contract preservation source contract id";
    return false;
  }
  if (!surface.type_system_generic_contract_ready ||
      !surface.type_system_generic_contract_deterministic ||
      surface.type_system_generic_contract_replay_key.empty() ||
      surface.type_system_generic_type_semantic_replay_key.empty()) {
    error =
        "type-system generic contract preservation is not ready for import";
    return false;
  }
  if (surface.type_system_generic_variance_annotation_count <
      surface.type_system_generic_parameter_count) {
    error =
        "type-system generic contract preservation dropped variance annotations";
    return false;
  }

  surface.type_system_generic_contract_preservation_present = true;
  return true;
}

bool PopulateImportedTypeSystemNullabilityContractPreservation(
    const JsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *preservation_value =
      FindMember(root, "objc_type_system_nullability_contract_preservation");
  if (preservation_value == nullptr) {
    return true;
  }

  const JsonValue::Object *preservation_object = AsObject(*preservation_value);
  if (preservation_object == nullptr) {
    error =
        "type-system nullability contract preservation must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  if (!ReadStringMember(*preservation_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*preservation_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadSizeMember(*preservation_object, "canonical_type_count",
                      surface.type_system_nullability_canonical_type_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "object_type_count",
                      surface.type_system_nullability_object_type_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "nullable_entry_count",
                      surface.type_system_nullable_entry_count, error) ||
      !ReadSizeMember(*preservation_object, "nonnull_entry_count",
                      surface.type_system_nonnull_entry_count, error) ||
      !ReadSizeMember(*preservation_object,
                      "implicitly_unwrapped_entry_count",
                      surface.type_system_implicitly_unwrapped_entry_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "null_resettable_entry_count",
                      surface.type_system_null_resettable_entry_count, error) ||
      !ReadSizeMember(*preservation_object,
                      "unspecified_nullability_entry_count",
                      surface.type_system_unspecified_nullability_entry_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "invalid_nullability_entry_count",
                      surface.type_system_invalid_nullability_entry_count,
                      error) ||
      !ReadBoolMember(*preservation_object, "ready",
                      surface.type_system_nullability_contract_ready, error) ||
      !ReadBoolMember(*preservation_object, "deterministic",
                      surface.type_system_nullability_contract_deterministic,
                      error) ||
      !ReadStringMember(
          *preservation_object, "type_semantic_replay_key",
          surface.type_system_nullability_type_semantic_replay_key, error) ||
      !ReadStringMember(*preservation_object, "replay_key",
                        surface.type_system_nullability_contract_replay_key,
                        error)) {
    return false;
  }

  if (contract_id != kObjc3TypeSystemNullabilityContractPreservationContractId) {
    error =
        "unexpected type-system nullability contract preservation id in import surface";
    return false;
  }
  if (source_contract_id != kObjc3TypeSystemTypeSemanticModelContractId) {
    error =
        "unexpected type-system nullability contract preservation source contract id";
    return false;
  }
  if (!surface.type_system_nullability_contract_ready ||
      !surface.type_system_nullability_contract_deterministic ||
      surface.type_system_nullability_contract_replay_key.empty() ||
      surface.type_system_nullability_type_semantic_replay_key.empty()) {
    error =
        "type-system nullability contract preservation is not ready for import";
    return false;
  }
  const std::size_t nullability_entry_count =
      surface.type_system_nullable_entry_count +
      surface.type_system_nonnull_entry_count +
      surface.type_system_implicitly_unwrapped_entry_count +
      surface.type_system_null_resettable_entry_count +
      surface.type_system_unspecified_nullability_entry_count;
  if (surface.type_system_nullability_canonical_type_count !=
      nullability_entry_count) {
    error =
        "type-system nullability contract preservation dropped canonical nullability entries";
    return false;
  }

  surface.type_system_nullability_contract_preservation_present = true;
  return true;
}

bool PopulateImportedTypeSystemProtocolContractPreservation(
    const JsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *preservation_value =
      FindMember(root, "objc_type_system_protocol_contract_preservation");
  if (preservation_value == nullptr) {
    return true;
  }

  const JsonValue::Object *preservation_object = AsObject(*preservation_value);
  if (preservation_object == nullptr) {
    error = "type-system protocol contract preservation must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  if (!ReadStringMember(*preservation_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*preservation_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadSizeMember(*preservation_object, "protocol_decl_count",
                      surface.type_system_protocol_decl_count, error) ||
      !ReadSizeMember(*preservation_object,
                      "protocol_forward_declaration_count",
                      surface.type_system_protocol_forward_declaration_count,
                      error) ||
      !ReadSizeMember(*preservation_object,
                      "protocol_inheritance_edge_count",
                      surface.type_system_protocol_inheritance_edge_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "protocol_required_method_count",
                      surface.type_system_protocol_required_method_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "protocol_optional_method_count",
                      surface.type_system_protocol_optional_method_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "protocol_required_property_count",
                      surface.type_system_protocol_required_property_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "protocol_optional_property_count",
                      surface.type_system_protocol_optional_property_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "class_protocol_adoption_count",
                      surface.type_system_class_protocol_adoption_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "category_protocol_adoption_count",
                      surface.type_system_category_protocol_adoption_count,
                      error) ||
      !ReadBoolMember(*preservation_object, "ready",
                      surface.type_system_protocol_contract_ready, error) ||
      !ReadBoolMember(*preservation_object, "deterministic",
                      surface.type_system_protocol_contract_deterministic,
                      error) ||
      !ReadStringMember(
          *preservation_object, "type_semantic_replay_key",
          surface.type_system_protocol_type_semantic_replay_key, error) ||
      !ReadStringMember(*preservation_object, "replay_key",
                        surface.type_system_protocol_contract_replay_key,
                        error)) {
    return false;
  }

  if (contract_id != kObjc3TypeSystemProtocolContractPreservationContractId) {
    error =
        "unexpected type-system protocol contract preservation id in import surface";
    return false;
  }
  if (source_contract_id != kObjc3TypeSystemTypeSemanticModelContractId) {
    error =
        "unexpected type-system protocol contract preservation source contract id";
    return false;
  }
  if (!surface.type_system_protocol_contract_ready ||
      !surface.type_system_protocol_contract_deterministic ||
      surface.type_system_protocol_contract_replay_key.empty() ||
      surface.type_system_protocol_type_semantic_replay_key.empty()) {
    error =
        "type-system protocol contract preservation is not ready for import";
    return false;
  }
  if (surface.type_system_protocol_forward_declaration_count >
      surface.type_system_protocol_decl_count) {
    error =
        "type-system protocol contract preservation has impossible forward declaration count";
    return false;
  }

  const JsonValue *protocols_value =
      FindMember(*preservation_object, "protocol_contract_protocols");
  if (protocols_value == nullptr) {
    error = "type-system protocol contract preservation is missing protocols";
    return false;
  }
  const JsonValue::Array *protocols_array = AsArray(*protocols_value);
  if (protocols_array == nullptr) {
    error =
        "type-system protocol contract preservation protocols must be an array";
    return false;
  }
  if (protocols_array->size() != surface.type_system_protocol_decl_count) {
    error =
        "type-system protocol contract preservation dropped protocol records";
    return false;
  }

  std::size_t inherited_edge_count = 0;
  std::size_t required_method_count = 0;
  std::size_t optional_method_count = 0;
  std::size_t required_property_count = 0;
  std::size_t optional_property_count = 0;
  std::size_t forward_declaration_count = 0;
  for (const JsonValue &protocol_value : *protocols_array) {
    const JsonValue::Object *protocol_object = AsObject(protocol_value);
    if (protocol_object == nullptr) {
      error =
          "type-system protocol contract preservation protocols must contain objects";
      return false;
    }
    std::string protocol_name;
    bool is_forward_declaration = false;
    std::size_t required_methods = 0;
    std::size_t optional_methods = 0;
    std::size_t required_properties = 0;
    std::size_t optional_properties = 0;
    std::vector<std::string> inherited_protocols;
    if (!ReadStringMember(*protocol_object, "name", protocol_name, error) ||
        !ReadBoolMember(*protocol_object, "is_forward_declaration",
                        is_forward_declaration, error) ||
        !ReadStringArrayMember(*protocol_object,
                               "inherited_protocols_lexicographic",
                               inherited_protocols, error) ||
        !ReadSizeMember(*protocol_object, "required_method_count",
                        required_methods, error) ||
        !ReadSizeMember(*protocol_object, "optional_method_count",
                        optional_methods, error) ||
        !ReadSizeMember(*protocol_object, "required_property_count",
                        required_properties, error) ||
        !ReadSizeMember(*protocol_object, "optional_property_count",
                        optional_properties, error)) {
      return false;
    }
    if (protocol_name.empty()) {
      error =
          "type-system protocol contract preservation contains an unnamed protocol";
      return false;
    }
    if (is_forward_declaration) {
      ++forward_declaration_count;
    }
    inherited_edge_count += inherited_protocols.size();
    required_method_count += required_methods;
    optional_method_count += optional_methods;
    required_property_count += required_properties;
    optional_property_count += optional_properties;
  }
  if (forward_declaration_count !=
          surface.type_system_protocol_forward_declaration_count ||
      inherited_edge_count !=
          surface.type_system_protocol_inheritance_edge_count ||
      required_method_count !=
          surface.type_system_protocol_required_method_count ||
      optional_method_count !=
          surface.type_system_protocol_optional_method_count ||
      required_property_count !=
          surface.type_system_protocol_required_property_count ||
      optional_property_count !=
          surface.type_system_protocol_optional_property_count) {
    error =
        "type-system protocol contract preservation dropped requirement or inheritance entries";
    return false;
  }

  surface.type_system_protocol_contract_preservation_present = true;
  return true;
}

bool PopulateImportedErrorHandlingResultAndBridgingArtifactReplay(
    const JsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *replay_value =
      FindMember(root, "objc_error_handling_result_and_bridging_artifact_replay");
  if (replay_value == nullptr) {
    return true;
  }

  const JsonValue::Object *replay_object = AsObject(*replay_value);
  if (replay_object == nullptr) {
    error =
        "error_handling result/bridging imported replay contract must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  if (!ReadStringMember(*replay_object, "contract_id", contract_id, error) ||
      !ReadStringMember(*replay_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadBoolMember(*replay_object, "binary_artifact_replay_ready",
                      surface.error_handling_binary_artifact_replay_ready, error) ||
      !ReadBoolMember(*replay_object, "runtime_import_artifact_ready",
                      surface.error_handling_runtime_import_artifact_ready, error) ||
      !ReadBoolMember(*replay_object, "separate_compilation_replay_ready",
                      surface.error_handling_separate_compilation_replay_ready, error) ||
      !ReadBoolMember(*replay_object, "deterministic",
                      surface.error_handling_deterministic, error) ||
      !ReadStringMember(*replay_object, "replay_key",
                        surface.error_handling_result_and_bridging_artifact_replay_key,
                        error) ||
      !ReadStringMember(*replay_object, "error_handling_replay_key",
                        surface.error_handling_error_handling_replay_key, error) ||
      !ReadStringMember(*replay_object, "throws_replay_key",
                        surface.error_handling_throws_replay_key, error) ||
      !ReadStringMember(*replay_object, "result_like_replay_key",
                        surface.error_handling_result_like_replay_key, error) ||
      !ReadStringMember(*replay_object, "ns_error_replay_key",
                        surface.error_handling_ns_error_replay_key, error) ||
      !ReadStringMember(*replay_object, "unwind_replay_key",
                        surface.error_handling_unwind_replay_key, error)) {
    return false;
  }

  if (contract_id != kObjc3ErrorHandlingResultAndBridgingArtifactReplayContractId) {
    error =
        "unexpected Part 6 result/bridging artifact replay contract id in import surface";
    return false;
  }
  if (source_contract_id != kObjc3ErrorHandlingThrowsAbiPropagationLoweringContractId) {
    error =
        "unexpected Part 6 throws ABI propagation source contract id in import surface";
    return false;
  }

  surface.error_handling_result_and_bridging_artifact_replay_present = true;
  surface.error_handling_contract_id = std::move(contract_id);
  surface.error_handling_source_contract_id = std::move(source_contract_id);
  return true;
}

bool PopulateImportedConcurrencyActorMailboxRuntimeImport(
    const JsonValue::Object &root, Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *runtime_value = FindMember(
      root, "objc_concurrency_actor_mailbox_and_isolation_runtime_import_surface");
  if (runtime_value == nullptr) {
    return true;
  }

  const JsonValue::Object *runtime_object = AsObject(*runtime_value);
  if (runtime_object == nullptr) {
    error = "concurrency actor mailbox runtime import surface must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  if (!ReadStringMember(*runtime_object, "contract_id", contract_id, error) ||
      !ReadStringMember(*runtime_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadBoolMember(*runtime_object, "actor_mailbox_runtime_ready",
                      surface.concurrency_actor_mailbox_runtime_ready, error) ||
      !ReadBoolMember(*runtime_object, "deterministic",
                      surface.concurrency_actor_mailbox_runtime_deterministic,
                      error) ||
      !ReadStringMember(*runtime_object, "replay_key",
                        surface.concurrency_actor_mailbox_runtime_replay_key,
                        error) ||
      !ReadStringMember(*runtime_object, "actor_lowering_replay_key",
                        surface.concurrency_actor_lowering_replay_key, error) ||
      !ReadStringMember(*runtime_object, "actor_isolation_lowering_replay_key",
                        surface.concurrency_actor_isolation_lowering_replay_key,
                        error)) {
    return false;
  }

  if (contract_id !=
      "objc3c.concurrency.actor.mailbox.isolation.import.surface.v1") {
    error = "unexpected Part 7 actor mailbox runtime import contract id in import surface";
    return false;
  }
  if (source_contract_id !=
      "objc3c.concurrency.actor.lowering.and.metadata.contract.v1") {
    error = "unexpected Part 7 actor mailbox runtime source contract id in import surface";
    return false;
  }

  if (!surface.concurrency_actor_mailbox_runtime_ready) {
    return true;
  }

  surface.concurrency_actor_mailbox_runtime_import_present = true;
  surface.concurrency_actor_mailbox_runtime_contract_id = std::move(contract_id);
  surface.concurrency_actor_mailbox_runtime_source_contract_id =
      std::move(source_contract_id);
  return true;
}

bool PopulateImportedMetaprogrammingModuleInterfaceReplayPreservation(
    const JsonValue::Object &root, Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *preservation_value = FindMember(
      root, kObjc3MetaprogrammingModuleInterfaceReplayPreservationImportArtifactMemberName);
  if (preservation_value == nullptr) {
    return true;
  }

  const JsonValue::Object *preservation_object = AsObject(*preservation_value);
  if (preservation_object == nullptr) {
    error =
        "metaprogramming module/interface replay preservation surface must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  if (!ReadStringMember(*preservation_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*preservation_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadBoolMember(*preservation_object, "runtime_import_artifact_ready",
                      surface.metaprogramming_runtime_import_artifact_ready, error) ||
      !ReadBoolMember(*preservation_object,
                      "separate_compilation_preservation_ready",
                      surface.metaprogramming_separate_compilation_preservation_ready,
                      error) ||
      !ReadBoolMember(*preservation_object, "deterministic",
                      surface.metaprogramming_deterministic, error) ||
      !ReadStringMember(*preservation_object, "replay_key",
                        surface.metaprogramming_replay_key, error) ||
      !ReadStringMember(*preservation_object, "expansion_lowering_replay_key",
                        surface.metaprogramming_expansion_lowering_replay_key, error) ||
      !ReadStringMember(*preservation_object,
                        "synthesized_emission_replay_key",
                        surface.metaprogramming_synthesized_emission_replay_key,
                        error) ||
      !ReadSizeMember(*preservation_object, "local_derive_method_count",
                      surface.metaprogramming_local_derive_method_count, error) ||
      !ReadSizeMember(*preservation_object, "local_macro_artifact_count",
                      surface.metaprogramming_local_macro_artifact_count, error) ||
      !ReadSizeMember(
          *preservation_object,
          "local_interface_property_behavior_artifact_count",
          surface.metaprogramming_local_interface_property_behavior_artifact_count,
          error) ||
      !ReadSizeMember(
          *preservation_object,
          "local_implementation_property_behavior_artifact_count",
          surface.metaprogramming_local_implementation_property_behavior_artifact_count,
          error) ||
      !ReadSizeMember(*preservation_object, "local_runtime_method_list_count",
                      surface.metaprogramming_local_runtime_method_list_count, error)) {
    return false;
  }

  if (contract_id !=
      kObjc3MetaprogrammingModuleInterfaceReplayPreservationContractId) {
    error =
        "unexpected Part 10 module/interface replay preservation contract id in import surface";
    return false;
  }
  if (source_contract_id !=
      kObjc3MetaprogrammingSynthesizedArtifactEmissionContractId) {
    error =
        "unexpected Part 10 module/interface replay preservation source contract id in import surface";
    return false;
  }

  surface.metaprogramming_module_interface_replay_preservation_present = true;
  surface.metaprogramming_contract_id = std::move(contract_id);
  surface.metaprogramming_source_contract_id = std::move(source_contract_id);
  return true;
}

bool PopulateImportedInteropForeignSurfaceInterfacePreservation(
    const JsonValue::Object &root, Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  // import-surface anchor: Part 11 preservation stays at the
  // manifest/runtime-import-surface layer so provider foreign/import and
  // C++/Swift-facing annotation facts survive separate compilation before any
  // ABI lowering or runnable bridge generation claims land.
  const JsonValue *preservation_value = FindMember(
      root, kObjc3InteropForeignSurfaceInterfacePreservationImportArtifactMemberName);
  if (preservation_value == nullptr) {
    return true;
  }

  const JsonValue::Object *preservation_object = AsObject(*preservation_value);
  if (preservation_object == nullptr) {
    error =
        "interop foreign surface interface preservation surface must be a JSON object";
    return false;
  }

  std::string contract_id;
  if (!ReadStringMember(*preservation_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*preservation_object,
                        "foreign_import_source_contract_id",
                        surface.interop_foreign_import_source_contract_id,
                        error) ||
      !ReadStringMember(*preservation_object, "cpp_swift_source_contract_id",
                        surface.interop_cpp_swift_source_contract_id, error) ||
      !ReadBoolMember(*preservation_object, "runtime_import_artifact_ready",
                      surface.interop_runtime_import_artifact_ready, error) ||
      !ReadBoolMember(*preservation_object,
                      "separate_compilation_preservation_ready",
                      surface.interop_separate_compilation_preservation_ready,
                      error) ||
      !ReadBoolMember(*preservation_object, "deterministic",
                      surface.interop_deterministic, error) ||
      !ReadStringMember(*preservation_object, "replay_key",
                        surface.interop_replay_key, error) ||
      !ReadStringMember(*preservation_object,
                        "foreign_import_source_replay_key",
                        surface.interop_foreign_import_source_replay_key,
                        error) ||
      !ReadStringMember(*preservation_object, "cpp_swift_source_replay_key",
                        surface.interop_cpp_swift_source_replay_key, error) ||
      !ReadSizeMember(*preservation_object, "local_foreign_callable_count",
                      surface.interop_local_foreign_callable_count, error) ||
      !ReadSizeMember(*preservation_object,
