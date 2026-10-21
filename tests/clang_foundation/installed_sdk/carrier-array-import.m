@import ArrayCarriers;
int main(void) {
  if (array_provider(50) != 1 || call_destroyed(50) != 1) return 1;
  ArrayWeakState inspect;
  {
    id object = call_object(51);
    CallAction action = ^{ if (!object) __builtin_trap(); };
    ArrayWeakBlock weak = array_weak_identity(__objc3_some(CallAction __weak, action));
    ArrayWeakBlock values[2] = {weak, weak};
    inspect = array_capture_weak(values[1]);
    if (inspect() != 0) return 2;
  }
  if (call_destroyed(51) != 1 || inspect() != 1) return 3;
  return 0;
}
