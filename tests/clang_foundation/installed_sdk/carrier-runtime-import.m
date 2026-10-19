@import RuntimeCarrierCalls;
int main(void) {
  RuntimeCalls *receiver = [RuntimeCalls new];
  id<RuntimeCarrierProtocol> protocol = receiver;
  {
    CallStrong source = call_make(45);
    CallStrong result = [protocol echo:source];
    source = __objc3_none(id);
    if (call_destroyed(45) || !__objc3_has_value(result)) return 1;
  }
  if (call_destroyed(45) != 1) return 2;
  {
    RuntimeStrongBlock block = runtime_make_block(46);
    CallStrong result = block(__objc3_none(id));
    block = (RuntimeStrongBlock)0;
    if (call_destroyed(46) || !__objc3_has_value(result)) return 3;
  }
  if (call_destroyed(46) != 1) return 4;
  {
    id object = call_object(47);
    CallWeak weak = __objc3_some(id __weak, object);
    CallWeak result = [receiver echoWeak:weak];
    object = (id)0;
    if (call_destroyed(47) != 1 || !__objc3_has_value(result)) return 5;
  }
  return 0;
}
