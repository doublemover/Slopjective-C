@import CapturedCarriers;
int main(void) {
  CapturedState block = capture_strong(45);
  if (block() || call_destroyed(45)) return 1;
  block = (CapturedState)0;
  if (call_destroyed(45) != 1) return 2;
  id object = call_object(46);
  block = capture_weak(object);
  object = (id)0;
  if (block() != 1 || call_destroyed(46) != 1) return 3;
  CapturedThrowing checked = capture_throwing(47);
  @autoreleasepool {
    Optional(NestedStrong) missing = tryOptional(checked(48, 1));
    if (hasValue(missing) || call_destroyed(47) || call_destroyed(48) != 2) return 4;
  }
  checked = (CapturedThrowing)0;
  if (call_destroyed(47) != 1 || throws_created() != throws_destroyed()) return 5;
  return 0;
}
