#include "runtime/storage/weak_slots.h"

namespace objc3c::runtime {

bool RuntimeWeakSlotTargetIsTrackable(int receiver) {
  return receiver != 0;
}

}  // namespace objc3c::runtime
