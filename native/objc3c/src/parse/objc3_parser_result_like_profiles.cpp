#include "parse/objc3_parser_result_like_profiles.h"

#include <sstream>

namespace objc3c::parse {
namespace {

#include "parse/objc3_parser_result_like_site_collection.inc"

}  // namespace

std::string BuildResultLikeProfile(
    std::size_t result_like_sites,
    std::size_t result_success_sites,
    std::size_t result_failure_sites,
    std::size_t result_branch_sites,
    std::size_t result_payload_sites,
    std::size_t normalized_sites,
    std::size_t branch_merge_sites,
    std::size_t contract_violation_sites,
    bool deterministic_result_like_lowering_handoff) {
  std::ostringstream out;
  out << "result-like-lowering:result_like_sites=" << result_like_sites
      << ";result_success_sites=" << result_success_sites
      << ";result_failure_sites=" << result_failure_sites
      << ";result_branch_sites=" << result_branch_sites
      << ";result_payload_sites=" << result_payload_sites
      << ";normalized_sites=" << normalized_sites
      << ";branch_merge_sites=" << branch_merge_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_result_like_lowering_handoff="
      << (deterministic_result_like_lowering_handoff ? "true" : "false");
  return out.str();
}

bool IsResultLikeProfileNormalized(
    std::size_t result_like_sites,
    std::size_t result_success_sites,
    std::size_t result_failure_sites,
    std::size_t result_branch_sites,
    std::size_t result_payload_sites,
    std::size_t normalized_sites,
    std::size_t branch_merge_sites,
    std::size_t contract_violation_sites) {
  if (result_success_sites + result_failure_sites != normalized_sites) {
    return false;
  }
  if (result_success_sites > result_like_sites ||
      result_failure_sites > result_like_sites ||
      result_branch_sites > result_like_sites ||
      result_payload_sites > result_like_sites) {
    return false;
  }
  if (normalized_sites + branch_merge_sites != result_like_sites) {
    return false;
  }
  return contract_violation_sites == 0;
}

Objc3ResultLikeProfile BuildResultLikeProfileFromBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3ResultLikeProfile profile;
  for (const auto &stmt : body) {
    CollectResultLikeStmtProfile(stmt.get(), profile);
  }

  if (profile.result_success_sites + profile.result_failure_sites !=
      profile.normalized_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.result_success_sites > profile.result_like_sites ||
      profile.result_failure_sites > profile.result_like_sites ||
      profile.result_branch_sites > profile.result_like_sites ||
      profile.result_payload_sites > profile.result_like_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.branch_merge_sites !=
      profile.result_like_sites) {
    profile.contract_violation_sites += 1u;
  }
  profile.deterministic_result_like_lowering_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}

Objc3ResultLikeProfile BuildResultLikeProfileFromOpaqueBody(bool has_body) {
  Objc3ResultLikeProfile profile;
  if (has_body) {
    profile.result_like_sites = 1u;
    profile.result_branch_sites = 1u;
    profile.branch_merge_sites = 1u;
  }
  profile.deterministic_result_like_lowering_handoff = true;
  return profile;
}

}  // namespace objc3c::parse
