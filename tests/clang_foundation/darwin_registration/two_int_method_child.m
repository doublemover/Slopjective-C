#include "two_int_method_api.h"

@implementation DarwinTwoIntChild
+ (int)classValue:(int)first offset:(DarwinOffset)second {
  return [super classValue:first - 2 offset:second + 3] + 5;
}
- (int)instanceValue:(int)first offset:(DarwinOffset)second {
  return [super instanceValue:first + 2 offset:second - 1] + 7;
}
@end
