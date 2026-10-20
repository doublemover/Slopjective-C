#include "objc3_runtime_services.h"

_Static_assert(OBJC3_SERVICE_ABI_VERSION == 3u, "expected service ABI v3");
_Static_assert(OBJC3_VALUE_OPS_VERSION == 1u, "expected value ops v1");

int objc3_task_services_c18_header_check(void) {
  objc3_completion completion = {0};
  objc3_task_start_options options = {0};
  objc3_service_status (*start_fn)(const objc3_task_start_options *,
                                   objc3_task_t **) = objc3_task_start;
  objc3_service_status (*complete_fn)(objc3_task_t *,
                                      const objc3_completion *) =
      objc3_task_complete;
  objc3_service_status (*enqueue_fn)(objc3_executor_t *,
                                     objc3_executor_callback, void *) =
      objc3_executor_enqueue;
  return (start_fn && complete_fn && enqueue_fn && !completion.version &&
          !options.version) ? 0 : 1;
}
