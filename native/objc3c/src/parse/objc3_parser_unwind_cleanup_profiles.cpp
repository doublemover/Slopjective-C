#include "parse/objc3_parser_unwind_cleanup_profiles.h"

#include <algorithm>
#include <limits>
#include <memory>
#include <sstream>
#include <vector>

#include "parse/objc3_parser_profile_helpers.h"
#include "parse/objc3_parser_profile_symbol_walk.h"

namespace objc3c::parse {
namespace {

bool IsExceptionalExitSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("throw") != std::string::npos ||
         lowered.find("exception") != std::string::npos ||
         lowered.find("unwind") != std::string::npos ||
         lowered.find("raise") != std::string::npos;
}

bool IsCleanupActionSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("cleanup") != std::string::npos ||
         lowered.find("defer") != std::string::npos ||
         lowered.find("finally") != std::string::npos ||
         lowered.find("release") != std::string::npos;
}

bool IsCleanupScopeSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("cleanup_scope") != std::string::npos ||
         lowered.find("scope_guard") != std::string::npos ||
         lowered.find("defer_scope") != std::string::npos ||
         lowered.find("guard_scope") != std::string::npos;
}

bool IsCleanupResumeSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("resume") != std::string::npos ||
         lowered.find("rethrow") != std::string::npos ||
         lowered.find("continue_unwind") != std::string::npos;
}

struct Objc3UnwindCleanupSiteCounts {
  std::size_t exceptional_exit_sites = 0;
  std::size_t cleanup_action_sites = 0;
  std::size_t cleanup_scope_sites = 0;
  std::size_t cleanup_resume_sites = 0;
};

void CollectUnwindCleanupSitesFromSymbol(
    const std::string &symbol,
    Objc3UnwindCleanupSiteCounts &counts) {
  if (IsExceptionalExitSymbol(symbol)) {
    counts.exceptional_exit_sites += 1u;
  }
  if (IsCleanupActionSymbol(symbol)) {
    counts.cleanup_action_sites += 1u;
  }
  if (IsCleanupScopeSymbol(symbol)) {
    counts.cleanup_scope_sites += 1u;
  }
  if (IsCleanupResumeSymbol(symbol)) {
    counts.cleanup_resume_sites += 1u;
  }
}

void CollectUnwindCleanupProfileSymbol(
    const std::string &symbol,
    void *context) {
  auto *counts = static_cast<Objc3UnwindCleanupSiteCounts *>(context);
  CollectUnwindCleanupSitesFromSymbol(symbol, *counts);
}

Objc3UnwindCleanupSiteCounts CountUnwindCleanupSitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3UnwindCleanupSiteCounts counts;
  Objc3ProfileSymbolWalker walker{
      &CollectUnwindCleanupProfileSymbol, &counts};
  WalkObjc3ProfileSymbolsInBody(body, walker);
  return counts;
}

Objc3UnwindCleanupProfile BuildUnwindCleanupProfileFromCounts(
    std::size_t exceptional_exit_sites,
    std::size_t cleanup_action_sites,
    std::size_t cleanup_scope_sites,
    std::size_t cleanup_resume_sites) {
  Objc3UnwindCleanupProfile profile;
  profile.exceptional_exit_sites = exceptional_exit_sites;
  profile.cleanup_action_sites = cleanup_action_sites;
  profile.cleanup_scope_sites = cleanup_scope_sites;
  profile.cleanup_resume_sites = cleanup_resume_sites;
  profile.unwind_cleanup_sites = profile.exceptional_exit_sites;
  if (profile.unwind_cleanup_sites >
      std::numeric_limits<std::size_t>::max() -
          profile.cleanup_action_sites) {
    profile.unwind_cleanup_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.unwind_cleanup_sites += profile.cleanup_action_sites;
  }
  if (profile.unwind_cleanup_sites >
      std::numeric_limits<std::size_t>::max() - profile.cleanup_scope_sites) {
    profile.unwind_cleanup_sites = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.unwind_cleanup_sites += profile.cleanup_scope_sites;
  }
  profile.fail_closed_sites =
      std::min(profile.unwind_cleanup_sites, profile.cleanup_resume_sites);
  profile.normalized_sites =
      profile.unwind_cleanup_sites - profile.fail_closed_sites;
  if (profile.exceptional_exit_sites > profile.unwind_cleanup_sites ||
      profile.cleanup_action_sites > profile.unwind_cleanup_sites ||
      profile.cleanup_scope_sites > profile.unwind_cleanup_sites ||
      profile.cleanup_resume_sites > profile.unwind_cleanup_sites ||
      profile.normalized_sites > profile.unwind_cleanup_sites ||
      profile.fail_closed_sites > profile.unwind_cleanup_sites ||
      profile.contract_violation_sites > profile.unwind_cleanup_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.normalized_sites + profile.fail_closed_sites !=
      profile.unwind_cleanup_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.contract_violation_sites > profile.unwind_cleanup_sites) {
    profile.contract_violation_sites = profile.unwind_cleanup_sites;
  }
  profile.deterministic_unwind_cleanup_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}

}  // namespace

std::string BuildUnwindCleanupProfile(
    std::size_t unwind_cleanup_sites,
    std::size_t exceptional_exit_sites,
    std::size_t cleanup_action_sites,
    std::size_t cleanup_scope_sites,
    std::size_t cleanup_resume_sites,
    std::size_t normalized_sites,
    std::size_t fail_closed_sites,
    std::size_t contract_violation_sites,
    bool deterministic_unwind_cleanup_handoff) {
  std::ostringstream out;
  out << "unwind-cleanup:unwind_cleanup_sites=" << unwind_cleanup_sites
      << ";exceptional_exit_sites=" << exceptional_exit_sites
      << ";cleanup_action_sites=" << cleanup_action_sites
      << ";cleanup_scope_sites=" << cleanup_scope_sites
      << ";cleanup_resume_sites=" << cleanup_resume_sites
      << ";normalized_sites=" << normalized_sites
      << ";fail_closed_sites=" << fail_closed_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_unwind_cleanup_handoff="
      << (deterministic_unwind_cleanup_handoff ? "true" : "false");
  return out.str();
}

bool IsUnwindCleanupProfileNormalized(
    std::size_t unwind_cleanup_sites,
    std::size_t exceptional_exit_sites,
    std::size_t cleanup_action_sites,
    std::size_t cleanup_scope_sites,
    std::size_t cleanup_resume_sites,
    std::size_t normalized_sites,
    std::size_t fail_closed_sites,
    std::size_t contract_violation_sites) {
  if (exceptional_exit_sites > unwind_cleanup_sites ||
      cleanup_action_sites > unwind_cleanup_sites ||
      cleanup_scope_sites > unwind_cleanup_sites ||
      cleanup_resume_sites > unwind_cleanup_sites ||
      normalized_sites > unwind_cleanup_sites ||
      fail_closed_sites > unwind_cleanup_sites ||
      contract_violation_sites > unwind_cleanup_sites) {
    return false;
  }
  if (normalized_sites + fail_closed_sites != unwind_cleanup_sites) {
    return false;
  }
  return contract_violation_sites == 0u;
}

Objc3UnwindCleanupProfile BuildUnwindCleanupProfileFromFunction(
    const FunctionDecl &fn) {
  const Objc3UnwindCleanupSiteCounts counts =
      CountUnwindCleanupSitesInBody(fn.body);
  return BuildUnwindCleanupProfileFromCounts(
      counts.exceptional_exit_sites,
      counts.cleanup_action_sites,
      counts.cleanup_scope_sites,
      counts.cleanup_resume_sites);
}

Objc3UnwindCleanupProfile BuildUnwindCleanupProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  Objc3UnwindCleanupSiteCounts counts;
  Objc3ProfileSymbolWalker walker{
      &CollectUnwindCleanupProfileSymbol, &counts};
  WalkObjc3ProfileSymbolsInOpaqueMethodBody(method, walker);
  return BuildUnwindCleanupProfileFromCounts(
      counts.exceptional_exit_sites,
      counts.cleanup_action_sites,
      counts.cleanup_scope_sites,
      counts.cleanup_resume_sites);
}

}  // namespace objc3c::parse
