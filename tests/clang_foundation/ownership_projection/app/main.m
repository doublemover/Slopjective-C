@import fixture.Reexport;
#include <objc/runtime.h>
#include <objc/objc-arc.h>
#include <stdio.h>
#define CHECK(test) do { if (!(test)) { fprintf(stderr, "projection failure %d: %s\n", __LINE__, #test); return __LINE__; } } while (0)
int main(void) {
  CHECK(reexport_created() == 0 && projected_destroyed() == 0);
  @autoreleasepool {
#if __has_feature(objc_arc)
    Optional(id) value = Some(id, projected_new());
    __weak id witness;
    match(value) {
      case .Some(let object): { witness = object; CHECK(projected_observe(object)); }
      case .None: { CHECK(0); }
    }
    CHECK(witness != (id)0 && projected_destroyed() == 0);
    value = None(id);
    CHECK(witness == (id)0 && projected_destroyed() == 1);
#else
    id value = projected_new();
    CHECK(projected_observe(value) && projected_destroyed() == 0);
    objc_release(value);
    CHECK(projected_destroyed() == 1);
#endif
    id borrowed;
    @autoreleasepool {
      borrowed = projected_borrow();
#if !__has_feature(objc_arc)
      borrowed = objc_retain(borrowed);
#endif
      projected_clear();
      CHECK(projected_observe(borrowed) && projected_destroyed() == 1);
    }
#if __has_feature(objc_arc)
    borrowed = (id)0;
#else
    objc_release(borrowed);
#endif
    CHECK(projected_destroyed() == 2);
    id autoreleased;
    @autoreleasepool {
      autoreleased = projected_autoreleased();
#if !__has_feature(objc_arc)
      autoreleased = objc_retain(autoreleased);
#endif
      CHECK(projected_observe(autoreleased));
    }
    CHECK(projected_destroyed() == 2 && projected_observe(autoreleased));
#if __has_feature(objc_arc)
    autoreleased = (id)0;
#else
    objc_release(autoreleased);
#endif
    CHECK(projected_destroyed() == 3 && projected_created() == 3);
  }
  CHECK(projected_created() == projected_destroyed());
  return 0;
}
