#include "runtime/public/objc3_keypath.h"

extern "C" Class objc_getClass(const char *);
@interface __ObjC3KVCStringFactory
+ (id)stringWithUTF8String:(const char *)bytes;
@end

extern "C" id objc3_keypath_kvc_string(
    const objc3_keypath_descriptor_v2 *descriptor) {
  if (objc3_keypath_validate_v2(descriptor) != OBJC3_KEYPATH_OK ||
      !(descriptor->flags & OBJC3_KEYPATH_KVC_COMPATIBLE))
    return (id)0;
  Class factory=objc_getClass("NSString");
  return factory ? [factory stringWithUTF8String:descriptor->path] : (id)0;
}
