#include "runtime.h"
// A moved capture is committed before a later capture can throw. The completed
// destination owns cleanup; the source remains in its sentinel state on failure.
struct CaptureFailure {};
struct CaptureThrowLater {
  static int copies, fail_on;
  CaptureThrowLater() = default;
  CaptureThrowLater(const CaptureThrowLater &) {
    if (++copies == fail_on) throw CaptureFailure{};
  }
};
int CaptureThrowLater::copies;
int CaptureThrowLater::fail_on;

static void capture_failed_creation(void) {
  int before = capture_destroyed;
  CaptureObject *source = [CaptureObject make:51];
#if __has_feature(objc_arc)
  CaptureObject * __weak observer = source;
#else
  CaptureObject *manual_owner = source;
#endif
  CaptureThrowLater later;
  CaptureThrowLater::copies = 0;
  CaptureThrowLater::fail_on = 1;
  try {
    auto failed = ^[move source, later] { (void)later; };
    (void)failed;
    CAPTURE_CHECK(0);
  } catch (const CaptureFailure &) {
#if !__has_feature(objc_arc)
    [manual_owner release];
#endif
    CAPTURE_CHECK(source == (id)0 && capture_destroyed == before + 1);
#if __has_feature(objc_arc)
    CAPTURE_CHECK(observer == (id)0);
#endif
  }
}
static void capture_failed_heap_copy(void) {
  int before = capture_closed;
  int handle __attribute__((objc_resource(close=capture_close, invalid=-1))) = 61;
  CaptureThrowLater later;
  CaptureThrowLater::copies = 0;
  CaptureThrowLater::fail_on = 2;
  {
    CaptureIntBlock __unsafe_unretained stack = ^[move handle, later] { return handle; };
    CAPTURE_CHECK(handle == -1 && capture_closed == before && stack() == 61);
    try {
      auto failed = [stack copy];
      (void)failed;
      CAPTURE_CHECK(0);
    } catch (const CaptureFailure &) {
      CAPTURE_CHECK(capture_closed == before && stack() == 61);
    }
  }
  CAPTURE_CHECK(capture_closed == before + 1);
}
static void capture_failed_resource_creation(void) {
  int before = capture_closed;
  int handle __attribute__((objc_resource(close=capture_close, invalid=-1))) = 62;
  CaptureThrowLater later;
  CaptureThrowLater::copies = 0;
  CaptureThrowLater::fail_on = 1;
  try {
    auto failed = ^[move handle, later] { (void)later; };
    (void)failed;
    CAPTURE_CHECK(0);
  } catch (const CaptureFailure &) {
    CAPTURE_CHECK(handle == -1 && capture_closed == before + 1);
  }
}
struct CaptureThrowingMove {
  CaptureThrowingMove() = default;
  CaptureThrowingMove(const CaptureThrowingMove &) = default;
  CaptureThrowingMove(CaptureThrowingMove &&) { throw CaptureFailure{}; }
};
static void capture_failed_move_constructor(void) {
  int before = capture_closed;
  int handle __attribute__((objc_resource(close=capture_close, invalid=-1))) = 63;
  CaptureThrowingMove later;
  try {
    auto failed = ^[move handle, move later] {};
    (void)failed;
    CAPTURE_CHECK(0);
  } catch (const CaptureFailure &) {
    CAPTURE_CHECK(handle == -1 && capture_closed == before + 1);
  }
}
int main(void) {
  capture_failed_creation();
  capture_failed_heap_copy();
  capture_failed_resource_creation();
  capture_failed_move_constructor();
  return capture_failures != 0;
}
