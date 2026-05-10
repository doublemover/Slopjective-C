#include "parse/objc3_parser_error_diagnostics_recovery_profiles.h"

#include <algorithm>
#include <limits>
#include <memory>
#include <sstream>
#include <vector>

#include "parse/objc3_parser_profile_helpers.h"
#include "parse/objc3_parser_profile_symbol_walk.h"

namespace objc3c::parse {
namespace {

#include "parse/objc3_parser_error_diagnostics_recovery_site_collection.inc"

bool TryAddErrorDiagnosticsRecoverySiteCounts(
    std::size_t lhs,
    std::size_t rhs,
    std::size_t &out) {
  if (lhs > std::numeric_limits<std::size_t>::max() - rhs) {
    return false;
  }
  out = lhs + rhs;
  return true;
}

Objc3ErrorDiagnosticsRecoveryProfile
BuildErrorDiagnosticsRecoveryProfileFromCounts(
    std::size_t diagnostic_emit_sites,
    std::size_t recovery_anchor_sites,
    std::size_t recovery_boundary_sites,
    std::size_t fail_closed_diagnostic_sites) {
  Objc3ErrorDiagnosticsRecoveryProfile profile;
  profile.diagnostic_emit_sites = diagnostic_emit_sites;
  profile.recovery_anchor_sites = recovery_anchor_sites;
  profile.recovery_boundary_sites = recovery_boundary_sites;
  profile.fail_closed_diagnostic_sites = fail_closed_diagnostic_sites;
  profile.error_diagnostics_recovery_sites = profile.diagnostic_emit_sites;
  std::size_t summed_sites = 0u;
  if (!TryAddErrorDiagnosticsRecoverySiteCounts(
          profile.error_diagnostics_recovery_sites,
          profile.recovery_anchor_sites,
          summed_sites)) {
    profile.error_diagnostics_recovery_sites =
        std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.error_diagnostics_recovery_sites = summed_sites;
  }
  if (!TryAddErrorDiagnosticsRecoverySiteCounts(
          profile.error_diagnostics_recovery_sites,
          profile.recovery_boundary_sites,
          summed_sites)) {
    profile.error_diagnostics_recovery_sites =
        std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.error_diagnostics_recovery_sites = summed_sites;
  }
  if (!TryAddErrorDiagnosticsRecoverySiteCounts(
          profile.error_diagnostics_recovery_sites,
          profile.fail_closed_diagnostic_sites,
          summed_sites)) {
    profile.error_diagnostics_recovery_sites =
        std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  } else {
    profile.error_diagnostics_recovery_sites = summed_sites;
  }

  const bool gate_open =
      profile.diagnostic_emit_sites > 0u && profile.recovery_anchor_sites > 0u;
  std::size_t blocked_total = profile.recovery_boundary_sites;
  if (!TryAddErrorDiagnosticsRecoverySiteCounts(
          blocked_total,
          profile.fail_closed_diagnostic_sites,
          blocked_total)) {
    blocked_total = std::numeric_limits<std::size_t>::max();
    profile.contract_violation_sites += 1u;
  }
  profile.gate_blocked_sites =
      gate_open ? 0u
                : std::min(profile.error_diagnostics_recovery_sites,
                           blocked_total);
  profile.normalized_sites =
      profile.error_diagnostics_recovery_sites - profile.gate_blocked_sites;

  if (profile.diagnostic_emit_sites >
          profile.error_diagnostics_recovery_sites ||
      profile.recovery_anchor_sites >
          profile.error_diagnostics_recovery_sites ||
      profile.recovery_boundary_sites >
          profile.error_diagnostics_recovery_sites ||
      profile.fail_closed_diagnostic_sites >
          profile.error_diagnostics_recovery_sites ||
      profile.normalized_sites > profile.error_diagnostics_recovery_sites ||
      profile.gate_blocked_sites > profile.error_diagnostics_recovery_sites ||
      profile.contract_violation_sites >
          profile.error_diagnostics_recovery_sites) {
    profile.contract_violation_sites += 1u;
  }
  std::size_t normalized_total = 0u;
  if (!TryAddErrorDiagnosticsRecoverySiteCounts(
          profile.normalized_sites,
          profile.gate_blocked_sites,
          normalized_total) ||
      normalized_total != profile.error_diagnostics_recovery_sites) {
    profile.contract_violation_sites += 1u;
  }
  if (gate_open && profile.gate_blocked_sites != 0u) {
    profile.contract_violation_sites += 1u;
  }
  if (profile.contract_violation_sites >
      profile.error_diagnostics_recovery_sites) {
    profile.contract_violation_sites =
        profile.error_diagnostics_recovery_sites;
  }
  profile.deterministic_error_diagnostics_recovery_handoff =
      profile.contract_violation_sites == 0u;
  return profile;
}

}  // namespace

std::string BuildErrorDiagnosticsRecoveryProfile(
    std::size_t error_diagnostics_recovery_sites,
    std::size_t diagnostic_emit_sites,
    std::size_t recovery_anchor_sites,
    std::size_t recovery_boundary_sites,
    std::size_t fail_closed_diagnostic_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites,
    bool deterministic_error_diagnostics_recovery_handoff) {
  std::ostringstream out;
  out << "error-diagnostics-recovery:error_diagnostics_recovery_sites="
      << error_diagnostics_recovery_sites
      << ";diagnostic_emit_sites=" << diagnostic_emit_sites
      << ";recovery_anchor_sites=" << recovery_anchor_sites
      << ";recovery_boundary_sites=" << recovery_boundary_sites
      << ";fail_closed_diagnostic_sites=" << fail_closed_diagnostic_sites
      << ";normalized_sites=" << normalized_sites
      << ";gate_blocked_sites=" << gate_blocked_sites
      << ";contract_violation_sites=" << contract_violation_sites
      << ";deterministic_error_diagnostics_recovery_handoff="
      << (deterministic_error_diagnostics_recovery_handoff ? "true"
                                                           : "false");
  return out.str();
}

bool IsErrorDiagnosticsRecoveryProfileNormalized(
    std::size_t error_diagnostics_recovery_sites,
    std::size_t diagnostic_emit_sites,
    std::size_t recovery_anchor_sites,
    std::size_t recovery_boundary_sites,
    std::size_t fail_closed_diagnostic_sites,
    std::size_t normalized_sites,
    std::size_t gate_blocked_sites,
    std::size_t contract_violation_sites) {
  if (diagnostic_emit_sites > error_diagnostics_recovery_sites ||
      recovery_anchor_sites > error_diagnostics_recovery_sites ||
      recovery_boundary_sites > error_diagnostics_recovery_sites ||
      fail_closed_diagnostic_sites > error_diagnostics_recovery_sites ||
      normalized_sites > error_diagnostics_recovery_sites ||
      gate_blocked_sites > error_diagnostics_recovery_sites ||
      contract_violation_sites > error_diagnostics_recovery_sites) {
    return false;
  }
  std::size_t normalized_total = 0u;
  if (!TryAddErrorDiagnosticsRecoverySiteCounts(
          normalized_sites, gate_blocked_sites, normalized_total)) {
    return false;
  }
  if (normalized_total != error_diagnostics_recovery_sites) {
    return false;
  }
  return contract_violation_sites == 0u;
}

Objc3ErrorDiagnosticsRecoveryProfile
BuildErrorDiagnosticsRecoveryProfileFromFunction(const FunctionDecl &fn) {
  const Objc3ErrorDiagnosticsRecoverySiteCounts counts =
      CountErrorDiagnosticsRecoverySitesInBody(fn.body);
  return BuildErrorDiagnosticsRecoveryProfileFromCounts(
      counts.diagnostic_emit_sites,
      counts.recovery_anchor_sites,
      counts.recovery_boundary_sites,
      counts.fail_closed_diagnostic_sites);
}

Objc3ErrorDiagnosticsRecoveryProfile
BuildErrorDiagnosticsRecoveryProfileFromOpaqueBody(
    const Objc3MethodDecl &method) {
  Objc3ErrorDiagnosticsRecoverySiteCounts counts;
  Objc3ProfileSymbolWalker walker{
      &CollectErrorDiagnosticsRecoveryProfileSymbol, &counts};
  WalkObjc3ProfileSymbolsInOpaqueMethodBody(method, walker);
  return BuildErrorDiagnosticsRecoveryProfileFromCounts(
      counts.diagnostic_emit_sites,
      counts.recovery_anchor_sites,
      counts.recovery_boundary_sites,
      counts.fail_closed_diagnostic_sites);
}

}  // namespace objc3c::parse
