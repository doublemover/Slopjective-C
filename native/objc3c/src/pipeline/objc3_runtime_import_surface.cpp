#include "pipeline/objc3_runtime_import_surface.h"

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

bool ParseClassRecord(const JsonValue::Object &object,
                      Objc3RuntimeMetadataClassSourceRecord &record,
                      std::string &error) {
  return ReadStringMember(object, "record_kind", record.record_kind, error) &&
         ReadStringMember(object, "name", record.name, error) &&
         ReadOptionalStringMember(object, "super_name", record.super_name, error) &&
         ReadBoolMember(object, "has_super", record.has_super, error) &&
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
