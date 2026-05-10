#include "sema/objc3_semantic_passes.h"

#include <sstream>

Objc3DispatchDispatchIntentSemanticModelSummary
BuildDispatchDispatchIntentSemanticModelSummary(
    const Objc3FrontendDispatchDispatchIntentSourceCompletionSummary
        &source_summary,
    const Objc3SemanticIntegrationSurface &surface) {
  Objc3DispatchDispatchIntentSemanticModelSummary summary;
  const Objc3MethodLookupOverrideConflictSummary &override_summary =
      surface.method_lookup_override_conflict_summary;

  summary.prefixed_container_attribute_sites =
      source_summary.prefixed_container_attribute_sites;
  summary.direct_members_container_sites =
      source_summary.direct_members_container_sites;
  summary.final_container_sites = source_summary.final_container_sites;
  summary.sealed_container_sites = source_summary.sealed_container_sites;
  summary.effective_direct_member_sites =
      source_summary.effective_direct_member_sites;
  summary.direct_members_defaulted_method_sites =
      source_summary.direct_members_defaulted_method_sites;
  summary.direct_members_dynamic_opt_out_sites =
      source_summary.direct_members_dynamic_opt_out_sites;
  summary.override_lookup_sites = override_summary.override_lookup_sites;
  summary.override_lookup_hits = override_summary.override_lookup_hits;
  summary.override_lookup_misses = override_summary.override_lookup_misses;
  summary.override_conflicts = override_summary.override_conflicts;
  summary.unresolved_base_interfaces =
      override_summary.unresolved_base_interfaces;

  summary.source_dependency_required = true;
  summary.dispatch_intent_source_supported =
      source_summary.prefixed_attribute_source_supported &&
      source_summary.defaulting_source_supported &&
      source_summary.deterministic_handoff &&
      source_summary.ready_for_semantic_expansion &&
      summary.direct_members_defaulted_method_sites +
              summary.direct_members_dynamic_opt_out_sites <=
          summary.effective_direct_member_sites +
              summary.direct_members_dynamic_opt_out_sites;
  summary.override_semantic_surface_reused =
      override_summary.deterministic &&
      summary.override_lookup_hits <= summary.override_lookup_sites &&
      summary.override_lookup_hits + summary.override_lookup_misses ==
          summary.override_lookup_sites &&
      summary.override_conflicts <= summary.override_lookup_hits;
  summary.direct_dispatch_reserved_non_goal = true;
  summary.final_sealed_enforcement_deferred = true;
  summary.lowering_runtime_deferred = true;
  summary.deterministic =
      summary.dispatch_intent_source_supported &&
      summary.override_semantic_surface_reused;
  summary.ready_for_core_implementation = summary.deterministic;
  if (!summary.deterministic) {
    summary.failure_reason =
        "dispatch-intent source completion and override legality accounting must remain deterministic";
  }

  std::ostringstream out;
  out << summary.contract_id
      << ";source=" << source_summary.contract_id
      << ";prefixed=" << summary.prefixed_container_attribute_sites
      << ";containers=" << summary.direct_members_container_sites << ":"
      << summary.final_container_sites << ":" << summary.sealed_container_sites
      << ";effective-direct=" << summary.effective_direct_member_sites
      << ";defaulted=" << summary.direct_members_defaulted_method_sites
      << ";dynamic-opt-out=" << summary.direct_members_dynamic_opt_out_sites
      << ";override=" << summary.override_lookup_sites << ":"
      << summary.override_lookup_hits << ":"
      << summary.override_lookup_misses << ":"
      << summary.override_conflicts << ":"
      << summary.unresolved_base_interfaces
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}
