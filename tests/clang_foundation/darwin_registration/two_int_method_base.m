#include "two_int_method_api.h"

@implementation DarwinTwoIntBase
+ (int)classValue:(int)first offset:(DarwinOffset)second {
  return 3 * first + 7 * second;
}
- (int)instanceValue:(int)first offset:(DarwinOffset)second {
  return 10 * first - 3 * second;
}
@end
