#ifndef OBJC3_RUNTIME_SERVICE_AUTORELEASE_POOL_H
#define OBJC3_RUNTIME_SERVICE_AUTORELEASE_POOL_H

// Value-only C/C++ clients do not require an Objective-C runtime. Linked
// Objective-C clients supply these hooks for each execution slice.
extern "C" void *objc_autoreleasePoolPush(void) __attribute__((weak));
extern "C" void objc_autoreleasePoolPop(void *) __attribute__((weak));

namespace objc3::services {

struct CallbackAutoreleasePool {
  const bool active = objc_autoreleasePoolPush && objc_autoreleasePoolPop;
  void *const token = active ? objc_autoreleasePoolPush() : nullptr;
  CallbackAutoreleasePool() = default;
  CallbackAutoreleasePool(const CallbackAutoreleasePool &) = delete;
  CallbackAutoreleasePool &operator=(const CallbackAutoreleasePool &) = delete;
  ~CallbackAutoreleasePool() {
    if (active) objc_autoreleasePoolPop(token);
  }
};

} // namespace objc3::services

#endif
