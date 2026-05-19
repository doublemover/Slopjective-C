#pragma once

#include "lower/contracts/runtime_artifact_retention_contracts.h"
#include "lower/contracts/runtime_metadata_object_format_contracts.h"

// Visibility and retention policy owns linkage spelling, COMDAT behavior,
// llvm.used ordering, and object-format surface references for metadata roots.
inline constexpr const char *kObjc3RuntimeMetadataComdatPolicy = "disabled";
inline constexpr const char *kObjc3RuntimeMetadataVisibilitySpellingPolicy =
    "local-linkage-omits-explicit-ir-visibility";
inline constexpr const char *kObjc3RuntimeMetadataRetentionOrderingModel =
    "llvm.used-emission-order";
inline constexpr const char *kObjc3RuntimeMetadataObjectFormatPolicyModel =
    "object-format-neutral-until-next-runtime-phase";
