#pragma once

#include <cstddef>
#include <string>

inline constexpr const char *kObjc3OwnershipSystemExtensionSemanticModelDependencyContractId =
    "objc3c.ownership.retainable.c.family.source.completion.v1";
inline constexpr const char *kObjc3OwnershipSystemExtensionSemanticModelContractId =
    "objc3c.ownership.system.extension.semantic.model.v1";
inline constexpr const char *kObjc3OwnershipSystemExtensionSemanticModelSurfacePath =
    "frontend.pipeline.semantic_surface.objc_ownership_system_extension_semantic_model";
inline constexpr const char *kObjc3OwnershipSystemExtensionSemanticModelRule =
    "cleanup-resource-borrowed-capture-and-retainable-family-source-surfaces-now-share-one-truthful-sema-model-while-resource-move-borrowed-escape-and-runtime-interop-work-remain-later-runtime-lanes";
inline constexpr const char *kObjc3OwnershipSystemExtensionSemanticModelDeferredRule =
    "resource-move-use-after-move-borrowed-escape-retainable-family-legality-and-runtime-interop-remain-deferred-to-later-runtime-lanes";

struct Objc3OwnershipSystemExtensionSemanticModelSummary {
  std::string contract_id = kObjc3OwnershipSystemExtensionSemanticModelContractId;
  std::string frontend_dependency_contract_id =
      kObjc3OwnershipSystemExtensionSemanticModelDependencyContractId;
  std::string surface_path = kObjc3OwnershipSystemExtensionSemanticModelSurfacePath;
  std::string semantic_model = kObjc3OwnershipSystemExtensionSemanticModelRule;
  std::string deferred_model = kObjc3OwnershipSystemExtensionSemanticModelDeferredRule;
  std::size_t cleanup_attribute_sites = 0;
  std::size_t cleanup_sugar_sites = 0;
  std::size_t resource_attribute_sites = 0;
  std::size_t resource_sugar_sites = 0;
  std::size_t borrowed_pointer_sites = 0;
  std::size_t returns_borrowed_attribute_sites = 0;
  std::size_t explicit_capture_list_sites = 0;
  std::size_t explicit_capture_item_sites = 0;
  std::size_t retainable_family_annotation_sites = 0;
  std::size_t retainable_family_compatibility_alias_sites = 0;
  bool source_dependency_required = false;
  bool cleanup_resource_semantic_model_frozen = false;
  bool borrowed_pointer_semantic_model_frozen = false;
  bool capture_legality_semantic_model_frozen = false;
  bool retainable_family_semantic_model_frozen = false;
  bool resource_move_semantics_deferred = false;
  bool borrowed_escape_semantics_deferred = false;
  bool retainable_family_legality_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3OwnershipSystemExtensionSemanticModelSummary(
    const Objc3OwnershipSystemExtensionSemanticModelSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.frontend_dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() &&
         summary.source_dependency_required &&
         summary.cleanup_resource_semantic_model_frozen &&
         summary.borrowed_pointer_semantic_model_frozen &&
         summary.capture_legality_semantic_model_frozen &&
         summary.retainable_family_semantic_model_frozen &&
         summary.resource_move_semantics_deferred &&
         summary.borrowed_escape_semantics_deferred &&
         summary.retainable_family_legality_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3OwnershipResourceMoveUseAfterMoveSemanticsDependencyContractId =
        "objc3c.ownership.system.extension.semantic.model.v1";
inline constexpr const char
    *kObjc3OwnershipResourceMoveUseAfterMoveSemanticsContractId =
        "objc3c.ownership.resource.move.use.after.move.semantics.v1";
inline constexpr const char
    *kObjc3OwnershipResourceMoveUseAfterMoveSemanticsSurfacePath =
        "frontend.pipeline.semantic_surface.objc_ownership_resource_move_and_use_after_move_semantics";
inline constexpr const char
    *kObjc3OwnershipResourceMoveUseAfterMoveSemanticsRule =
        "cleanup-owned-resource-locals-now-transfer-cleanup-ownership-through-explicit-move-captures-and-fail-closed-on-use-after-move-while-borrowed-escape-retainable-family-legality-lowering-and-runtime-remain-later-runtime-work";
inline constexpr const char
    *kObjc3OwnershipResourceMoveUseAfterMoveSemanticsDeferredRule =
        "borrowed-escape-retainable-family-legality-lowering-and-runtime-remain-deferred-to-later-runtime-lanes";

struct Objc3OwnershipResourceMoveUseAfterMoveSemanticsSummary {
  std::string contract_id =
      kObjc3OwnershipResourceMoveUseAfterMoveSemanticsContractId;
  std::string dependency_contract_id =
      kObjc3OwnershipResourceMoveUseAfterMoveSemanticsDependencyContractId;
  std::string surface_path =
      kObjc3OwnershipResourceMoveUseAfterMoveSemanticsSurfacePath;
  std::string semantic_model =
      kObjc3OwnershipResourceMoveUseAfterMoveSemanticsRule;
  std::string deferred_model =
      kObjc3OwnershipResourceMoveUseAfterMoveSemanticsDeferredRule;
  std::size_t cleanup_owned_local_sites = 0;
  std::size_t resource_move_capture_sites = 0;
  std::size_t illegal_non_resource_move_sites = 0;
  std::size_t illegal_use_after_move_sites = 0;
  std::size_t illegal_duplicate_move_sites = 0;
  bool dependency_required = false;
  bool cleanup_ownership_transfer_enforced = false;
  bool use_after_move_fail_closed = false;
  bool duplicate_move_fail_closed = false;
  bool borrowed_escape_semantics_deferred = false;
  bool retainable_family_legality_deferred = false;
  bool lowering_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3OwnershipResourceMoveUseAfterMoveSemanticsSummary(
    const Objc3OwnershipResourceMoveUseAfterMoveSemanticsSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() && summary.dependency_required &&
         summary.cleanup_ownership_transfer_enforced &&
         summary.use_after_move_fail_closed &&
         summary.duplicate_move_fail_closed &&
         summary.borrowed_escape_semantics_deferred &&
         summary.retainable_family_legality_deferred &&
         summary.lowering_runtime_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3OwnershipBorrowedPointerEscapeAnalysisDependencyContractId =
        "objc3c.ownership.resource.move.use.after.move.semantics.v1";
inline constexpr const char
    *kObjc3OwnershipBorrowedPointerEscapeAnalysisContractId =
        "objc3c.ownership.borrowed.pointer.escape.analysis.v1";
inline constexpr const char
    *kObjc3OwnershipBorrowedPointerEscapeAnalysisSurfacePath =
        "frontend.pipeline.semantic_surface.objc_ownership_borrowed_pointer_escape_analysis";
inline constexpr const char
    *kObjc3OwnershipBorrowedPointerEscapeAnalysisRule =
        "borrowed-pointer-bindings-now-fail-closed-on-unproven-call-boundaries-escaping-block-capture-and-invalid-borrowed-return-contracts-while-retainable-family-legality-lowering-and-runtime-remain-later-runtime-work";
inline constexpr const char
    *kObjc3OwnershipBorrowedPointerEscapeAnalysisDeferredRule =
        "retainable-family-legality-lowering-and-runtime-remain-deferred-to-later-runtime-lanes";

struct Objc3OwnershipBorrowedPointerEscapeAnalysisSummary {
  std::string contract_id =
      kObjc3OwnershipBorrowedPointerEscapeAnalysisContractId;
  std::string dependency_contract_id =
      kObjc3OwnershipBorrowedPointerEscapeAnalysisDependencyContractId;
  std::string surface_path =
      kObjc3OwnershipBorrowedPointerEscapeAnalysisSurfacePath;
  std::string semantic_model =
      kObjc3OwnershipBorrowedPointerEscapeAnalysisRule;
  std::string deferred_model =
      kObjc3OwnershipBorrowedPointerEscapeAnalysisDeferredRule;
  std::size_t borrowed_parameter_sites = 0;
  std::size_t borrowed_return_callable_sites = 0;
  std::size_t borrowed_escape_candidate_sites = 0;
  std::size_t illegal_unproven_call_escape_sites = 0;
  std::size_t illegal_escaping_block_capture_sites = 0;
  std::size_t illegal_borrowed_return_sites = 0;
  bool dependency_required = false;
  bool borrowed_call_boundary_enforced = false;
  bool escaping_block_capture_fail_closed = false;
  bool borrowed_return_contract_enforced = false;
  bool retainable_family_legality_deferred = false;
  bool lowering_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3OwnershipBorrowedPointerEscapeAnalysisSummary(
    const Objc3OwnershipBorrowedPointerEscapeAnalysisSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() && summary.dependency_required &&
         summary.borrowed_call_boundary_enforced &&
         summary.escaping_block_capture_fail_closed &&
         summary.borrowed_return_contract_enforced &&
         summary.retainable_family_legality_deferred &&
         summary.lowering_runtime_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionDependencyContractId =
        "objc3c.ownership.borrowed.pointer.escape.analysis.v1";
inline constexpr const char
    *kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionContractId =
        "objc3c.ownership.capture.list.retainable.family.legality.v1";
inline constexpr const char
    *kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionSurfacePath =
        "frontend.pipeline.semantic_surface.objc_ownership_capture_list_and_retainable_family_legality_completion";
inline constexpr const char
    *kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionRule =
        "explicit-capture-lists-now-fail-closed-on-duplicate-unused-and-non-object-ownership-modes-while-retainable-family-callables-fail-closed-on-conflicting-and-shape-invalid-annotations-before-lowering-and-runtime-integration";
inline constexpr const char
    *kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionDeferredRule =
        "lowering-runtime-interop-and-runnable-retainable-family-behavior-remain-deferred-to-later-runtime-lanes";

struct Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary {
  std::string contract_id =
      kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionContractId;
  std::string dependency_contract_id =
      kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionDependencyContractId;
  std::string surface_path =
      kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionSurfacePath;
  std::string semantic_model =
      kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionRule;
  std::string deferred_model =
      kObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionDeferredRule;
  std::size_t explicit_capture_list_sites = 0;
  std::size_t explicit_capture_item_sites = 0;
  std::size_t explicit_capture_ownership_mode_sites = 0;
  std::size_t retainable_family_callable_sites = 0;
  std::size_t retainable_family_operation_callable_sites = 0;
  std::size_t retainable_family_alias_callable_sites = 0;
  std::size_t illegal_duplicate_explicit_capture_sites = 0;
  std::size_t illegal_non_object_capture_mode_sites = 0;
  std::size_t illegal_unused_explicit_capture_sites = 0;
  std::size_t illegal_conflicting_retainable_family_sites = 0;
  std::size_t illegal_invalid_family_operation_shape_sites = 0;
  std::size_t illegal_invalid_family_alias_shape_sites = 0;
  bool dependency_required = false;
  bool explicit_capture_duplicate_fail_closed = false;
  bool explicit_capture_ownership_mode_enforced = false;
  bool explicit_capture_inventory_enforced = false;
  bool retainable_family_conflict_enforced = false;
  bool retainable_family_operation_shape_enforced = false;
  bool retainable_family_alias_shape_enforced = false;
  bool lowering_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool
IsReadyObjc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary(
    const Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary
        &summary) {
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() && summary.dependency_required &&
         summary.explicit_capture_duplicate_fail_closed &&
         summary.explicit_capture_ownership_mode_enforced &&
         summary.explicit_capture_inventory_enforced &&
         summary.retainable_family_conflict_enforced &&
         summary.retainable_family_operation_shape_enforced &&
         summary.retainable_family_alias_shape_enforced &&
         summary.lowering_runtime_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3InteropInteropSemanticModelDependencyContractId =
        "objc3c.interop.foreign.surface.interface.preservation.v1";
inline constexpr const char *kObjc3InteropInteropSemanticModelContractId =
    "objc3c.interop.interop.semantic.model.v1";
inline constexpr const char *kObjc3InteropInteropSemanticModelSurfacePath =
    "frontend.pipeline.semantic_surface.objc_interop_interop_semantic_model";
inline constexpr const char *kObjc3InteropInteropSemanticModelRule =
    "foreign-import-swift-cpp-facing-annotation-surfaces-now-freeze-one-deterministic-sema-model-over-existing-ownership-error-async-and-actor-interaction-profiles-while-ffi-lowering-and-runnable-bridge-generation-remain-later-runtime-work";
inline constexpr const char *kObjc3InteropInteropSemanticModelDeferredRule =
    "ffi-abi-lowering-runtime-bridge-gates-cross-language-object-ownership-and-runnable-call-behavior-remain-deferred-to-later-runtime-lanes";

struct Objc3InteropInteropSemanticModelSummary {
  std::string contract_id = kObjc3InteropInteropSemanticModelContractId;
  std::string dependency_contract_id =
      kObjc3InteropInteropSemanticModelDependencyContractId;
  std::string surface_path = kObjc3InteropInteropSemanticModelSurfacePath;
  std::string semantic_model = kObjc3InteropInteropSemanticModelRule;
  std::string deferred_model = kObjc3InteropInteropSemanticModelDeferredRule;
  std::size_t foreign_callable_sites = 0;
  std::size_t import_module_annotation_sites = 0;
  std::size_t imported_module_name_sites = 0;
  std::size_t export_header_annotation_sites = 0;
  std::size_t export_header_name_sites = 0;
  std::size_t mixed_image_annotation_sites = 0;
  std::size_t mixed_image_name_sites = 0;
  std::size_t package_entry_annotation_sites = 0;
  std::size_t package_entry_name_sites = 0;
  std::size_t swift_name_annotation_sites = 0;
  std::size_t swift_private_annotation_sites = 0;
  std::size_t cpp_name_annotation_sites = 0;
  std::size_t header_name_annotation_sites = 0;
  std::size_t abi_alignment_annotation_sites = 0;
  std::size_t foreign_type_annotation_sites = 0;
  std::size_t named_annotation_payload_sites = 0;
  std::size_t retainable_family_callable_sites = 0;
  std::size_t bridge_callable_sites = 0;
  std::size_t async_executor_affinity_sites = 0;
  std::size_t actor_hazard_sites = 0;
  std::size_t interop_metadata_annotation_sites = 0;
  bool source_dependency_required = false;
  bool foreign_annotation_source_supported = false;
  bool ownership_interaction_profile_frozen = false;
  bool error_bridge_profile_reused = false;
  bool async_affinity_profile_reused = false;
  bool actor_hazard_profile_reused = false;
  bool metadata_payload_profile_frozen = false;
  bool ffi_abi_lowering_deferred = false;
  bool runtime_bridge_generation_deferred = false;
  bool deterministic = false;
  bool ready_for_semantic_expansion = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3InteropInteropSemanticModelSummary(
    const Objc3InteropInteropSemanticModelSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() && summary.source_dependency_required &&
         summary.foreign_annotation_source_supported &&
         summary.ownership_interaction_profile_frozen &&
         summary.error_bridge_profile_reused &&
         summary.async_affinity_profile_reused &&
         summary.actor_hazard_profile_reused &&
         summary.metadata_payload_profile_frozen &&
         summary.ffi_abi_lowering_deferred &&
         summary.runtime_bridge_generation_deferred && summary.deterministic &&
         summary.ready_for_semantic_expansion && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3InteropInteropRuntimeParitySummaryDependencyContractId =
        kObjc3InteropInteropSemanticModelContractId;
inline constexpr const char
    *kObjc3InteropInteropRuntimeParitySummaryContractId =
        "objc3c.interop.c.and.objc.runtime.parity.semantics.v1";
inline constexpr const char
    *kObjc3InteropInteropRuntimeParitySummarySurfacePath =
        "frontend.pipeline.semantic_surface.objc_interop_c_and_objc_runtime_parity_semantics";
inline constexpr const char
    *kObjc3InteropInteropRuntimeParitySummaryRule =
        "declaration-only-foreign-c-surfaces-import-module-requires-foreign-and-implementation-annotation-rejections-are-now-live-fail-closed-sema-rules-while-ffi-abi-lowering-and-runnable-bridge-generation-remain-later-runtime-work";
inline constexpr const char
    *kObjc3InteropInteropRuntimeParitySummaryDeferredRule =
        "ffi-abi-lowering-runtime-bridge-gates-cross-language-ownership-and-runnable-foreign-call-behavior-remain-deferred-to-later-runtime-lanes";

struct Objc3InteropInteropRuntimeParitySummary {
  std::string contract_id = kObjc3InteropInteropRuntimeParitySummaryContractId;
  std::string dependency_contract_id =
      kObjc3InteropInteropRuntimeParitySummaryDependencyContractId;
  std::string surface_path =
      kObjc3InteropInteropRuntimeParitySummarySurfacePath;
  std::string semantic_model = kObjc3InteropInteropRuntimeParitySummaryRule;
  std::string deferred_model =
      kObjc3InteropInteropRuntimeParitySummaryDeferredRule;
  std::size_t foreign_callable_sites = 0;
  std::size_t c_foreign_callable_sites = 0;
  std::size_t objc_method_foreign_callable_sites = 0;
  std::size_t import_module_annotation_sites = 0;
  std::size_t import_module_foreign_callable_sites = 0;
  std::size_t objc_runtime_parity_callable_sites = 0;
  std::size_t foreign_definition_rejection_sites = 0;
  std::size_t import_without_foreign_rejection_sites = 0;
  std::size_t implementation_annotation_rejection_sites = 0;
  bool dependency_required = false;
  bool declaration_only_foreign_c_enforced = false;
  bool import_module_requires_foreign_enforced = false;
  bool implementation_annotations_fail_closed = false;
  bool objc_runtime_parity_classified = false;
  bool ffi_abi_lowering_deferred = false;
  bool runtime_bridge_generation_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3InteropInteropRuntimeParitySummary(
    const Objc3InteropInteropRuntimeParitySummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() && summary.dependency_required &&
         summary.declaration_only_foreign_c_enforced &&
         summary.import_module_requires_foreign_enforced &&
         summary.implementation_annotations_fail_closed &&
         summary.objc_runtime_parity_classified &&
         summary.ffi_abi_lowering_deferred &&
         summary.runtime_bridge_generation_deferred &&
         summary.deterministic && summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3InteropCppInteropInteractionSummaryDependencyContractId =
        kObjc3InteropInteropRuntimeParitySummaryContractId;
inline constexpr const char
    *kObjc3InteropCppInteropInteractionSummaryContractId =
        "objc3c.interop.cpp.ownership.throws.and.async.interaction.completion.v1";
inline constexpr const char
    *kObjc3InteropCppInteropInteractionSummarySurfacePath =
        "frontend.pipeline.semantic_surface.objc_interop_cpp_ownership_throws_and_async_interactions";
inline constexpr const char
    *kObjc3InteropCppInteropInteractionSummaryRule =
        "cxx-facing-interop-callables-now-fail-closed-on-ownership-managed-throws-and-async-combinations-while-ffi-abi-lowering-runtime-bridge-gates-and-runnable-cross-language-behavior-remain-later-runtime-work";
inline constexpr const char
    *kObjc3InteropCppInteropInteractionSummaryDeferredRule =
        "ffi-abi-lowering-runtime-bridge-gates-cross-language-ownership-async-propagation-and-runnable-cxx-interop-behavior-remain-deferred-to-later-runtime-lanes";

struct Objc3InteropCppInteropInteractionSummary {
  std::string contract_id = kObjc3InteropCppInteropInteractionSummaryContractId;
  std::string dependency_contract_id =
      kObjc3InteropCppInteropInteractionSummaryDependencyContractId;
  std::string surface_path =
      kObjc3InteropCppInteropInteractionSummarySurfacePath;
  std::string semantic_model = kObjc3InteropCppInteropInteractionSummaryRule;
  std::string deferred_model =
      kObjc3InteropCppInteropInteractionSummaryDeferredRule;
  std::size_t cpp_interop_callable_sites = 0;
  std::size_t cpp_named_callable_sites = 0;
  std::size_t header_named_callable_sites = 0;
  std::size_t ownership_interaction_sites = 0;
  std::size_t throws_interaction_sites = 0;
  std::size_t async_interaction_sites = 0;
  std::size_t ownership_rejection_sites = 0;
  std::size_t throws_rejection_sites = 0;
  std::size_t async_rejection_sites = 0;
  bool dependency_required = false;
  bool cpp_annotation_profile_reused = false;
  bool ownership_interactions_fail_closed = false;
  bool throws_interactions_fail_closed = false;
  bool async_interactions_fail_closed = false;
  bool ffi_abi_lowering_deferred = false;
  bool runtime_bridge_generation_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3InteropCppInteropInteractionSummary(
    const Objc3InteropCppInteropInteractionSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() && summary.dependency_required &&
         summary.cpp_annotation_profile_reused &&
         summary.ownership_interactions_fail_closed &&
         summary.throws_interactions_fail_closed &&
         summary.async_interactions_fail_closed &&
         summary.ffi_abi_lowering_deferred &&
         summary.runtime_bridge_generation_deferred &&
         summary.deterministic && summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3InteropSwiftInteropIsolationSummaryDependencyContractId =
        kObjc3InteropCppInteropInteractionSummaryContractId;
inline constexpr const char
    *kObjc3InteropSwiftInteropIsolationSummaryContractId =
        "objc3c.interop.swift.metadata.and.isolation.mapping.completion.v1";
inline constexpr const char
    *kObjc3InteropSwiftInteropIsolationSummarySurfacePath =
        "frontend.pipeline.semantic_surface.objc_interop_swift_metadata_and_isolation_mapping";
inline constexpr const char
    *kObjc3InteropSwiftInteropIsolationSummaryRule =
        "swift-facing-metadata-now-fails-closed-on-missing-name-pairing-actor-owned-surfaces-and-objc-nonisolated-isolation-mapping-gaps-while-ffi-abi-lowering-runtime-bridge-gates-and-runnable-cross-language-behavior-remain-later-runtime-work";
inline constexpr const char
    *kObjc3InteropSwiftInteropIsolationSummaryDeferredRule =
        "ffi-abi-lowering-runtime-bridge-gates-swift-facing-isolation-export-and-runnable-cross-language-behavior-remain-deferred-to-later-runtime-lanes";

struct Objc3InteropSwiftInteropIsolationSummary {
  std::string contract_id = kObjc3InteropSwiftInteropIsolationSummaryContractId;
  std::string dependency_contract_id =
      kObjc3InteropSwiftInteropIsolationSummaryDependencyContractId;
  std::string surface_path =
      kObjc3InteropSwiftInteropIsolationSummarySurfacePath;
  std::string semantic_model = kObjc3InteropSwiftInteropIsolationSummaryRule;
  std::string deferred_model =
      kObjc3InteropSwiftInteropIsolationSummaryDeferredRule;
  std::size_t swift_interop_callable_sites = 0;
  std::size_t swift_named_callable_sites = 0;
  std::size_t swift_private_callable_sites = 0;
  std::size_t swift_private_without_name_sites = 0;
  std::size_t actor_owned_swift_callable_sites = 0;
  std::size_t nonisolated_swift_callable_sites = 0;
  std::size_t implementation_swift_callable_sites = 0;
  std::size_t swift_private_without_name_rejection_sites = 0;
  std::size_t actor_isolation_mapping_rejection_sites = 0;
  std::size_t nonisolated_mapping_rejection_sites = 0;
  std::size_t implementation_surface_rejection_sites = 0;
  bool dependency_required = false;
  bool swift_metadata_profile_reused = false;
  bool swift_private_requires_name_enforced = false;
  bool actor_isolation_mapping_fail_closed = false;
  bool nonisolated_mapping_fail_closed = false;
  bool implementation_surface_fail_closed = false;
  bool ffi_abi_lowering_deferred = false;
  bool runtime_bridge_generation_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3InteropSwiftInteropIsolationSummary(
    const Objc3InteropSwiftInteropIsolationSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() && summary.dependency_required &&
         summary.swift_metadata_profile_reused &&
         summary.swift_private_requires_name_enforced &&
         summary.actor_isolation_mapping_fail_closed &&
         summary.nonisolated_mapping_fail_closed &&
         summary.implementation_surface_fail_closed &&
         summary.ffi_abi_lowering_deferred &&
         summary.runtime_bridge_generation_deferred &&
         summary.deterministic && summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}
