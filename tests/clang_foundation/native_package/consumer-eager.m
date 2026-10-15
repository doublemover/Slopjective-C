Answer saved;

int main(void) {
  Answer value = answer();
  CheckedCallback callback = checked_callback();
  int zero = __objc3_try_force(callback(0));
  int forwarded = __objc3_try_force(call_checked(callback, 7));
  CheckedBlock block = checked_block();
  int block_value = __objc3_try_force(block(11));
  return block_value == 11 && value == 42 && saved == 0 && zero == 0 && forwarded == 7 ? 0 : 1;
}
