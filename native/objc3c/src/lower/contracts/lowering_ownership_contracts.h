#pragma once

#include <string>

inline constexpr const char *kObjc3LoweringOwnerSplitContractId =
    "objc3c.lowering.owner.split.hard-cutover.v1";
inline constexpr const char *kObjc3TypedSemanticHandoffOwner =
    "native.frontend.sema.typed-handoff";
inline constexpr const char *kObjc3LoweringArtifactPublicationOwner =
    "native.lower.artifact-publication";
inline constexpr const char *kObjc3LoweringDiagnosticHandoffOwner =
    "native.lower.diagnostic-handoff";
inline constexpr const char *kObjc3LoweringIRHandoffOwner =
    "native.lower.ir-handoff";
inline constexpr const char *kObjc3RuntimeDispatchLoweringOwner =
    "native.lower.runtime-dispatch";
inline constexpr const char *kObjc3LoweringBackendHandoffOwner =
    "native.lower.backend-handoff";
inline constexpr const char *kObjc3LoweringSemanticBoundaryConsumerOwner =
    "native.lower.semantic-boundary-consumer";
inline constexpr const char *kObjc3RuntimeMetadataHandoffOwner =
    "native.lower.runtime-metadata-handoff";
inline constexpr const char *kObjc3RuntimeRegistrationDescriptorLoweringOwner =
    "native.lower.runtime-registration-descriptor";
inline constexpr const char *kObjc3RuntimeConstructorRootPublicationOwner =
    "native.lower.runtime-constructor-root-publication";
inline constexpr const char *kObjc3RuntimeInitStubPublicationOwner =
    "native.lower.runtime-init-stub-publication";
inline constexpr const char *kObjc3RuntimeRegistrationTablePublicationOwner =
    "native.lower.runtime-registration-table-publication";
inline constexpr const char *kObjc3IRModuleArtifactOwner =
    "native.ir.module-artifact";
inline constexpr const char *kObjc3IRRuntimeDispatchResultOwner =
    "native.ir.runtime-dispatch-result";
inline constexpr const char *kObjc3IRDirectDispatchResultOwner =
    "native.ir.direct-dispatch-result";
inline constexpr const char *kObjc3LoweringNoRetiredRouteOwnerModel =
    "strict-hard-cutover-no-retired-route-no-compatibility-gate";

inline bool Objc3LoweringContractOwnerIsExplicit(const std::string &owner) {
  return !owner.empty() && owner.rfind("native.", 0) == 0;
}

inline bool Objc3LoweringStrictOwnerModelIsReady(
    const std::string &owner,
    const std::string &owner_model,
    bool strict_no_retired_route,
    bool strict_no_compatibility) {
  return Objc3LoweringContractOwnerIsExplicit(owner) &&
         owner_model == kObjc3LoweringNoRetiredRouteOwnerModel &&
         strict_no_retired_route && strict_no_compatibility;
}

inline std::string Objc3LoweringOwnerReplayKey(
    const std::string &owner,
    const std::string &owner_model,
    bool strict_no_retired_route,
    bool strict_no_compatibility) {
  return "owner=" + owner + ";owner_model=" + owner_model +
         ";strict_no_retired_route=" + (strict_no_retired_route ? "true" : "false") +
         ";strict_no_compatibility=" +
         (strict_no_compatibility ? "true" : "false");
}
