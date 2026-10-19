@import fixture.NativePackage;

@fixture.NativePackage.Answer saved;

int main(void) {
  @fixture.NativePackage.Answer value = @fixture.NativePackage.answer();
  @fixture.NativePackage.CheckedCallback callback =
      @fixture.NativePackage.checked_callback();
  int zero = tryForce(callback(0));
  int forwarded = tryForce(@fixture.NativePackage.call_checked(callback, 7));
  @fixture.NativePackage.CheckedBlock block = @fixture.NativePackage.checked_block();
  int block_value = tryForce(block(11));
  __auto_type receiver = @fixture.NativePackage.checked_message_class();
  int message_value = tryForce([receiver messageValue:19]);
  int before = @fixture.NativePackage.destroyed_managed_values();
  int caught = 0;
  int cleanups = 0;
  @autoreleasepool {
    __auto_type managed = tryForce(@fixture.NativePackage.make_managed_value());
#if __has_feature(objc_arc)
    defer { if (!managed) __builtin_trap(); ++cleanups; }
#else
    defer { @fixture.NativePackage.release_managed_value_mrc(managed); ++cleanups; }
#endif
    __auto_type object_callback = @fixture.NativePackage.checked_object_callback();
    if (tryForce(object_callback(managed, (id)0)) != managed) return 2;
#if __has_feature(objc_arc)
    Optional(id) optional_object =
        tryOptional(object_callback(managed, (id)0));
    Optional(id) optional_failure =
        tryOptional(object_callback(managed, managed));
    __auto_type optional_block =
        tryOptional(@fixture.NativePackage.checked_managed_block(block));
    if (!hasValue(optional_object) ||
        !hasValue(optional_block) ||
        hasValue(optional_failure)) return 5;
#endif
    __auto_type managed_block = tryForce(@fixture.NativePackage.checked_managed_block(block));
    if (tryForce(managed_block(13)) != 13) return 3;
    handle { (void)tryValue(@fixture.NativePackage.checked_object(managed, managed)); }
    catch { caught = 1; }
  }
  if (!caught || cleanups != 1 || @fixture.NativePackage.destroyed_managed_values() != before + 1) return 4;
  return message_value == 19 && block_value == 11 && value == 42 && saved == 0 && zero == 0 && forwarded == 7 ? 0 : 1;
}
