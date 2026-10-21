#import <objc/runtime.h>
#include <stdio.h>

static int destroyed;

__attribute__((objc_root_class))
@interface H3Root
{
  Class isa;
}
+ (instancetype)alloc;
- (instancetype)init;
- (int)value;
- (void)dealloc;
@end

@implementation H3Root
+ (instancetype)alloc { return class_createInstance(self, 0); }
- (instancetype)init { return self; }
- (int)value { return 37; }
- (void)dealloc {
  ++destroyed;
  object_dispose(self);
}
@end

static int (^MakeReader(H3Root *object))(void) {
  return ^{ return [object value]; };
}

int main(void) {
  H3Root *strong = [[H3Root alloc] init];
  __weak H3Root *weak = strong;
  int (^reader)(void) = MakeReader(strong);
  if (weak != strong || reader() != 37) {
    fprintf(stderr, "initial weak=%p strong=%p block-value=%d\n",
            weak, strong, reader());
    return 1;
  }
  strong = nil;
  if (weak == nil || destroyed != 0) {
    fprintf(stderr, "after owner release weak=%p destroyed=%d\n", weak, destroyed);
    return 2; // escaping Block owns capture
  }
  reader = nil;
  if (weak != nil || destroyed != 1) {
    fprintf(stderr, "after Block release weak=%p destroyed=%d\n", weak, destroyed);
    return 3;
  }
  return 0;
}
