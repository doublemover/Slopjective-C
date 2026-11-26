#include "int_method_api.h"

@implementation DarwinIntBase
+ (int)classValue:(DarwinArgument)input {
  return 2 * input + 13;
}
- (int)instanceValue:(DarwinArgument)input {
  return 3 * input + 25;
}
- (int)legacyValue { return 10; }
@end
