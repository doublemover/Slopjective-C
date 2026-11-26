#include <objc3_darwin.h>

extern int puts(const char *);

@class DarwinCarrierValue;
typedef Optional(DarwinCarrierValue *) MaybeValue;
typedef Optional(unsigned long) MaybeNumber;

__attribute__((objc_root_class))
@interface DarwinCarrierValue {
  Class isa;
  MaybeValue _saved;
}
- (void)save:(MaybeValue)value;
- (MaybeValue)saved;
- (MaybeValue)echo:(MaybeValue)value;
@end

@implementation DarwinCarrierValue
- (void)save:(MaybeValue)value {
  _saved = value;
}
- (MaybeValue)saved {
  return _saved;
}
- (MaybeValue)echo:(MaybeValue)value {
  return value;
}
@end

__attribute__((noinline))
static MaybeNumber number(unsigned long input) {
  return input ? Some(unsigned long, input) : None(unsigned long);
}

static DarwinCarrierValue *unwrap(MaybeValue value) {
  return match (value) {
    case .Some(let object) => object;
    case .None => (DarwinCarrierValue *)0;
  };
}

int main(void) {
  unsigned long answer = match (number(42)) {
    case .Some(let value) => value;
    case .None => 0UL;
  };
  if (answer != 42 || match (number(0)) {
        case .Some(let value) => value != 0;
        case .None => 0;
      })
    return 1;

  const objc3_darwin_class *cls = objc3_darwin_class_v1("DarwinCarrierValue");
  DarwinCarrierValue *holder = (__bridge_transfer DarwinCarrierValue *)objc3_darwin_allocate_v1(cls);
  __weak DarwinCarrierValue *observer = (id)0;
  @autoreleasepool {
    if (unwrap([holder saved]))
      return 2;
    DarwinCarrierValue *value = (__bridge_transfer DarwinCarrierValue *)objc3_darwin_allocate_v1(cls);
    observer = value;
    [holder save:Some(DarwinCarrierValue *, value)];
    if (unwrap([holder echo:[holder saved]]) != value)
      return 3;
    if (unwrap([(DarwinCarrierValue *)0 echo:Some(DarwinCarrierValue *, value)]))
      return 4;
  }
  if (!observer)
    return 5;
  holder = (id)0;
  if (observer)
    return 6;
  puts("owned Objective-C 3 Darwin carrier dispatch: PASS");
  return 0;
}
