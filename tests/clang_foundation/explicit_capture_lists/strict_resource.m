// Compile in strict-system profile. Select one negative with -D; without a
// selection this is an executable semantic success fixture.
void capture_resource_close(int) __attribute__((nothrow));
void capture_resource_use(int);
void capture_resource_strict(void) {
  int handle __attribute__((objc_resource(close=capture_resource_close,
                                           invalid=-1))) = 41;
  void (^owner)(void) = ^[move handle] {
#if defined(CAPTURE_BODY_CLOSE)
    capture_resource_close(handle); // error: owner would clean up again
#endif
    capture_resource_use(handle);
  };
  if (handle != -1) __builtin_trap();
#if defined(CAPTURE_USE_AFTER_MOVE)
  capture_resource_use(handle); // error: resource may have been moved
#elif defined(CAPTURE_DOUBLE_CLEANUP)
  capture_resource_close(handle); // error: already moved or invalid
#elif defined(CAPTURE_SECOND_MOVE)
  void (^second)(void) = ^[move handle] { capture_resource_use(handle); };
  (void)second;
#else
  reset(handle, 42);
  void (^second)(void) = ^[move handle] { capture_resource_use(handle); };
  owner();
  second();
#endif
  (void)owner;
}
