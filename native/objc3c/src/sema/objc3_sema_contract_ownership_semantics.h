#pragma once

#include <cstddef>
#include <string>

#include "sema/objc3_sema_contract_ownership_memory_model.h"

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
