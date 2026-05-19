#include "sema/objc3_semantic_passes.h"

#include "sema/objc3_semantic_dispatch_intent_helpers.h"

#include <algorithm>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

Objc3DispatchDispatchIntentLegalitySummary
BuildDispatchDispatchIntentLegalitySummary(
    const Objc3Program &program,
    const Objc3DispatchDispatchIntentSemanticModelSummary &dependency_summary,
    const std::vector<std::string> &diagnostics) {
  Objc3DispatchDispatchIntentLegalitySummary summary;

  const auto count_diagnostic_code = [&diagnostics](std::string_view code) {
    return std::count_if(
        diagnostics.begin(), diagnostics.end(),
        [code](const std::string &entry) {
          return entry.find(code) != std::string::npos;
        });
  };

  for (const auto &interface_decl : program.interfaces) {
    if (!interface_decl.has_category && !interface_decl.super_name.empty()) {
      ++summary.subclass_sites;
    }
  }

  summary.override_sites = dependency_summary.override_lookup_sites;
  summary.illegal_final_superclass_sites = count_diagnostic_code("O3S307");
  summary.illegal_sealed_superclass_sites = count_diagnostic_code("O3S308");
  summary.illegal_final_override_sites = count_diagnostic_code("O3S309");
  summary.illegal_direct_override_sites = count_diagnostic_code("O3S310");

  const bool dependency_surface_present =
      !dependency_summary.contract_id.empty() &&
      !dependency_summary.surface_path.empty() &&
      dependency_summary.ready_for_core_implementation;
  summary.dependency_required = true;
  summary.final_superclass_fail_closed =
      dependency_surface_present &&
      summary.illegal_final_superclass_sites <= summary.subclass_sites;
  summary.sealed_superclass_fail_closed =
      dependency_surface_present &&
      summary.illegal_sealed_superclass_sites <= summary.subclass_sites;
  summary.final_override_fail_closed =
      dependency_surface_present &&
      summary.illegal_final_override_sites <= summary.override_sites;
  summary.direct_override_fail_closed =
      dependency_surface_present &&
      summary.illegal_direct_override_sites <= summary.override_sites;
  summary.lowering_runtime_deferred = true;
  summary.deterministic =
      summary.final_superclass_fail_closed &&
      summary.sealed_superclass_fail_closed &&
      summary.final_override_fail_closed &&
      summary.direct_override_fail_closed;
  summary.ready_for_lowering_and_runtime = summary.deterministic;
  if (!summary.deterministic) {
    summary.failure_reason =
        "dispatch-control legality diagnostics must remain deterministic";
  }

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << summary.dependency_contract_id
      << ";subclass-sites=" << summary.subclass_sites
      << ";override-sites=" << summary.override_sites
      << ";illegal-sites=" << summary.illegal_final_superclass_sites << ":"
      << summary.illegal_sealed_superclass_sites << ":"
      << summary.illegal_final_override_sites << ":"
      << summary.illegal_direct_override_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

Objc3DispatchDispatchIntentCompatibilitySummary
BuildDispatchDispatchIntentCompatibilitySummary(
    const Objc3Program &program,
    const Objc3DispatchDispatchIntentLegalitySummary &dependency_summary,
    const std::vector<std::string> &diagnostics) {
  Objc3DispatchDispatchIntentCompatibilitySummary summary;

  const auto count_diagnostic_code = [&diagnostics](std::string_view code) {
    return std::count_if(
        diagnostics.begin(), diagnostics.end(),
        [code](const std::string &entry) {
          return entry.find(code) != std::string::npos;
        });
  };

  for (const auto &fn : program.functions) {
    if (HasDispatchCallableDispatchIntentAttributes(fn)) {
      ++summary.callable_dispatch_intent_sites;
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method : protocol_decl.methods) {
      if (HasDispatchCallableDispatchIntentAttributes(method)) {
        ++summary.callable_dispatch_intent_sites;
      }
    }
  }
  for (const auto &interface_decl : program.interfaces) {
    if (HasDispatchContainerDispatchIntentAttributes(interface_decl)) {
      ++summary.container_dispatch_intent_sites;
    }
    for (const auto &method : interface_decl.methods) {
      if (HasDispatchCallableDispatchIntentAttributes(method)) {
        ++summary.callable_dispatch_intent_sites;
      }
    }
  }
  for (const auto &implementation_decl : program.implementations) {
    for (const auto &method : implementation_decl.methods) {
      if (HasDispatchCallableDispatchIntentAttributes(method)) {
        ++summary.callable_dispatch_intent_sites;
      }
    }
  }

  summary.illegal_direct_dynamic_conflict_sites =
      count_diagnostic_code("O3S311");
  summary.illegal_final_dynamic_conflict_sites =
      count_diagnostic_code("O3S312");
  summary.illegal_non_method_callable_sites =
      count_diagnostic_code("O3S313");
  summary.illegal_protocol_method_sites =
      count_diagnostic_code("O3S314");
  summary.illegal_category_method_sites =
      count_diagnostic_code("O3S315");
  summary.illegal_category_container_sites =
      count_diagnostic_code("O3S316");

  const bool dependency_surface_present =
      !dependency_summary.contract_id.empty() &&
      !dependency_summary.surface_path.empty() &&
      dependency_summary.ready_for_lowering_and_runtime;
  summary.dependency_required = true;
  summary.callable_conflict_fail_closed =
      dependency_surface_present &&
      summary.illegal_direct_dynamic_conflict_sites <=
          summary.callable_dispatch_intent_sites &&
      summary.illegal_final_dynamic_conflict_sites <=
          summary.callable_dispatch_intent_sites;
  summary.unsupported_callable_topology_fail_closed =
      dependency_surface_present &&
      summary.illegal_non_method_callable_sites <=
          summary.callable_dispatch_intent_sites &&
      summary.illegal_protocol_method_sites <=
          summary.callable_dispatch_intent_sites &&
      summary.illegal_category_method_sites <=
          summary.callable_dispatch_intent_sites;
  summary.unsupported_container_topology_fail_closed =
      dependency_surface_present &&
      summary.illegal_category_container_sites <=
          summary.container_dispatch_intent_sites;
  summary.lowering_runtime_deferred = true;
  summary.deterministic =
      summary.callable_conflict_fail_closed &&
      summary.unsupported_callable_topology_fail_closed &&
      summary.unsupported_container_topology_fail_closed;
  summary.ready_for_lowering_and_runtime = summary.deterministic;
  if (!summary.deterministic) {
    summary.failure_reason =
        "dispatch-control compatibility diagnostics must remain deterministic";
  }

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << summary.dependency_contract_id
      << ";callable-sites=" << summary.callable_dispatch_intent_sites
      << ";container-sites=" << summary.container_dispatch_intent_sites
      << ";illegal-sites=" << summary.illegal_direct_dynamic_conflict_sites
      << ":" << summary.illegal_final_dynamic_conflict_sites << ":"
      << summary.illegal_non_method_callable_sites << ":"
      << summary.illegal_protocol_method_sites << ":"
      << summary.illegal_category_method_sites << ":"
      << summary.illegal_category_container_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}
