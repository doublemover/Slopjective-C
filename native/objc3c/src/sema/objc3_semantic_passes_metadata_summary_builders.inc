  summary.pointer_declarator_sites = type_annotation_summary.pointer_declarator_sites;
  summary.normalized_protocol_composition_sites =
      std::min(summary.terminated_protocol_composition_sites, summary.object_pointer_type_sites);
  summary.contract_violation_sites =
      type_annotation_summary.invalid_generic_suffix_sites +
      (summary.protocol_composition_sites > summary.object_pointer_type_sites
           ? summary.protocol_composition_sites - summary.object_pointer_type_sites
           : 0u);
  summary.deterministic =
      type_annotation_summary.deterministic &&
      summary.contract_violation_sites == 0u &&
      summary.terminated_protocol_composition_sites <= summary.protocol_composition_sites &&
      summary.normalized_protocol_composition_sites <= summary.protocol_qualified_object_type_sites &&
      summary.contract_violation_sites <= summary.protocol_qualified_object_type_sites;
  return summary;
}

#include "sema/objc3_semantic_passes_type_metadata_module_summary_builders.inc"

static Objc3UnwindCleanupSummary
BuildUnwindCleanupSummaryFromThrowsAndResultSummaries(
    const Objc3ThrowsPropagationSummary &throws_summary,
    const Objc3ResultLikeLoweringSummary &result_summary) {
  Objc3UnwindCleanupSummary summary;
  summary.unwind_cleanup_sites = throws_summary.throws_propagation_sites;

  const auto saturating_add = [&](std::size_t lhs, std::size_t rhs) {
    if (rhs > std::numeric_limits<std::size_t>::max() - lhs) {
      summary.deterministic = false;
      return std::numeric_limits<std::size_t>::max();
    }
    return lhs + rhs;
  };

  summary.exceptional_exit_sites = std::min(
      summary.unwind_cleanup_sites,
      saturating_add(throws_summary.namespace_segment_sites,
                     throws_summary.import_edge_candidate_sites));
  summary.cleanup_action_sites = std::min(
      summary.unwind_cleanup_sites,
      saturating_add(throws_summary.cache_invalidation_candidate_sites,
                     result_summary.result_payload_sites));
  summary.cleanup_scope_sites = std::min(summary.unwind_cleanup_sites,
                                         result_summary.branch_merge_sites);
  summary.cleanup_resume_sites = std::min(summary.unwind_cleanup_sites,
                                          result_summary.result_success_sites);
  summary.normalized_sites =
      std::min(summary.unwind_cleanup_sites, throws_summary.normalized_sites);
  summary.fail_closed_sites =
      summary.unwind_cleanup_sites - summary.normalized_sites;
  summary.contract_violation_sites = std::min(
      summary.unwind_cleanup_sites,
      saturating_add(throws_summary.contract_violation_sites,
                     result_summary.contract_violation_sites));
  summary.deterministic =
      summary.deterministic &&
      throws_summary.deterministic &&
      result_summary.deterministic &&
      summary.exceptional_exit_sites <= summary.unwind_cleanup_sites &&
      summary.cleanup_action_sites <= summary.unwind_cleanup_sites &&
      summary.cleanup_scope_sites <= summary.unwind_cleanup_sites &&
      summary.cleanup_resume_sites <= summary.unwind_cleanup_sites &&
      summary.normalized_sites <= summary.unwind_cleanup_sites &&
      summary.fail_closed_sites <= summary.unwind_cleanup_sites &&
      summary.contract_violation_sites <= summary.unwind_cleanup_sites &&
      summary.normalized_sites + summary.fail_closed_sites ==
          summary.unwind_cleanup_sites;
  return summary;
}

#include "sema/objc3_semantic_passes_unsafe_intrinsic_summary_builders.inc"

#include "sema/objc3_semantic_passes_result_like_summary_builders.inc"

#include "sema/objc3_semantic_passes_await_lowering_summary_builders.inc"

#include "sema/objc3_semantic_passes_error_diagnostics_summary_builders.inc"

#include "sema/objc3_semantic_passes_async_continuation_summary_builders.inc"

#include "sema/objc3_semantic_passes_actor_isolation_summary_builders.inc"

#include "sema/objc3_semantic_passes_task_runtime_summary_builders.inc"

#include "sema/objc3_semantic_passes_concurrency_replay_summary_builders.inc"

#include "sema/objc3_semantic_passes_nserror_bridging_summary_builders.inc"

#include "sema/objc3_semantic_passes_symbol_graph_summary_builders.inc"

static const Objc3MethodInfo *FindMethodInfoWithKind(
    const std::unordered_map<std::string, Objc3MethodInfo> &methods,
    const std::string &selector,
    bool is_class_method) {
  const auto method_it = methods.find(selector);
  if (method_it == methods.end() || method_it->second.is_class_method != is_class_method) {
    return nullptr;
  }
  return &method_it->second;
}

static const Objc3MethodInfo *FindSurfaceMethodInSuperChain(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &interface_name,
    const std::string &selector,
    bool is_class_method,
    bool &missing_base,
    bool &cycle_detected) {
  missing_base = false;
  cycle_detected = false;
  const auto interface_it = surface.interfaces.find(interface_name);
  if (interface_it == surface.interfaces.end()) {
    missing_base = true;
    return nullptr;
  }

  std::string next_super = interface_it->second.super_name;
  std::unordered_set<std::string> visited;
  while (!next_super.empty()) {
    if (!visited.insert(next_super).second) {
      cycle_detected = true;
      return nullptr;
    }
    const auto super_it = surface.interfaces.find(next_super);
    if (super_it == surface.interfaces.end()) {
      missing_base = true;
      return nullptr;
    }
    const Objc3MethodInfo *method =
        FindMethodInfoWithKind(super_it->second.methods, selector, is_class_method);
    if (method != nullptr) {
      return method;
    }
    next_super = super_it->second.super_name;
  }
  return nullptr;
}

static const Objc3MethodInfo *ResolveMethodInOwnerInterface(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &selector,
    bool is_class_method) {
  const auto interface_it = surface.interfaces.find(owner_name);
  if (interface_it == surface.interfaces.end()) {
    return nullptr;
  }
  return FindMethodInfoWithKind(interface_it->second.methods, selector,
                                is_class_method);
}

static const Objc3MethodInfo *ResolveMethodInOwnerImplementation(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &selector,
    bool is_class_method) {
  const auto implementation_it = surface.implementations.find(owner_name);
  if (implementation_it == surface.implementations.end()) {
    return nullptr;
  }
  return FindMethodInfoWithKind(implementation_it->second.methods, selector,
                                is_class_method);
}

static const Objc3MethodInfo *ResolveMethodInCategoryInterface(
    const std::unordered_map<std::string, Objc3InterfaceInfo> &category_interfaces,
    const std::string &owner_identity,
    const std::string &selector,
    bool is_class_method) {
  const auto interface_it = category_interfaces.find(owner_identity);
  if (interface_it == category_interfaces.end()) {
    return nullptr;
  }
  return FindMethodInfoWithKind(interface_it->second.methods, selector,
                                is_class_method);
}

static const Objc3MethodInfo *ResolveMethodInCategoryImplementation(
    const std::unordered_map<std::string, Objc3ImplementationInfo>
        &category_implementations,
    const std::string &owner_identity,
    const std::string &selector,
    bool is_class_method) {
  const auto implementation_it = category_implementations.find(owner_identity);
  if (implementation_it == category_implementations.end()) {
    return nullptr;
  }
  return FindMethodInfoWithKind(implementation_it->second.methods, selector,
                                is_class_method);
}

static const Objc3MethodInfo *ResolveMethodInMergedCategorySurface(
    const Objc3SemanticIntegrationSurface &surface,
    const std::string &owner_name,
    const std::string &selector,
    bool is_class_method) {
  const auto merge_it = surface.category_merge_surfaces.find(owner_name);
  if (merge_it == surface.category_merge_surfaces.end()) {
    return nullptr;
  }
  return FindMethodInfoWithKind(merge_it->second.merged_methods, selector,
