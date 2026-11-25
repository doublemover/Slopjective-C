#include <objc3_darwin.h>
#include <pthread.h>

extern int puts(const char *);

__attribute__((objc_root_class))
@interface DarwinBlockValue {
  Class isa;
  int _value;
}
- (void)setValue:(int)value;
- (int)value;
@end

@implementation DarwinBlockValue
- (void)setValue:(int)value {
  _value = value;
}
- (int)value {
  return _value;
}
@end

typedef void (^Work)(void);

static void *run_work(void *argument) {
  @autoreleasepool {
    Work work = (__bridge_transfer Work)argument;
    work();
  }
  return NULL;
}

int main(void) {
  const objc3_darwin_class *cls = objc3_darwin_class_v1("DarwinBlockValue");
  DarwinBlockValue *strong = (__bridge_transfer DarwinBlockValue *)objc3_darwin_allocate_v1(cls);
  DarwinBlockValue *weak_only = (__bridge_transfer DarwinBlockValue *)objc3_darwin_allocate_v1(cls);
  [strong setValue:42];
  [weak_only setValue:17];
  __weak DarwinBlockValue *strong_observer = strong;
  __weak DarwinBlockValue *weak_observer = weak_only;
  __block int result = -1;
  void *argument;
  {
    Work inner = ^{
      DarwinBlockValue *observed = weak_observer;
      result = observed ? -2 : [strong value];
    };
    Work outer = ^{
      inner();
    };
    argument = (__bridge_retained void *)outer;
  }
  strong = (id)0;
  weak_only = (id)0;
  if (!strong_observer || weak_observer) {
    (void)(__bridge_transfer Work)argument;
    return 1;
  }
  pthread_t thread;
  if (pthread_create(&thread, NULL, run_work, argument) != 0) {
    (void)(__bridge_transfer Work)argument;
    return 2;
  }
  if (pthread_join(thread, NULL) != 0 || result != 42 || strong_observer || weak_observer)
    return 3;
  puts("owned Objective-C 3 Darwin block lifetime: PASS");
  return 0;
}
