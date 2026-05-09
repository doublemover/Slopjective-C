#pragma once

#include "lower/contracts/lowering_ownership_contracts.h"

#include <string>

// Strict runtime dispatch validation contracts own the hard-cutover owner
// readiness and replay surface used by dispatch boundary normalization.
inline constexpr const char *kObjc3RuntimeDispatchLoweringOwnerModel =
    kObjc3LoweringNoFallbackOwnerModel;

inline bool Objc3RuntimeDispatchLoweringOwnerIsReady() {
  return Objc3LoweringStrictOwnerModelIsReady(
      kObjc3RuntimeDispatchLoweringOwner,
      kObjc3RuntimeDispatchLoweringOwnerModel,
      true,
      true);
}

inline std::string Objc3RuntimeDispatchLoweringOwnerReplayKey() {
  return Objc3LoweringOwnerReplayKey(
      kObjc3RuntimeDispatchLoweringOwner,
      kObjc3RuntimeDispatchLoweringOwnerModel,
      true,
      true);
}
