#pragma once

inline constexpr const char *kObjc3RuntimeBootstrapRegistrarContractId =
    "objc3c.runtime.bootstrap.registrar.image.walk.v1";
inline constexpr const char *kObjc3RuntimeBootstrapRegistrarSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_registrar_contract";
inline constexpr const char *kObjc3RuntimeBootstrapInternalHeaderPath =
    "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h";
inline constexpr const char
    *kObjc3RuntimeBootstrapStageRegistrationTableSymbol =
        "objc3_runtime_stage_registration_table_for_bootstrap";
inline constexpr const char
    *kObjc3RuntimeBootstrapImageWalkSnapshotSymbol =
        "objc3_runtime_copy_image_walk_state_for_testing";
inline constexpr const char *kObjc3RuntimeBootstrapImageWalkModel =
    "registration-table-roots-validated-and-staged-before-realization";
inline constexpr const char
    *kObjc3RuntimeBootstrapDiscoveryRootValidationModel =
        "linker-anchor-must-point-at-discovery-root-and-discovery-root-must-close-over-registration-roots";
inline constexpr const char
    *kObjc3RuntimeBootstrapSelectorPoolInterningModel =
        "canonical-selector-pool-preinterned-during-startup-image-walk";
inline constexpr const char *kObjc3RuntimeBootstrapRealizationStagingModel =
    "registration-table-roots-retained-for-later-realization";
inline constexpr const char *kObjc3RuntimeBootstrapResetContractId =
    "objc3c.runtime.bootstrap.reset.replay.v1";
inline constexpr const char *kObjc3RuntimeBootstrapResetSurfacePath =
    "frontend.pipeline.semantic_surface.objc_runtime_bootstrap_reset_contract";
inline constexpr const char
    *kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol =
        "objc3_runtime_replay_registered_images_for_testing";
inline constexpr const char
    *kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol =
        "objc3_runtime_copy_reset_replay_state_for_testing";
inline constexpr const char *kObjc3RuntimeInstallationLifecycleProbePath =
    "tests/tooling/runtime/runtime_installation_loader_lifecycle_probe.cpp";
inline constexpr const char *kObjc3RuntimeBootstrapResetLifecycleModel =
    "reset-clears-live-runtime-state-and-zeroes-image-local-init-cells";
inline constexpr const char *kObjc3RuntimeBootstrapReplayOrderModel =
    "replay-re-registers-retained-images-in-original-registration-order";
inline constexpr const char
    *kObjc3RuntimeBootstrapImageLocalInitStateResetModel =
        "retained-bootstrap-image-local-init-cells-reset-to-zero-before-replay";
inline constexpr const char *kObjc3RuntimeBootstrapCatalogRetentionModel =
    "bootstrap-catalog-retained-across-reset-for-deterministic-replay";
