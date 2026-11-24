#include "int_method_api.h"

extern int puts(const char *message);

static int evaluated;

__attribute__((noinline))
static int evaluate_argument(void) {
  ++evaluated;
  return -4096;
}

__attribute__((noinline))
static int send_to_instance(DarwinIntChild *object, int input) {
  return [object instanceValue:input];
}

int main(void) {
  const struct class_record *child = objc3_darwin_class_v1("DarwinIntChild");
  if (!child || [DarwinIntChild classValue:-5] != 6)
    return 1;
#if __has_feature(objc_arc)
  DarwinIntChild *object = (__bridge_transfer DarwinIntChild *)objc3_darwin_allocate_v1(child);
#else
  DarwinIntChild *object = objc3_darwin_allocate_v1(child);
#endif
  if (!object || send_to_instance(object, -7) != 17 || [object legacyValue] != 10)
    return 2;
  if (send_to_instance((DarwinIntChild *)0, evaluate_argument()) != 0 || evaluated != 1)
    return 3;
#if !__has_feature(objc_arc)
  objc3_darwin_deallocate_v1(object);
#endif
  puts("owned Objective-C 3 Darwin int-argument methods: PASS");
  return 0;
}
