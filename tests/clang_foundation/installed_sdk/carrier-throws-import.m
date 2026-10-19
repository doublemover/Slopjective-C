@import ManagedThrows;
int main(void) {
  ThrowsCalls *receiver = [ThrowsCalls new];
  id<ThrowsProtocol> protocol = receiver;
  CallStrong source = call_make(45);
  CallStrong result = __objc3_try_force(throws_forward(source, 46, 0));
  source = None(id);
  if (call_destroyed(45) || !hasValue(result)) return 1;
  int caught = 0;
  @autoreleasepool {
    __objc3_do { result = __objc3_try([protocol value:result key:46 fail:1]); }
    __objc3_catch(id<Error> error) { if (!error) return 2; caught = 1; }
  }
  if (!caught || !hasValue(result) || call_destroyed(45) || call_destroyed(46) != 1) return 3;
  result = None(id);
  if (call_destroyed(45) != 1 || throws_created() != throws_destroyed()) return 4;
  ThrowsBlock block = throws_make_block(call_object(47));
  result = __objc3_try_force(block(call_make(48), 49, 0));
  block = (ThrowsBlock)0;
  if (call_destroyed(47) != 1 || call_destroyed(48)) return 5;
  result = None(id);
  if (call_destroyed(48) != 1) return 6;
  id object = call_object(50);
  CallWeak weak = Some(id __weak, object);
  CallWeak copy = __objc3_try_force([protocol weak:weak fail:0]);
  object = (id)0;
  if (call_destroyed(50) != 1 || !hasValue(copy)) return 7;
  return 0;
}
