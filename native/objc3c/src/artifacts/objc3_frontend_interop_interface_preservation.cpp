#include "artifacts/objc3_frontend_interop_semantic_artifacts.h"

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include "artifacts/objc3_frontend_artifact_semantic_contract_records.h"
#include "ast/objc3_ast_declarations.h"
#include "pipeline/objc3_frontend_types.h"
#include "pipeline/objc3_runtime_import_surface.h"

namespace objc3::artifacts::frontend {
namespace {

std::vector<std::string> CollectInteropLocalImportModuleNames(
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

std::size_t CountInteropInterfaceAnnotationSites(
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

}  // namespace

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

}  // namespace objc3::artifacts::frontend
