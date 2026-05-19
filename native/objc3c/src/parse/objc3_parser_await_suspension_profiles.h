#pragma once

#include <cstddef>
#include <string>

#include "ast/objc3_ast_function_decl_nodes.h"
#include "ast/objc3_ast_method_decl_nodes.h"

namespace objc3c::parse {

struct Objc3AwaitSuspensionProfile {
  std::size_t await_suspension_sites = 0;
  std::size_t await_keyword_sites = 0;
  std::size_t await_suspension_point_sites = 0;
  std::size_t await_resume_sites = 0;
  std::size_t await_state_machine_sites = 0;
  std::size_t await_continuation_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic_await_suspension_handoff = false;
};

std::string BuildAwaitSuspensionProfile(
    std::size_t await_suspension_sites,
    std::size_t await_keyword_sites,
    std::size_t await_suspension_point_sites,
    std::size_t await_resume_sites,
    std::size_t await_state_machine_sites,
    std::size_t await_continuation_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites,
    bool deterministic_await_suspension_handoff);

bool IsAwaitSuspensionProfileNormalized(
    std::size_t await_suspension_sites,
    std::size_t await_keyword_sites,
    std::size_t await_suspension_point_sites,
    std::size_t await_resume_sites,
    std::size_t await_state_machine_sites,
    std::size_t await_continuation_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites);

Objc3AwaitSuspensionProfile BuildAwaitSuspensionProfileFromFunction(
    const FunctionDecl &fn);

Objc3AwaitSuspensionProfile BuildAwaitSuspensionProfileFromOpaqueBody(
    const Objc3MethodDecl &method);

}  // namespace objc3c::parse
