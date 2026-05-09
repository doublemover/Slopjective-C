#include "ir/objc3_ir_emission_helpers.h"

#include "parse/objc3_parse_support.h"

#include <cctype>

bool ParseOwnershipResourceInvalidLiteral(const std::string &text, int &value) {
  std::string normalized;
  normalized.reserve(text.size());
  for (unsigned char ch : text) {
    if (!std::isspace(ch)) {
      normalized.push_back(static_cast<char>(ch));
    }
  }
  if (normalized.empty()) {
    return false;
  }
  bool negative = false;
  if (normalized.front() == '+' || normalized.front() == '-') {
    negative = normalized.front() == '-';
    normalized.erase(normalized.begin());
  }
  if (normalized.empty() ||
      !objc3c::parse::support::ParseIntegerLiteralValue(normalized, value)) {
    return false;
  }
  if (negative) {
    value = -value;
  }
  return true;
}

std::string BuildSynthesizedInstanceMethodOwnerIdentity(
    const std::string &declaration_owner_identity,
    const std::string &selector) {
  return declaration_owner_identity + "::instance_method:" + selector;
}

std::string BuildSynthesizedPropertyStorageSymbol(
    const std::string &binding_symbol) {
  std::string out = "objc3_property_storage_";
  out.reserve(out.size() + binding_symbol.size());
  for (unsigned char ch : binding_symbol) {
    if (std::isalnum(ch) != 0) {
      out.push_back(static_cast<char>(ch));
    } else {
      out.push_back('_');
    }
  }
  if (out == "objc3_property_storage_") {
    out += "anonymous";
  }
  return out;
}

std::string BuildDirectDispatchMethodKey(
    const std::string &implementation_name, const std::string &selector,
    bool is_class_method) {
  return implementation_name + "|" + (is_class_method ? "class" : "instance") +
         "|" + selector;
}

ValueType RuntimeMetadataValueType(const std::string &type_name) {
  if (type_name == "void") {
    return ValueType::Void;
  }
  if (type_name == "bool") {
    return ValueType::Bool;
  }
  if (type_name.empty()) {
    return ValueType::Unknown;
  }
  return ValueType::I32;
}
