#include "artifacts/objc3_frontend_interop_semantic_artifacts.h"

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include "ast/objc3_ast_declarations.h"
#include "io/json/json_writer.h"
#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

std::string BuildStringArrayJson(const std::vector<std::string> &values) {
  return objc3::io::json::RenderJsonStringArray(values);
}

}  // namespace

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

static std::vector<std::string> CollectInteropLocalImportModuleNames(
    const Objc3Program &program) {
  std::vector<std::string> names;
  const auto accumulate_callable = [&names](const auto &decl) {
    if (decl.objc_import_module_declared &&
        !decl.objc_import_module_name.empty()) {
      names.push_back(decl.objc_import_module_name);
    }
  };

  for (const auto &fn : program.functions) {
    accumulate_callable(fn);
  }
  for (const auto &interface_decl : program.interfaces) {
    for (const auto &method : interface_decl.methods) {
      accumulate_callable(method);
    }
  }
  for (const auto &protocol_decl : program.protocols) {
    for (const auto &method : protocol_decl.methods) {
      accumulate_callable(method);
    }
  }
  for (const auto &implementation : program.implementations) {
    for (const auto &method : implementation.methods) {
      accumulate_callable(method);
    }
  }

  std::sort(names.begin(), names.end());
  names.erase(std::unique(names.begin(), names.end()), names.end());
  return names;
}

Objc3InteropForeignSurfaceInterfacePreservationSummary
BuildInteropForeignSurfaceInterfacePreservationSummary(
    const Objc3Program &program,
    const Objc3FrontendInteropForeignImportSourceClosureSummary
        &foreign_import_source_summary,
    const Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary
        &cpp_swift_source_summary,
    bool runtime_import_artifact_ready,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces) {
  Objc3InteropForeignSurfaceInterfacePreservationSummary summary;
  summary.foreign_import_source_replay_key =
      foreign_import_source_summary.replay_key;
  summary.cpp_swift_source_replay_key = cpp_swift_source_summary.replay_key;
  summary.local_import_module_names_lexicographic =
      CollectInteropLocalImportModuleNames(program);
  summary.local_foreign_callable_count =
      foreign_import_source_summary.foreign_callable_sites;
  summary.local_import_module_annotation_count =
      foreign_import_source_summary.import_module_annotation_sites;
  summary.local_imported_module_name_count =
      foreign_import_source_summary.imported_module_name_sites;
  summary.local_swift_name_annotation_count =
      cpp_swift_source_summary.swift_name_annotation_sites;
  summary.local_swift_private_annotation_count =
      cpp_swift_source_summary.swift_private_annotation_sites;
  summary.local_cpp_name_annotation_count =
      cpp_swift_source_summary.cpp_name_annotation_sites;
  summary.local_header_name_annotation_count =
      cpp_swift_source_summary.header_name_annotation_sites;
  summary.local_named_annotation_payload_count =
      cpp_swift_source_summary.named_annotation_payload_sites;
  summary.runtime_import_artifact_ready =
      runtime_import_artifact_ready &&
      foreign_import_source_summary.ready_for_semantic_expansion &&
      cpp_swift_source_summary.ready_for_semantic_expansion;
  summary.deterministic =
      foreign_import_source_summary.deterministic_handoff &&
      cpp_swift_source_summary.deterministic_handoff &&
      summary.local_import_module_names_lexicographic.size() <=
          summary.local_imported_module_name_count;
  for (const auto &surface : imported_runtime_module_surfaces) {
    if (!surface.interop_foreign_surface_interface_preservation_present) {
      continue;
    }
    ++summary.imported_module_count;
    if (!surface.frontend_closure_summary.module_name.empty()) {
      summary.imported_provider_module_names_lexicographic.push_back(
          surface.frontend_closure_summary.module_name);
    }
    summary.imported_foreign_callable_count +=
        surface.interop_local_foreign_callable_count;
    summary.imported_import_module_annotation_count +=
        surface.interop_local_import_module_annotation_count;
    summary.imported_imported_module_name_count +=
        surface.interop_local_imported_module_name_count;
    summary.imported_swift_name_annotation_count +=
        surface.interop_local_swift_name_annotation_count;
    summary.imported_swift_private_annotation_count +=
        surface.interop_local_swift_private_annotation_count;
    summary.imported_cpp_name_annotation_count +=
        surface.interop_local_cpp_name_annotation_count;
    summary.imported_header_name_annotation_count +=
        surface.interop_local_header_name_annotation_count;
    summary.imported_named_annotation_payload_count +=
        surface.interop_local_named_annotation_payload_count;
    summary.deterministic =
        summary.deterministic && surface.interop_deterministic;
  }
  std::sort(summary.imported_provider_module_names_lexicographic.begin(),
            summary.imported_provider_module_names_lexicographic.end());
  summary.separate_compilation_preservation_ready =
      summary.runtime_import_artifact_ready &&
      summary.imported_provider_module_names_lexicographic.size() ==
          summary.imported_module_count;
  std::ostringstream replay_key;
  replay_key << summary.contract_id
             << ";runtime_import_artifact_ready="
             << (summary.runtime_import_artifact_ready ? "true" : "false")
             << ";separate_compilation_preservation_ready="
             << (summary.separate_compilation_preservation_ready ? "true"
                                                                 : "false")
             << ";imported_module_count=" << summary.imported_module_count
             << ";deterministic="
             << (summary.deterministic ? "true" : "false")
             << ";foreign_import_source_replay_key="
             << summary.foreign_import_source_replay_key
             << ";cpp_swift_source_replay_key="
             << summary.cpp_swift_source_replay_key
             << ";local_foreign_callable_count="
             << summary.local_foreign_callable_count
             << ";local_import_module_annotation_count="
             << summary.local_import_module_annotation_count
             << ";local_imported_module_name_count="
             << summary.local_imported_module_name_count
             << ";local_swift_name_annotation_count="
             << summary.local_swift_name_annotation_count
             << ";local_swift_private_annotation_count="
             << summary.local_swift_private_annotation_count
             << ";local_cpp_name_annotation_count="
             << summary.local_cpp_name_annotation_count
             << ";local_header_name_annotation_count="
             << summary.local_header_name_annotation_count
             << ";local_named_annotation_payload_count="
             << summary.local_named_annotation_payload_count
             << ";imported_foreign_callable_count="
             << summary.imported_foreign_callable_count
             << ";imported_import_module_annotation_count="
             << summary.imported_import_module_annotation_count
             << ";imported_imported_module_name_count="
             << summary.imported_imported_module_name_count
             << ";imported_swift_name_annotation_count="
             << summary.imported_swift_name_annotation_count
             << ";imported_swift_private_annotation_count="
             << summary.imported_swift_private_annotation_count
             << ";imported_cpp_name_annotation_count="
             << summary.imported_cpp_name_annotation_count
             << ";imported_header_name_annotation_count="
             << summary.imported_header_name_annotation_count
             << ";imported_named_annotation_payload_count="
             << summary.imported_named_annotation_payload_count;
  summary.replay_key = replay_key.str();
  return summary;
}

static std::size_t CountInteropInterfaceAnnotationSites(
    const Objc3InteropForeignSurfaceInterfacePreservationSummary &summary,
    bool imported) {
  if (imported) {
    return summary.imported_import_module_annotation_count +
           summary.imported_imported_module_name_count +
           summary.imported_swift_name_annotation_count +
           summary.imported_swift_private_annotation_count +
           summary.imported_cpp_name_annotation_count +
           summary.imported_header_name_annotation_count +
           summary.imported_named_annotation_payload_count;
  }
  return summary.local_import_module_annotation_count +
         summary.local_imported_module_name_count +
         summary.local_swift_name_annotation_count +
         summary.local_swift_private_annotation_count +
         summary.local_cpp_name_annotation_count +
         summary.local_header_name_annotation_count +
         summary.local_named_annotation_payload_count;
}

Objc3InteropFfiMetadataInterfacePreservationContract
BuildInteropFfiMetadataInterfacePreservationContract(
    const Objc3InteropForeignCallLifetimeLoweringContract &lowering_contract,
    const std::string &lowering_replay_key,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces,
    bool runtime_import_artifact_ready,
    std::string &replay_key_out) {
  Objc3InteropFfiMetadataInterfacePreservationContract contract;
  contract.local_foreign_callable_count =
      lowering_contract.foreign_callable_sites;
  contract.local_metadata_preservation_sites =
      lowering_contract.metadata_preservation_sites;
  contract.local_interface_annotation_sites =
      CountInteropInterfaceAnnotationSites(preservation_summary, false);
  contract.runtime_import_artifact_ready =
      runtime_import_artifact_ready && lowering_contract.deterministic &&
      preservation_summary.runtime_import_artifact_ready;
  contract.deterministic =
      lowering_contract.deterministic && preservation_summary.deterministic;

  for (const auto &surface : imported_runtime_module_surfaces) {
    if (!surface.interop_ffi_metadata_interface_preservation_present) {
      continue;
    }
    ++contract.imported_module_count;
    contract.imported_foreign_callable_count +=
        surface.interop_ffi_local_foreign_callable_count;
    contract.imported_metadata_preservation_sites +=
        surface.interop_ffi_local_metadata_preservation_sites;
    contract.imported_interface_annotation_sites +=
        surface.interop_ffi_local_interface_annotation_sites;
    contract.deterministic =
        contract.deterministic && surface.interop_ffi_deterministic;
  }
  contract.separate_compilation_preservation_ready =
      contract.runtime_import_artifact_ready;

  std::ostringstream replay_key;
  replay_key
      << Objc3InteropFfiMetadataInterfacePreservationReplayKey(contract)
      << ";lowering_replay_key=" << lowering_replay_key
      << ";preservation_replay_key=" << preservation_summary.replay_key;
  replay_key_out = replay_key.str();
  return contract;
}

Objc3InteropHeaderModuleBridgeGenerationSummary
BuildInteropHeaderModuleBridgeGenerationSummary(
    const Objc3Program &program,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary,
    const Objc3InteropFfiMetadataInterfacePreservationContract
        &ffi_preservation_contract,
    const std::string &ffi_preservation_replay_key,
    const std::vector<Objc3ImportedRuntimeModuleSurface>
        &imported_runtime_module_surfaces) {
  Objc3InteropHeaderModuleBridgeGenerationSummary summary;
  summary.local_import_module_names_lexicographic =
      preservation_summary.local_import_module_names_lexicographic;
  summary.local_foreign_callable_count =
      ffi_preservation_contract.local_foreign_callable_count;
  summary.local_import_module_name_count =
      preservation_summary.local_imported_module_name_count;
  summary.local_cpp_name_annotation_count =
      preservation_summary.local_cpp_name_annotation_count;
  summary.local_header_name_annotation_count =
      preservation_summary.local_header_name_annotation_count;
  summary.local_swift_name_annotation_count =
      preservation_summary.local_swift_name_annotation_count;
  summary.runtime_generation_ready =
      ffi_preservation_contract.runtime_import_artifact_ready &&
      ffi_preservation_contract.separate_compilation_preservation_ready &&
      ffi_preservation_contract.deterministic &&
      summary.local_foreign_callable_count > 0;
  summary.deterministic =
      preservation_summary.deterministic &&
      ffi_preservation_contract.deterministic &&
      summary.local_import_module_names_lexicographic.size() <=
          summary.local_import_module_name_count;
  summary.preservation_replay_key = ffi_preservation_replay_key;
  for (const auto &surface : imported_runtime_module_surfaces) {
    if (!surface.interop_header_module_bridge_generation_present) {
      continue;
    }
    ++summary.imported_module_count;
    if (!surface.frontend_closure_summary.module_name.empty()) {
      summary.imported_provider_module_names_lexicographic.push_back(
          surface.frontend_closure_summary.module_name);
    }
    summary.deterministic =
        summary.deterministic &&
        surface.interop_header_module_bridge_deterministic;
  }
  std::sort(summary.imported_provider_module_names_lexicographic.begin(),
            summary.imported_provider_module_names_lexicographic.end());
  summary.cross_module_packaging_ready =
      summary.runtime_generation_ready &&
      summary.imported_provider_module_names_lexicographic.size() ==
          summary.imported_module_count;

  std::ostringstream replay_key;
  replay_key << summary.contract_id
             << ";source_contract_id=" << summary.source_contract_id
             << ";preservation_contract_id="
             << summary.preservation_contract_id
             << ";header_artifact_relative_path="
             << summary.header_artifact_relative_path
             << ";module_artifact_relative_path="
             << summary.module_artifact_relative_path
             << ";bridge_artifact_relative_path="
             << summary.bridge_artifact_relative_path
             << ";local_foreign_callable_count="
             << summary.local_foreign_callable_count
             << ";local_import_module_name_count="
             << summary.local_import_module_name_count
             << ";local_cpp_name_annotation_count="
             << summary.local_cpp_name_annotation_count
             << ";local_header_name_annotation_count="
             << summary.local_header_name_annotation_count
             << ";local_swift_name_annotation_count="
             << summary.local_swift_name_annotation_count
             << ";imported_module_count=" << summary.imported_module_count
             << ";runtime_generation_ready="
             << (summary.runtime_generation_ready ? "true" : "false")
             << ";cross_module_packaging_ready="
             << (summary.cross_module_packaging_ready ? "true" : "false")
             << ";deterministic="
             << (summary.deterministic ? "true" : "false")
             << ";preservation_replay_key="
             << summary.preservation_replay_key;
  summary.replay_key = replay_key.str();
  (void)program;
  return summary;
}

std::string BuildInteropInteropSemanticModelSummaryJson(
    const Objc3InteropInteropSemanticModelSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(summary.deferred_model)
      << "\",\"foreign_callable_sites\":" << summary.foreign_callable_sites
      << ",\"import_module_annotation_sites\":"
      << summary.import_module_annotation_sites
      << ",\"imported_module_name_sites\":"
      << summary.imported_module_name_sites
      << ",\"export_header_annotation_sites\":"
      << summary.export_header_annotation_sites
      << ",\"export_header_name_sites\":"
      << summary.export_header_name_sites
      << ",\"mixed_image_annotation_sites\":"
      << summary.mixed_image_annotation_sites
      << ",\"mixed_image_name_sites\":" << summary.mixed_image_name_sites
      << ",\"package_entry_annotation_sites\":"
      << summary.package_entry_annotation_sites
      << ",\"package_entry_name_sites\":"
      << summary.package_entry_name_sites
      << ",\"swift_name_annotation_sites\":"
      << summary.swift_name_annotation_sites
      << ",\"swift_private_annotation_sites\":"
      << summary.swift_private_annotation_sites
      << ",\"cpp_name_annotation_sites\":"
      << summary.cpp_name_annotation_sites
      << ",\"header_name_annotation_sites\":"
      << summary.header_name_annotation_sites
      << ",\"abi_alignment_annotation_sites\":"
      << summary.abi_alignment_annotation_sites
      << ",\"foreign_type_annotation_sites\":"
      << summary.foreign_type_annotation_sites
      << ",\"named_annotation_payload_sites\":"
      << summary.named_annotation_payload_sites
      << ",\"retainable_family_callable_sites\":"
      << summary.retainable_family_callable_sites
      << ",\"bridge_callable_sites\":" << summary.bridge_callable_sites
      << ",\"async_executor_affinity_sites\":"
      << summary.async_executor_affinity_sites
      << ",\"actor_hazard_sites\":" << summary.actor_hazard_sites
      << ",\"interop_metadata_annotation_sites\":"
      << summary.interop_metadata_annotation_sites
      << ",\"source_dependency_required\":"
      << (summary.source_dependency_required ? "true" : "false")
      << ",\"foreign_annotation_source_supported\":"
      << (summary.foreign_annotation_source_supported ? "true" : "false")
      << ",\"ownership_interaction_profile_frozen\":"
      << (summary.ownership_interaction_profile_frozen ? "true" : "false")
      << ",\"error_bridge_profile_reused\":"
      << (summary.error_bridge_profile_reused ? "true" : "false")
      << ",\"async_affinity_profile_reused\":"
      << (summary.async_affinity_profile_reused ? "true" : "false")
      << ",\"actor_hazard_profile_reused\":"
      << (summary.actor_hazard_profile_reused ? "true" : "false")
      << ",\"metadata_payload_profile_frozen\":"
      << (summary.metadata_payload_profile_frozen ? "true" : "false")
      << ",\"ffi_abi_lowering_deferred\":"
      << (summary.ffi_abi_lowering_deferred ? "true" : "false")
      << ",\"runtime_bridge_generation_deferred\":"
      << (summary.runtime_bridge_generation_deferred ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildInteropInteropRuntimeParitySummaryJson(
    const Objc3InteropInteropRuntimeParitySummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(summary.deferred_model)
      << "\",\"foreign_callable_sites\":" << summary.foreign_callable_sites
      << ",\"c_foreign_callable_sites\":" << summary.c_foreign_callable_sites
      << ",\"objc_method_foreign_callable_sites\":"
      << summary.objc_method_foreign_callable_sites
      << ",\"import_module_annotation_sites\":"
      << summary.import_module_annotation_sites
      << ",\"import_module_foreign_callable_sites\":"
      << summary.import_module_foreign_callable_sites
      << ",\"objc_runtime_parity_callable_sites\":"
      << summary.objc_runtime_parity_callable_sites
      << ",\"foreign_definition_rejection_sites\":"
      << summary.foreign_definition_rejection_sites
      << ",\"import_without_foreign_rejection_sites\":"
      << summary.import_without_foreign_rejection_sites
      << ",\"implementation_annotation_rejection_sites\":"
      << summary.implementation_annotation_rejection_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"declaration_only_foreign_c_enforced\":"
      << (summary.declaration_only_foreign_c_enforced ? "true" : "false")
      << ",\"import_module_requires_foreign_enforced\":"
      << (summary.import_module_requires_foreign_enforced ? "true" : "false")
      << ",\"implementation_annotations_fail_closed\":"
      << (summary.implementation_annotations_fail_closed ? "true" : "false")
      << ",\"objc_runtime_parity_classified\":"
      << (summary.objc_runtime_parity_classified ? "true" : "false")
      << ",\"ffi_abi_lowering_deferred\":"
      << (summary.ffi_abi_lowering_deferred ? "true" : "false")
      << ",\"runtime_bridge_generation_deferred\":"
      << (summary.runtime_bridge_generation_deferred ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildInteropCppInteropInteractionSummaryJson(
    const Objc3InteropCppInteropInteractionSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(summary.deferred_model)
      << "\",\"cpp_interop_callable_sites\":"
      << summary.cpp_interop_callable_sites
      << ",\"cpp_named_callable_sites\":" << summary.cpp_named_callable_sites
      << ",\"header_named_callable_sites\":"
      << summary.header_named_callable_sites
      << ",\"ownership_interaction_sites\":"
      << summary.ownership_interaction_sites
      << ",\"throws_interaction_sites\":" << summary.throws_interaction_sites
      << ",\"async_interaction_sites\":" << summary.async_interaction_sites
      << ",\"ownership_rejection_sites\":"
      << summary.ownership_rejection_sites
      << ",\"throws_rejection_sites\":" << summary.throws_rejection_sites
      << ",\"async_rejection_sites\":" << summary.async_rejection_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"cpp_annotation_profile_reused\":"
      << (summary.cpp_annotation_profile_reused ? "true" : "false")
      << ",\"ownership_interactions_fail_closed\":"
      << (summary.ownership_interactions_fail_closed ? "true" : "false")
      << ",\"throws_interactions_fail_closed\":"
      << (summary.throws_interactions_fail_closed ? "true" : "false")
      << ",\"async_interactions_fail_closed\":"
      << (summary.async_interactions_fail_closed ? "true" : "false")
      << ",\"ffi_abi_lowering_deferred\":"
      << (summary.ffi_abi_lowering_deferred ? "true" : "false")
      << ",\"runtime_bridge_generation_deferred\":"
      << (summary.runtime_bridge_generation_deferred ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildInteropSwiftInteropIsolationSummaryJson(
    const Objc3InteropSwiftInteropIsolationSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(summary.deferred_model)
      << "\",\"swift_interop_callable_sites\":"
      << summary.swift_interop_callable_sites
      << ",\"swift_named_callable_sites\":"
      << summary.swift_named_callable_sites
      << ",\"swift_private_callable_sites\":"
      << summary.swift_private_callable_sites
      << ",\"swift_private_without_name_sites\":"
      << summary.swift_private_without_name_sites
      << ",\"actor_owned_swift_callable_sites\":"
      << summary.actor_owned_swift_callable_sites
      << ",\"nonisolated_swift_callable_sites\":"
      << summary.nonisolated_swift_callable_sites
      << ",\"implementation_swift_callable_sites\":"
      << summary.implementation_swift_callable_sites
      << ",\"swift_private_without_name_rejection_sites\":"
      << summary.swift_private_without_name_rejection_sites
      << ",\"actor_isolation_mapping_rejection_sites\":"
      << summary.actor_isolation_mapping_rejection_sites
      << ",\"nonisolated_mapping_rejection_sites\":"
      << summary.nonisolated_mapping_rejection_sites
      << ",\"implementation_surface_rejection_sites\":"
      << summary.implementation_surface_rejection_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"swift_metadata_profile_reused\":"
      << (summary.swift_metadata_profile_reused ? "true" : "false")
      << ",\"swift_private_requires_name_enforced\":"
      << (summary.swift_private_requires_name_enforced ? "true" : "false")
      << ",\"actor_isolation_mapping_fail_closed\":"
      << (summary.actor_isolation_mapping_fail_closed ? "true" : "false")
      << ",\"nonisolated_mapping_fail_closed\":"
      << (summary.nonisolated_mapping_fail_closed ? "true" : "false")
      << ",\"implementation_surface_fail_closed\":"
      << (summary.implementation_surface_fail_closed ? "true" : "false")
      << ",\"ffi_abi_lowering_deferred\":"
      << (summary.ffi_abi_lowering_deferred ? "true" : "false")
      << ",\"runtime_bridge_generation_deferred\":"
      << (summary.runtime_bridge_generation_deferred ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildInteropForeignSurfaceInterfacePreservationSummaryJson(
    const Objc3InteropForeignSurfaceInterfacePreservationSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"foreign_import_source_contract_id\":\""
      << EscapeJsonString(summary.foreign_import_source_contract_id)
      << "\",\"cpp_swift_source_contract_id\":\""
      << EscapeJsonString(summary.cpp_swift_source_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"import_artifact_member_name\":\""
      << EscapeJsonString(summary.import_artifact_member_name)
      << "\",\"source_model\":\"" << EscapeJsonString(summary.source_model)
      << "\",\"preservation_model\":\""
      << EscapeJsonString(summary.preservation_model)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(summary.fail_closed_model)
      << "\",\"foreign_import_source_replay_key\":\""
      << EscapeJsonString(summary.foreign_import_source_replay_key)
      << "\",\"cpp_swift_source_replay_key\":\""
      << EscapeJsonString(summary.cpp_swift_source_replay_key)
      << "\",\"local_import_module_names_lexicographic\":"
      << BuildStringArrayJson(summary.local_import_module_names_lexicographic)
      << ",\"imported_provider_module_names_lexicographic\":"
      << BuildStringArrayJson(
             summary.imported_provider_module_names_lexicographic)
      << ",\"local_foreign_callable_count\":"
      << summary.local_foreign_callable_count
      << ",\"local_import_module_annotation_count\":"
      << summary.local_import_module_annotation_count
      << ",\"local_imported_module_name_count\":"
      << summary.local_imported_module_name_count
      << ",\"local_swift_name_annotation_count\":"
      << summary.local_swift_name_annotation_count
      << ",\"local_swift_private_annotation_count\":"
      << summary.local_swift_private_annotation_count
      << ",\"local_cpp_name_annotation_count\":"
      << summary.local_cpp_name_annotation_count
      << ",\"local_header_name_annotation_count\":"
      << summary.local_header_name_annotation_count
      << ",\"local_named_annotation_payload_count\":"
      << summary.local_named_annotation_payload_count
      << ",\"imported_module_count\":" << summary.imported_module_count
      << ",\"imported_foreign_callable_count\":"
      << summary.imported_foreign_callable_count
      << ",\"imported_import_module_annotation_count\":"
      << summary.imported_import_module_annotation_count
      << ",\"imported_imported_module_name_count\":"
      << summary.imported_imported_module_name_count
      << ",\"imported_swift_name_annotation_count\":"
      << summary.imported_swift_name_annotation_count
      << ",\"imported_swift_private_annotation_count\":"
      << summary.imported_swift_private_annotation_count
      << ",\"imported_cpp_name_annotation_count\":"
      << summary.imported_cpp_name_annotation_count
      << ",\"imported_header_name_annotation_count\":"
      << summary.imported_header_name_annotation_count
      << ",\"imported_named_annotation_payload_count\":"
      << summary.imported_named_annotation_payload_count
      << ",\"runtime_import_artifact_ready\":"
      << (summary.runtime_import_artifact_ready ? "true" : "false")
      << ",\"separate_compilation_preservation_ready\":"
      << (summary.separate_compilation_preservation_ready ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildInteropHeaderModuleBridgeGenerationSummaryJson(
    const Objc3InteropHeaderModuleBridgeGenerationSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(summary.source_contract_id)
      << "\",\"preservation_contract_id\":\""
      << EscapeJsonString(summary.preservation_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"import_artifact_member_name\":\""
      << EscapeJsonString(summary.import_artifact_member_name)
      << "\",\"generation_model\":\""
      << EscapeJsonString(summary.generation_model)
      << "\",\"packaging_model\":\""
      << EscapeJsonString(summary.packaging_model)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(summary.fail_closed_model)
      << "\",\"header_artifact_relative_path\":\""
      << EscapeJsonString(summary.header_artifact_relative_path)
      << "\",\"module_artifact_relative_path\":\""
      << EscapeJsonString(summary.module_artifact_relative_path)
      << "\",\"bridge_artifact_relative_path\":\""
      << EscapeJsonString(summary.bridge_artifact_relative_path)
      << "\",\"local_import_module_names_lexicographic\":"
      << BuildStringArrayJson(summary.local_import_module_names_lexicographic)
      << ",\"imported_provider_module_names_lexicographic\":"
      << BuildStringArrayJson(
             summary.imported_provider_module_names_lexicographic)
      << ",\"local_foreign_callable_count\":"
      << summary.local_foreign_callable_count
      << ",\"local_import_module_name_count\":"
      << summary.local_import_module_name_count
      << ",\"local_cpp_name_annotation_count\":"
      << summary.local_cpp_name_annotation_count
      << ",\"local_header_name_annotation_count\":"
      << summary.local_header_name_annotation_count
      << ",\"local_swift_name_annotation_count\":"
      << summary.local_swift_name_annotation_count
      << ",\"imported_module_count\":" << summary.imported_module_count
      << ",\"runtime_generation_ready\":"
      << (summary.runtime_generation_ready ? "true" : "false")
      << ",\"cross_module_packaging_ready\":"
      << (summary.cross_module_packaging_ready ? "true" : "false")
      << ",\"deterministic\":"
      << (summary.deterministic ? "true" : "false")
      << ",\"preservation_replay_key\":\""
      << EscapeJsonString(summary.preservation_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildInteropInteropLoweringContractJson(
    const Objc3InteropInteropSemanticModelSummary &semantic_summary,
    const Objc3InteropInteropRuntimeParitySummary &runtime_parity_summary,
    const Objc3InteropCppInteropInteractionSummary &cpp_summary,
    const Objc3InteropSwiftInteropIsolationSummary &swift_summary,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary,
    const Objc3InteropInteropLoweringContract &contract,
    const std::string &replay_key) {
  const bool ready_for_ir_emission =
      contract.deterministic &&
      IsValidObjc3InteropInteropLoweringContract(contract);
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3InteropInteropLoweringContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3InteropInteropLoweringSurfacePath)
      << "\",\"semantic_contract_id\":\""
      << EscapeJsonString(semantic_summary.contract_id)
      << "\",\"runtime_parity_contract_id\":\""
      << EscapeJsonString(runtime_parity_summary.contract_id)
      << "\",\"cpp_interaction_contract_id\":\""
      << EscapeJsonString(cpp_summary.contract_id)
      << "\",\"swift_isolation_contract_id\":\""
      << EscapeJsonString(swift_summary.contract_id)
      << "\",\"preservation_contract_id\":\""
      << EscapeJsonString(preservation_summary.contract_id)
      << "\",\"lane_contract_id\":\""
      << EscapeJsonString(kObjc3InteropInteropLoweringLaneContract)
      << "\",\"lowering_model\":\""
      << EscapeJsonString(kObjc3InteropInteropLoweringModel)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(kObjc3InteropInteropLoweringDeferredModel)
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key)
      << "\",\"foreign_callable_sites\":" << contract.foreign_callable_sites
      << ",\"c_foreign_callable_sites\":" << contract.c_foreign_callable_sites
      << ",\"objc_runtime_parity_callable_sites\":"
      << contract.objc_runtime_parity_callable_sites
      << ",\"ownership_bridge_callable_sites\":"
      << contract.ownership_bridge_callable_sites
      << ",\"error_surface_sites\":" << contract.error_surface_sites
      << ",\"async_boundary_sites\":" << contract.async_boundary_sites
      << ",\"swift_concurrency_metadata_sites\":"
      << contract.swift_concurrency_metadata_sites
      << ",\"interface_preserved_foreign_callable_sites\":"
      << contract.interface_preserved_foreign_callable_sites
      << ",\"interface_preserved_metadata_annotation_sites\":"
      << contract.interface_preserved_metadata_annotation_sites
      << ",\"guard_blocked_sites\":" << contract.guard_blocked_sites
      << ",\"contract_violation_sites\":"
      << contract.contract_violation_sites
      << ",\"deterministic_handoff\":"
      << (contract.deterministic ? "true" : "false")
      << ",\"ready_for_ir_emission\":"
      << (ready_for_ir_emission ? "true" : "false")
      << "}";
  return out.str();
}

std::string BuildInteropForeignCallLifetimeLoweringContractJson(
    const Objc3InteropInteropLoweringContract &dependency_contract,
    const Objc3InteropCppInteropInteractionSummary &cpp_summary,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary,
    const Objc3InteropForeignCallLifetimeLoweringContract &contract,
    const std::string &replay_key) {
  const bool ready_for_ir_emission =
      contract.deterministic &&
      IsValidObjc3InteropForeignCallLifetimeLoweringContract(contract);
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3InteropForeignCallLifetimeLoweringContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3InteropForeignCallLifetimeLoweringSurfacePath)
      << "\",\"interop_contract_id\":\""
      << EscapeJsonString(kObjc3InteropInteropLoweringContractId)
      << "\",\"bridge_dependency_contract_id\":\""
      << EscapeJsonString(kObjc3InteropCppInteropInteractionSummaryContractId)
      << "\",\"preservation_contract_id\":\""
      << EscapeJsonString(preservation_summary.contract_id)
      << "\",\"dependency_replay_key\":\""
      << EscapeJsonString(Objc3InteropInteropLoweringReplayKey(
             dependency_contract))
      << "\",\"cpp_dependency_replay_key\":\""
      << EscapeJsonString(cpp_summary.replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key)
      << "\",\"lowering_model\":\""
      << EscapeJsonString(kObjc3InteropForeignCallLifetimeLoweringModel)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(kObjc3InteropForeignCallLifetimeLoweringDeferredModel)
      << "\",\"foreign_callable_sites\":" << contract.foreign_callable_sites
      << ",\"c_foreign_callable_sites\":" << contract.c_foreign_callable_sites
      << ",\"objc_runtime_parity_callable_sites\":"
      << contract.objc_runtime_parity_callable_sites
      << ",\"ownership_bridge_sites\":" << contract.ownership_bridge_sites
      << ",\"lifetime_bridge_sites\":" << contract.lifetime_bridge_sites
      << ",\"metadata_preservation_sites\":"
      << contract.metadata_preservation_sites
      << ",\"guard_blocked_sites\":" << contract.guard_blocked_sites
      << ",\"contract_violation_sites\":"
      << contract.contract_violation_sites
      << ",\"deterministic_handoff\":"
      << (contract.deterministic ? "true" : "false")
      << ",\"ready_for_ir_emission\":"
      << (ready_for_ir_emission ? "true" : "false")
      << "}";
  return out.str();
}

std::string BuildInteropFfiMetadataInterfacePreservationContractJson(
    const Objc3InteropForeignCallLifetimeLoweringContract &lowering_contract,
    const std::string &lowering_replay_key,
    const Objc3InteropForeignSurfaceInterfacePreservationSummary
        &preservation_summary,
    const Objc3InteropFfiMetadataInterfacePreservationContract &contract,
    const std::string &replay_key) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3InteropFfiMetadataInterfacePreservationContractId)
      << "\",\"source_contract_id\":\""
      << EscapeJsonString(
             kObjc3InteropFfiMetadataInterfacePreservationSourceContractId)
      << "\",\"preservation_contract_id\":\""
      << EscapeJsonString(
             kObjc3InteropForeignSurfaceInterfacePreservationContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(
             kObjc3InteropFfiMetadataInterfacePreservationSurfacePath)
      << "\",\"import_artifact_member_name\":\""
      << EscapeJsonString(
             kObjc3InteropFfiMetadataInterfacePreservationImportArtifactMemberName)
      << "\",\"source_model\":\""
      << EscapeJsonString(kObjc3InteropFfiMetadataInterfacePreservationSourceModel)
      << "\",\"preservation_model\":\""
      << EscapeJsonString(
             kObjc3InteropFfiMetadataInterfacePreservationPreservationModel)
      << "\",\"fail_closed_model\":\""
      << EscapeJsonString(
             kObjc3InteropFfiMetadataInterfacePreservationFailClosedModel)
      << "\",\"lowering_replay_key\":\""
      << EscapeJsonString(lowering_replay_key)
      << "\",\"preservation_replay_key\":\""
      << EscapeJsonString(preservation_summary.replay_key)
      << "\",\"local_foreign_callable_count\":"
      << contract.local_foreign_callable_count
      << ",\"local_metadata_preservation_sites\":"
      << contract.local_metadata_preservation_sites
      << ",\"local_interface_annotation_sites\":"
      << contract.local_interface_annotation_sites
      << ",\"imported_module_count\":" << contract.imported_module_count
      << ",\"imported_foreign_callable_count\":"
      << contract.imported_foreign_callable_count
      << ",\"imported_metadata_preservation_sites\":"
      << contract.imported_metadata_preservation_sites
      << ",\"imported_interface_annotation_sites\":"
      << contract.imported_interface_annotation_sites
      << ",\"runtime_import_artifact_ready\":"
      << (contract.runtime_import_artifact_ready ? "true" : "false")
      << ",\"separate_compilation_preservation_ready\":"
      << (contract.separate_compilation_preservation_ready ? "true" : "false")
      << ",\"deterministic\":"
      << (contract.deterministic ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(replay_key) << "\"}";
  (void)lowering_contract;
  return out.str();
}

}  // namespace objc3::artifacts::frontend
