#include "runtime/concurrency/actor.h"

namespace objc3c::runtime {

bool RuntimeActorHandleIsValid(int actor_handle) {
  return actor_handle != 0;
}

}  // namespace objc3c::runtime
