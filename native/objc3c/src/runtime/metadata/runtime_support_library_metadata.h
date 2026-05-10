#pragma once

#include <string>

struct Objc3RuntimeSupportLibraryContractSummary {
  std::string contract_id = kObjc3RuntimeSupportLibraryContractId;
  std::string metadata_publication_contract_id =
      kObjc3RuntimeMetadataSectionPublicationContractId;
  bool boundary_frozen = false;
  bool fail_closed = false;
  bool target_name_frozen = false;
  bool exported_entrypoints_frozen = false;
  bool ownership_boundaries_frozen = false;
  bool build_constraints_frozen = false;
  bool strict_dispatch_errors_required = false;
  bool native_runtime_library_present = false;
  bool driver_link_wiring_pending = true;
  bool ready_for_runtime_library_skeleton = false;
  std::string cmake_target_name = kObjc3RuntimeSupportLibraryTargetName;
  std::string public_header_path = kObjc3RuntimeSupportLibraryPublicHeaderPath;
  std::string source_root = kObjc3RuntimeSupportLibrarySourceRoot;
  std::string library_kind = kObjc3RuntimeSupportLibraryKind;
  std::string archive_basename = kObjc3RuntimeSupportLibraryArchiveBasename;
  std::string register_image_symbol =
      kObjc3RuntimeSupportLibraryRegisterImageSymbol;
  std::string lookup_selector_symbol =
      kObjc3RuntimeSupportLibraryLookupSelectorSymbol;
  std::string dispatch_i32_symbol =
      kObjc3RuntimeSupportLibraryDispatchI32Symbol;
  std::string reset_for_testing_symbol =
      kObjc3RuntimeSupportLibraryResetForTestingSymbol;
  std::string driver_link_mode = kObjc3RuntimeSupportLibraryDriverLinkMode;
  std::string compiler_ownership_boundary =
      kObjc3RuntimeSupportLibraryCompilerOwnershipBoundary;
  std::string runtime_ownership_boundary =
      kObjc3RuntimeSupportLibraryRuntimeOwnershipBoundary;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeSupportLibraryContractSummary(
    const Objc3RuntimeSupportLibraryContractSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.metadata_publication_contract_id.empty() &&
         summary.boundary_frozen &&
         summary.fail_closed &&
         summary.target_name_frozen &&
         summary.exported_entrypoints_frozen &&
         summary.ownership_boundaries_frozen &&
         summary.build_constraints_frozen &&
         summary.strict_dispatch_errors_required &&
         !summary.native_runtime_library_present &&
         summary.driver_link_wiring_pending &&
         summary.ready_for_runtime_library_skeleton &&
         !summary.cmake_target_name.empty() &&
         !summary.public_header_path.empty() &&
         !summary.source_root.empty() &&
         !summary.library_kind.empty() &&
         !summary.archive_basename.empty() &&
         !summary.register_image_symbol.empty() &&
         !summary.lookup_selector_symbol.empty() &&
         !summary.dispatch_i32_symbol.empty() &&
         !summary.reset_for_testing_symbol.empty() &&
         !summary.driver_link_mode.empty() &&
         !summary.compiler_ownership_boundary.empty() &&
         !summary.runtime_ownership_boundary.empty() &&
         summary.failure_reason.empty();
}

struct Objc3RuntimeSupportLibraryCoreFeatureSummary {
  std::string contract_id = kObjc3RuntimeSupportLibraryCoreFeatureContractId;
  std::string support_library_contract_id = kObjc3RuntimeSupportLibraryContractId;
  std::string metadata_publication_contract_id =
      kObjc3RuntimeMetadataSectionPublicationContractId;
  bool fail_closed = false;
  bool native_runtime_library_sources_present = false;
  bool native_runtime_library_header_present = false;
  bool native_runtime_library_archive_build_enabled = false;
  bool native_runtime_library_entrypoints_implemented = false;
  bool selector_lookup_stateful = false;
  bool deterministic_dispatch_formula_matches_runtime_test_helper = false;
  bool reset_for_testing_supported = false;
  bool strict_dispatch_errors_required = false;
  bool driver_link_wiring_pending = true;
  bool ready_for_driver_link_wiring = false;
  std::string cmake_target_name = kObjc3RuntimeSupportLibraryTargetName;
  std::string public_header_path = kObjc3RuntimeSupportLibraryPublicHeaderPath;
  std::string source_root = kObjc3RuntimeSupportLibrarySourceRoot;
  std::string implementation_source_path =
      kObjc3RuntimeSupportLibraryImplementationSourcePath;
  std::string library_kind = kObjc3RuntimeSupportLibraryKind;
  std::string archive_basename = kObjc3RuntimeSupportLibraryArchiveBasename;
  std::string archive_relative_path =
      kObjc3RuntimeSupportLibraryArchiveRelativePath;
  std::string probe_source_path = kObjc3RuntimeSupportLibraryProbeSourcePath;
  std::string register_image_symbol =
      kObjc3RuntimeSupportLibraryRegisterImageSymbol;
  std::string lookup_selector_symbol =
      kObjc3RuntimeSupportLibraryLookupSelectorSymbol;
  std::string dispatch_i32_symbol =
      kObjc3RuntimeSupportLibraryDispatchI32Symbol;
  std::string reset_for_testing_symbol =
      kObjc3RuntimeSupportLibraryResetForTestingSymbol;
  std::string driver_link_mode = kObjc3RuntimeSupportLibraryDriverLinkMode;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeSupportLibraryCoreFeatureSummary(
    const Objc3RuntimeSupportLibraryCoreFeatureSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.support_library_contract_id.empty() &&
         !summary.metadata_publication_contract_id.empty() &&
         summary.fail_closed &&
         summary.native_runtime_library_sources_present &&
         summary.native_runtime_library_header_present &&
         summary.native_runtime_library_archive_build_enabled &&
         summary.native_runtime_library_entrypoints_implemented &&
         summary.selector_lookup_stateful &&
         summary.deterministic_dispatch_formula_matches_runtime_test_helper &&
         summary.reset_for_testing_supported &&
         summary.strict_dispatch_errors_required &&
         summary.driver_link_wiring_pending &&
         summary.ready_for_driver_link_wiring &&
         !summary.cmake_target_name.empty() &&
         !summary.public_header_path.empty() &&
         !summary.source_root.empty() &&
         !summary.implementation_source_path.empty() &&
         !summary.library_kind.empty() &&
         !summary.archive_basename.empty() &&
         !summary.archive_relative_path.empty() &&
         !summary.probe_source_path.empty() &&
         !summary.register_image_symbol.empty() &&
         !summary.lookup_selector_symbol.empty() &&
         !summary.dispatch_i32_symbol.empty() &&
         !summary.reset_for_testing_symbol.empty() &&
         !summary.driver_link_mode.empty() &&
         summary.failure_reason.empty();
}

struct Objc3RuntimeSupportLibraryLinkWiringSummary {
  std::string contract_id = kObjc3RuntimeSupportLibraryLinkWiringContractId;
  std::string support_library_core_feature_contract_id =
      kObjc3RuntimeSupportLibraryCoreFeatureContractId;
  bool fail_closed = false;
  bool runtime_library_archive_available = false;
  bool driver_emits_runtime_link_contract = false;
  bool execution_smoke_consumes_runtime_library = false;
  bool strict_dispatch_errors_required = false;
  bool ready_for_runtime_library_consumption = false;
  std::string archive_relative_path =
      kObjc3RuntimeSupportLibraryArchiveRelativePath;
  std::string runtime_dispatch_symbol =
      kObjc3RuntimeSupportLibraryDispatchI32Symbol;
  std::string execution_smoke_script_path =
      kObjc3RuntimeSupportLibraryExecutionSmokeScriptPath;
  std::string driver_link_mode = kObjc3RuntimeSupportLibraryLinkWiringMode;
  std::string failure_reason;
};

inline bool IsReadyObjc3RuntimeSupportLibraryLinkWiringSummary(
    const Objc3RuntimeSupportLibraryLinkWiringSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.support_library_core_feature_contract_id.empty() &&
         summary.fail_closed &&
         summary.runtime_library_archive_available &&
         summary.driver_emits_runtime_link_contract &&
         summary.execution_smoke_consumes_runtime_library &&
         summary.strict_dispatch_errors_required &&
         summary.ready_for_runtime_library_consumption &&
         !summary.archive_relative_path.empty() &&
         !summary.runtime_dispatch_symbol.empty() &&
         !summary.execution_smoke_script_path.empty() &&
         !summary.driver_link_mode.empty() &&
         summary.failure_reason.empty();
}
