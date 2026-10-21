#include "carrier-array-call.h"
#include <stdio.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "array failure %d: %s\n", __LINE__, #x); return __LINE__; } } while (0)
static int weak_nil(CallWeak value) {
  match(value) {
    case .Some(let object): { return object == (id)0; }
    case .None: { return -1; }
  }
  return -2;
}
static int ordinary(void) {
  {
    CallStrong values[3] = {call_make(0), call_make(1)};
    CHECK(hasValue(values[0]) && !hasValue(values[2]));
    CallStrong copy = values[0];
    values[0] = values[1];
    CHECK(!call_destroyed(0) && !call_destroyed(1));
    copy = None(id); CHECK(call_destroyed(0) == 1);
    values[1] = None(id); CHECK(!call_destroyed(1));
  }
  CHECK(call_destroyed(1) == 1);
  {
    CallStrong values[2][2] = {{call_make(2)}, {call_make(3), call_make(4)}};
    CHECK(!hasValue(values[0][1]));
    values[0][1] = values[1][0];
  }
  CHECK(call_destroyed(2) == 1 && call_destroyed(3) == 1 && call_destroyed(4) == 1);
  {
    id object = call_object(5);
    CallWeak values[3] = {Some(id __weak, object), None(id __weak)};
    values[2] = values[0];
    object = (id)0;
    CHECK(call_destroyed(5) == 1 && weak_nil(values[0]) == 1 && weak_nil(values[2]) == 1);
  }
  {
    NestedStrong values[2] = {nested_make(6), nested_make(7)};
    values[0] = None(CallStrong); CHECK(call_destroyed(6) == 1);
  }
  CHECK(call_destroyed(7) == 1);
  {
    CallMixed values[2] = {Ok(id, id __weak, call_object(8)), Err(id, id __weak, (id)0)};
    CHECK(isOk(values[0]) && !isOk(values[1]));
  }
  CHECK(call_destroyed(8) == 1);
  { const CallStrong values[2] = {call_make(9)}; CHECK(hasValue(values[0])); }
  CHECK(call_destroyed(9) == 1);
  @autoreleasepool {
    @try {
      CallStrong values[3] = {call_make(10), call_make(11)};
      @throw call_object(12);
    } @catch (id error) { CHECK(error != (id)0 && call_destroyed(10) == 1 && call_destroyed(11) == 1); }
  }
  CHECK(call_destroyed(12) == 1);
  return 0;
}
static CallStrong fail_native(void) { @throw call_object(15); }
static int partial_native(void) {
  @autoreleasepool {
    @try { CallStrong values[3] = {call_make(13), call_make(14), fail_native()}; CHECK(0); }
    @catch (id error) { CHECK(error != (id)0 && call_destroyed(13) == 1 && call_destroyed(14) == 1); }
  }
  CHECK(call_destroyed(15) == 1);
  return 0;
}
static int fail_typed(void) throws(id<Error>) {
  CallStrong values[3] = {call_make(16), tryValue(throws_value(call_make(17), 18, 1))};
  return 1;
}
static int errors(void) {
  @autoreleasepool {
    int caught = 0;
    handle { (void)tryValue(fail_typed()); CHECK(0); }
    catch(id<Error> error) { CHECK(error != (id)0); caught = 1; }
    CHECK(caught && call_destroyed(16) == 1 && call_destroyed(17) == 1 && call_destroyed(18) == 1);
  }
  CHECK(throws_created() == throws_destroyed());
  return 0;
}
#ifdef __cplusplus
struct FailCleanup { ~FailCleanup() noexcept(false) { throw 42; } };
static CallStrong return_cleanup(void) {
  return Some(id, (FailCleanup{}, call_object(27)));
}
static CallStrong return_lexical_cleanup(void) {
  return __extension__ ({
    CallStrong value = call_make(28);
    FailCleanup cleanup;
    value;
  });
}
static CallStrong return_typed_cleanup(void) throws(id<Error>) {
  return __extension__ ({
    CallStrong value = call_make(29);
    (void)tryValue(throws_scalar(None(id), 30, 1));
    value;
  });
}
static int payload_destroyed;
struct Counted {
  Counted() {}
  Counted(const Counted &) = delete;
  Counted(Counted &&) = delete;
  ~Counted() { ++payload_destroyed; }
};
static int cpp(void) {
  try { CallStrong value = Some(id, (FailCleanup{}, call_object(19))); CHECK(0); }
  catch (int error) { CHECK(error == 42 && call_destroyed(19) == 1); }
  {
    CallStrong target = call_make(20);
    try { target = Some(id, (FailCleanup{}, call_object(21))); CHECK(0); }
    catch (int error) { CHECK(error == 42 && !call_destroyed(21) && call_destroyed(20) == 1); }
  }
  CHECK(call_destroyed(20) == 1 && call_destroyed(21) == 1);
  try {
    CallStrong values[3] = {call_make(22), Some(id, (FailCleanup{}, call_object(23))), call_make(24)};
    CHECK(0);
  } catch (int error) { CHECK(error == 42 && call_destroyed(22) == 1 && call_destroyed(23) == 1 && call_destroyed(24) == 1); }
  {
    CallStrong values[2] = {call_make(25), call_make(26)};
    CallStrong moved = moveValue(values[0]);
    CHECK(hasValue(values[0]) && !call_destroyed(25));
    moved = None(id); CHECK(call_destroyed(25) == 1);
  }
  CHECK(call_destroyed(26) == 1);
  try { return_cleanup(); CHECK(0); }
  catch (int error) { CHECK(error == 42 && call_destroyed(27) == 1); }
  try { return_lexical_cleanup(); CHECK(0); }
  catch (int error) { CHECK(error == 42 && call_destroyed(28) == 1); }
  @autoreleasepool {
    int caught = 0;
    handle { (void)tryValue(return_typed_cleanup()); CHECK(0); }
    catch(id<Error> error) { CHECK(error != (id)0); caught = 1; }
    CHECK(caught && call_destroyed(29) == 1 && call_destroyed(30) == 1);
  }
  CHECK(throws_created() == throws_destroyed());
  try {
    Optional(Counted) value = Some(Counted, (FailCleanup{}, Counted{}));
    CHECK(0);
  } catch (int error) { CHECK(error == 42 && payload_destroyed == 1); }
  return 0;
}
#endif
int main(int argc, char **argv) {
  if (argc > 1) { (void)argv; (void)tryForce(fail_typed()); return 90; }
  CHECK(ordinary() == 0); CHECK(partial_native() == 0); CHECK(errors() == 0);
  CHECK(array_provider(31) == 1 && call_destroyed(31) == 1);
  CHECK(array_weak_blocks() == 0);
#ifdef __cplusplus
  CHECK(cpp() == 0);
#endif
  return 0;
}
