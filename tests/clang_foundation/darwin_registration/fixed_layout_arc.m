#include "fixed_layout_api.h"

extern int puts(const char *);

int main(void) {
  Class cls = objc3_darwin_class_v1("DarwinFixedChild");
  if (!cls || [DarwinFixedChild layoutVersion] != 1)
    return 1;

  DarwinFixedChild *second = objc3_darwin_allocate_v1(cls);
  __weak DarwinFixedChild *observer = (id)0;
  @autoreleasepool {
    DarwinFixedChild *first = objc3_darwin_allocate_v1(cls);
    observer = first;
    if (!first || !second || first == second || [first readInheritedValue] ||
        [first bump] != 11 || [observer readInheritedValue] != 11 || [second readValue])
      return 2;
    first = (id)0;
    if (observer)
      return 3;
  }
  if ([second bump] != 11)
    return 4;
  puts("owned Objective-C 3 Darwin fixed root layout ARC lifetime: PASS");
  return 0;
}
