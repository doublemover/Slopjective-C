#include "ir/objc3_ir_symbol_model.h"

#include <cctype>

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

std::string BuildImplementationMethodFunctionSymbol(
    const std::string &implementation_name, const std::string &selector,
    bool is_class_method) {
  auto sanitize = [](const std::string &text) {
    std::string out;
    out.reserve(text.size());
    for (unsigned char ch : text) {
      if (std::isalnum(ch) != 0) {
        out.push_back(static_cast<char>(ch));
      } else {
        out.push_back('_');
      }
    }
    if (out.empty()) {
      out = "anonymous";
    }
    return out;
  };
  const std::string owner = sanitize(implementation_name);
  const std::string selector_key = sanitize(selector);
  const char *dispatch_kind = is_class_method ? "class" : "instance";
  return "objc3_method_" + owner + "_" + dispatch_kind + "_" + selector_key;
}
