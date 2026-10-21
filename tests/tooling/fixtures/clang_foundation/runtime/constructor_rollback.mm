#include <objc/runtime.h>
#include <exception>
#include <stdio.h>
static int constructed, destructed, throw_mode, completed, failed_destructors;
static int destroyed[3], order[512], order_count;
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"constructor rollback failure %d: %s\n",__LINE__,#x); return __LINE__; } } while(0)
__attribute__((objc_root_class))
@interface ConstructionProbe { Class isa; int key; }
+ (id)newWithKey:(int)value;
@end
@implementation ConstructionProbe
+ (id)newWithKey:(int)value {
  ConstructionProbe *object=class_createInstance(self,0); object->key=value; return object;
}
- (void)dealloc { ++destroyed[key]; object_dispose(self); }
@end
struct ConstructedIvar {
  int key;
  id object;
  ConstructedIvar():key(constructed++ & 1),object([ConstructionProbe newWithKey:key]) {}
  ~ConstructedIvar() { order[order_count++]=key; ++destructed; }
};
struct FailingIvar {
  FailingIvar() {
    if (throw_mode==0) throw 42;
    if (throw_mode==1) @throw [ConstructionProbe newWithKey:2];
    ++completed;
  }
  ~FailingIvar() { ++failed_destructors; }
};
__attribute__((objc_root_class))
@interface ConstructionRoot { Class isa; ConstructedIvar root; }
+ (id)new;
@end
@implementation ConstructionRoot
+ (id)new { return class_createInstance(self,0); }
- (void)dealloc { object_dispose(self); }
@end
@interface ConstructionMiddle: ConstructionRoot @end
@implementation ConstructionMiddle @end
@interface ConstructionLeaf: ConstructionMiddle { ConstructedIvar prefix; FailingIvar failure; }
@end
@implementation ConstructionLeaf @end
int main(void) {
  for (int mode=0;mode!=2;++mode) {
    throw_mode=mode;
    for (int repeat=0;repeat!=32;++repeat) {
      int before=destructed;
      @autoreleasepool {
        if (mode==0) {
          try { (void)[ConstructionLeaf new]; CHECK(0); }
          catch(int error) { CHECK(error==42); }
        } else {
          @try { (void)[ConstructionLeaf new]; CHECK(0); }
          @catch(id error) { CHECK(error); }
        }
        CHECK(constructed==before+2 && destructed==constructed);
        CHECK(order[order_count-2]==1 && order[order_count-1]==0);
        CHECK(!completed && !failed_destructors && std::uncaught_exceptions()==0);
      }
      CHECK(destroyed[0]==mode*32+repeat+1 && destroyed[1]==destroyed[0]);
    }
  }
  CHECK(destroyed[2]==32);
  throw_mode=2;
  { id value=[ConstructionLeaf new]; CHECK(value && completed==1); }
  CHECK(failed_destructors==1 && constructed==destructed);
  CHECK(destroyed[0]==65 && destroyed[1]==65 && std::uncaught_exceptions()==0);
  return 0;
}
