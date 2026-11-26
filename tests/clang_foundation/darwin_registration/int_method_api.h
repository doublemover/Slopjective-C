#include "api.h"

typedef signed int DarwinArgument;

__attribute__((objc_root_class))
@interface DarwinIntBase
+ (int)classValue:(DarwinArgument)input;
- (int)instanceValue:(DarwinArgument)input;
- (int)legacyValue;
@end

@interface DarwinIntChild : DarwinIntBase
+ (int)classValue:(DarwinArgument)input;
- (int)instanceValue:(DarwinArgument)input;
@end
