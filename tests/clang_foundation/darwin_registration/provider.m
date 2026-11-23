#include "api.h"

#if !defined(__OBJC3__) || __OBJC3__ != 1
#error Objective-C 3 language mode is required
#endif

@implementation DarwinRegistrationRoot
+ (int)answer {
  int value = 1;
  {
    defer { value += 14; }
    value += 27;
  }
  return value;
}

- (int)answer {
  return 17;
}

- (int)combined {
  return [self answer] + 3;
}
@end

__attribute__((objc_root_class))
@interface DarwinRegistrationSibling
@end

@implementation DarwinRegistrationSibling
@end
