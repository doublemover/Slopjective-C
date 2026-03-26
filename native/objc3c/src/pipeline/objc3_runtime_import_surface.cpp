#include "pipeline/objc3_runtime_import_surface.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "io/objc3_manifest_artifacts.h"
#include "lower/objc3_lowering_contract.h"

namespace {

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

std::string ReadTextFile(const std::filesystem::path &path, std::string &error) {
  std::ifstream input(path, std::ios::in | std::ios::binary);
  if (!input.is_open()) {
    error = "unable to open file";
    return "";
  }
  std::ostringstream out;
  out << input.rdbuf();
  if (!input.good() && !input.eof()) {
    error = "failed to read file";
    return "";
  }
  return out.str();
}

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

bool PopulateImportedPart3OptionalKeypathSurface(
    const JsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *lowering_value =
      FindMember(root, "objc_part3_optional_keypath_lowering_contract");
  const JsonValue *runtime_value =
      FindMember(root, "objc_part3_optional_keypath_runtime_helper_contract");
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
                      surface.part3_optional_send_sites, error) ||
      !ReadSizeMember(*lowering_object, "typed_keypath_literal_sites",
                      surface.part3_typed_keypath_literal_sites, error) ||
      !ReadSizeMember(*lowering_object, "live_optional_lowering_sites",
                      surface.part3_live_optional_lowering_sites, error) ||
      !ReadSizeMember(*lowering_object, "live_typed_keypath_artifact_sites",
                      surface.part3_live_typed_keypath_artifact_sites, error) ||
      !ReadBoolMember(*lowering_object, "ready_for_native_optional_lowering",
                      surface.part3_ready_for_native_optional_lowering, error) ||
      !ReadStringMember(*lowering_object, "replay_key", lowering_replay_key,
                        error) ||
      !ReadStringMember(*runtime_object, "contract_id", runtime_contract_id,
                        error) ||
      !ReadBoolMember(*runtime_object, "optional_send_runtime_ready",
                      surface.part3_optional_send_runtime_ready, error) ||
      !ReadBoolMember(*runtime_object, "typed_keypath_descriptor_handles_ready",
                      surface.part3_typed_keypath_descriptor_handles_ready,
                      error) ||
      !ReadBoolMember(*runtime_object,
                      "typed_keypath_runtime_execution_helper_landed",
                      surface.part3_typed_keypath_runtime_execution_helper_landed,
                      error) ||
      !ReadStringMember(*runtime_object, "replay_key", runtime_replay_key,
                        error)) {
    return false;
  }

  if (lowering_contract_id != kObjc3Part3OptionalKeypathLoweringContractId) {
    error = "unexpected optional/keypath lowering contract id in import surface";
    return false;
  }
  if (runtime_contract_id != kObjc3Part3OptionalKeypathRuntimeHelperContractId) {
    error =
        "unexpected optional/keypath runtime helper contract id in import surface";
    return false;
  }

  surface.part3_optional_keypath_lowering_contract_present = true;
  surface.part3_optional_keypath_lowering_replay_key =
      std::move(lowering_replay_key);
  surface.part3_optional_keypath_runtime_helper_replay_key =
      std::move(runtime_replay_key);
  return true;
}

bool PopulateImportedPart6ResultAndBridgingArtifactReplay(
    const JsonValue::Object &root,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *replay_value =
      FindMember(root, "objc_part6_result_and_bridging_artifact_replay");
  if (replay_value == nullptr) {
    return true;
  }

  const JsonValue::Object *replay_object = AsObject(*replay_value);
  if (replay_object == nullptr) {
    error =
        "part6 result/bridging imported replay contract must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  if (!ReadStringMember(*replay_object, "contract_id", contract_id, error) ||
      !ReadStringMember(*replay_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadBoolMember(*replay_object, "binary_artifact_replay_ready",
                      surface.part6_binary_artifact_replay_ready, error) ||
      !ReadBoolMember(*replay_object, "runtime_import_artifact_ready",
                      surface.part6_runtime_import_artifact_ready, error) ||
      !ReadBoolMember(*replay_object, "separate_compilation_replay_ready",
                      surface.part6_separate_compilation_replay_ready, error) ||
      !ReadBoolMember(*replay_object, "deterministic",
                      surface.part6_deterministic, error) ||
      !ReadStringMember(*replay_object, "replay_key",
                        surface.part6_result_and_bridging_artifact_replay_key,
                        error) ||
      !ReadStringMember(*replay_object, "part6_replay_key",
                        surface.part6_part6_replay_key, error) ||
      !ReadStringMember(*replay_object, "throws_replay_key",
                        surface.part6_throws_replay_key, error) ||
      !ReadStringMember(*replay_object, "result_like_replay_key",
                        surface.part6_result_like_replay_key, error) ||
      !ReadStringMember(*replay_object, "ns_error_replay_key",
                        surface.part6_ns_error_replay_key, error) ||
      !ReadStringMember(*replay_object, "unwind_replay_key",
                        surface.part6_unwind_replay_key, error)) {
    return false;
  }

  if (contract_id != kObjc3Part6ResultAndBridgingArtifactReplayContractId) {
    error =
        "unexpected Part 6 result/bridging artifact replay contract id in import surface";
    return false;
  }
  if (source_contract_id != kObjc3Part6ThrowsAbiPropagationLoweringContractId) {
    error =
        "unexpected Part 6 throws ABI propagation source contract id in import surface";
    return false;
  }

  surface.part6_result_and_bridging_artifact_replay_present = true;
  surface.part6_contract_id = std::move(contract_id);
  surface.part6_source_contract_id = std::move(source_contract_id);
  return true;
}

bool PopulateImportedPart7ActorMailboxRuntimeImport(
    const JsonValue::Object &root, Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *runtime_value = FindMember(
      root, "objc_part7_actor_mailbox_and_isolation_runtime_import_surface");
  if (runtime_value == nullptr) {
    return true;
  }

  const JsonValue::Object *runtime_object = AsObject(*runtime_value);
  if (runtime_object == nullptr) {
    error = "part7 actor mailbox runtime import surface must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  if (!ReadStringMember(*runtime_object, "contract_id", contract_id, error) ||
      !ReadStringMember(*runtime_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadBoolMember(*runtime_object, "actor_mailbox_runtime_ready",
                      surface.part7_actor_mailbox_runtime_ready, error) ||
      !ReadBoolMember(*runtime_object, "deterministic",
                      surface.part7_actor_mailbox_runtime_deterministic,
                      error) ||
      !ReadStringMember(*runtime_object, "replay_key",
                        surface.part7_actor_mailbox_runtime_replay_key,
                        error) ||
      !ReadStringMember(*runtime_object, "actor_lowering_replay_key",
                        surface.part7_actor_lowering_replay_key, error) ||
      !ReadStringMember(*runtime_object, "actor_isolation_lowering_replay_key",
                        surface.part7_actor_isolation_lowering_replay_key,
                        error)) {
    return false;
  }

  if (contract_id !=
      "objc3c.part7.actor.mailbox.isolation.import.surface.v1") {
    error = "unexpected Part 7 actor mailbox runtime import contract id in import surface";
    return false;
  }
  if (source_contract_id !=
      "objc3c.part7.actor.lowering.and.metadata.contract.v1") {
    error = "unexpected Part 7 actor mailbox runtime source contract id in import surface";
    return false;
  }

  if (!surface.part7_actor_mailbox_runtime_ready) {
    return true;
  }

  surface.part7_actor_mailbox_runtime_import_present = true;
  surface.part7_actor_mailbox_runtime_contract_id = std::move(contract_id);
  surface.part7_actor_mailbox_runtime_source_contract_id =
      std::move(source_contract_id);
  return true;
}

bool PopulateImportedPart10ModuleInterfaceReplayPreservation(
    const JsonValue::Object &root, Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *preservation_value = FindMember(
      root, kObjc3Part10ModuleInterfaceReplayPreservationImportArtifactMemberName);
  if (preservation_value == nullptr) {
    return true;
  }

  const JsonValue::Object *preservation_object = AsObject(*preservation_value);
  if (preservation_object == nullptr) {
    error =
        "part10 module/interface replay preservation surface must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  if (!ReadStringMember(*preservation_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*preservation_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadBoolMember(*preservation_object, "runtime_import_artifact_ready",
                      surface.part10_runtime_import_artifact_ready, error) ||
      !ReadBoolMember(*preservation_object,
                      "separate_compilation_preservation_ready",
                      surface.part10_separate_compilation_preservation_ready,
                      error) ||
      !ReadBoolMember(*preservation_object, "deterministic",
                      surface.part10_deterministic, error) ||
      !ReadStringMember(*preservation_object, "replay_key",
                        surface.part10_replay_key, error) ||
      !ReadStringMember(*preservation_object, "expansion_lowering_replay_key",
                        surface.part10_expansion_lowering_replay_key, error) ||
      !ReadStringMember(*preservation_object,
                        "synthesized_emission_replay_key",
                        surface.part10_synthesized_emission_replay_key,
                        error) ||
      !ReadSizeMember(*preservation_object, "local_derive_method_count",
                      surface.part10_local_derive_method_count, error) ||
      !ReadSizeMember(*preservation_object, "local_macro_artifact_count",
                      surface.part10_local_macro_artifact_count, error) ||
      !ReadSizeMember(
          *preservation_object,
          "local_interface_property_behavior_artifact_count",
          surface.part10_local_interface_property_behavior_artifact_count,
          error) ||
      !ReadSizeMember(
          *preservation_object,
          "local_implementation_property_behavior_artifact_count",
          surface.part10_local_implementation_property_behavior_artifact_count,
          error) ||
      !ReadSizeMember(*preservation_object, "local_runtime_method_list_count",
                      surface.part10_local_runtime_method_list_count, error)) {
    return false;
  }

  if (contract_id !=
      kObjc3Part10ModuleInterfaceReplayPreservationContractId) {
    error =
        "unexpected Part 10 module/interface replay preservation contract id in import surface";
    return false;
  }
  if (source_contract_id !=
      kObjc3Part10SynthesizedArtifactEmissionContractId) {
    error =
        "unexpected Part 10 module/interface replay preservation source contract id in import surface";
    return false;
  }

  surface.part10_module_interface_replay_preservation_present = true;
  surface.part10_contract_id = std::move(contract_id);
  surface.part10_source_contract_id = std::move(source_contract_id);
  return true;
}

bool PopulateImportedPart11ForeignSurfaceInterfacePreservation(
    const JsonValue::Object &root, Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  // import-surface anchor: Part 11 preservation stays at the
  // manifest/runtime-import-surface layer so provider foreign/import and
  // C++/Swift-facing annotation facts survive separate compilation before any
  // ABI lowering or runnable bridge generation claims land.
  const JsonValue *preservation_value = FindMember(
      root, kObjc3Part11ForeignSurfaceInterfacePreservationImportArtifactMemberName);
  if (preservation_value == nullptr) {
    return true;
  }

  const JsonValue::Object *preservation_object = AsObject(*preservation_value);
  if (preservation_object == nullptr) {
    error =
        "part11 foreign surface interface preservation surface must be a JSON object";
    return false;
  }

  std::string contract_id;
  if (!ReadStringMember(*preservation_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*preservation_object,
                        "foreign_import_source_contract_id",
                        surface.part11_foreign_import_source_contract_id,
                        error) ||
      !ReadStringMember(*preservation_object, "cpp_swift_source_contract_id",
                        surface.part11_cpp_swift_source_contract_id, error) ||
      !ReadBoolMember(*preservation_object, "runtime_import_artifact_ready",
                      surface.part11_runtime_import_artifact_ready, error) ||
      !ReadBoolMember(*preservation_object,
                      "separate_compilation_preservation_ready",
                      surface.part11_separate_compilation_preservation_ready,
                      error) ||
      !ReadBoolMember(*preservation_object, "deterministic",
                      surface.part11_deterministic, error) ||
      !ReadStringMember(*preservation_object, "replay_key",
                        surface.part11_replay_key, error) ||
      !ReadStringMember(*preservation_object,
                        "foreign_import_source_replay_key",
                        surface.part11_foreign_import_source_replay_key,
                        error) ||
      !ReadStringMember(*preservation_object, "cpp_swift_source_replay_key",
                        surface.part11_cpp_swift_source_replay_key, error) ||
      !ReadSizeMember(*preservation_object, "local_foreign_callable_count",
                      surface.part11_local_foreign_callable_count, error) ||
      !ReadSizeMember(*preservation_object,
                      "local_import_module_annotation_count",
                      surface.part11_local_import_module_annotation_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "local_imported_module_name_count",
                      surface.part11_local_imported_module_name_count, error) ||
      !ReadSizeMember(*preservation_object,
                      "local_swift_name_annotation_count",
                      surface.part11_local_swift_name_annotation_count,
                      error) ||
      !ReadSizeMember(*preservation_object,
                      "local_swift_private_annotation_count",
                      surface.part11_local_swift_private_annotation_count,
                      error) ||
      !ReadSizeMember(*preservation_object, "local_cpp_name_annotation_count",
                      surface.part11_local_cpp_name_annotation_count, error) ||
      !ReadSizeMember(*preservation_object,
                      "local_header_name_annotation_count",
                      surface.part11_local_header_name_annotation_count,
                      error) ||
      !ReadSizeMember(*preservation_object,
                      "local_named_annotation_payload_count",
                      surface.part11_local_named_annotation_payload_count,
                      error)) {
    return false;
  }

  if (contract_id != kObjc3Part11ForeignSurfaceInterfacePreservationContractId) {
    error =
        "unexpected Part 11 foreign surface interface preservation contract id in import surface";
    return false;
  }
  if (surface.part11_foreign_import_source_contract_id !=
      kObjc3Part11ForeignImportSourceClosureContractId) {
    error =
        "unexpected Part 11 foreign/import source contract id in import surface";
    return false;
  }
  if (surface.part11_cpp_swift_source_contract_id !=
      kObjc3Part11CppSwiftInteropAnnotationSourceCompletionContractId) {
    error =
        "unexpected Part 11 C++/Swift annotation source contract id in import surface";
    return false;
  }

  surface.part11_foreign_surface_interface_preservation_present = true;
  surface.part11_contract_id = std::move(contract_id);
  return true;
}

bool PopulateImportedPart10MacroHostProcessCacheRuntimeIntegration(
    const JsonValue::Object &root, Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *integration_value = FindMember(
      root,
      kObjc3Part10MacroHostProcessCacheRuntimeIntegrationImportArtifactMemberName);
  if (integration_value == nullptr) {
    return true;
  }

  const JsonValue::Object *integration_object = AsObject(*integration_value);
  if (integration_object == nullptr) {
    error =
        "part10 macro host process/cache runtime integration surface must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  if (!ReadStringMember(*integration_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*integration_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadBoolMember(*integration_object, "runtime_import_artifact_ready",
                      surface.part10_macro_host_process_cache_runtime_ready,
                      error) ||
      !ReadBoolMember(*integration_object, "separate_compilation_ready",
                      surface.part10_macro_host_process_cache_separate_compilation_ready,
                      error) ||
      !ReadBoolMember(*integration_object, "deterministic",
                      surface.part10_macro_host_process_cache_deterministic,
                      error) ||
      !ReadStringMember(
          *integration_object, "replay_key",
          surface.part10_macro_host_process_cache_replay_key, error) ||
      !ReadStringMember(*integration_object, "host_executable_relative_path",
                        surface
                            .part10_macro_host_process_cache_host_executable_relative_path,
                        error) ||
      !ReadStringMember(*integration_object, "cache_root_relative_path",
                        surface.part10_macro_host_process_cache_root_relative_path,
                        error)) {
    return false;
  }

  if (contract_id !=
      kObjc3Part10MacroHostProcessCacheRuntimeIntegrationContractId) {
    error =
        "unexpected Part 10 macro host process/cache runtime integration contract id in import surface";
    return false;
  }
  if (source_contract_id !=
      kObjc3Part10MacroHostProcessCacheRuntimeIntegrationSourceContractId) {
    error =
        "unexpected Part 10 macro host process/cache runtime integration source contract id in import surface";
    return false;
  }

  surface.part10_macro_host_process_cache_runtime_integration_present = true;
  surface.part10_macro_host_process_cache_contract_id = std::move(contract_id);
  surface.part10_macro_host_process_cache_source_contract_id =
      std::move(source_contract_id);
  return true;
}

bool PopulateImportedPart11FfiMetadataInterfacePreservation(
    const JsonValue::Object &root, Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *preservation_value = FindMember(
      root, kObjc3Part11FfiMetadataInterfacePreservationImportArtifactMemberName);
  if (preservation_value == nullptr) {
    return true;
  }

  const JsonValue::Object *preservation_object = AsObject(*preservation_value);
  if (preservation_object == nullptr) {
    error =
        "part11 ffi metadata/interface preservation surface must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  std::string preservation_contract_id;
  if (!ReadStringMember(*preservation_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*preservation_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadStringMember(*preservation_object, "preservation_contract_id",
                        preservation_contract_id, error) ||
      !ReadBoolMember(*preservation_object, "runtime_import_artifact_ready",
                      surface.part11_ffi_runtime_import_artifact_ready, error) ||
      !ReadBoolMember(*preservation_object,
                      "separate_compilation_preservation_ready",
                      surface.part11_ffi_separate_compilation_preservation_ready,
                      error) ||
      !ReadBoolMember(*preservation_object, "deterministic",
                      surface.part11_ffi_deterministic, error) ||
      !ReadStringMember(*preservation_object, "replay_key",
                        surface.part11_ffi_replay_key, error) ||
      !ReadStringMember(*preservation_object, "lowering_replay_key",
                        surface.part11_ffi_lowering_replay_key, error) ||
      !ReadStringMember(*preservation_object, "preservation_replay_key",
                        surface.part11_ffi_preservation_replay_key, error) ||
      !ReadSizeMember(*preservation_object, "local_foreign_callable_count",
                      surface.part11_ffi_local_foreign_callable_count, error) ||
      !ReadSizeMember(*preservation_object,
                      "local_metadata_preservation_sites",
                      surface.part11_ffi_local_metadata_preservation_sites,
                      error) ||
      !ReadSizeMember(*preservation_object,
                      "local_interface_annotation_sites",
                      surface.part11_ffi_local_interface_annotation_sites,
                      error)) {
    return false;
  }

  if (contract_id != kObjc3Part11FfiMetadataInterfacePreservationContractId) {
    error =
        "unexpected Part 11 ffi metadata/interface preservation contract id in import surface";
    return false;
  }
  if (source_contract_id !=
      kObjc3Part11FfiMetadataInterfacePreservationSourceContractId) {
    error =
        "unexpected Part 11 ffi metadata/interface preservation source contract id in import surface";
    return false;
  }
  if (preservation_contract_id !=
      kObjc3Part11ForeignSurfaceInterfacePreservationContractId) {
    error =
        "unexpected Part 11 ffi metadata/interface preservation dependency contract id in import surface";
    return false;
  }

  surface.part11_ffi_metadata_interface_preservation_present = true;
  surface.part11_ffi_contract_id = std::move(contract_id);
  surface.part11_ffi_source_contract_id = std::move(source_contract_id);
  surface.part11_ffi_preservation_contract_id =
      std::move(preservation_contract_id);
  return true;
}

bool PopulateImportedPart11HeaderModuleBridgeGeneration(
    const JsonValue::Object &root, Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *generation_value = FindMember(
      root, kObjc3Part11HeaderModuleBridgeGenerationImportArtifactMemberName);
  if (generation_value == nullptr) {
    return true;
  }

  const JsonValue::Object *generation_object = AsObject(*generation_value);
  if (generation_object == nullptr) {
    error =
        "part11 header/module/bridge generation surface must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  std::string preservation_contract_id;
  if (!ReadStringMember(*generation_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*generation_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadStringMember(*generation_object, "preservation_contract_id",
                        preservation_contract_id, error) ||
      !ReadStringMember(*generation_object, "header_artifact_relative_path",
                        surface.part11_bridge_header_artifact_relative_path,
                        error) ||
      !ReadStringMember(*generation_object, "module_artifact_relative_path",
                        surface.part11_bridge_module_artifact_relative_path,
                        error) ||
      !ReadStringMember(*generation_object, "bridge_artifact_relative_path",
                        surface.part11_bridge_artifact_relative_path, error) ||
      !ReadBoolMember(*generation_object, "runtime_generation_ready",
                      surface.part11_header_module_bridge_runtime_generation_ready,
                      error) ||
      !ReadBoolMember(
          *generation_object, "cross_module_packaging_ready",
          surface.part11_header_module_bridge_cross_module_packaging_ready,
          error) ||
      !ReadBoolMember(*generation_object, "deterministic",
                      surface.part11_header_module_bridge_deterministic,
                      error) ||
      !ReadStringMember(*generation_object, "replay_key",
                        surface.part11_header_module_bridge_replay_key, error) ||
      !ReadStringMember(*generation_object, "preservation_replay_key",
                        surface.part11_header_module_bridge_preservation_replay_key,
                        error) ||
      !ReadSizeMember(*generation_object, "local_foreign_callable_count",
                      surface.part11_header_module_bridge_local_foreign_callable_count,
                      error)) {
    return false;
  }

  if (contract_id != kObjc3Part11HeaderModuleBridgeGenerationContractId) {
    error =
        "unexpected Part 11 header/module/bridge generation contract id in import surface";
    return false;
  }
  if (source_contract_id !=
      kObjc3Part11HeaderModuleBridgeGenerationSourceContractId) {
    error =
        "unexpected Part 11 header/module/bridge generation source contract id in import surface";
    return false;
  }
  if (preservation_contract_id !=
      kObjc3Part11HeaderModuleBridgeGenerationPreservationContractId) {
    error =
        "unexpected Part 11 header/module/bridge generation preservation contract id in import surface";
    return false;
  }

  surface.part11_header_module_bridge_generation_present = true;
  surface.part11_header_module_bridge_contract_id = std::move(contract_id);
  surface.part11_header_module_bridge_source_contract_id =
      std::move(source_contract_id);
  surface.part11_header_module_bridge_preservation_contract_id =
      std::move(preservation_contract_id);
  return true;
}

bool PopulateImportedPart9DispatchMetadataInterfacePreservation(
    const JsonValue::Object &root, Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *preservation_value = FindMember(
      root, "objc_part9_dispatch_metadata_and_interface_preservation");
  if (preservation_value == nullptr) {
    return true;
  }

  const JsonValue::Object *preservation_object = AsObject(*preservation_value);
  if (preservation_object == nullptr) {
    error =
        "part9 dispatch metadata/interface preservation surface must be a JSON object";
    return false;
  }

  std::string contract_id;
  std::string source_contract_id;
  if (!ReadStringMember(*preservation_object, "contract_id", contract_id,
                        error) ||
      !ReadStringMember(*preservation_object, "source_contract_id",
                        source_contract_id, error) ||
      !ReadBoolMember(*preservation_object, "runtime_import_artifact_ready",
                      surface.part9_runtime_import_artifact_ready, error) ||
      !ReadBoolMember(*preservation_object,
                      "separate_compilation_preservation_ready",
                      surface.part9_separate_compilation_preservation_ready,
                      error) ||
      !ReadBoolMember(*preservation_object, "deterministic",
                      surface.part9_deterministic, error) ||
      !ReadStringMember(*preservation_object, "replay_key",
                        surface.part9_replay_key, error) ||
      !ReadStringMember(*preservation_object, "lowering_replay_key",
                        surface.part9_lowering_replay_key, error) ||
      !ReadSizeMember(*preservation_object,
                      "local_direct_callable_record_count",
                      surface.part9_local_direct_callable_record_count, error) ||
      !ReadSizeMember(*preservation_object,
                      "local_final_callable_record_count",
                      surface.part9_local_final_callable_record_count, error) ||
      !ReadSizeMember(*preservation_object,
                      "local_final_container_record_count",
                      surface.part9_local_final_container_record_count, error) ||
      !ReadSizeMember(*preservation_object,
                      "local_sealed_container_record_count",
                      surface.part9_local_sealed_container_record_count,
                      error)) {
    return false;
  }

  if (contract_id !=
      "objc3c.part9.dispatch.metadata.interface.preservation.v1") {
    error =
        "unexpected Part 9 dispatch metadata/interface preservation contract id in import surface";
    return false;
  }
  if (source_contract_id !=
      "objc3c.part9.dispatch.control.lowering.contract.v1") {
    error =
        "unexpected Part 9 dispatch metadata/interface preservation source contract id in import surface";
    return false;
  }

  surface.part9_dispatch_metadata_interface_preservation_present = true;
  surface.part9_contract_id = std::move(contract_id);
  surface.part9_source_contract_id = std::move(source_contract_id);
  return true;
}

bool ParseRuntimeMetadataSourceRecordSet(
    const JsonValue::Object &root, const std::string &declarations_name,
    Objc3RuntimeMetadataSourceRecordSet &record_set, std::string &error) {
  if (!ParseRecordArray(root, declarations_name, "classes",
                        record_set.classes_lexicographic, ParseClassRecord,
                        error) ||
      !ParseRecordArray(root, declarations_name, "protocols",
                        record_set.protocols_lexicographic,
                        ParseProtocolRecord, error) ||
      !ParseRecordArray(root, declarations_name, "categories",
                        record_set.categories_lexicographic, ParseCategoryRecord,
                        error) ||
      !ParseRecordArray(root, declarations_name, "properties",
                        record_set.properties_lexicographic, ParsePropertyRecord,
                        error) ||
      !ParseRecordArray(root, declarations_name, "methods",
                        record_set.methods_lexicographic, ParseMethodRecord,
                        error) ||
      !ParseRecordArray(root, declarations_name, "ivars",
                        record_set.ivars_lexicographic, ParseIvarRecord,
                        error)) {
    return false;
  }
  record_set.deterministic = true;
  return true;
}

std::size_t CountRuntimeMetadataSourceRecordSetDeclarations(
    const Objc3RuntimeMetadataSourceRecordSet &record_set) {
  return record_set.classes_lexicographic.size() +
         record_set.protocols_lexicographic.size() +
         record_set.categories_lexicographic.size() +
         record_set.properties_lexicographic.size() +
         record_set.methods_lexicographic.size() +
         record_set.ivars_lexicographic.size();
}

std::size_t CountRuntimeMetadataSourceRecordSetReferences(
    const Objc3RuntimeMetadataSourceRecordSet &record_set) {
  std::size_t references = 0;
  for (const auto &class_record : record_set.classes_lexicographic) {
    if (class_record.has_super && !class_record.super_name.empty()) {
      ++references;
    }
    references += class_record.adopted_protocols_lexicographic.size();
  }
  for (const auto &protocol_record : record_set.protocols_lexicographic) {
    references += protocol_record.inherited_protocols_lexicographic.size();
  }
  for (const auto &category_record : record_set.categories_lexicographic) {
    references += category_record.adopted_protocols_lexicographic.size();
  }
  for (const auto &property_record : record_set.properties_lexicographic) {
    if (!property_record.effective_getter_selector.empty()) {
      ++references;
    }
    if (property_record.effective_setter_available &&
        !property_record.effective_setter_selector.empty()) {
      ++references;
    }
    if (!property_record.ivar_binding_symbol.empty()) {
      ++references;
    }
  }
  for (const auto &method_record : record_set.methods_lexicographic) {
    if (!method_record.selector.empty()) {
      ++references;
    }
  }
  return references;
}

bool ParseSerializedRuntimeMetadataReusePayload(
    const JsonValue::Object &root, Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  const JsonValue *payload_value =
      FindMember(root, kObjc3SerializedRuntimeMetadataArtifactReusePayloadMemberName);
  if (payload_value == nullptr) {
    surface.reused_module_names_lexicographic = {
        surface.frontend_closure_summary.module_name};
    surface.uses_serialized_runtime_metadata_payload = false;
    return true;
  }
  const JsonValue::Object *payload_object = AsObject(*payload_value);
  if (payload_object == nullptr) {
    error = "serialized runtime metadata reuse payload must be an object";
    return false;
  }

  std::string contract_id;
  std::string module_name;
  std::string replay_key;
  std::vector<std::string> reused_module_names;
  std::size_t runtime_owned_declaration_count = 0;
  std::size_t metadata_reference_count = 0;
  bool ready = false;
  if (!ReadStringMember(*payload_object, "contract_id", contract_id, error) ||
      !ReadStringMember(*payload_object, "module_name", module_name, error) ||
      !ReadStringArrayMember(*payload_object, "reused_module_names_lexicographic",
                             reused_module_names, error) ||
      !ReadSizeMember(*payload_object, "runtime_owned_declaration_count",
                      runtime_owned_declaration_count, error) ||
      !ReadSizeMember(*payload_object, "metadata_reference_count",
                      metadata_reference_count, error) ||
      !ReadBoolMember(*payload_object, "ready", ready, error) ||
      !ReadStringMember(*payload_object, "replay_key", replay_key, error)) {
    return false;
  }
  if (contract_id != kObjc3SerializedRuntimeMetadataArtifactReuseContractId) {
    error = "unexpected serialized runtime metadata reuse payload contract id";
    return false;
  }
  if (!ready) {
    error = "serialized runtime metadata reuse payload is not ready";
    return false;
  }
  if (reused_module_names.empty()) {
    error = "serialized runtime metadata reuse payload must list reused modules";
    return false;
  }

  Objc3RuntimeMetadataSourceRecordSet payload_record_set;
  if (!ParseRuntimeMetadataSourceRecordSet(*payload_object,
                                           "runtime_owned_declarations",
                                           payload_record_set, error)) {
    return false;
  }
  const JsonValue *references_value =
      FindMember(*payload_object, "metadata_references");
  if (references_value == nullptr) {
    error = "serialized runtime metadata reuse payload is missing metadata_references";
    return false;
  }
  const JsonValue::Array *references_array = AsArray(*references_value);
  if (references_array == nullptr) {
    error =
        "serialized runtime metadata reuse payload metadata_references must be an array";
    return false;
  }
  if (CountRuntimeMetadataSourceRecordSetDeclarations(payload_record_set) !=
      runtime_owned_declaration_count) {
    error =
        "serialized runtime metadata reuse payload declaration count does not match payload inventory";
    return false;
  }
  if (references_array->size() != metadata_reference_count ||
      CountRuntimeMetadataSourceRecordSetReferences(payload_record_set) !=
          metadata_reference_count) {
    error =
        "serialized runtime metadata reuse payload reference count does not match payload inventory";
    return false;
  }

  surface.runtime_metadata_source_records = std::move(payload_record_set);
  surface.reused_module_names_lexicographic = std::move(reused_module_names);
  surface.uses_serialized_runtime_metadata_payload = true;
  (void)module_name;
  (void)replay_key;
  return true;
}

bool ParseImportedRuntimeModuleSurface(const JsonValue::Object &root,
                                       Objc3ImportedRuntimeModuleSurface &surface,
                                       std::string &error) {
  if (!PopulateFrontendClosureSummary(root,
                                      surface.frontend_closure_summary,
                                      error)) {
    return false;
  }
  if (!PopulateImportedPart3OptionalKeypathSurface(root, surface, error)) {
    return false;
  }
  if (!PopulateImportedPart6ResultAndBridgingArtifactReplay(root, surface,
                                                            error)) {
    return false;
  }
  if (!PopulateImportedPart7ActorMailboxRuntimeImport(root, surface, error)) {
    return false;
  }
  if (!PopulateImportedPart11ForeignSurfaceInterfacePreservation(root, surface,
                                                                 error)) {
    return false;
  }
  if (!PopulateImportedPart11FfiMetadataInterfacePreservation(root, surface,
                                                              error)) {
    return false;
  }
  if (!PopulateImportedPart11HeaderModuleBridgeGeneration(root, surface,
                                                          error)) {
    return false;
  }
  if (!PopulateImportedPart10ModuleInterfaceReplayPreservation(root, surface,
                                                               error)) {
    return false;
  }
  if (!PopulateImportedPart10MacroHostProcessCacheRuntimeIntegration(root,
                                                                     surface,
                                                                     error)) {
    return false;
  }
  if (!PopulateImportedPart9DispatchMetadataInterfacePreservation(root, surface,
                                                                  error)) {
    return false;
  }
  Objc3RuntimeMetadataSourceRecordSet local_runtime_metadata_source_records;
  if (!ParseRuntimeMetadataSourceRecordSet(root, "runtime_owned_declarations",
                                           local_runtime_metadata_source_records,
                                           error)) {
    return false;
  }
  surface.frontend_closure_summary.runtime_metadata_source_records_ready = true;

  const JsonValue *references_value = FindMember(root, "metadata_references");
  if (references_value == nullptr) {
    error = "missing JSON array member 'metadata_references'";
    return false;
  }
  const JsonValue::Array *references_array = AsArray(*references_value);
  if (references_array == nullptr) {
    error = "JSON member 'metadata_references' must be an array";
    return false;
  }

  surface.frontend_closure_summary.class_record_count =
      local_runtime_metadata_source_records.classes_lexicographic.size();
  surface.frontend_closure_summary.protocol_record_count =
      local_runtime_metadata_source_records.protocols_lexicographic.size();
  surface.frontend_closure_summary.category_record_count =
      local_runtime_metadata_source_records.categories_lexicographic.size();
  surface.frontend_closure_summary.property_record_count =
      local_runtime_metadata_source_records.properties_lexicographic.size();
  surface.frontend_closure_summary.method_record_count =
      local_runtime_metadata_source_records.methods_lexicographic.size();
  surface.frontend_closure_summary.ivar_record_count =
      local_runtime_metadata_source_records.ivars_lexicographic.size();

  surface.frontend_closure_summary.superclass_reference_count = 0;
  surface.frontend_closure_summary.protocol_reference_count = 0;
  surface.frontend_closure_summary.property_accessor_reference_count = 0;
  surface.frontend_closure_summary.property_ivar_binding_reference_count = 0;
  surface.frontend_closure_summary.method_selector_reference_count = 0;
  for (const JsonValue &reference_value : *references_array) {
    const JsonValue::Object *reference_object = AsObject(reference_value);
    if (reference_object == nullptr) {
      error = "metadata_references must contain objects";
      return false;
    }
    std::string reference_kind;
    if (!ReadStringMember(*reference_object, "reference_kind", reference_kind,
                          error)) {
      return false;
    }
    if (reference_kind == "class-superclass") {
      ++surface.frontend_closure_summary.superclass_reference_count;
    } else if (reference_kind == "class-adopted-protocol" ||
               reference_kind == "protocol-inherited-protocol" ||
               reference_kind == "category-adopted-protocol") {
      ++surface.frontend_closure_summary.protocol_reference_count;
    } else if (reference_kind == "property-getter-selector" ||
               reference_kind == "property-setter-selector") {
      ++surface.frontend_closure_summary.property_accessor_reference_count;
    } else if (reference_kind == "property-ivar-binding") {
      ++surface.frontend_closure_summary.property_ivar_binding_reference_count;
    } else if (reference_kind == "method-selector") {
      ++surface.frontend_closure_summary.method_selector_reference_count;
    }
  }

  const std::size_t declaration_count =
      CountRuntimeMetadataSourceRecordSetDeclarations(
          local_runtime_metadata_source_records);
  if (surface.frontend_closure_summary.runtime_owned_declaration_count !=
      declaration_count) {
    error = "runtime-owned declaration count does not match imported record inventory";
    return false;
  }
  if (surface.frontend_closure_summary.metadata_reference_count !=
      references_array->size()) {
    error = "metadata reference count does not match imported reference inventory";
    return false;
  }
  if (surface.frontend_closure_summary.contract_id !=
      kObjc3RuntimeAwareImportModuleFrontendClosureContractId) {
    error = "unexpected import-surface contract id";
    return false;
  }
  if (surface.frontend_closure_summary.source_surface_contract_id !=
      kObjc3RuntimeAwareImportModuleSurfaceContractId) {
    error = "unexpected import-surface source contract id";
    return false;
  }
  if (surface.frontend_closure_summary.frontend_surface_path !=
      kObjc3RuntimeAwareImportModuleFrontendClosureSurfacePath) {
    error = "unexpected import-surface frontend surface path";
    return false;
  }
  if (surface.frontend_closure_summary.payload_model !=
      kObjc3RuntimeAwareImportModuleFrontendClosurePayloadModel) {
    error = "unexpected import-surface payload model";
    return false;
  }
  if (surface.frontend_closure_summary.artifact_relative_path !=
      kObjc3RuntimeAwareImportModuleFrontendClosureArtifactRelativePath) {
    error = "unexpected import-surface artifact name";
    return false;
  }
  if (surface.frontend_closure_summary.authority_model !=
      kObjc3RuntimeAwareImportModuleFrontendAuthorityModel) {
    error = "unexpected import-surface authority model";
    return false;
  }
  if (surface.frontend_closure_summary.payload_ownership_model !=
      kObjc3RuntimeAwareImportModuleFrontendPayloadOwnershipModel) {
    error = "unexpected import-surface ownership model";
    return false;
  }
  if (!surface.frontend_closure_summary.ready_for_frontend_module_consumption) {
    error = "import-surface artifact is not ready for frontend module consumption";
    return false;
  }
  if (!IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(
          surface.frontend_closure_summary)) {
    error = "import-surface closure summary is incomplete";
    return false;
  }
  if (!ParseSerializedRuntimeMetadataReusePayload(root, surface, error)) {
    return false;
  }
  if (!surface.uses_serialized_runtime_metadata_payload) {
    surface.runtime_metadata_source_records =
        std::move(local_runtime_metadata_source_records);
  }
  return true;
}

bool PopulateImportedRuntimeRegistrationManifestPeerArtifacts(
    const JsonValue::Object &root,
    Objc3ImportedRuntimeModulePackagingPeerArtifacts &artifacts,
    std::string &error) {
  std::string contract_id;
  bool ready_for_runtime_bootstrap_enforcement = false;
  if (!ReadStringMember(root, "contract_id", contract_id, error) ||
      !ReadStringMember(root, "runtime_support_library_archive_relative_path",
                        artifacts.runtime_support_library_archive_relative_path,
                        error) ||
      !ReadStringMember(root, "translation_unit_identity_model",
                        artifacts.translation_unit_identity_model, error) ||
      !ReadStringMember(root, "translation_unit_identity_key",
                        artifacts.translation_unit_identity_key, error) ||
      !ReadStringMember(root, "object_format", artifacts.object_format, error) ||
      !ReadUnsignedMember(root, "translation_unit_registration_order_ordinal",
                          artifacts.translation_unit_registration_order_ordinal,
                          error) ||
      !ReadStringArrayMember(root, "driver_linker_flags",
                             artifacts.driver_linker_flags, error) ||
      !ReadBoolMember(root, "ready_for_runtime_bootstrap_enforcement",
                      ready_for_runtime_bootstrap_enforcement, error)) {
    return false;
  }
  if (contract_id !=
      kObjc3RuntimeTranslationUnitRegistrationManifestContractId) {
    error = "unexpected runtime registration manifest contract id";
    return false;
  }
  if (!ready_for_runtime_bootstrap_enforcement) {
    error =
        "runtime registration manifest is not ready for runtime bootstrap enforcement";
    return false;
  }
  for (const auto &flag : artifacts.driver_linker_flags) {
    if (flag.empty()) {
      error = "runtime registration manifest contains an empty driver linker flag";
      return false;
    }
  }
  return true;
}

bool ValidateImportedRuntimeDiscoveryPeerArtifacts(
    const JsonValue::Object &root,
    const Objc3ImportedRuntimeModulePackagingPeerArtifacts &artifacts,
    std::string &object_artifact_relative_path, std::string &error) {
  std::string contract_id;
  std::string translation_unit_identity_model;
  std::string translation_unit_identity_key;
  std::string object_format;
  std::vector<std::string> driver_linker_flags;
  if (!ReadStringMember(root, "contract_id", contract_id, error) ||
      !ReadStringMember(root, "object_artifact",
                        object_artifact_relative_path, error) ||
      !ReadStringMember(root, "translation_unit_identity_model",
                        translation_unit_identity_model, error) ||
      !ReadStringMember(root, "translation_unit_identity_key",
                        translation_unit_identity_key, error) ||
      !ReadStringMember(root, "object_format", object_format, error) ||
      !ReadStringArrayMember(root, "driver_linker_flags", driver_linker_flags,
                             error)) {
    return false;
  }
  if (contract_id != kObjc3RuntimeLinkerRetentionContractId) {
    error = "unexpected runtime metadata discovery contract id";
    return false;
  }
  if (translation_unit_identity_model !=
      artifacts.translation_unit_identity_model) {
    error =
        "runtime metadata discovery translation-unit identity model mismatch";
    return false;
  }
  if (translation_unit_identity_key != artifacts.translation_unit_identity_key) {
    error = "runtime metadata discovery translation-unit identity key mismatch";
    return false;
  }
  if (object_format != artifacts.object_format) {
    error = "runtime metadata discovery object format mismatch";
    return false;
  }
  if (driver_linker_flags != artifacts.driver_linker_flags) {
    error = "runtime metadata discovery driver linker flags mismatch";
    return false;
  }
  return true;
}

}  // namespace

bool TryLoadObjc3ImportedRuntimeModuleSurface(
    const std::filesystem::path &path,
    Objc3ImportedRuntimeModuleSurface &surface,
    std::string &error) {
  std::string io_error;
  const std::string payload = ReadTextFile(path, io_error);
  if (!io_error.empty()) {
    error = path.generic_string() + ": " + io_error;
    return false;
  }

  JsonParser parser(payload);
  JsonValue root_value;
  std::string parse_error;
  if (!parser.Parse(root_value, parse_error)) {
    error = path.generic_string() + ": " + parse_error;
    return false;
  }
  const JsonValue::Object *root_object = AsObject(root_value);
  if (root_object == nullptr) {
    error = path.generic_string() + ": import surface payload must be a JSON object";
    return false;
  }

  Objc3ImportedRuntimeModuleSurface parsed_surface;
  parsed_surface.source_path = path;
  if (!ParseImportedRuntimeModuleSurface(*root_object, parsed_surface,
                                         parse_error)) {
    error = path.generic_string() + ": " + parse_error;
    return false;
  }
  surface = std::move(parsed_surface);
  return true;
}

bool TryLoadObjc3ImportedRuntimeModulePackagingPeerArtifacts(
    const Objc3ImportedRuntimeModuleSurface &surface,
    Objc3ImportedRuntimeModulePackagingPeerArtifacts &artifacts,
    std::string &error) {
  artifacts = Objc3ImportedRuntimeModulePackagingPeerArtifacts{};
  error.clear();

  std::string emit_prefix;
  if (!TryResolveEmitPrefixFromImportSurfacePath(surface.source_path, emit_prefix,
                                                 error)) {
    error = surface.source_path.generic_string() + ": " + error;
    return false;
  }

  const std::filesystem::path parent = surface.source_path.parent_path();
  const std::filesystem::path registration_manifest_path =
      BuildRuntimeRegistrationManifestArtifactPath(parent, emit_prefix)
          .lexically_normal();
  const std::filesystem::path discovery_artifact_path =
      BuildRuntimeMetadataDiscoveryArtifactPath(parent, emit_prefix)
          .lexically_normal();
  const std::filesystem::path linker_response_artifact_path =
      BuildRuntimeMetadataLinkerResponseArtifactPath(parent, emit_prefix)
          .lexically_normal();

  std::string manifest_io_error;
  const std::string manifest_payload =
      ReadTextFile(registration_manifest_path, manifest_io_error);
  if (!manifest_io_error.empty()) {
    error = registration_manifest_path.generic_string() + ": " + manifest_io_error;
    return false;
  }
  JsonParser manifest_parser(manifest_payload);
  JsonValue manifest_root_value;
  std::string manifest_parse_error;
  if (!manifest_parser.Parse(manifest_root_value, manifest_parse_error)) {
    error = registration_manifest_path.generic_string() + ": " +
            manifest_parse_error;
    return false;
  }
  const JsonValue::Object *manifest_root_object = AsObject(manifest_root_value);
  if (manifest_root_object == nullptr) {
    error = registration_manifest_path.generic_string() +
            ": runtime registration manifest payload must be a JSON object";
    return false;
  }

  Objc3ImportedRuntimeModulePackagingPeerArtifacts parsed_artifacts;
  if (!PopulateImportedRuntimeRegistrationManifestPeerArtifacts(
          *manifest_root_object, parsed_artifacts, manifest_parse_error)) {
    error = registration_manifest_path.generic_string() + ": " +
            manifest_parse_error;
    return false;
  }

  std::string discovery_io_error;
  const std::string discovery_payload =
      ReadTextFile(discovery_artifact_path, discovery_io_error);
  if (!discovery_io_error.empty()) {
    error = discovery_artifact_path.generic_string() + ": " + discovery_io_error;
    return false;
  }
  JsonParser discovery_parser(discovery_payload);
  JsonValue discovery_root_value;
  std::string discovery_parse_error;
  if (!discovery_parser.Parse(discovery_root_value, discovery_parse_error)) {
    error = discovery_artifact_path.generic_string() + ": " +
            discovery_parse_error;
    return false;
  }
  const JsonValue::Object *discovery_root_object = AsObject(discovery_root_value);
  if (discovery_root_object == nullptr) {
    error = discovery_artifact_path.generic_string() +
            ": runtime metadata discovery payload must be a JSON object";
    return false;
  }

  std::string object_artifact_relative_path;
  if (!ValidateImportedRuntimeDiscoveryPeerArtifacts(*discovery_root_object,
                                                     parsed_artifacts,
                                                     object_artifact_relative_path,
                                                     discovery_parse_error)) {
    error = discovery_artifact_path.generic_string() + ": " +
            discovery_parse_error;
    return false;
  }

  std::string response_io_error;
  const std::string linker_response_payload =
      ReadTextFile(linker_response_artifact_path, response_io_error);
  if (!response_io_error.empty()) {
    error = linker_response_artifact_path.generic_string() + ": " +
            response_io_error;
    return false;
  }
  const std::vector<std::string> response_flags =
      SplitNonEmptyLines(linker_response_payload);
  if (response_flags != parsed_artifacts.driver_linker_flags) {
    error = linker_response_artifact_path.generic_string() +
            ": linker response payload drifted from imported driver linker flags";
    return false;
  }

  const std::filesystem::path object_artifact_path =
      (parent / object_artifact_relative_path).lexically_normal();
  if (!std::filesystem::exists(object_artifact_path)) {
    error = object_artifact_path.generic_string() +
            ": imported object artifact is missing";
    return false;
  }

  parsed_artifacts.registration_manifest_path = registration_manifest_path;
  parsed_artifacts.discovery_artifact_path = discovery_artifact_path;
  parsed_artifacts.linker_response_artifact_path = linker_response_artifact_path;
  parsed_artifacts.object_artifact_path = object_artifact_path;
  artifacts = std::move(parsed_artifacts);
  return true;
}
