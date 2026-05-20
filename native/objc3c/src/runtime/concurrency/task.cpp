#include "runtime/concurrency/task.h"

namespace objc3c::runtime {

bool RuntimeTaskKindIsSupported(int task_kind) {
  return task_kind == 1 || task_kind == 2;
}

}  // namespace objc3c::runtime
