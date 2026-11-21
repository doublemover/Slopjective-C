#include "block_values.h"
#include <objc/runtime.h>
#include <objc/blocks_runtime.h>
#include <objc/objc-arc.h>
namespace NativeBlockValues {
__attribute__((ns_returns_retained)) Callback producer(int offset) {
#if __has_feature(objc_arc)
  return ^(int input){return input+offset;};
#else
  return (Callback)_Block_copy((const void *)^(int input){return input+offset;});
#endif
}
Callback shared_producer(int offset) {
#if __has_feature(objc_arc)
  return producer(offset);
#else
  return (Callback)objc_autorelease((id)producer(offset));
#endif
}
__attribute__((nothrow)) void dispose(Callback value __attribute__((ns_consumed))) {
#if __has_feature(objc_arc)
  (void)value;
#else
  _Block_release((const void *)value);
#endif
}
}
