#include "typed_method_api.h"

@implementation DarwinTypedChild
- (DarwinTypedTriple)combine:(DarwinTypedTriple)value offset:(const int *)offset bias:(int)bias {
  DarwinTypedTriple result = [super combine:value offset:offset bias:bias];
  result.third += 100;
  return result;
}
- (id)identity:(id)value {
  return [super identity:value];
}
@end
