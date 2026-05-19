#pragma once

#include "runtime/metadata/runtime_ownership_contracts.h"

namespace objc3c::runtime {

struct RuntimeDispatchFrameOwnership {
  const char *owner_split_contract_id = kObjc3RuntimeOwnerSplitContractId;
  const char *dispatch_frame_state_owner =
      kObjc3RuntimeDispatchFrameStateOwner;
  const char *fail_closed_ownership_model =
      kObjc3RuntimeFailClosedOwnershipModel;
  bool ownership_explicit = true;
};

inline RuntimeDispatchFrameOwnership
RuntimeDispatchFrameOwnershipForHardCutover() {
  RuntimeDispatchFrameOwnership ownership;
  ownership.ownership_explicit = RuntimeOwnerSplitContractIsReady();
  return ownership;
}

}  // namespace objc3c::runtime
