// runtime.h is delivered through either a normal include or a PCH.
#ifndef DEFER_RUNTIME_PCH
#include "runtime.h"
#endif
int main(int argc, char **argv) {
  @autoreleasepool {
    DeferredError *owner __attribute__((objc_precise_lifetime)) = [DeferredError new];
    token = owner;
    int mode = argc > 1 ? argv[1][0] - '0' : 0;
    if(mode == 1) { objc_double_exception(); return 81; }
#ifdef __cplusplus
    if(mode == 2) { cpp_double_exception(); return 82; }
#endif
    resource_ordering();
    lifetime_ordering();
#if __has_feature(objc_arc)
    arc_ordering();
#endif
    if(normal_return() != 7)bad=10;
    expect(21);
    objc_unwind(); expect(312);
    objc_normal_cleanup(); expect(32);
    @try { objc_outside_registration(); bad=11; }
    @catch(id error) { if(error != token)bad=12; first(); }
    expect(31);
    @try { objc_local_catch(); bad=13; }
    @catch(id error) { if(error != token)bad=14; second(); }
    expect(312);
    checked_cleanup(); expect(2451);
    copied_body(1, 0, 3); expect(12);
    copied_body(0, 0, 3); expect(512);
    @try { copied_body(0, 1, 3); bad=15; }
    @catch(id error) { if(error != token)bad=16; record(4); }
    expect(3124);
#ifdef __cplusplus
    cpp_unwind(); expect(312);
    cpp_normal_cleanup(); expect(32);
    try { cpp_outside_registration(); bad=17; }
    catch(int value) { if(value != 42)bad=18; first(); }
    expect(31);
    try { cpp_local_catch(); bad=19; }
    catch(int value) { if(value != 7)bad=20; second(); }
    expect(312);
#endif
#if !__has_feature(objc_arc)
    objc_release(owner);
#endif
  }
  return bad;
}
