#include "sema/objc3_semantic_passes.h"

#include "sema/objc3_semantic_interop_surface_helpers.h"

#include <algorithm>
#include <sstream>
#include <string_view>

Objc3InteropSwiftInteropIsolationSummary
BuildInteropSwiftInteropIsolationSummary(
    const Objc3Program &program,
    const Objc3InteropCppInteropInteractionSummary &dependency_summary,
    const std::vector<std::string> &diagnostics) {
  Objc3InteropSwiftInteropIsolationSummary summary;

  const auto count_diagnostic_code = [&diagnostics](std::string_view code) {
    return std::count_if(
        diagnostics.begin(), diagnostics.end(),
        [code](const std::string &entry) {
          return entry.find(code) != std::string::npos;
        });
  };
  const auto is_actor_interface =
      [&program](const std::string &name) -> bool {
    const auto interface_it =
        std::find_if(program.interfaces.begin(), program.interfaces.end(),
                     [&name](const Objc3InterfaceDecl &interface_decl) {
                       return !interface_decl.has_category &&
                              interface_decl.name == name &&
                              interface_decl.is_actor;
                     });
    return interface_it != program.interfaces.end();
  };
  const auto accumulate_callable =
      [&summary](const auto &decl, bool actor_owned,
                 bool implementation_surface) {
        if (!HasInteropSwiftInteropAnnotations(decl)) {
          return;
        }
        ++summary.swift_interop_callable_sites;
        if (decl.objc_swift_name_declared) {
          ++summary.swift_named_callable_sites;
        }
        if (decl.objc_swift_private_declared) {
          ++summary.swift_private_callable_sites;
        }
        if (decl.objc_swift_private_declared && !decl.objc_swift_name_declared) {
          ++summary.swift_private_without_name_sites;
        }
        if (actor_owned) {
          ++summary.actor_owned_swift_callable_sites;
        }
        if (decl.objc_nonisolated_declared) {
          ++summary.nonisolated_swift_callable_sites;
        }
        if (implementation_surface) {
          ++summary.implementation_swift_callable_sites;
        }
      };

  for (const auto &fn : program.functions) {
    accumulate_callable(fn, false, false);
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method_decl : interface_decl.methods) {
      accumulate_callable(method_decl, interface_decl.is_actor, false);
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method_decl : protocol_decl.methods) {
      accumulate_callable(method_decl, false, false);
    }
  }
  for (const auto &implementation_decl : program.implementations) {
    const bool actor_owned =
        !implementation_decl.has_category &&
        is_actor_interface(implementation_decl.name);
    for (const auto &method_decl : implementation_decl.methods) {
      accumulate_callable(method_decl, actor_owned, true);
    }
  }

  summary.swift_private_without_name_rejection_sites =
      count_diagnostic_code("O3S337");
  summary.actor_isolation_mapping_rejection_sites =
      count_diagnostic_code("O3S338");
  summary.nonisolated_mapping_rejection_sites =
      count_diagnostic_code("O3S339");
  summary.implementation_surface_rejection_sites =
      count_diagnostic_code("O3S340");

  const bool dependency_surface_present =
      IsReadyObjc3InteropCppInteropInteractionSummary(dependency_summary);
  summary.dependency_required = true;
  summary.swift_metadata_profile_reused =
      dependency_surface_present &&
      summary.swift_named_callable_sites <= summary.swift_interop_callable_sites &&
      summary.swift_private_callable_sites <=
          summary.swift_interop_callable_sites;
  summary.swift_private_requires_name_enforced =
      dependency_surface_present &&
      summary.swift_private_without_name_rejection_sites ==
          summary.swift_private_without_name_sites;
  summary.actor_isolation_mapping_fail_closed =
      dependency_surface_present &&
      summary.actor_isolation_mapping_rejection_sites ==
          summary.actor_owned_swift_callable_sites;
  summary.nonisolated_mapping_fail_closed =
      dependency_surface_present &&
      summary.nonisolated_mapping_rejection_sites ==
          summary.nonisolated_swift_callable_sites;
  summary.implementation_surface_fail_closed =
      dependency_surface_present &&
      summary.implementation_surface_rejection_sites ==
          summary.implementation_swift_callable_sites;
  summary.ffi_abi_lowering_deferred = true;
  summary.runtime_bridge_generation_deferred = true;
  summary.deterministic =
      summary.swift_metadata_profile_reused &&
      summary.swift_private_requires_name_enforced &&
      summary.actor_isolation_mapping_fail_closed &&
      summary.nonisolated_mapping_fail_closed &&
      summary.implementation_surface_fail_closed;
  summary.ready_for_lowering_and_runtime = summary.deterministic;
  if (!summary.deterministic) {
    summary.failure_reason =
        "interop swift metadata and isolation diagnostics must remain deterministic";
  }

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << summary.dependency_contract_id
      << ";swift-callables=" << summary.swift_interop_callable_sites << ":"
      << summary.swift_named_callable_sites << ":"
      << summary.swift_private_callable_sites
      << ";unsupported=" << summary.swift_private_without_name_sites << ":"
      << summary.actor_owned_swift_callable_sites << ":"
      << summary.nonisolated_swift_callable_sites << ":"
      << summary.implementation_swift_callable_sites
      << ";rejections=" << summary.swift_private_without_name_rejection_sites
      << ":" << summary.actor_isolation_mapping_rejection_sites << ":"
      << summary.nonisolated_mapping_rejection_sites << ":"
      << summary.implementation_surface_rejection_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}
