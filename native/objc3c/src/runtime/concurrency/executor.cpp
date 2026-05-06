#include "runtime/concurrency/executor.h"

namespace objc3c::runtime {

bool RuntimeExecutorTagIsValid(int executor_tag) {
  return executor_tag >= 0;
}

}  // namespace objc3c::runtime
