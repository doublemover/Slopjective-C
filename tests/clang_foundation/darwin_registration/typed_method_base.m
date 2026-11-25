#include "typed_method_api.h"

@implementation DarwinTypedBase
- (DarwinTypedTriple)combine:(DarwinTypedTriple)value offset:(const int *)offset bias:(int)bias {
  return (DarwinTypedTriple){value.first + *offset, value.second + bias, value.third};
}
- (id)identity:(id)value {
  return value;
}
@end
