#include "inheritance_api.h"

@implementation DarwinInheritanceBase
+ (int)classValue { return [self inheritedClassValue] + 11; }
+ (int)inheritedClassValue { return 19; }
- (int)value { return [self inheritedValue] + 10; }
- (int)inheritedValue { return 23; }
@end
