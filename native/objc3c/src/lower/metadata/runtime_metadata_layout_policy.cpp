#include "lower/metadata/runtime_metadata_layout_policy.h"

#include "lower/metadata/runtime_metadata_layout_policy_build_support.h"
#include "lower/metadata/runtime_metadata_layout_policy_readiness_support.h"
#include "lower/metadata/runtime_metadata_layout_policy_replay_support.h"

#include <sstream>
#include <string>

bool TryBuildObjc3RuntimeMetadataLayoutPolicy(
    const Objc3RuntimeMetadataLayoutPolicyInput &input,
    Objc3RuntimeMetadataLayoutPolicy &policy, std::string &error) {
  InitializeRuntimeMetadataLayoutPolicyFromInput(input, policy);
  if (!ValidateRuntimeMetadataLayoutPolicyInputPrerequisites(input, policy,
                                                            error) ||
      !PopulateRuntimeMetadataLayoutPolicyFamilies(input, policy, error) ||
      !ValidateRuntimeMetadataLayoutPolicyDescriptorInventory(input, policy,
                                                              error)) {
    return false;
  }

  policy.ready = true;
  policy.fail_closed = true;
  return true;
}

bool IsReadyObjc3RuntimeMetadataLayoutPolicy(
    const Objc3RuntimeMetadataLayoutPolicy &policy) {
  return HasReadyRuntimeMetadataLayoutPolicyHeader(policy) &&
         HasReadyRuntimeMetadataLayoutPolicyFamilies(policy) &&
         HasReadyRuntimeMetadataLayoutPolicyDescriptorInventory(policy);
}

std::string Objc3RuntimeMetadataLayoutPolicyReplayKey(
    const Objc3RuntimeMetadataLayoutPolicy &policy) {
  std::ostringstream out;
  // normalized layout policy anchor: replay proof now serializes the
  // canonical normalized metadata layout decision rather than relying on
  // emitter-local hardcoded family ordering or relocation semantics.
  // object-format policy expansion anchor: replay proof now also
  // serializes the explicit host-format surface, including emitted section
  // spellings and retention-anchor behavior.
  AppendRuntimeMetadataLayoutPolicyReplayHeader(out, policy);
  AppendRuntimeMetadataLayoutPolicyReplayFamilies(out, policy);
  AppendRuntimeMetadataLayoutPolicyReplayFailure(out, policy);
  return out.str();
}
