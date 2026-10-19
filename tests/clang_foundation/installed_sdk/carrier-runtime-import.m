@import RuntimeCarrierCalls;
int main(void) {
  RuntimeCalls *receiver = [RuntimeCalls new];
  id<RuntimeCarrierProtocol> protocol = receiver;
  {
    CallStrong source = call_make(45);
    CallStrong result = [protocol echo:source];
    source = None(id);
    if (call_destroyed(45) || !hasValue(result)) return 1;
  }
  if (call_destroyed(45) != 1) return 2;
  {
    RuntimeStrongBlock block = runtime_make_block(46);
    CallStrong result = block(None(id));
    block = (RuntimeStrongBlock)0;
    if (call_destroyed(46) || !hasValue(result)) return 3;
  }
  if (call_destroyed(46) != 1) return 4;
  {
    id object = call_object(47);
    CallWeak weak = Some(id __weak, object);
    CallWeak result = [receiver echoWeak:weak];
    object = (id)0;
    if (call_destroyed(47) != 1 || !hasValue(result)) return 5;
  }
  return 0;
}
