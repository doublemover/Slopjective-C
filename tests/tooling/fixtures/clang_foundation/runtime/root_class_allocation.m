#include <objc/runtime.h>
#include <stdio.h>
#include <stdint.h>
typedef struct _NSZone NSZone;
id objc_alloc_init(Class type) __attribute__((ns_returns_retained));
id objc_allocWithZone(Class type) __attribute__((ns_returns_retained));
static int initialized, destroyed, zones;
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "allocation failure %d: %s\n", __LINE__, #x); return __LINE__; } } while (0)
__attribute__((objc_root_class))
@interface AllocRoot { Class isa; }
+ (id)alloc;
+ (id)allocWithZone:(NSZone *)zone;
- (id)init;
@end
@implementation AllocRoot
+ (id)alloc { return class_createInstance(self, 0); }
+ (id)allocWithZone:(NSZone *)zone { ++zones; return class_createInstance(self, 0); }
- (id)init { ++initialized; return self; }
- (void)dealloc { ++destroyed; object_dispose(self); }
@end
@interface NilAlloc : AllocRoot @end
@implementation NilAlloc
+ (id)alloc { return (id)0; }
+ (id)allocWithZone:(NSZone *)zone { return (id)0; }
@end
@interface RedirectAlloc : AllocRoot @end
@implementation RedirectAlloc
+ (id)alloc { return (id)[AllocRoot alloc]; }
@end
@interface NilInit : AllocRoot @end
@implementation NilInit
- (id)init { return (id)0; }
@end
int main(void) {
  CHECK(class_createInstance(objc_getClass("AllocRoot"), SIZE_MAX) == (id)0);
  CHECK(class_createInstance(objc_getClass("AllocRoot"), SIZE_MAX-sizeof(intptr_t)) == (id)0);
  CHECK(objc_alloc_init((Class)0) == (id)0);
  CHECK(objc_alloc_init(objc_getClass("NilAlloc")) == (id)0);
  CHECK([[NilAlloc alloc] init] == (id)0);
  CHECK(objc_allocWithZone(objc_getClass("NilAlloc")) == (id)0);
  CHECK(!initialized && !destroyed && !zones);
  {
    id first = [[AllocRoot alloc] init];
    id redirected = [[RedirectAlloc alloc] init];
    id zoned = objc_allocWithZone(objc_getClass("AllocRoot"));
    CHECK(first && redirected && zoned && zones == 1 && initialized == 2);
    CHECK(object_getClass(redirected) == objc_getClass("AllocRoot"));
  }
  CHECK(destroyed == 3);
  CHECK([[NilInit alloc] init] == (id)0);
  CHECK(destroyed == 4);
  return 0;
}
