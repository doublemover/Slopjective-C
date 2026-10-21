@import NestedCarrierCalls;
int main(void) {
  NestedCalls *receiver = [NestedCalls new];
  NestedStrong source = nested_make(45), result = [receiver echo:source];
  source = None(CallStrong);
  if (call_destroyed(45) || nested_state(result)) return 1;
  result = [receiver direct:result];
  @autoreleasepool {
    int caught = 0;
    handle { result = tryValue(nested_forward(result, 46, 1)); }
    catch(id<Error> error) { if (!error) return 2; caught = 1; }
    if (!caught || call_destroyed(45) || call_destroyed(46) != 2 || nested_state(result)) return 3;
  }
  result = None(CallStrong);
  if (call_destroyed(45) != 1 || throws_created() != throws_destroyed()) return 4;
  id object = call_object(47);
  NestedWeak weak = Some(CallWeak, Some(id __weak, object));
  object = (id)0;
  if (nested_weak_state(weak) != 1 || call_destroyed(47) != 1) return 5;
  return 0;
}
