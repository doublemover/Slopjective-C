#include "fixed_layout_api.h"

@implementation DarwinFixedBase
- (int)readValue {
  return value;
}
- (int)bump {
  return ++value;
}
@end
