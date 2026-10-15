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
  return block_value == 11 && value == 42 && saved == 0 && zero == 0 && forwarded == 7 ? 0 : 1;
}
