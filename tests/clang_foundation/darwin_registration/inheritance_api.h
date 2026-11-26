#include "api.h"

__attribute__((objc_root_class))
@interface DarwinInheritanceBase
+ (int)classValue;
+ (int)inheritedClassValue;
- (int)value;
- (int)inheritedValue;
@end

@interface DarwinInheritanceChild : DarwinInheritanceBase
+ (int)classValue;
- (int)value;
@end
