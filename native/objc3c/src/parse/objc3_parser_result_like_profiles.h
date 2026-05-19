#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "ast/objc3_ast_core.h"

namespace objc3c::parse {

struct Objc3ResultLikeProfile {
  std::size_t result_like_sites = 0;
  std::size_t result_success_sites = 0;
  std::size_t result_failure_sites = 0;
  std::size_t result_branch_sites = 0;
  std::size_t result_payload_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t branch_merge_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic_result_like_lowering_handoff = false;
};

std::string BuildResultLikeProfile(
    std::size_t result_like_sites,
    std::size_t result_success_sites,
    std::size_t result_failure_sites,
    std::size_t result_branch_sites,
    std::size_t result_payload_sites,
    std::size_t normalized_sites,
    std::size_t branch_merge_sites,
    std::size_t contract_violation_sites,
    bool deterministic_result_like_lowering_handoff);

bool IsResultLikeProfileNormalized(
    std::size_t result_like_sites,
    std::size_t result_success_sites,
    std::size_t result_failure_sites,
    std::size_t result_branch_sites,
    std::size_t result_payload_sites,
    std::size_t normalized_sites,
    std::size_t branch_merge_sites,
    std::size_t contract_violation_sites);

Objc3ResultLikeProfile BuildResultLikeProfileFromBody(
    const std::vector<std::unique_ptr<Stmt>> &body);

Objc3ResultLikeProfile BuildResultLikeProfileFromOpaqueBody(bool has_body);

}  // namespace objc3c::parse
