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
inline constexpr const char *kObjc3RuntimeDispatchLoweringOwner =
    "native.lower.runtime-dispatch";
inline constexpr const char *kObjc3LoweringBackendHandoffOwner =
    "native.lower.backend-handoff";
inline constexpr const char *kObjc3LoweringNoFallbackOwnerModel =
    "strict-hard-cutover-no-fallback-no-compatibility-shim";

inline bool Objc3LoweringContractOwnerIsExplicit(const std::string &owner) {
  return !owner.empty() && owner.rfind("native.", 0) == 0;
}

inline bool Objc3LoweringStrictOwnerModelIsReady(
    const std::string &owner,
    const std::string &owner_model,
    bool strict_no_fallback,
    bool strict_no_compatibility) {
  return Objc3LoweringContractOwnerIsExplicit(owner) &&
         owner_model == kObjc3LoweringNoFallbackOwnerModel &&
         strict_no_fallback && strict_no_compatibility;
}

inline std::string Objc3LoweringOwnerReplayKey(
    const std::string &owner,
    const std::string &owner_model,
    bool strict_no_fallback,
    bool strict_no_compatibility) {
  return "owner=" + owner + ";owner_model=" + owner_model +
         ";strict_no_fallback=" + (strict_no_fallback ? "true" : "false") +
         ";strict_no_compatibility=" +
         (strict_no_compatibility ? "true" : "false");
}
