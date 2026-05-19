#pragma once

#include "runtime/classes/receiver_identity.h"

#include <cstddef>
#include <cstdint>

namespace objc3c::runtime {

struct RuntimeState;

void RebuildRealizedClassGraphUnlocked(RuntimeState &state);

}  // namespace objc3c::runtime
