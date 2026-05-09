#include "runtime/storage/weak_slot_ref_match.h"

#include "runtime/storage/runtime_instance_records.h"

namespace objc3c::runtime {

bool RuntimeWeakSlotRefMatchesStorage(const RuntimeWeakSlotRef &ref,
                                      int owner_receiver,
                                      std::size_t offset,
                                      std::size_t size) {
  return ref.owner_receiver == owner_receiver && ref.offset == offset &&
         ref.size == size;
}

bool RuntimeWeakSlotRefIsOwnedByReceiver(const RuntimeWeakSlotRef &ref,
                                         int owner_receiver) {
  return ref.owner_receiver == owner_receiver;
}

bool RuntimeWeakSlotRefIsAddressableInOwner(const RuntimeWeakSlotRef &ref,
                                            const RuntimeInstanceRecord &owner) {
  return ref.size != 0u && ref.offset <= owner.storage_bytes.size() &&
         ref.size <= owner.storage_bytes.size() - ref.offset;
}

}  // namespace objc3c::runtime
