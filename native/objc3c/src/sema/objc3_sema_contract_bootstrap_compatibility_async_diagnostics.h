#pragma once

#include <cstddef>
#include <string>

inline constexpr const char *kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummaryDependencyContractId =
    "objc3c.concurrency.await.suspension.resume.semantics.v1";
inline constexpr const char *kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummaryContractId =
    "objc3c.concurrency.async.diagnostics.compatibility.completion.v1";
inline constexpr const char *kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummarySurfacePath =
    "frontend.pipeline.semantic_surface.objc_concurrency_async_diagnostics_and_compatibility_completion";
inline constexpr const char *kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummaryRule =
    "async-topology-diagnostics-now-fail-closed-for-non-async-executor-affinity-async-function-prototypes-and-async-throws-while-runnable-frame-and-runtime-integration-remain-later-runtime-work";
inline constexpr const char *kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummaryDeferredRule =
    "async-prototype-import-surfaces-async-error-propagation-abi-and-runnable-executor-runtime-behavior-remain-deferred-to-later-runtime-lanes";

struct Objc3ConcurrencyAsyncDiagnosticsCompatibilitySummary {
  std::string contract_id =
      kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummaryContractId;
  std::string dependency_contract_id =
      kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummaryDependencyContractId;
  std::string surface_path =
      kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummarySurfacePath;
  std::string semantic_model =
      kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummaryRule;
  std::string deferred_model =
      kObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummaryDeferredRule;
  std::size_t async_callable_sites = 0;
  std::size_t executor_affinity_sites = 0;
  std::size_t illegal_non_async_executor_sites = 0;
  std::size_t illegal_async_function_prototype_sites = 0;
  std::size_t illegal_async_throws_sites = 0;
  std::size_t compatibility_diagnostic_sites = 0;
  std::size_t supported_async_callable_sites = 0;
  bool dependency_required = false;
  bool executor_affinity_requires_async_enforced = false;
  bool async_function_prototypes_fail_closed = false;
  bool async_throws_fail_closed = false;
  bool unsupported_topology_fail_closed = false;
  bool deterministic = true;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ConcurrencyAsyncDiagnosticsCompatibilitySummary(
    const Objc3ConcurrencyAsyncDiagnosticsCompatibilitySummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() && summary.dependency_required &&
         summary.executor_affinity_requires_async_enforced &&
         summary.async_function_prototypes_fail_closed &&
         summary.async_throws_fail_closed &&
         summary.unsupported_topology_fail_closed && summary.deterministic &&
         summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}
