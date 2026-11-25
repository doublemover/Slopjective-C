#include "api.h"

__attribute__((objc_root_class))
@interface DarwinRegistrationLateParent {
  Class isa;
  int stored;
}
- (int)store:(int)value;
- (int)storedValue;
@end
