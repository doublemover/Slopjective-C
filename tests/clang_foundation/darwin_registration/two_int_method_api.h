#include "api.h"

typedef signed int DarwinOffset;

__attribute__((objc_root_class))
@interface DarwinTwoIntBase
+ (int)classValue:(int)first offset:(DarwinOffset)second;
- (int)instanceValue:(int)first offset:(DarwinOffset)second;
@end

@interface DarwinTwoIntChild : DarwinTwoIntBase
+ (int)classValue:(int)first offset:(DarwinOffset)second;
- (int)instanceValue:(int)first offset:(DarwinOffset)second;
@end
