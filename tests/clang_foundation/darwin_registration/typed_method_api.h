#include "api.h"

typedef struct DarwinTypedTriple {
  long first;
  long second;
  long third;
} DarwinTypedTriple;

__attribute__((objc_root_class))
@interface DarwinTypedBase
- (DarwinTypedTriple)combine:(DarwinTypedTriple)value offset:(const int *)offset bias:(int)bias;
- (id)identity:(id)value;
@end

@interface DarwinTypedChild : DarwinTypedBase
- (DarwinTypedTriple)combine:(DarwinTypedTriple)value offset:(const int *)offset bias:(int)bias;
- (id)identity:(id)value;
@end
