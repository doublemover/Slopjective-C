#include "two_int_method_api.h"

extern int puts(const char *message);

static int first_evaluated;
static int second_evaluated;

static int evaluate_first(void) {
  ++first_evaluated;
  return -4096;
}

static int evaluate_second(void) {
  ++second_evaluated;
  return 9;
}

__attribute__((noinline))
static int send_to_instance(DarwinTwoIntChild *object, int first, DarwinOffset second) {
  return [object instanceValue:first offset:second];
}

int main(void) {
  const struct class_record *child = objc3_darwin_class_v1("DarwinTwoIntChild");
  if (!child || [DarwinTwoIntChild classValue:-6 offset:4] != 30)
    return 1;
#if __has_feature(objc_arc)
  DarwinTwoIntChild *object = (__bridge_transfer DarwinTwoIntChild *)objc3_darwin_allocate_v1(child);
#else
  DarwinTwoIntChild *object = objc3_darwin_allocate_v1(child);
#endif
  if (!object || send_to_instance(object, -4, 9) != -37)
    return 2;
  if (send_to_instance((DarwinTwoIntChild *)0, evaluate_first(), evaluate_second()) != 0 ||
      first_evaluated != 1 || second_evaluated != 1)
    return 3;
#if !__has_feature(objc_arc)
  objc3_darwin_deallocate_v1(object);
#endif
  puts("owned Objective-C 3 Darwin two-int-argument methods: PASS");
  return 0;
}
