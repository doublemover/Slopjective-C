#include "late_api.h"

@implementation DarwinRegistrationLateParent
- (int)store:(int)value { return stored = value; }
- (int)storedValue { return stored; }
@end
