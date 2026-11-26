#include "inheritance_api.h"
#include <stdio.h>

int main(void) {
  const struct class_record *base = objc3_darwin_class_v1("DarwinInheritanceBase");
  const struct class_record *child = objc3_darwin_class_v1("DarwinInheritanceChild");
  if (!base || !child || base == child)
    return 1;
  if ([DarwinInheritanceChild classValue] != 37 || [DarwinInheritanceChild inheritedClassValue] != 19)
    return 2;
  DarwinInheritanceChild *object = objc3_darwin_allocate_v1(child);
  if (!object || [object value] != 42 || [object inheritedValue] != 23)
    return 3;
  objc3_darwin_deallocate_v1(object);
  puts("owned Objective-C 3 Darwin inheritance: PASS");
  return 0;
}
