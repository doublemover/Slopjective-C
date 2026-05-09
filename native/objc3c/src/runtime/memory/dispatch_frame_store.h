#pragma once

#include "runtime/metadata/runtime_ownership_contracts.h"
#include "runtime/state/runtime_thread_records.h"

#include <vector>

namespace objc3c::runtime {

struct RuntimeDispatchFrameState {
  const char *owner_split_contract_id = kObjc3RuntimeOwnerSplitContractId;
  const char *dispatch_frame_state_owner =
      kObjc3RuntimeDispatchFrameStateOwner;
  const char *fail_closed_ownership_model =
      kObjc3RuntimeFailClosedOwnershipModel;
  std::vector<RuntimeDispatchFrame> frames;
  RuntimeDispatchFrame testing_frame;
  bool has_testing_frame = false;
  bool ownership_explicit = true;
  bool fallback_path_allowed = false;
};

RuntimeDispatchFrameState &RuntimeDispatchFrameStateForCurrentThread();

}  // namespace objc3c::runtime
