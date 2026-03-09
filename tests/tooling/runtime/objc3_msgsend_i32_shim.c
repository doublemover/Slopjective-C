#include <stddef.h>
#include <stdint.h>

/*
Deterministic Objective-C 3 runtime shim for test harness calls.
All arithmetic is reduced modulo kModulus to avoid signed overflow UB.
M251-A001 freeze: this shim remains test-only evidence and is not the native
runtime library, metadata registration path, or executable object model.
M251-C001 freeze: this shim does not define metadata section inventory,
symbol retention roots, or native object-file symbol policy.
M251-C002 scaffold: the native driver now emits retained metadata placeholder
globals, but this shim still is not the runtime registration, lookup, or executable object-model implementation.
M251-C003 object inspection harness: emitted objects are now inspected with
llvm-readobj/llvm-objdump evidence, but that harness still validates compiler layout and retention rather than pretending the shim is a native runtime.
M251-D001 freeze: the canonical native runtime-library surface is reserved for
`objc3_runtime` / `objc3_runtime_register_image` / `objc3_runtime_lookup_selector` /
`objc3_runtime_dispatch_i32` / `objc3_runtime_reset_for_testing`; this shim remains
separate test-only evidence until M251-D002 and M251-D003 land.
M251-D002 core feature: the in-tree native runtime library now exists and the
native `objc3_runtime_dispatch_i32` entrypoint intentionally mirrors this
deterministic formula, but the driver/link path still targets this shim until
M251-D003 wires the real library into executable builds.
M251-D003 link wiring: emitted objects now link against
`artifacts/lib/objc3_runtime.lib`, and this shim remains explicit test-only
compatibility evidence for negative unresolved-symbol coverage and formula
parity documentation.
M255-A001 dispatch-classification freeze: instance/class/super/dynamic dispatch families all remain routed through the live runtime compatibility family while direct dispatch remains a reserved non-goal.
M255-A002 dispatch-site modeling: frontend/sema/lowering now materialize non-zero receiver identities for implicit self/super and known class-name sites so the same compatibility family remains reachable through native LLVM emission.
M255-B001 dispatch legality/selector-resolution freeze: the shim consumes one already-normalized selector string and explicit receiver value only; it does not resolve ambiguity, recover missing selector forms, or simulate direct dispatch.
*/
int objc3_msgsend_i32(int receiver, const char *selector, int a0, int a1, int a2, int a3) {
    static const int64_t kModulus = 2147483629LL;
    int64_t selector_score = 0;

    if (selector != NULL) {
        int64_t index = 1;
        const unsigned char *cursor = (const unsigned char *)selector;
        while (*cursor != 0U) {
            selector_score = (selector_score + ((int64_t)(*cursor) * index)) % kModulus;
            ++cursor;
            ++index;
        }
    }

    int64_t value = 41;
    value += ((int64_t)receiver * 97);
    value += ((int64_t)a0 * 7);
    value += ((int64_t)a1 * 11);
    value += ((int64_t)a2 * 13);
    value += ((int64_t)a3 * 17);
    value += (selector_score * 19);

    value %= kModulus;
    if (value < 0) {
        value += kModulus;
    }

    return (int)value;
}
