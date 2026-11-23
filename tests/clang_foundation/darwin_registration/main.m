#include "api.h"
#include <dlfcn.h>
#include <stdio.h>

int main(int argc, char **argv) {
  if (!objc3_darwin_class_v1("DarwinRegistrationRoot") ||
      !objc3_darwin_class_v1("DarwinRegistrationSibling") ||
      objc3_darwin_class_v1("DarwinRegistrationLate"))
    return 1;
  if ([DarwinRegistrationRoot answer] != 42)
    return 2;
  if (argc == 2) {
    void *plugin = dlopen(argv[1], RTLD_NOW | RTLD_LOCAL | RTLD_NODELETE);
    if (!plugin) {
      fprintf(stderr, "%s\n", dlerror());
      return 3;
    }
    int (*answer)(void) = (int (*)(void))dlsym(plugin, "darwin_registration_late_answer");
    if (!objc3_darwin_class_v1("DarwinRegistrationLate") || !answer || answer() != 7)
      return 4;
    dlclose(plugin);
  }
  puts("owned Objective-C 3 Darwin registration: PASS");
  return 0;
}
