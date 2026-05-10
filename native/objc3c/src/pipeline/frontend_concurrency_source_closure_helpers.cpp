#include "pipeline/frontend_concurrency_source_closure_helpers.h"

#include "pipeline/frontend_source_closure_replay_keys.h"

namespace objc3c::pipeline::orchestration {

Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary
BuildConcurrencyActorMemberIsolationSourceClosureSummary(
    const Objc3Program &program) {
  Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary summary;

  for (const auto &interface_decl : program.interfaces) {
    if (!interface_decl.is_actor) {
      continue;
    }
    ++summary.actor_interface_sites;
    summary.actor_property_sites += interface_decl.properties.size();
    summary.actor_method_sites += interface_decl.methods.size();
    summary.actor_member_metadata_sites +=
        interface_decl.properties.size() + interface_decl.methods.size();
    for (const auto &method : interface_decl.methods) {
      if (method.objc_nonisolated_declared) {
        ++summary.objc_nonisolated_annotation_sites;
      }
      if (method.executor_affinity_declared) {
        ++summary.actor_member_executor_annotation_sites;
      }
      if (method.async_declared) {
        ++summary.actor_async_method_sites;
      }
    }
  }

  summary.actor_declaration_source_supported = true;
  summary.actor_member_source_supported = true;
  summary.isolation_annotation_source_supported = true;
  summary.actor_metadata_surface_supported = true;
  summary.deterministic_handoff =
      summary.objc_nonisolated_annotation_sites <= summary.actor_method_sites &&
      summary.actor_member_executor_annotation_sites <=
          summary.actor_method_sites &&
      summary.actor_async_method_sites <= summary.actor_method_sites &&
      summary.actor_member_metadata_sites ==
          summary.actor_method_sites + summary.actor_property_sites;
  summary.ready_for_semantic_expansion = summary.deterministic_handoff;
  summary.replay_key =
      BuildConcurrencyActorMemberIsolationSourceClosureReplayKey(summary);
  return summary;
}

}  // namespace objc3c::pipeline::orchestration
