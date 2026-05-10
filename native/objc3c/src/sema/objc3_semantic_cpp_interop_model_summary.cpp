#include "sema/objc3_semantic_passes.h"

#include "sema/objc3_semantic_interop_surface_helpers.h"

#include <algorithm>
#include <sstream>
#include <string_view>

Objc3InteropCppInteropInteractionSummary BuildInteropCppInteropInteractionSummary(
    const Objc3Program &program,
    const Objc3InteropInteropRuntimeParitySummary &dependency_summary,
    const std::vector<std::string> &diagnostics) {
  Objc3InteropCppInteropInteractionSummary summary;

  const auto count_diagnostic_code = [&diagnostics](std::string_view code) {
    return std::count_if(
        diagnostics.begin(), diagnostics.end(),
        [code](const std::string &entry) {
          return entry.find(code) != std::string::npos;
        });
  };

  const auto accumulate_callable = [&summary](const auto &decl) {
    if (!HasInteropCppInteropAnnotations(decl)) {
      return;
    }
    ++summary.cpp_interop_callable_sites;
    if (decl.objc_cxx_name_declared) {
      ++summary.cpp_named_callable_sites;
    }
    if (decl.objc_header_name_declared) {
      ++summary.header_named_callable_sites;
    }
    if (HasInteropOwnershipInteractionSurface(decl)) {
      ++summary.ownership_interaction_sites;
    }
    if (decl.throws_declared) {
      ++summary.throws_interaction_sites;
    }
    if (HasInteropAsyncInteractionSurface(decl)) {
      ++summary.async_interaction_sites;
    }
  };

  for (const auto &fn : program.functions) {
    accumulate_callable(fn);
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method_decl : interface_decl.methods) {
      accumulate_callable(method_decl);
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method_decl : protocol_decl.methods) {
      accumulate_callable(method_decl);
    }
  }
  for (const auto &implementation_decl : program.implementations) {
    for (const auto &method_decl : implementation_decl.methods) {
      accumulate_callable(method_decl);
    }
  }

  summary.ownership_rejection_sites = count_diagnostic_code("O3S334");
  summary.throws_rejection_sites = count_diagnostic_code("O3S335");
  summary.async_rejection_sites = count_diagnostic_code("O3S336");

  const bool dependency_surface_present =
      IsReadyObjc3InteropInteropRuntimeParitySummary(dependency_summary);
  summary.dependency_required = true;
  summary.cpp_annotation_profile_reused =
      dependency_surface_present &&
      summary.cpp_named_callable_sites <= summary.cpp_interop_callable_sites &&
      summary.header_named_callable_sites <=
          summary.cpp_interop_callable_sites;
  summary.ownership_interactions_fail_closed =
      dependency_surface_present &&
      summary.ownership_rejection_sites == summary.ownership_interaction_sites;
  summary.throws_interactions_fail_closed =
      dependency_surface_present &&
      summary.throws_rejection_sites == summary.throws_interaction_sites;
  summary.async_interactions_fail_closed =
      dependency_surface_present &&
      summary.async_rejection_sites == summary.async_interaction_sites;
  summary.ffi_abi_lowering_deferred = true;
  summary.runtime_bridge_generation_deferred = true;
  summary.deterministic =
      summary.cpp_annotation_profile_reused &&
      summary.ownership_interactions_fail_closed &&
      summary.throws_interactions_fail_closed &&
      summary.async_interactions_fail_closed;
  summary.ready_for_lowering_and_runtime = summary.deterministic;
  if (!summary.deterministic) {
    summary.failure_reason =
        "interop cxx-facing ownership, throws, and async diagnostics must remain deterministic";
  }

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << summary.dependency_contract_id
      << ";cpp-callables=" << summary.cpp_interop_callable_sites << ":"
      << summary.cpp_named_callable_sites << ":"
      << summary.header_named_callable_sites
      << ";interactions=" << summary.ownership_interaction_sites << ":"
      << summary.throws_interaction_sites << ":" << summary.async_interaction_sites
      << ";rejections=" << summary.ownership_rejection_sites << ":"
      << summary.throws_rejection_sites << ":" << summary.async_rejection_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}
