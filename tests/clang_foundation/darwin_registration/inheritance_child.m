#include "inheritance_api.h"

@implementation DarwinInheritanceChild
+ (int)classValue { return [super classValue] + 7; }
- (int)value { return [super value] + 9; }
@end
