#pragma once

namespace objc3c::runtime {

struct RuntimeState;

bool RetainRuntimeBlockHandleUnlocked(RuntimeState &state, int block_handle);
bool ReleaseRuntimeBlockHandleUnlocked(RuntimeState &state, int block_handle);

}  // namespace objc3c::runtime
