@import CarrierCalls;
int main(void) {
  @CarrierCalls.Strong source = @CarrierCalls.call_make(40);
  @CarrierCalls.Strong result = @CarrierCalls.call_identity(source);
  source = __objc3_none(id);
  if (@CarrierCalls.call_destroyed(40) != 0) return 1;
  result = __objc3_none(id);
  if (@CarrierCalls.call_destroyed(40) != 1) return 2;
  id object = @CarrierCalls.call_object(41);
  @CarrierCalls.Weak weak = __objc3_some(id __weak, object);
  @CarrierCalls.Weak returned = @CarrierCalls.call_weak(weak);
  object = (id)0;
  if (@CarrierCalls.call_destroyed(41) != 1 ||
      !__objc3_has_value(returned)) return 3;
  returned = @CarrierCalls.call_weak(returned);
  return __objc3_has_value(returned) ? 0 : 4;
}
