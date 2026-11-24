#include "int_method_api.h"

@implementation DarwinIntChild
+ (int)classValue:(DarwinArgument)input {
  return [super classValue:input - 2] + 7;
}
- (int)instanceValue:(DarwinArgument)input {
  return [super instanceValue:input + 1] + [self legacyValue];
}
@end
