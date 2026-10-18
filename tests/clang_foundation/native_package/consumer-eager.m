Answer saved;

int main(void) {
  Answer value = answer();
  CheckedCallback callback = checked_callback();
  int zero = __objc3_try_force(callback(0));
  int forwarded = __objc3_try_force(call_checked(callback, 7));
  CheckedBlock block = checked_block();
  int block_value = __objc3_try_force(block(11));
  __auto_type receiver = checked_message_class();
  int message_value = __objc3_try_force([receiver messageValue:19]);
  int before = destroyed_managed_values();
  int caught = 0;
  @autoreleasepool {
    __auto_type managed = __objc3_try_force(make_managed_value());
    defer { release_managed_value_mrc(managed); }
    __auto_type object_callback = checked_object_callback();
    if (__objc3_try_force(object_callback(managed, (id)0)) != managed) return 2;
    __auto_type managed_block = __objc3_try_force(checked_managed_block(block));
    if (__objc3_try_force(managed_block(13)) != 13) return 3;
    __objc3_do { (void)__objc3_try(checked_object(managed, managed)); }
    __objc3_catch { caught = 1; }
  }
  if (!caught || destroyed_managed_values() != before + 1) return 4;
  return message_value == 19 && block_value == 11 && value == 42 && saved == 0 && zero == 0 && forwarded == 7 ? 0 : 1;
}
