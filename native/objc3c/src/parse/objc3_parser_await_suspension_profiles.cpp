#include "parse/objc3_parser_await_suspension_profiles.h"

#include <algorithm>
#include <limits>
#include <memory>
#include <sstream>
#include <vector>

#include "parse/objc3_parser_profile_helpers.h"

namespace objc3c::parse {
namespace {

#include "parse/objc3_parser_await_suspension_site_collection.inc"

Objc3AwaitSuspensionProfile BuildAwaitSuspensionProfileFromCounts(
    std::size_t await_keyword_sites,
    std::size_t await_suspension_point_sites,
    std::size_t await_resume_sites,
    std::size_t await_state_machine_sites,
    std::size_t await_continuation_sites) {
  Objc3AwaitSuspensionProfile profile;
  profile.await_keyword_sites = await_keyword_sites;
  profile.await_suspension_point_sites = await_suspension_point_sites;
  profile.await_resume_sites = await_resume_sites;
  profile.await_state_machine_sites = await_state_machine_sites;
  profile.await_continuation_sites = await_continuation_sites;
  profile.await_suspension_sites = profile.await_keyword_sites;
  if (profile.await_suspension_sites >
      std::numeric_limits<std::size_t>::max() -
          profile.await_suspension_point_sites) {
    profile.await_suspension_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.await_suspension_sites += profile.await_suspension_point_sites;
  }
  if (profile.await_suspension_sites >
      std::numeric_limits<std::size_t>::max() -
          profile.await_continuation_sites) {
    profile.await_suspension_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.await_suspension_sites += profile.await_continuation_sites;
  }
  profile.gate_blocked_sites = std::min(
      profile.await_suspension_sites,
      std::min(profile.await_resume_sites, profile.await_state_machine_sites));
  profile.normalized_sites =
      profile.await_suspension_sites - profile.gate_blocked_sites;
  if (profile.await_keyword_sites > profile.await_suspension_sites ||
      profile.await_suspension_point_sites > profile.await_suspension_sites ||
      profile.await_resume_sites > profile.await_suspension_sites ||
      profile.await_state_machine_sites > profile.await_suspension_sites ||
      profile.await_continuation_sites > profile.await_suspension_sites ||
      profile.normalized_sites > profile.await_suspension_sites ||
      profile.gate_blocked_sites > profile.await_suspension_sites ||
      profile.contract_violation_sites > profile.await_suspension_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.gate_blocked_sites !=
      profile.await_suspension_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.contract_violation_sites > profile.await_suspension_sites) {
    profile.contract_violation_sites = profile.await_suspension_sites;
  }
  profile.deterministic_await_suspension_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}

}  // namespace

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
    bool deterministic_await_suspension_handoff) {
  std::ostringstream out;
  out << "await-suspension:await_suspension_sites="
      << await_suspension_sites
      << ";await_keyword_sites=" << await_keyword_sites
      << ";await_suspension_point_sites=" << await_suspension_point_sites
      << ";await_resume_sites=" << await_resume_sites
      << ";await_state_machine_sites=" << await_state_machine_sites
      << ";await_continuation_sites=" << await_continuation_sites
      << ";normalized_sites=" << normalized_sites
      << ";gate_blocked_sites=" << gate_blocked_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_await_suspension_handoff="
      << (deterministic_await_suspension_handoff ? "true" : "false");
  return out.str();
}

bool IsAwaitSuspensionProfileNormalized(
    std::size_t await_suspension_sites,
    std::size_t await_keyword_sites,
    std::size_t await_suspension_point_sites,
    std::size_t await_resume_sites,
    std::size_t await_state_machine_sites,
    std::size_t await_continuation_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites) {
  if (await_keyword_sites > await_suspension_sites ||
      await_suspension_point_sites > await_suspension_sites ||
      await_resume_sites > await_suspension_sites ||
      await_state_machine_sites > await_suspension_sites ||
      await_continuation_sites > await_suspension_sites ||
      normalized_sites > await_suspension_sites ||
      gate_blocked_sites > await_suspension_sites ||
      contract_violation_sites > await_suspension_sites) {
    return false;
  }
  if (normalized_sites + gate_blocked_sites != await_suspension_sites) {
    return false;
  }
  return contract_violation_sites == 0u;
}

Objc3AwaitSuspensionProfile BuildAwaitSuspensionProfileFromFunction(
    const FunctionDecl &fn) {
  const Objc3AwaitSuspensionSiteCounts counts =
      CountAwaitSuspensionSitesInBody(fn.body);
  return BuildAwaitSuspensionProfileFromCounts(
      counts.await_keyword_sites,
      counts.await_suspension_point_sites,
      counts.await_resume_sites,
      counts.await_state_machine_sites,
      counts.await_continuation_sites);
}

Objc3AwaitSuspensionProfile BuildAwaitSuspensionProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  Objc3AwaitSuspensionSiteCounts counts;
  if (method.has_body) {
    CollectAwaitSuspensionSitesFromSymbol(method.selector, counts);
  }
  return BuildAwaitSuspensionProfileFromCounts(
      counts.await_keyword_sites,
      counts.await_suspension_point_sites,
      counts.await_resume_sites,
      counts.await_state_machine_sites,
      counts.await_continuation_sites);
}

}  // namespace objc3c::parse
