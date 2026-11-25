#include "typed_method_api.h"

extern int puts(const char *);
static int evaluations;

static int evaluate_bias(void) {
  ++evaluations;
  return 3;
}

__attribute__((noinline))
static DarwinTypedTriple send_result(DarwinTypedChild *receiver, const int *offset) {
  return [receiver combine:(DarwinTypedTriple){2, 4, 6} offset:offset bias:evaluate_bias()];
}

__attribute__((noinline))
static id send_identity(DarwinTypedChild *receiver, id value) {
  return [receiver identity:value];
}

int main(void) {
  const struct class_record *cls = objc3_darwin_class_v1("DarwinTypedChild");
  if (!cls)
    return 1;
#if __has_feature(objc_arc)
  DarwinTypedChild *object = (__bridge_transfer DarwinTypedChild *)objc3_darwin_allocate_v1(cls);
#else
  DarwinTypedChild *object = objc3_darwin_allocate_v1(cls);
#endif
  int offset = 9;
  DarwinTypedTriple value = send_result(object, &offset);
  if (value.first != 11 || value.second != 7 || value.third != 106 || evaluations != 1)
    return 2;
  DarwinTypedTriple zero = send_result((DarwinTypedChild *)0, &offset);
  if (zero.first || zero.second || zero.third || evaluations != 2)
    return 3;
#if __has_feature(objc_arc)
  @autoreleasepool {
#endif
    if (send_identity(object, object) != object || send_identity((DarwinTypedChild *)0, object))
      return 4;
#if __has_feature(objc_arc)
  }
#else
  objc3_darwin_deallocate_v1(object);
#endif
  puts("owned Objective-C 3 Darwin typed methods: PASS");
  return 0;
}
