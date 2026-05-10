#include "artifacts/objc3_frontend_interop_semantic_artifacts.h"

#include <algorithm>

#include "ast/objc3_ast_declarations.h"

namespace objc3::artifacts::frontend {

Objc3InteropInteropLoweringContract BuildInteropInteropLoweringContract(
    const Objc3InteropInteropSemanticModelSummary &semantic_summary,
    const Objc3InteropInteropRuntimeParitySummary &runtime_parity_summary,
    const Objc3InteropCppInteropInteractionSummary &cpp_summary,
    const Objc3InteropSwiftInteropIsolationSummary &swift_summary,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary) {
  Objc3InteropInteropLoweringContract contract;
  contract.foreign_callable_sites = semantic_summary.foreign_callable_sites;
  contract.c_foreign_callable_sites =
      runtime_parity_summary.c_foreign_callable_sites;
  contract.objc_runtime_parity_callable_sites =
      runtime_parity_summary.objc_runtime_parity_callable_sites;
  contract.ownership_bridge_callable_sites =
      semantic_summary.bridge_callable_sites +
      cpp_summary.ownership_interaction_sites;
  contract.error_surface_sites = cpp_summary.throws_interaction_sites;
  contract.async_boundary_sites =
      semantic_summary.async_executor_affinity_sites +
      cpp_summary.async_interaction_sites;
  contract.swift_concurrency_metadata_sites =
      swift_summary.actor_owned_swift_callable_sites +
      swift_summary.nonisolated_swift_callable_sites;
  contract.interface_preserved_foreign_callable_sites =
      preservation_summary.local_foreign_callable_count +
      preservation_summary.imported_foreign_callable_count;
  contract.interface_preserved_metadata_annotation_sites =
      preservation_summary.local_import_module_annotation_count +
      preservation_summary.local_imported_module_name_count +
      preservation_summary.local_swift_name_annotation_count +
      preservation_summary.local_swift_private_annotation_count +
      preservation_summary.local_cpp_name_annotation_count +
      preservation_summary.local_header_name_annotation_count +
      preservation_summary.local_named_annotation_payload_count +
      preservation_summary.imported_import_module_annotation_count +
      preservation_summary.imported_imported_module_name_count +
      preservation_summary.imported_swift_name_annotation_count +
      preservation_summary.imported_swift_private_annotation_count +
      preservation_summary.imported_cpp_name_annotation_count +
      preservation_summary.imported_header_name_annotation_count +
      preservation_summary.imported_named_annotation_payload_count;
  contract.guard_blocked_sites =
      runtime_parity_summary.foreign_definition_rejection_sites +
      runtime_parity_summary.import_without_foreign_rejection_sites +
      runtime_parity_summary.implementation_annotation_rejection_sites +
      cpp_summary.ownership_rejection_sites + cpp_summary.throws_rejection_sites +
      cpp_summary.async_rejection_sites +
      swift_summary.swift_private_without_name_rejection_sites +
      swift_summary.actor_isolation_mapping_rejection_sites +
      swift_summary.nonisolated_mapping_rejection_sites +
      swift_summary.implementation_surface_rejection_sites;
  contract.contract_violation_sites = 0;
  contract.deterministic =
      semantic_summary.deterministic &&
      semantic_summary.ready_for_semantic_expansion &&
      runtime_parity_summary.deterministic &&
      runtime_parity_summary.ready_for_lowering_and_runtime &&
      cpp_summary.deterministic &&
      cpp_summary.ready_for_lowering_and_runtime &&
      swift_summary.deterministic &&
      swift_summary.ready_for_lowering_and_runtime &&
      preservation_summary.deterministic &&
      preservation_summary.runtime_import_artifact_ready &&
      preservation_summary.separate_compilation_preservation_ready;
  return contract;
}

Objc3InteropForeignCallLifetimeLoweringContract
BuildInteropForeignCallLifetimeLoweringContract(
    const Objc3Program &program,
    const Objc3InteropInteropLoweringContract &dependency_contract,
    const Objc3InteropCppInteropInteractionSummary &cpp_summary,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary) {
  Objc3InteropForeignCallLifetimeLoweringContract contract;
  contract.foreign_callable_sites = dependency_contract.foreign_callable_sites;
  contract.c_foreign_callable_sites =
      dependency_contract.c_foreign_callable_sites;
  contract.objc_runtime_parity_callable_sites =
      dependency_contract.objc_runtime_parity_callable_sites;

  const auto callable_has_metadata = [](const auto &decl) {
    return decl.objc_foreign_declared || decl.objc_import_module_declared ||
           decl.objc_cxx_name_declared || decl.objc_header_name_declared ||
           decl.objc_swift_name_declared || decl.objc_swift_private_declared ||
           decl.objc_export_header_declared || decl.objc_abi_align_declared ||
           decl.objc_foreign_type_declared || decl.objc_mixed_image_declared ||
           decl.objc_package_entry_declared;
  };
  const auto callable_has_lifetime_bridge = [](const auto &decl) {
    if (!decl.return_ownership_lifetime_profile.empty()) {
      return true;
    }
    return std::any_of(decl.params.begin(), decl.params.end(),
                       [](const auto &param) {
                         return !param.ownership_lifetime_profile.empty();
                       });
  };
  const auto callable_has_ownership_bridge = [](const auto &decl) {
    if (decl.return_ownership_insert_retain ||
        decl.return_ownership_insert_release ||
        decl.return_ownership_insert_autorelease) {
      return true;
    }
    return std::any_of(decl.params.begin(), decl.params.end(),
                       [](const auto &param) {
                         return param.ownership_insert_retain ||
                                param.ownership_insert_release ||
                                param.ownership_insert_autorelease;
                       });
  };

  for (const auto &fn : program.functions) {
    if (callable_has_metadata(fn)) {
      ++contract.metadata_preservation_sites;
    }
    if ((fn.objc_cxx_name_declared || fn.objc_swift_name_declared ||
         fn.objc_swift_private_declared) &&
        callable_has_ownership_bridge(fn)) {
      ++contract.ownership_bridge_sites;
    }
    if ((fn.objc_cxx_name_declared || fn.objc_swift_name_declared ||
         fn.objc_swift_private_declared) &&
        callable_has_lifetime_bridge(fn)) {
      ++contract.lifetime_bridge_sites;
    }
  }

  contract.guard_blocked_sites =
      dependency_contract.guard_blocked_sites +
      cpp_summary.ownership_rejection_sites + cpp_summary.throws_rejection_sites +
      cpp_summary.async_rejection_sites;
  contract.contract_violation_sites =
      dependency_contract.contract_violation_sites;
  contract.deterministic =
      IsValidObjc3InteropInteropLoweringContract(dependency_contract) &&
      dependency_contract.deterministic && cpp_summary.deterministic &&
      cpp_summary.ready_for_lowering_and_runtime &&
      preservation_summary.deterministic &&
      preservation_summary.runtime_import_artifact_ready &&
      preservation_summary.separate_compilation_preservation_ready;
  return contract;
}

}  // namespace objc3::artifacts::frontend
