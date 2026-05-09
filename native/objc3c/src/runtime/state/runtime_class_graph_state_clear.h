#pragma once

namespace objc3c::runtime {

struct RuntimeState;

void ClearRealizedClassGraphUnlocked(RuntimeState &state);

}  // namespace objc3c::runtime
