#pragma once

#include <string>

#include "artifacts/identity/artifact_identity.h"

// binary inspection harness expansion anchor: lane-C now freezes one
// emitted-metadata inspection corpus over llvm-readobj/llvm-objdump so every
// currently emitted metadata section family can be asserted structurally from
// produced objects, including fail-closed negative gating when metadata
// compilation stops before object emission.
inline constexpr const char *kObjc3RuntimeBinaryInspectionHarnessContractId =
    "objc3c.runtime.binary.inspection.harness.v1";
inline constexpr const char *kObjc3RuntimeBinaryInspectionPositiveCorpusModel =
    "positive-structural-section-and-symbol-corpus-with-case-specific-absence-checks";
inline constexpr const char *kObjc3RuntimeBinaryInspectionNegativeCorpusModel =
    "negative-compile-failure-gating-with-no-object-inspection";
inline constexpr const char *kObjc3RuntimeBinaryInspectionSectionCommand =
    objc3::artifacts::identity::kObjc3NativeDefaultObjectSectionInventoryCommand;
inline constexpr const char *kObjc3RuntimeBinaryInspectionSymbolCommand =
    objc3::artifacts::identity::kObjc3NativeDefaultObjectSymbolInventoryCommand;

// object-packaging/retention freeze anchor: lane-D now freezes the
// current produced-object boundary around the host default object artifact plus
// retained aggregate metadata symbols. Later archive/link/startup-registration
// work must preserve these anchors instead of redefining the object boundary ad
// hoc.
inline constexpr const char *kObjc3RuntimeObjectPackagingRetentionContractId =
    "objc3c.runtime.object.packaging.retention.boundary.v1";
inline constexpr const char *kObjc3RuntimeObjectPackagingRetentionBoundaryModel =
    "current-object-file-boundary-with-retained-metadata-section-aggregates";
inline constexpr const char *kObjc3RuntimeObjectPackagingRetentionAnchorModel =
    "llvm.used-plus-aggregate-section-symbols";
inline constexpr const char *kObjc3RuntimeObjectPackagingRetentionArtifact =
    objc3::artifacts::identity::kObjc3NativeDefaultObjectArtifactName;
inline constexpr const char *kObjc3RuntimeObjectPackagingRetentionSymbolPrefix =
    "__objc3_sec_";

// linker-retention/dead-strip resistance anchor: lane-D now adds one
// public linker anchor plus one public discovery root over the retained
// metadata aggregates. The compiler publishes a driver-friendly linker-response
// file and a discovery JSON sidecar so single-library packaging can survive
// dead stripping without claiming the later multi-archive/TU edge cases.
inline constexpr const char *kObjc3RuntimeLinkerRetentionContractId =
    "objc3c.runtime.linker.retention.and.dead.strip.resistance.v1";
inline constexpr const char *kObjc3RuntimeLinkerRetentionAnchorModel =
    "public-linker-anchor-rooted-in-discovery-table";
inline constexpr const char *kObjc3RuntimeLinkerDiscoveryModel =
    "public-discovery-root-over-retained-metadata-aggregates";
inline constexpr const char *kObjc3RuntimeLinkerResponseArtifactSuffix =
    ".runtime-metadata-linker-options.rsp";
inline constexpr const char *kObjc3RuntimeLinkerDiscoveryArtifactSuffix =
    ".runtime-metadata-discovery.json";
inline constexpr const char *kObjc3RuntimeLinkerAnchorLogicalSection =
    "objc3.runtime.linker_anchor";
inline constexpr const char *kObjc3RuntimeLinkerDiscoveryRootLogicalSection =
    "objc3.runtime.discovery_root";
inline constexpr const char *kObjc3RuntimeLinkerRetentionCoffFlagModel =
    "-Wl,/include:<symbol>";
inline constexpr const char *kObjc3RuntimeLinkerRetentionElfFlagModel =
    "-Wl,--undefined=<symbol>";
inline constexpr const char *kObjc3RuntimeLinkerRetentionMachOFlagModel =
    "-Wl,-u,_<symbol>";

std::string Objc3RuntimeMetadataBinaryInspectionHarnessSummary();
std::string Objc3RuntimeMetadataObjectPackagingRetentionSummary();
std::string Objc3RuntimeMetadataLinkerRetentionSummary();
