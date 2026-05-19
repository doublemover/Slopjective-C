#include "runtime/storage/weak_slot_target.h"

namespace objc3c::runtime {

bool RuntimeWeakSlotTargetIsTrackable(int receiver) {
  return receiver != 0;
}

}  // namespace objc3c::runtime
