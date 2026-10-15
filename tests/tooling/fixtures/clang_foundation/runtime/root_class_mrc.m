#import <objc/runtime.h>

static int destroyed;

__attribute__((objc_root_class))
@interface H3MrcRoot {
  Class isa;
  unsigned int retainCount;
}
+ (instancetype)alloc;
- (instancetype)init;
- (instancetype)retain;
- (oneway void)release;
- (void)dealloc;
@end

@implementation H3MrcRoot
+ (instancetype)alloc {
  H3MrcRoot *object = class_createInstance(self, 0);
  object->retainCount = 1;
  return object;
}
- (instancetype)init { return self; }
- (instancetype)retain { ++retainCount; return self; }
- (oneway void)release {
  if (--retainCount == 0) [self dealloc];
}
- (void)dealloc { ++destroyed; object_dispose(self); }
@end

int main(void) {
  H3MrcRoot *value = [[H3MrcRoot alloc] init];
  [value retain];
  [value release];
  if (destroyed != 0) return 1;
  [value release];
  return destroyed == 1 ? 0 : 2;
}
