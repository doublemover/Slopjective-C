#pragma once

#include "lower/contracts/interop_foreign_call_lowering_contracts.h"

#include <cstddef>
#include <string>

// Interop FFI metadata preservation owns provider and consumer import-surface
// replay facts for foreign callables, annotations, and separate-compilation
// metadata preservation.
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationContractId =
        "objc3c.interop.ffi.metadata.interface.preservation.v1";
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_interop_ffi_metadata_and_interface_preservation";
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationImportArtifactMemberName =
        "objc_interop_ffi_metadata_and_interface_preservation";
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationSourceContractId =
        kObjc3InteropForeignCallLifetimeLoweringContractId;
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationPreservationModel =
        "provider-runtime-import-surfaces-now-preserve-interop-callable-and-annotation-metadata-beyond-local-manifest-ir-and-object-emission";
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationSourceModel =
        "local-lane-c-interop-lowering-and-lane-a-preservation-packets-feed-one-runtime-import-surface-summary-for-separate-compilation-replay";
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationFailClosedModel =
        "missing-import-surface-packet-drifted-replay-keys-or-non-deterministic-provider-preservation-disables-cross-module-interop-preservation-claims";

struct Objc3InteropFfiMetadataInterfacePreservationContract {
  std::size_t local_foreign_callable_count = 0;
  std::size_t local_metadata_preservation_sites = 0;
  std::size_t local_interface_annotation_sites = 0;
  std::size_t imported_module_count = 0;
  std::size_t imported_foreign_callable_count = 0;
  std::size_t imported_metadata_preservation_sites = 0;
  std::size_t imported_interface_annotation_sites = 0;
  bool runtime_import_artifact_ready = false;
  bool separate_compilation_preservation_ready = false;
  bool deterministic = false;
};

bool IsValidObjc3InteropFfiMetadataInterfacePreservationContract(
    const Objc3InteropFfiMetadataInterfacePreservationContract &contract);
std::string Objc3InteropFfiMetadataInterfacePreservationReplayKey(
    const Objc3InteropFfiMetadataInterfacePreservationContract &contract);
