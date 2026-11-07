#define OBJC3_RUNTIME_SERVICE_FAILURE_ONLY
#include "runtime/public/objc3_task.hpp"
#undef OBJC3_RUNTIME_SERVICE_FAILURE_ONLY

extern "C" [[noreturn]] void objc3_checked_nil_result(void) {
  throw objc3::service_failure(OBJC3_SERVICE_INVALID_ARGUMENT);
}
