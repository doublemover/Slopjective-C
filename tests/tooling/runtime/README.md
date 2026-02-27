# Objective-C 3 Runtime Shim

`objc3_msgsend_i32` is a deterministic test shim:

- `M = 2147483629`
- `selector_score = sum((byte_i * i), i starting at 1) mod M`, using selector bytes as unsigned 8-bit values
- If `selector == NULL`, `selector_score = 0`
- Return value:

`(41 + 97*receiver + 7*a0 + 11*a1 + 13*a2 + 17*a3 + 19*selector_score) mod M`

The implementation normalizes negative modulo results into `[0, M-1]`.
