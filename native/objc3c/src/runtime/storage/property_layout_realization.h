#pragma once

namespace objc3c::runtime {

struct RealizedClassNode;
struct RuntimeState;

bool AttachRealizedPropertyLayoutRecordsUnlocked(RuntimeState &state,
                                                 RealizedClassNode &node);

}  // namespace objc3c::runtime
