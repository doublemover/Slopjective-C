#include "ir/objc3_ir_class_receiver_bindings.h"

#include <cstddef>
#include <set>
#include <string>
#include <unordered_map>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_receiver_identity_contracts.h"

std::unordered_map<std::string, int> BuildObjc3IRKnownClassReceiverConstants(
    const Objc3Program &program) {
  std::set<std::string> class_names;
  for (const auto &interface_decl : program.interfaces) {
    if (!interface_decl.has_category && !interface_decl.name.empty()) {
      class_names.insert(interface_decl.name);
    }
  }
  for (const auto &implementation : program.implementations) {
    if (!implementation.has_category && !implementation.name.empty()) {
      class_names.insert(implementation.name);
    }
  }

  std::unordered_map<std::string, int> class_receiver_constants;
  std::size_t ordinal = 0;
  for (const std::string &class_name : class_names) {
    class_receiver_constants[class_name] =
        NextNonZeroReceiverIdentityValue(ordinal++, 0);
  }
  return class_receiver_constants;
}

int LookupObjc3IRClassReceiverIdentityValue(
    const std::unordered_map<std::string, int> &class_receiver_constants,
    const std::string &class_name) {
  const auto value_it = class_receiver_constants.find(class_name);
  if (value_it == class_receiver_constants.end()) {
    return 0;
  }
  return value_it->second;
}

void SeedObjc3IRKnownClassReceiverBindings(
    const std::unordered_map<std::string, int> &class_receiver_constants,
    FunctionContext &ctx) {
  for (const auto &entry : class_receiver_constants) {
    ctx.immediate_identifiers.emplace(entry.first, entry.second);
  }
}
