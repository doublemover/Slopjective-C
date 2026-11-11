#include <objc3/concurrency.h>
#include <assert.h>
#include "dynamic_frame_storage.h"

int main(void) {
  objc3_executor_t *source = NULL;
  assert(objc3_executor_create(1, &source) == OBJC3_SERVICE_OK);
  dynamic_frame_storage_contract(source);
  objc3_executor_release(source);
}
