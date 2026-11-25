#ifndef OBJC3_RUNTIME_SERVICE_AUTORELEASE_POOL_H
#define OBJC3_RUNTIME_SERVICE_AUTORELEASE_POOL_H

#if !defined(OBJC3_VALUE_ONLY_SERVICE_RUNTIME)
// Value-only C/C++ clients do not require an Objective-C runtime. Linked
// Objective-C clients supply these hooks for each execution slice.
extern "C" void *objc_autoreleasePoolPush(void) __attribute__((weak));
extern "C" void objc_autoreleasePoolPop(void *) __attribute__((weak));
#endif

namespace objc3::services {

struct CallbackAutoreleasePool {
#if defined(OBJC3_VALUE_ONLY_SERVICE_RUNTIME)
  CallbackAutoreleasePool() = default;
  ~CallbackAutoreleasePool() {}
#else
  const bool active = objc_autoreleasePoolPush && objc_autoreleasePoolPop;
  void *const token = active ? objc_autoreleasePoolPush() : nullptr;
  CallbackAutoreleasePool() = default;
  CallbackAutoreleasePool(const CallbackAutoreleasePool &) = delete;
  CallbackAutoreleasePool &operator=(const CallbackAutoreleasePool &) = delete;
  ~CallbackAutoreleasePool() {
    if (active) objc_autoreleasePoolPop(token);
  }
#endif
};

} // namespace objc3::services

#endif
