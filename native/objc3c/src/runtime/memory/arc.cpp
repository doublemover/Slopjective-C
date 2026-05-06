#include "runtime/memory/arc.h"

namespace objc3c::runtime {

bool RuntimeArcValueIsRetainable(int value) {
  return value != 0;
}

}  // namespace objc3c::runtime
