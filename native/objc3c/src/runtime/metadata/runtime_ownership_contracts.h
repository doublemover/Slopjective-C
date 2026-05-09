#pragma once

#include <cstdint>

namespace objc3c::runtime {

inline constexpr const char *kObjc3RuntimeOwnerSplitContractId =
    "objc3c.runtime.owner_split.hard_cutover.v1";
inline constexpr const char *kObjc3RuntimeMetadataModelOwner =
    "native-runtime-metadata-model-owner";
inline constexpr const char *kObjc3RuntimeRegistrationTableOwner =
    "native-runtime-registration-table-owner";
inline constexpr const char *kObjc3RuntimeManifestDescriptorArtifactOwner =
    "native-runtime-manifest-descriptor-artifact-owner";
inline constexpr const char *kObjc3RuntimeBootstrapReplayOwner =
    "native-runtime-bootstrap-replay-owner";
inline constexpr const char *kObjc3RuntimeDispatchFrameStateOwner =
    "native-runtime-dispatch-frame-state-owner";
inline constexpr const char *kObjc3RuntimePublicRegistrationApiOwner =
    "native-runtime-public-registration-api-owner";
inline constexpr const char *kObjc3RuntimePublicDispatchDiagnosticsOwner =
    "native-runtime-public-dispatch-diagnostics-owner";
inline constexpr const char *kObjc3RuntimeFailClosedOwnershipModel =
    "runtime-owned-hard-cutover-no-fallback";
inline constexpr std::uint64_t kObjc3RuntimeRegistrationTableAbiVersion = 2;
inline constexpr std::uint64_t kObjc3RuntimeRegistrationTablePointerFieldCount =
    12;

inline bool RuntimeFallbackPathsAreAllowed() {
  return false;
}

inline bool RuntimeOwnerSplitContractIsReady() {
  return kObjc3RuntimeOwnerSplitContractId[0] != '\0' &&
         kObjc3RuntimeMetadataModelOwner[0] != '\0' &&
         kObjc3RuntimeRegistrationTableOwner[0] != '\0' &&
         kObjc3RuntimeManifestDescriptorArtifactOwner[0] != '\0' &&
         kObjc3RuntimeBootstrapReplayOwner[0] != '\0' &&
         kObjc3RuntimeDispatchFrameStateOwner[0] != '\0' &&
         kObjc3RuntimePublicRegistrationApiOwner[0] != '\0' &&
         kObjc3RuntimePublicDispatchDiagnosticsOwner[0] != '\0' &&
         kObjc3RuntimeFailClosedOwnershipModel[0] != '\0' &&
         kObjc3RuntimeRegistrationTableAbiVersion == 2 &&
         kObjc3RuntimeRegistrationTablePointerFieldCount == 12 &&
         !RuntimeFallbackPathsAreAllowed();
}

}  // namespace objc3c::runtime
