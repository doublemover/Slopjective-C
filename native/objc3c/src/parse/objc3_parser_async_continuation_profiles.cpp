#include "parse/objc3_parser_async_continuation_profiles.h"

#include <algorithm>
#include <limits>
#include <memory>
#include <sstream>
#include <vector>

#include "parse/objc3_parser_profile_helpers.h"
#include "parse/objc3_parser_profile_symbol_walk.h"

namespace objc3c::parse {
namespace {

#include "parse/objc3_parser_async_continuation_site_collection.inc"

Objc3AsyncContinuationProfile BuildAsyncContinuationProfileFromCounts(
    std::size_t async_keyword_sites,
    std::size_t async_function_sites,
    std::size_t continuation_allocation_sites,
    std::size_t continuation_resume_sites,
    std::size_t continuation_suspend_sites,
    std::size_t async_state_machine_sites) {
  Objc3AsyncContinuationProfile profile;
  profile.async_keyword_sites = async_keyword_sites;
  profile.async_function_sites = async_function_sites;
  profile.continuation_allocation_sites = continuation_allocation_sites;
  profile.continuation_resume_sites = continuation_resume_sites;
  profile.continuation_suspend_sites = continuation_suspend_sites;
  profile.async_state_machine_sites = async_state_machine_sites;
  profile.async_continuation_sites = profile.async_keyword_sites;
  if (profile.async_continuation_sites >
      std::numeric_limits<std::size_t>::max() -
          profile.async_function_sites) {
    profile.async_continuation_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.async_continuation_sites += profile.async_function_sites;
  }
  if (profile.async_continuation_sites >
      std::numeric_limits<std::size_t>::max() -
          profile.continuation_allocation_sites) {
    profile.async_continuation_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.async_continuation_sites +=
        profile.continuation_allocation_sites;
  }
  profile.gate_blocked_sites = std::min(
      profile.async_continuation_sites,
      std::min(profile.continuation_resume_sites,
               profile.continuation_suspend_sites));
  profile.normalized_sites =
      profile.async_continuation_sites - profile.gate_blocked_sites;
  if (profile.async_keyword_sites > profile.async_continuation_sites ||
      profile.async_function_sites > profile.async_continuation_sites ||
      profile.continuation_allocation_sites >
          profile.async_continuation_sites ||
      profile.continuation_resume_sites > profile.async_continuation_sites ||
      profile.continuation_suspend_sites > profile.async_continuation_sites ||
      profile.async_state_machine_sites > profile.async_continuation_sites ||
      profile.normalized_sites > profile.async_continuation_sites ||
      profile.gate_blocked_sites > profile.async_continuation_sites ||
      profile.contract_violation_sites > profile.async_continuation_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.gate_blocked_sites !=
      profile.async_continuation_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.contract_violation_sites > profile.async_continuation_sites) {
    profile.contract_violation_sites = profile.async_continuation_sites;
  }
  profile.deterministic_async_continuation_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}

}  // namespace

std::string BuildAsyncContinuationProfile(
    std::size_t async_continuation_sites,
    std::size_t async_keyword_sites,
    std::size_t async_function_sites,
    std::size_t continuation_allocation_sites,
    std::size_t continuation_resume_sites,
    std::size_t continuation_suspend_sites,
    std::size_t async_state_machine_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites,
    bool deterministic_async_continuation_handoff) {
  std::ostringstream out;
  out << "async-continuation:async_continuation_sites="
      << async_continuation_sites
      << ";async_keyword_sites=" << async_keyword_sites
      << ";async_function_sites=" << async_function_sites
      << ";continuation_allocation_sites=" << continuation_allocation_sites
      << ";continuation_resume_sites=" << continuation_resume_sites
      << ";continuation_suspend_sites=" << continuation_suspend_sites
      << ";async_state_machine_sites=" << async_state_machine_sites
      << ";normalized_sites=" << normalized_sites
      << ";gate_blocked_sites=" << gate_blocked_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_async_continuation_handoff="
      << (deterministic_async_continuation_handoff ? "true" : "false");
  return out.str();
}

bool IsAsyncContinuationProfileNormalized(
    std::size_t async_continuation_sites,
    std::size_t async_keyword_sites,
    std::size_t async_function_sites,
    std::size_t continuation_allocation_sites,
    std::size_t continuation_resume_sites,
    std::size_t continuation_suspend_sites,
    std::size_t async_state_machine_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites) {
  if (async_keyword_sites > async_continuation_sites ||
      async_function_sites > async_continuation_sites ||
      continuation_allocation_sites > async_continuation_sites ||
      continuation_resume_sites > async_continuation_sites ||
      continuation_suspend_sites > async_continuation_sites ||
      async_state_machine_sites > async_continuation_sites ||
      normalized_sites > async_continuation_sites ||
      gate_blocked_sites > async_continuation_sites ||
      contract_violation_sites > async_continuation_sites) {
    return false;
  }
  if (normalized_sites + gate_blocked_sites != async_continuation_sites) {
    return false;
  }
  return contract_violation_sites == 0u;
}

Objc3AsyncContinuationProfile BuildAsyncContinuationProfileFromFunction(
    const FunctionDecl &fn) {
  Objc3AsyncContinuationSiteCounts counts =
      CountAsyncContinuationSitesInBody(fn.body);
  if (fn.async_declared) {
    counts.async_keyword_sites += 1u;
    counts.async_function_sites += 1u;
  }
  return BuildAsyncContinuationProfileFromCounts(
      counts.async_keyword_sites,
      counts.async_function_sites,
      counts.continuation_allocation_sites,
      counts.continuation_resume_sites,
      counts.continuation_suspend_sites,
      counts.async_state_machine_sites);
}

Objc3AsyncContinuationProfile BuildAsyncContinuationProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  Objc3AsyncContinuationSiteCounts counts;
  Objc3ProfileSymbolWalker walker{
      &CollectAsyncContinuationProfileSymbol, &counts};
  WalkObjc3ProfileSymbolsInOpaqueMethodBody(method, walker);
  if (method.async_declared) {
    counts.async_keyword_sites += 1u;
    counts.async_function_sites += 1u;
  }
  return BuildAsyncContinuationProfileFromCounts(
      counts.async_keyword_sites,
      counts.async_function_sites,
      counts.continuation_allocation_sites,
      counts.continuation_resume_sites,
      counts.continuation_suspend_sites,
      counts.async_state_machine_sites);
}

}  // namespace objc3c::parse
