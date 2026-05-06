#include "runtime/memory/autorelease_pool.h"

namespace objc3c::runtime {

bool RuntimeAutoreleasePoolCanEnqueue(int value) {
  return value != 0;
}

}  // namespace objc3c::runtime
