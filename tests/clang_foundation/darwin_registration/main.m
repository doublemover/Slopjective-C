#include "api.h"
#include <dlfcn.h>
#include <stdio.h>

int main(int argc, char **argv) {
  if (!objc3_darwin_class_v1("DarwinRegistrationRoot") ||
      !objc3_darwin_class_v1("DarwinRegistrationSibling") ||
      objc3_darwin_class_v1("DarwinRegistrationLateParent") ||
      objc3_darwin_class_v1("DarwinRegistrationLate"))
    return 1;
  if ([DarwinRegistrationRoot answer] != 42)
    return 2;

  const struct class_record *root = objc3_darwin_class_v1("DarwinRegistrationRoot");
  DarwinRegistrationRoot *first = objc3_darwin_allocate_v1(root);
  DarwinRegistrationRoot *second = objc3_darwin_allocate_v1(root);
  if (!first || !second || first == second || [first answer] != 17 || [second combined] != 20)
    return 3;
  objc3_darwin_deallocate_v1(first);
  DarwinRegistrationRoot *empty = (DarwinRegistrationRoot *)0;
  if ([empty answer] != 0 || [second combined] != 20)
    return 4;
  objc3_darwin_deallocate_v1(second);

  if (argc == 2) {
    void *plugin = dlopen(argv[1], RTLD_NOW | RTLD_LOCAL);
    if (!plugin) {
      fprintf(stderr, "%s\n", dlerror());
      return 5;
    }
    int (*answer)(void) = (int (*)(void))dlsym(plugin, "darwin_registration_late_answer");
    int (*instance_answer)(void) = (int (*)(void))dlsym(plugin, "darwin_registration_late_instance_answer");
    const struct class_record *late = objc3_darwin_class_v1("DarwinRegistrationLate");
    const struct class_record *late_parent = objc3_darwin_class_v1("DarwinRegistrationLateParent");
    void *live = objc3_darwin_allocate_v1(late);
    if (!late || !late_parent || objc3_darwin_superclass_v1(late) != late_parent ||
        !live || !answer || answer() != 7 ||
        !instance_answer || instance_answer() != 23)
      return 6;
    if (dlclose(plugin) || objc3_darwin_object_class_v1(live) != late ||
        objc3_darwin_class_v1("DarwinRegistrationLate") != late ||
        objc3_darwin_superclass_v1(late) != late_parent ||
        answer() != 7 || instance_answer() != 23)
      return 7;
    objc3_darwin_deallocate_v1(live);
    plugin = dlopen(argv[1], RTLD_NOW | RTLD_LOCAL);
    if (!plugin || objc3_darwin_class_v1("DarwinRegistrationLate") != late || dlclose(plugin))
      return 8;
  }
  puts("owned Objective-C 3 Darwin registration: PASS");
  puts("owned Objective-C 3 Darwin instances: PASS");
  return 0;
}
