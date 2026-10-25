#include "objc3_runtime_services.h"

#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <new>

// Standalone value-runtime witness. Controlled failures target both error
// record allocation and its separately aligned payload storage. The native
// ownership hooks count the +1 consumed by the compiler's private error bridge.
namespace {
enum class AllocationFailure { None, Record, Payload };
thread_local AllocationFailure failure = AllocationFailure::None;
struct Object { unsigned owners = 1, destroyed = 0; };
}
extern "C" void *__wrap__ZnwmRKSt9nothrow_t(
    std::size_t size, const std::nothrow_t &) noexcept {
  if (failure == AllocationFailure::Record) {
    failure = AllocationFailure::None;
    return nullptr;
  }
  try { return ::operator new(size); } catch (...) { return nullptr; }
}
extern "C" void *__wrap__ZnwmSt11align_val_tRKSt9nothrow_t(
    std::size_t size, std::align_val_t alignment, const std::nothrow_t &) noexcept {
  if (failure == AllocationFailure::Payload) {
    failure = AllocationFailure::None;
    return nullptr;
  }
  try { return ::operator new(size, alignment); } catch (...) { return nullptr; }
}
extern "C" void *objc_retain(void *opaque) {
  if (opaque) ++static_cast<Object *>(opaque)->owners;
  return opaque;
}
extern "C" void objc_release(void *opaque) {
  if (opaque && --static_cast<Object *>(opaque)->owners == 0)
    ++static_cast<Object *>(opaque)->destroyed;
}
extern "C" void *_Block_copy(const void *value) {
  return const_cast<void *>(value);
}
extern "C" void _Block_release(const void *) {}
extern "C" objc3_error_t *__objc3_task_error_create(void *) noexcept;
extern "C" void *__objc3_task_error_object(const objc3_error_t *) noexcept;

int main() {
  for (auto fail : {AllocationFailure::Record, AllocationFailure::Payload}) {
    Object object;
    failure = fail;
    auto *error = __objc3_task_error_create(&object);
    assert(failure == AllocationFailure::None && error);
    assert(object.owners == 0 && object.destroyed == 1);
    objc3_service_status status = OBJC3_SERVICE_OK;
    assert(objc3_async_error_get_service(error, &status) == OBJC3_SERVICE_OK);
    assert(status == OBJC3_SERVICE_OUT_OF_MEMORY);
    assert(!__objc3_task_error_object(error));
    objc3_error_release(error);
    assert(object.destroyed == 1);
  }
  auto *invalid = __objc3_task_error_create(nullptr);
  objc3_service_status status = OBJC3_SERVICE_OK;
  assert(objc3_async_error_get_service(invalid, &status) == OBJC3_SERVICE_OK);
  assert(status == OBJC3_SERVICE_INVALID_ARGUMENT);
  objc3_error_release(invalid);

  Object object;
  auto *ordinary = __objc3_task_error_create(&object);
  assert(ordinary && object.owners == 1 && object.destroyed == 0);
  assert(__objc3_task_error_object(ordinary) == &object);
  assert(objc3_async_error_get_service(ordinary, &status) == OBJC3_SERVICE_TYPE_MISMATCH);
  objc3_error_release(ordinary);
  assert(object.owners == 0 && object.destroyed == 1);
}
