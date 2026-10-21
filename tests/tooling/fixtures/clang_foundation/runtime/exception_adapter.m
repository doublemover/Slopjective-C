#include "exception_adapter.h"
#include <objc/runtime.h>
#include <stdio.h>
static int destroyed[4], cleanups;
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "exception adapter failure %d: %s\n", __LINE__, #x); return __LINE__; } } while (0)
__attribute__((objc_root_class))
@interface AdapterProbe { Class isa; int key; }
+ (id)newWithKey:(int)value;
- (int)value;
@end
@implementation AdapterProbe
+ (id)newWithKey:(int)value {
  AdapterProbe *object = class_createInstance(self, 0);
  object->key = value;
  return object;
}
- (int)value { return key; }
- (void)dealloc { ++destroyed[key]; object_dispose(self); }
@end
id adapter_make(int key) { return [AdapterProbe newWithKey:key]; }
void adapter_cleaned(void) { ++cleanups; }
void adapter_throw_c(int key) { @throw adapter_make(key); }
int main(void) {
  for (int mode = 0; mode != 4; ++mode) {
    for (int repeat = 0; repeat != 32; ++repeat) {
      int before = cleanups, caught = 0;
      @autoreleasepool {
        @try { adapter_throw_cpp(mode, mode); CHECK(0); }
        @catch (id error) {
          CHECK(error && [(AdapterProbe *)error value] == mode);
          CHECK(cleanups == before + (mode == 1 ? 2 : 1));
          CHECK(adapter_uncaught() == 0);
          caught = 1;
        }
      }
      CHECK(caught && destroyed[mode] == repeat + 1 && adapter_uncaught() == 0);
    }
  }
  return 0;
}
