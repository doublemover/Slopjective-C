#include "fixed_layout_api.h"

@implementation DarwinFixedChild
+ (int)layoutVersion {
  return 1;
}
- (int)bump {
  [super bump];
  value += 10;
  return value;
}
- (int)readInheritedValue {
  return value;
}
@end
