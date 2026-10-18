@import fixture.NativePackage;

@fixture.NativePackage.Answer saved;

int main(void) {
  @fixture.NativePackage.Answer value = @fixture.NativePackage.answer();
  @fixture.NativePackage.CheckedCallback callback =
      @fixture.NativePackage.checked_callback();
  int zero = __objc3_try_force(callback(0));
  int forwarded = __objc3_try_force(@fixture.NativePackage.call_checked(callback, 7));
  @fixture.NativePackage.CheckedBlock block = @fixture.NativePackage.checked_block();
  int block_value = __objc3_try_force(block(11));
  __auto_type receiver = @fixture.NativePackage.checked_message_class();
  int message_value = __objc3_try_force([receiver messageValue:19]);
  int before = @fixture.NativePackage.destroyed_managed_values();
  int caught = 0;
  int cleanups = 0;
  @autoreleasepool {
    __auto_type managed = __objc3_try_force(@fixture.NativePackage.make_managed_value());
#if __has_feature(objc_arc)
    defer { if (!managed) __builtin_trap(); ++cleanups; }
#else
    defer { @fixture.NativePackage.release_managed_value_mrc(managed); ++cleanups; }
#endif
    __auto_type object_callback = @fixture.NativePackage.checked_object_callback();
    if (__objc3_try_force(object_callback(managed, (id)0)) != managed) return 2;
#if __has_feature(objc_arc)
    __objc3_optional(id) optional_object =
        __objc3_try_optional(object_callback(managed, (id)0));
    __objc3_optional(id) optional_failure =
        __objc3_try_optional(object_callback(managed, managed));
    __auto_type optional_block =
        __objc3_try_optional(@fixture.NativePackage.checked_managed_block(block));
    if (!__objc3_has_value(optional_object) ||
        !__objc3_has_value(optional_block) ||
        __objc3_has_value(optional_failure)) return 5;
#endif
    __auto_type managed_block = __objc3_try_force(@fixture.NativePackage.checked_managed_block(block));
    if (__objc3_try_force(managed_block(13)) != 13) return 3;
    __objc3_do { (void)__objc3_try(@fixture.NativePackage.checked_object(managed, managed)); }
    __objc3_catch { caught = 1; }
  }
  if (!caught || cleanups != 1 || @fixture.NativePackage.destroyed_managed_values() != before + 1) return 4;
  return message_value == 19 && block_value == 11 && value == 42 && saved == 0 && zero == 0 && forwarded == 7 ? 0 : 1;
}
