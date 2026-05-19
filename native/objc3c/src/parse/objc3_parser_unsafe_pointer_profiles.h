#pragma once

#include <cstddef>
#include <string>

#include "ast/objc3_ast_function_decl_nodes.h"
#include "ast/objc3_ast_method_decl_nodes.h"

namespace objc3c::parse {

struct Objc3UnsafePointerExtensionProfile {
  std::size_t unsafe_pointer_extension_sites = 0;
  std::size_t unsafe_keyword_sites = 0;
  std::size_t pointer_arithmetic_sites = 0;
  std::size_t raw_pointer_type_sites = 0;
  std::size_t unsafe_operation_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic_unsafe_pointer_extension_handoff = false;
};

std::string BuildUnsafePointerExtensionProfile(
    std::size_t unsafe_pointer_extension_sites,
    std::size_t unsafe_keyword_sites,
    std::size_t pointer_arithmetic_sites,
    std::size_t raw_pointer_type_sites,
    std::size_t unsafe_operation_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites,
    bool deterministic_unsafe_pointer_extension_handoff);

bool IsUnsafePointerExtensionProfileNormalized(
    std::size_t unsafe_pointer_extension_sites,
    std::size_t unsafe_keyword_sites,
    std::size_t pointer_arithmetic_sites,
    std::size_t raw_pointer_type_sites,
    std::size_t unsafe_operation_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites);

Objc3UnsafePointerExtensionProfile BuildUnsafePointerExtensionProfileFromFunction(
    const FunctionDecl &fn);

Objc3UnsafePointerExtensionProfile BuildUnsafePointerExtensionProfileFromOpaqueBody(
    const Objc3MethodDecl &method);

}  // namespace objc3c::parse
