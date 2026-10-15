Answer saved;

int main(void) {
  Answer value = answer();
  CheckedCallback callback = checked_callback();
  int zero = __objc3_try_force(callback(0));
  int forwarded = __objc3_try_force(call_checked(callback, 7));
  return value == 42 && saved == 0 && zero == 0 && forwarded == 7 ? 0 : 1;
}
