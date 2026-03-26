#include <stddef.h>
#include <stdint.h>

/*
Deterministic Objective-C 3 runtime shim for test harness calls.
All arithmetic is reduced modulo kModulus to avoid signed overflow UB.
This shim is explicit compatibility evidence only.
It is not the live runtime library, not a metadata-registration path, and not
authoritative proof of runtime execution.
The canonical live runtime surface is the emitted-object path linked against
`artifacts/lib/objc3_runtime.lib` through
`objc3_runtime_register_image`, `objc3_runtime_lookup_selector`,
`objc3_runtime_dispatch_i32`, and `objc3_runtime_reset_for_testing`.
Keep this file only for deterministic formula-parity and unresolved-symbol
coverage where the real runtime path is intentionally not under test.
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
