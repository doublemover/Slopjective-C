#include "fixed_layout_api.h"

extern int puts(const char *);

int main(void) {
  Class base = objc3_darwin_class_v1("DarwinFixedBase");
  Class child = objc3_darwin_class_v1("DarwinFixedChild");
  DarwinFixedBase *root = objc3_darwin_allocate_v1(base);
  DarwinFixedChild *first = objc3_darwin_allocate_v1(child);
  DarwinFixedChild *second = objc3_darwin_allocate_v1(child);
  if (!base || !child || [DarwinFixedChild layoutVersion] != 1 ||
      !root || !first || !second || root == (id)first ||
      first == second || [root readValue] || [first readInheritedValue] ||
      [second readValue])
    return 1;
  if ([root bump] != 1 || [first bump] != 11 || [first bump] != 22 ||
      [first readInheritedValue] != 22 || [second readValue] || [root readValue] != 1)
    return 2;
  objc3_darwin_deallocate_v1(first);
  if ([second bump] != 11 || [root readValue] != 1)
    return 3;
  objc3_darwin_deallocate_v1(root);
  objc3_darwin_deallocate_v1(second);

  DarwinFixedChild *fresh = objc3_darwin_allocate_v1(child);
  if (!fresh || [fresh readValue])
    return 4;
  objc3_darwin_deallocate_v1(fresh);
  puts("owned Objective-C 3 Darwin fixed root layout: PASS");
  return 0;
}
