#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "sema/model/semantic_symbol_advanced_source_contracts.h"
#include "sema/model/semantic_symbol_core_source_closures.h"

struct Objc3FrontendMetaprogrammingMetaprogrammingSourceClosureSummary {
  std::string contract_id = kObjc3MetaprogrammingMetaprogrammingSourceClosureContractId;
  std::string frontend_surface_path =
      kObjc3MetaprogrammingMetaprogrammingSourceClosureSurfacePath;
  std::string source_model =
      kObjc3MetaprogrammingMetaprogrammingSourceClosureSourceModel;
  std::string failure_model =
      kObjc3MetaprogrammingMetaprogrammingSourceClosureFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimDeriveMarkers,
      kObjc3SourceOnlyFeatureClaimMacroMarkers,
      kObjc3SourceOnlyFeatureClaimPropertyBehaviorMarkers,
  };
  std::size_t derive_marker_sites = 0;
  std::size_t macro_marker_sites = 0;
  std::size_t property_behavior_sites = 0;
  bool derive_marker_source_supported = false;
  bool macro_marker_source_supported = false;
  bool property_behavior_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendMetaprogrammingMacroPackageProvenanceSourceCompletionSummary {
  std::string contract_id =
      kObjc3MetaprogrammingMacroPackageProvenanceSourceCompletionContractId;
  std::string frontend_surface_path =
      kObjc3MetaprogrammingMacroPackageProvenanceSourceCompletionSurfacePath;
  std::string source_model =
      kObjc3MetaprogrammingMacroPackageProvenanceSourceCompletionSourceModel;
  std::string failure_model =
      kObjc3MetaprogrammingMacroPackageProvenanceSourceCompletionFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimMacroMarkers,
      kObjc3SourceOnlyFeatureClaimMacroPackageMarkers,
      kObjc3SourceOnlyFeatureClaimMacroProvenanceMarkers,
      kObjc3SourceOnlyFeatureClaimMacroCacheKeyMarkers,
      kObjc3SourceOnlyFeatureClaimMacroSandboxPolicyMarkers,
      kObjc3SourceOnlyFeatureClaimMacroExpansionVisibleState,
  };
  std::size_t macro_marker_sites = 0;
  std::size_t macro_package_sites = 0;
  std::size_t macro_provenance_sites = 0;
  std::size_t macro_cache_key_sites = 0;
  std::size_t macro_sandbox_policy_sites = 0;
  std::size_t expansion_visible_macro_sites = 0;
  bool macro_package_source_supported = false;
  bool macro_provenance_source_supported = false;
  bool macro_cache_key_source_supported = false;
  bool macro_sandbox_policy_source_supported = false;
  bool expansion_visible_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendMetaprogrammingPropertyBehaviorSourceCompletionSummary {
  std::string contract_id =
      kObjc3MetaprogrammingPropertyBehaviorSourceCompletionContractId;
  std::string frontend_surface_path =
      kObjc3MetaprogrammingPropertyBehaviorSourceCompletionSurfacePath;
  std::string source_model =
      kObjc3MetaprogrammingPropertyBehaviorSourceCompletionSourceModel;
  std::string failure_model =
      kObjc3MetaprogrammingPropertyBehaviorSourceCompletionFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimPropertyBehaviorMarkers,
      kObjc3SourceOnlyFeatureClaimPropertyBehaviorSynthesisVisibility,
  };
  std::size_t property_behavior_sites = 0;
  std::size_t interface_property_behavior_sites = 0;
  std::size_t implementation_property_behavior_sites = 0;
  std::size_t protocol_property_behavior_sites = 0;
  std::size_t synthesized_binding_visible_sites = 0;
  std::size_t synthesized_getter_visible_sites = 0;
  std::size_t synthesized_setter_visible_sites = 0;
  bool property_behavior_source_supported = false;
  bool synthesized_declaration_visibility_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendInteropForeignImportSourceClosureSummary {
  std::string contract_id = kObjc3InteropForeignImportSourceClosureContractId;
  std::string frontend_surface_path =
      kObjc3InteropForeignImportSourceClosureSurfacePath;
  std::string source_model =
      kObjc3InteropForeignImportSourceClosureSourceModel;
  std::string failure_model =
      kObjc3InteropForeignImportSourceClosureFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimForeignDeclarationMarkers,
      kObjc3SourceOnlyFeatureClaimImportedModuleAnnotations,
      kObjc3SourceOnlyFeatureClaimInteropAnnotationMarkers,
  };
  std::size_t foreign_callable_sites = 0;
  std::size_t extern_foreign_callable_sites = 0;
  std::size_t import_module_annotation_sites = 0;
  std::size_t imported_module_name_sites = 0;
  std::size_t export_header_annotation_sites = 0;
  std::size_t export_header_name_sites = 0;
  std::size_t mixed_image_annotation_sites = 0;
  std::size_t mixed_image_name_sites = 0;
  std::size_t package_entry_annotation_sites = 0;
  std::size_t package_entry_name_sites = 0;
  std::size_t interop_annotation_sites = 0;
  bool foreign_declaration_source_supported = false;
  bool imported_surface_source_supported = false;
  bool interop_annotation_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

struct Objc3FrontendInteropCppSwiftInteropAnnotationSourceCompletionSummary {
  std::string contract_id =
      kObjc3InteropCppSwiftInteropAnnotationSourceCompletionContractId;
  std::string frontend_surface_path =
      kObjc3InteropCppSwiftInteropAnnotationSourceCompletionSurfacePath;
  std::string source_model =
      kObjc3InteropCppSwiftInteropAnnotationSourceCompletionSourceModel;
  std::string failure_model =
      kObjc3InteropCppSwiftInteropAnnotationSourceCompletionFailureModel;
  std::vector<std::string> source_only_claim_ids = {
      kObjc3SourceOnlyFeatureClaimSwiftFacingAnnotationMarkers,
      kObjc3SourceOnlyFeatureClaimCppFacingAnnotationMarkers,
      kObjc3SourceOnlyFeatureClaimInteropMetadataAnnotationMarkers,
      kObjc3SourceOnlyFeatureClaimInteropNamedMetadataPayloads,
  };
  std::size_t swift_name_annotation_sites = 0;
  std::size_t swift_private_annotation_sites = 0;
  std::size_t cpp_name_annotation_sites = 0;
  std::size_t header_name_annotation_sites = 0;
  std::size_t abi_alignment_annotation_sites = 0;
  std::size_t foreign_type_annotation_sites = 0;
  std::size_t interop_metadata_annotation_sites = 0;
  std::size_t named_annotation_payload_sites = 0;
  bool swift_annotation_source_supported = false;
  bool cpp_annotation_source_supported = false;
  bool interop_metadata_source_supported = false;
  bool deterministic_handoff = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};
