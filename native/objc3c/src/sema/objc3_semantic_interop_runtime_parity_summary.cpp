#include "sema/objc3_semantic_passes.h"

#include "support/objc3_type_profile_helpers.h"

#include <algorithm>
#include <sstream>
#include <string_view>

template <typename CallableDeclT>
static bool HasInteropObjcRuntimeTypeSurface(const CallableDeclT &decl) {
  if (objc3c::support::HasObjCRuntimeReturnTypeSurface(decl)) {
    return true;
  }
  return std::any_of(decl.params.begin(), decl.params.end(),
                     [](const FuncParam &param) {
                       return objc3c::support::HasObjCRuntimeParamTypeSurface(
                           param);
                     });
}

Objc3InteropInteropRuntimeParitySummary
BuildInteropInteropRuntimeParitySummary(
    const Objc3Program &program,
    const Objc3InteropInteropSemanticModelSummary &dependency_summary,
    const std::vector<std::string> &diagnostics) {
  Objc3InteropInteropRuntimeParitySummary summary;

  const auto count_diagnostic_code = [&diagnostics](std::string_view code) {
    return std::count_if(
        diagnostics.begin(), diagnostics.end(),
        [code](const std::string &entry) {
          return entry.find(code) != std::string::npos;
        });
  };

  summary.foreign_callable_sites = dependency_summary.foreign_callable_sites;
  summary.import_module_annotation_sites =
      dependency_summary.import_module_annotation_sites;

  for (const auto &fn : program.functions) {
    if (fn.objc_foreign_declared) {
      ++summary.c_foreign_callable_sites;
      if (HasInteropObjcRuntimeTypeSurface(fn)) {
        ++summary.objc_runtime_parity_callable_sites;
      }
    }
    if (fn.objc_import_module_declared && fn.objc_foreign_declared) {
      ++summary.import_module_foreign_callable_sites;
    }
  }

  const auto accumulate_method_sites =
      [&summary](const auto &method_decl) {
        if (method_decl.objc_foreign_declared) {
          ++summary.objc_method_foreign_callable_sites;
          if (HasInteropObjcRuntimeTypeSurface(method_decl)) {
            ++summary.objc_runtime_parity_callable_sites;
          }
        }
        if (method_decl.objc_import_module_declared &&
            method_decl.objc_foreign_declared) {
          ++summary.import_module_foreign_callable_sites;
        }
      };

  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method_decl : interface_decl.methods) {
      accumulate_method_sites(method_decl);
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method_decl : protocol_decl.methods) {
      accumulate_method_sites(method_decl);
    }
  }

  summary.foreign_definition_rejection_sites = count_diagnostic_code("O3S331");
  summary.import_without_foreign_rejection_sites =
      count_diagnostic_code("O3S332");
  summary.implementation_annotation_rejection_sites =
      count_diagnostic_code("O3S333");

  const bool dependency_surface_present =
      IsReadyObjc3InteropInteropSemanticModelSummary(dependency_summary);
  summary.dependency_required = true;
  summary.declaration_only_foreign_c_enforced =
      dependency_surface_present &&
      summary.foreign_definition_rejection_sites <=
          summary.c_foreign_callable_sites;
  summary.import_module_requires_foreign_enforced =
      dependency_surface_present &&
      summary.import_module_foreign_callable_sites <=
          summary.import_module_annotation_sites &&
      summary.import_without_foreign_rejection_sites <=
          summary.import_module_annotation_sites;
  summary.implementation_annotations_fail_closed =
      dependency_surface_present &&
      summary.implementation_annotation_rejection_sites <=
          dependency_summary.interop_metadata_annotation_sites;
  summary.objc_runtime_parity_classified =
      dependency_surface_present &&
      summary.c_foreign_callable_sites <= summary.foreign_callable_sites &&
      summary.objc_method_foreign_callable_sites <=
          summary.foreign_callable_sites &&
      summary.objc_runtime_parity_callable_sites <=
          summary.foreign_callable_sites;
  summary.ffi_abi_lowering_deferred = true;
  summary.runtime_bridge_generation_deferred = true;
  summary.deterministic =
      summary.declaration_only_foreign_c_enforced &&
      summary.import_module_requires_foreign_enforced &&
      summary.implementation_annotations_fail_closed &&
      summary.objc_runtime_parity_classified;
  summary.ready_for_lowering_and_runtime = summary.deterministic;
  if (!summary.deterministic) {
    summary.failure_reason =
        "interop c and objective-c runtime parity diagnostics must remain deterministic";
  }

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << summary.dependency_contract_id
      << ";foreign-sites=" << summary.foreign_callable_sites << ":"
      << summary.c_foreign_callable_sites << ":"
      << summary.objc_method_foreign_callable_sites
      << ";import-sites=" << summary.import_module_annotation_sites << ":"
      << summary.import_module_foreign_callable_sites
      << ";objc-runtime-sites=" << summary.objc_runtime_parity_callable_sites
      << ";rejections=" << summary.foreign_definition_rejection_sites << ":"
      << summary.import_without_foreign_rejection_sites << ":"
      << summary.implementation_annotation_rejection_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}
