#include "managed_layout_api.h"

extern int puts(const char *);
extern int strcmp(const char *, const char *);

int main(void) {
  const objc3_darwin_class *leaf = objc3_darwin_class_v1("DarwinManagedChild");
  const objc3_darwin_class *parent = objc3_darwin_superclass_v1(leaf);
  const objc3_darwin_class *root = objc3_darwin_superclass_v1(parent);
  const objc3_darwin_class *value = objc3_darwin_class_v1("DarwinManagedValue");
  if (!leaf || !parent || !root || !value ||
      strcmp(objc3_darwin_class_name_v1(parent), "DarwinManagedParent") ||
      objc3_darwin_superclass_v1(root) ||
      objc3_darwin_instance_size_v1(root) != 8 ||
      objc3_darwin_instance_size_v1(parent) != 24 ||
      objc3_darwin_instance_size_v1(leaf) != 40)
    return 1;
  DarwinManagedChild *owner = (__bridge_transfer DarwinManagedChild *)objc3_darwin_allocate_v1(leaf);
  __weak DarwinManagedChild *owner_observer = owner;
  managed_observe_owner(owner);
  if (!owner || objc3_darwin_object_class_v1((__bridge const void *)owner) != leaf)
    return 2;
  DarwinManagedValue *first = (__bridge_transfer DarwinManagedValue *)objc3_darwin_allocate_v1(value);
  DarwinManagedValue *second = (__bridge_transfer DarwinManagedValue *)objc3_darwin_allocate_v1(value);
  __weak DarwinManagedValue *first_observer = first;
  __weak DarwinManagedValue *second_observer = second;
  [first setTag:10];
  [second setTag:20];
  [owner setInherited:first];
  [owner setAdded:second];
  @autoreleasepool {
    if ([owner inherited] != first || [owner added] != second)
      return 3;
  }
  first = (id)0;
  second = (id)0;
  if (!first_observer || !second_observer || managed_event_count())
    return 4;
  owner = (id)0;
  if (owner_observer || first_observer || second_observer || managed_event_count() != 7)
    return 5;
  const int expected[] = {2, 3, 4, 20, 4, 10, 4};
  for (unsigned i = 0; i < sizeof(expected) / sizeof(expected[0]); ++i)
    if (managed_event_at(i) != expected[i])
      return 6;
  puts("owned Objective-C 3 Darwin managed layout: PASS");
  return 0;
}
