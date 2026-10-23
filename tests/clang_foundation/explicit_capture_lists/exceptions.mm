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
static int capture_failed_replacement(void) { throw CaptureFailure{}; }
static bool capture_replace_throws;
static void capture_resource_operation_unwind(void) {
  int before = capture_closed;
  {
    int handle __attribute__((objc_resource(close=capture_close_address,
                                             invalid=-1))) = 84;
    CaptureIntBlock operate = ^[move handle] {
      if (capture_replace_throws)
        reset(handle, capture_failed_replacement());
      return take(handle);
    };
    capture_replace_throws = true;
    try {
      (void)operate();
      CAPTURE_CHECK(0);
    } catch (const CaptureFailure &) {
      CAPTURE_CHECK(handle == -1 && capture_closed == before + 1);
    }
    capture_replace_throws = false;
    CAPTURE_CHECK(operate() == -1);
  }
  CAPTURE_CHECK(capture_closed == before + 1);
  {
    int handle __attribute__((objc_resource(close=capture_close, invalid=-1))) = 85;
    void (^operate)(void) = ^[move handle] {
      defer { discard(handle); }
      reset(handle, 86);
      throw CaptureFailure{};
    };
    try {
      operate();
      CAPTURE_CHECK(0);
    } catch (const CaptureFailure &) {
      CAPTURE_CHECK(handle == -1 && capture_closed == before + 3);
    }
  }
  CAPTURE_CHECK(capture_closed == before + 3);
  __block int transferred = -1;
  {
    int handle __attribute__((objc_resource(close=capture_close, invalid=-1))) = 87;
    void (^operate)(void) = ^[move handle] {
      transferred = take(handle);
      throw CaptureFailure{};
    };
    try {
      operate();
      CAPTURE_CHECK(0);
    } catch (const CaptureFailure &) {
      CAPTURE_CHECK(handle == -1 && transferred == 87 &&
                    capture_closed == before + 3);
    }
  }
  CAPTURE_CHECK(capture_closed == before + 3);
  capture_close(transferred);
  CAPTURE_CHECK(capture_closed == before + 4);
}
struct CaptureThrowingTemporary {
  ~CaptureThrowingTemporary() noexcept(false) { throw CaptureFailure{}; }
};
static void capture_failed_full_expression(void) {
  int destroyed_before = capture_destroyed, closed_before = capture_closed;
  CaptureObject *source = [CaptureObject make:95];
#if !__has_feature(objc_arc)
  CaptureObject *manual_owner = source;
#endif
  int handle __attribute__((objc_resource(close=capture_close, invalid=-1))) = 96;
  CaptureMoveRecord record(97);
  int live_before = CaptureMoveRecord::live;
  try {
    defer {
      CAPTURE_CHECK(handle == -1 && capture_closed == closed_before + 1);
      CAPTURE_CHECK(CaptureMoveRecord::live == live_before);
    }
    CaptureIntBlock __unsafe_unretained stack =
        (CaptureThrowingTemporary{},
         ^[move source, move handle, move record] {
           return source->value + handle + record.value;
         });
    (void)stack;
    CAPTURE_CHECK(0);
  } catch (const CaptureFailure &) {
#if !__has_feature(objc_arc)
    [manual_owner release];
#endif
    CAPTURE_CHECK(source == (id)0 && handle == -1 && record.value == -1);
    CAPTURE_CHECK(capture_destroyed == destroyed_before + 1 &&
                  capture_closed == closed_before + 1 &&
                  CaptureMoveRecord::live == live_before);
  }
}
static void capture_failed_lifetime_receiver(bool bounded) {
  int destroyed_before = capture_destroyed, closed_before = capture_closed;
  CaptureObject *source = [CaptureObject make:98];
#if !__has_feature(objc_arc)
  CaptureObject *manual_owner = source;
#endif
  int handle __attribute__((objc_resource(close=capture_close, invalid=-1))) = 99;
  try {
    defer { CAPTURE_CHECK(handle == -1 && capture_closed == closed_before + 1); }
    if (bounded) {
      withLifetime((CaptureThrowingTemporary{}, ^[move source, move handle] {})) {
        CAPTURE_CHECK(0);
      }
    } else {
      keepAlive((CaptureThrowingTemporary{}, ^[move source, move handle] {}));
      CAPTURE_CHECK(0);
    }
  } catch (const CaptureFailure &) {
#if !__has_feature(objc_arc)
    [manual_owner release];
#endif
    CAPTURE_CHECK(source == (id)0 && handle == -1 &&
                  capture_destroyed == destroyed_before + 1 &&
                  capture_closed == closed_before + 1);
  }
}
static void capture_committed_field_unwind(void) {
  int before = capture_destroyed;
  try {
    defer { CAPTURE_CHECK(capture_destroyed == before); }
    CaptureObject *source = [CaptureObject make:100];
#if !__has_feature(objc_arc)
    CaptureObject *manual_owner = source;
#endif
    CaptureIntBlock __unsafe_unretained stack = ^[move source] {
      return source->value;
    };
#if !__has_feature(objc_arc)
    [manual_owner release];
#endif
    CAPTURE_CHECK(stack() == 100);
    throw CaptureFailure{};
  } catch (const CaptureFailure &) {
    CAPTURE_CHECK(capture_destroyed == before + 1);
  }
}
#if __has_feature(objc_arc)
static CaptureIntBlock capture_return_failed_temporary(void)
    __attribute__((ns_returns_retained));
static CaptureIntBlock capture_return_failed_temporary(void) {
  int handle __attribute__((objc_resource(close=capture_close, invalid=-1))) = 119;
  CaptureMoveRecord record(120);
  return (CaptureThrowingTemporary{}, ^[move handle, move record] {
    return handle + record.value;
  });
}
static id capture_return_caught_temporary(void)
    __attribute__((ns_returns_retained));
static id capture_return_caught_temporary(void) {
  int before = capture_closed;
  try {
    int handle __attribute__((objc_resource(close=capture_close, invalid=-1))) = 121;
    return (CaptureThrowingTemporary{}, ^[move handle] { return handle; });
  } catch (const CaptureFailure &) {
    CAPTURE_CHECK(capture_closed == before + 1);
    return (id)0;
  }
}
static CaptureIntBlock capture_return_cpp_local_catch(void)
    __attribute__((ns_returns_retained));
static CaptureIntBlock capture_return_cpp_local_catch(void) {
  int handle __attribute__((objc_resource(close=capture_close, invalid=-1))) = 122;
  defer {
    try { throw CaptureFailure{}; }
    catch (const CaptureFailure &) { CAPTURE_CHECK(1); }
  }
  return ^[move handle] { return handle; };
}
static CaptureIntBlock capture_return_cpp_outer_catch(void)
    __attribute__((ns_returns_retained));
static CaptureIntBlock capture_return_cpp_outer_catch(void) {
  int before = capture_closed;
  try {
    try {
      int handle __attribute__((objc_resource(close=capture_close, invalid=-1))) = 123;
      defer { throw CaptureFailure{}; }
      return ^[move handle] { return handle; };
    } catch (const CaptureFailure &) {
      CAPTURE_CHECK(capture_closed == before + 1);
      throw;
    }
  } catch (const CaptureFailure &) {
    CAPTURE_CHECK(capture_closed == before + 1);
    return (id)0;
  }
}
static void capture_owned_return_unwind(void) {
  int before = capture_closed, live_before = CaptureMoveRecord::live;
  try {
    (void)capture_return_failed_temporary();
    CAPTURE_CHECK(0);
  } catch (const CaptureFailure &) {
    CAPTURE_CHECK(capture_closed == before + 1 &&
                  CaptureMoveRecord::live == live_before);
  }
  CAPTURE_CHECK(capture_return_caught_temporary() == (id)0 &&
                capture_closed == before + 2);
  CaptureIntBlock result = capture_return_cpp_local_catch();
  CAPTURE_CHECK(result() == 122 && capture_closed == before + 2);
  result = (id)0;
  CAPTURE_CHECK(capture_closed == before + 3);
  CAPTURE_CHECK(capture_return_cpp_outer_catch() == (id)0 &&
                capture_closed == before + 4);
}
#endif
int main(void) {
  capture_failed_creation();
  capture_failed_heap_copy();
  capture_failed_resource_creation();
  capture_failed_move_constructor();
  capture_resource_operation_unwind();
  capture_failed_full_expression();
  capture_failed_lifetime_receiver(false);
  capture_failed_lifetime_receiver(true);
  capture_committed_field_unwind();
#if __has_feature(objc_arc)
  capture_owned_return_unwind();
#endif
  return capture_failures != 0;
}
