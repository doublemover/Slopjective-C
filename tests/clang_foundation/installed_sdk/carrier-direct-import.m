@import DirectCarrierCalls;
int main(void) {
  if (direct_initialized()) return 1;
  CallStrong value = tryForce([DirectCalls echo:call_make(45) fail:0]);
  if (direct_initialized() != 1 || call_destroyed(45) || !hasValue(value)) return 2;
  DirectCalls *receiver = [DirectCalls new];
  int caught = 0;
  @autoreleasepool {
    handle { value = tryValue([receiver echo:value key:46 fail:1]); }
    catch(id<Error> error) { if (!error) return 3; caught = 1; }
  }
  if (!caught || call_destroyed(45) || call_destroyed(46) != 1 || !hasValue(value)) return 4;
  value = None(id);
  if (call_destroyed(45) != 1 || throws_created() != throws_destroyed()) return 5;
  id object = call_object(47);
  CallWeak weak = tryForce([receiver weak:Some(id __weak, object) fail:0]);
  object = (id)0;
  if (call_destroyed(47) != 1 || !hasValue(weak)) return 6;
  DirectCalls *missing = (id)0;
  value = tryForce([missing echo:call_make(48) key:49 fail:1]);
  if (hasValue(value) || call_destroyed(48) != 1 || call_destroyed(49)) return 7;
  return 0;
}
