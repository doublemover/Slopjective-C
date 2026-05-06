#include "runtime/concurrency/task.h"

namespace objc3c::runtime {

bool RuntimeTaskKindIsSupported(int task_kind) {
  return task_kind >= 0;
}

}  // namespace objc3c::runtime
