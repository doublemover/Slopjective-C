#include "sema/objc3_sema_pass_flow_scaffold.h"

#include <cstddef>
#include <cstdint>
#include <numeric>
#include <sstream>

void MarkObjc3SemaPassExecuted(Objc3SemaPassFlowSummary &summary, Objc3SemaPassId pass) {
  const std::size_t pass_index = static_cast<std::size_t>(pass);
  if (pass_index >= summary.pass_executed.size()) {
    return;
  }
  if (!summary.pass_executed[pass_index]) {
    summary.pass_executed[pass_index] = true;
    ++summary.executed_pass_count;
  }
}

void FinalizeObjc3SemaPassFlowSummary(
    Objc3SemaPassFlowSummary &summary,
    const Objc3SemanticIntegrationSurface &integration_surface,
    const Objc3SemanticTypeMetadataHandoff &type_metadata_handoff,
    bool diagnostics_after_pass_monotonic,
    bool pass_order_matches_contract,
    bool deterministic_semantic_diagnostics,
    bool deterministic_type_metadata_handoff) {
  const auto fnv1a_mix = [](std::uint64_t seed, std::uint64_t value) {
    constexpr std::uint64_t kFnvPrime = 1099511628211ull;
    std::uint64_t mixed = seed;
    for (int i = 0; i < 8; ++i) {
      const std::uint8_t byte = static_cast<std::uint8_t>((value >> (i * 8)) & 0xffu);
      mixed ^= static_cast<std::uint64_t>(byte);
      mixed *= kFnvPrime;
    }
    return mixed;
  };

  summary.pass_order_matches_contract =
      pass_order_matches_contract && summary.configured_pass_order == kObjc3SemaPassOrder;
  summary.diagnostics_after_pass_monotonic = diagnostics_after_pass_monotonic;
  summary.diagnostics_total = summary.diagnostics_after_pass.back();
  const std::size_t diagnostics_emitted_total = std::accumulate(summary.diagnostics_emitted_by_pass.begin(),
                                                                summary.diagnostics_emitted_by_pass.end(),
                                                                static_cast<std::size_t>(0u));
  summary.diagnostics_emission_totals_consistent = diagnostics_emitted_total == summary.diagnostics_total;
  summary.transition_edge_count = summary.executed_pass_count > 0u ? summary.executed_pass_count - 1u : 0u;
  summary.compatibility_handoff_consistent =
      (!summary.migration_assist_enabled ||
       summary.compatibility_mode == Objc3SemaCompatibilityMode::Canonical) &&
      (summary.compatibility_mode == Objc3SemaCompatibilityMode::Canonical ||
       summary.compatibility_mode == Objc3SemaCompatibilityMode::Legacy);

  summary.symbol_globals_count = integration_surface.globals.size();
  summary.symbol_functions_count = integration_surface.functions.size();
  summary.symbol_interfaces_count = integration_surface.interfaces.size();
  summary.symbol_implementations_count = integration_surface.implementations.size();

  summary.type_metadata_global_entries = type_metadata_handoff.global_names_lexicographic.size();
  summary.type_metadata_function_entries = type_metadata_handoff.functions_lexicographic.size();
  summary.type_metadata_interface_entries = type_metadata_handoff.interfaces_lexicographic.size();
  summary.type_metadata_implementation_entries = type_metadata_handoff.implementations_lexicographic.size();

  summary.symbol_flow_counts_consistent =
      summary.symbol_globals_count == summary.type_metadata_global_entries &&
      summary.symbol_functions_count == summary.type_metadata_function_entries &&
      summary.symbol_interfaces_count == summary.type_metadata_interface_entries &&
      summary.symbol_implementations_count == summary.type_metadata_implementation_entries;

  std::uint64_t fingerprint = 1469598103934665603ull;
  fingerprint = fnv1a_mix(fingerprint, static_cast<std::uint64_t>(summary.configured_pass_count));
  fingerprint = fnv1a_mix(fingerprint, static_cast<std::uint64_t>(summary.executed_pass_count));
  fingerprint = fnv1a_mix(fingerprint, static_cast<std::uint64_t>(summary.diagnostics_total));
  fingerprint = fnv1a_mix(fingerprint, static_cast<std::uint64_t>(summary.migration_assist_enabled ? 1u : 0u));
  fingerprint = fnv1a_mix(fingerprint, static_cast<std::uint64_t>(summary.migration_legacy_literal_total));
  fingerprint = fnv1a_mix(fingerprint, static_cast<std::uint64_t>(summary.compatibility_mode));
  for (std::size_t i = 0; i < summary.pass_executed.size(); ++i) {
    fingerprint = fnv1a_mix(fingerprint, summary.pass_executed[i] ? 1ull : 0ull);
    fingerprint = fnv1a_mix(fingerprint, static_cast<std::uint64_t>(summary.diagnostics_after_pass[i]));
    fingerprint = fnv1a_mix(fingerprint, static_cast<std::uint64_t>(summary.diagnostics_emitted_by_pass[i]));
  }
  fingerprint = fnv1a_mix(fingerprint, static_cast<std::uint64_t>(summary.symbol_globals_count));
  fingerprint = fnv1a_mix(fingerprint, static_cast<std::uint64_t>(summary.symbol_functions_count));
  fingerprint = fnv1a_mix(fingerprint, static_cast<std::uint64_t>(summary.symbol_interfaces_count));
  fingerprint = fnv1a_mix(fingerprint, static_cast<std::uint64_t>(summary.symbol_implementations_count));
  fingerprint = fnv1a_mix(fingerprint, static_cast<std::uint64_t>(summary.type_metadata_global_entries));
  fingerprint = fnv1a_mix(fingerprint, static_cast<std::uint64_t>(summary.type_metadata_function_entries));
  fingerprint = fnv1a_mix(fingerprint, static_cast<std::uint64_t>(summary.type_metadata_interface_entries));
  fingerprint = fnv1a_mix(fingerprint, static_cast<std::uint64_t>(summary.type_metadata_implementation_entries));
  summary.pass_execution_fingerprint = fingerprint;

  std::ostringstream handoff_key;
  handoff_key << "sema-pass-flow:v1:"
              << summary.executed_pass_count << "/" << summary.configured_pass_count
              << ":compat=" << (summary.compatibility_mode == Objc3SemaCompatibilityMode::Canonical ? "canonical" : "legacy")
              << ":diag=" << summary.diagnostics_total
              << ":fp=" << summary.pass_execution_fingerprint;
  summary.deterministic_handoff_key = handoff_key.str();
  summary.replay_key_deterministic =
      summary.deterministic_handoff_key.rfind("sema-pass-flow:v1:", 0) == 0 &&
      summary.pass_execution_fingerprint != 1469598103934665603ull;

  summary.deterministic =
      summary.pass_order_matches_contract &&
      summary.configured_pass_count == kObjc3SemaPassOrder.size() &&
      summary.executed_pass_count == summary.configured_pass_count &&
      summary.diagnostics_after_pass_monotonic &&
      summary.diagnostics_emission_totals_consistent &&
      summary.transition_edge_count + 1u == summary.executed_pass_count &&
      summary.compatibility_handoff_consistent &&
      summary.symbol_flow_counts_consistent &&
      summary.diagnostics_total == summary.diagnostics_after_pass.back() &&
      summary.pass_execution_fingerprint != 1469598103934665603ull &&
      !summary.deterministic_handoff_key.empty() &&
      summary.replay_key_deterministic &&
      deterministic_semantic_diagnostics &&
      deterministic_type_metadata_handoff;
}
