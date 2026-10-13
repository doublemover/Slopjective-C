@import fixture.NativePackage;

@fixture.NativePackage.Answer saved;

int main(void) {
  @fixture.NativePackage.Answer value = @fixture.NativePackage.answer();
  return value == 42 && saved == 0 ? 0 : 1;
}
