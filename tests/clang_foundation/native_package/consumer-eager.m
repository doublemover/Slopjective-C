Answer saved;

int main(void) {
  Answer value = answer();
  CheckedCallback callback = checked_callback();
  int zero = tryForce(callback(0));
  int forwarded = tryForce(call_checked(callback, 7));
  CheckedBlock block = checked_block();
  int block_value = tryForce(block(11));
  __auto_type receiver = checked_message_class();
  int message_value = tryForce([receiver messageValue:19]);
  int before = destroyed_managed_values();
  int caught = 0;
  @autoreleasepool {
    __auto_type managed = tryForce(make_managed_value());
    defer { release_managed_value_mrc(managed); }
    __auto_type object_callback = checked_object_callback();
    if (tryForce(object_callback(managed, (id)0)) != managed) return 2;
    __auto_type managed_block = tryForce(checked_managed_block(block));
    if (tryForce(managed_block(13)) != 13) return 3;
    handle { (void)tryValue(checked_object(managed, managed)); }
    catch { caught = 1; }
  }
  if (!caught || destroyed_managed_values() != before + 1) return 4;
  return message_value == 19 && block_value == 11 && value == 42 && saved == 0 && zero == 0 && forwarded == 7 ? 0 : 1;
}
