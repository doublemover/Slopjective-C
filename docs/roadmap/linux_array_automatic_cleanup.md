# Fixed-array automatic cleanup

Source 2.161 / System 2.87 extends native automatic cleanup to fixed C/C++
arrays, including multidimensional arrays and dependent element types/bounds.
Callbacks can receive the decayed element/row pointer, the whole-array address,
or a C++ whole-array reference. The normal native call conversion selects the
binding; array decay no longer falls through the guard against record copies.
Converting record temporaries and ARC writeback remain rejected.

The callback runs once for a completely initialized array, before ordinary
reverse-order element destruction. It also runs on exceptional scope exit.
Partial construction destroys only completed elements and never invokes the
array callback. If cleanup throws on normal scope exit, element destruction
still completes and the original callback exception propagates.

Publication preserves selected callbacks, converted operands, dependent recipes
and whole-array shape after provider removal. The hard reader cutover is
`objc3.native-storage-lifetime.v5`; extension/runtime ABI layouts are unchanged.
VLA, reference storage, incomplete and atomic/address-space storage retain their
existing admission rules.

Eighteen compiler/PCH commands cover C17 and C++20/23 ARC/MRC, including dependent
bounds and converting-temporary rejection. The 76 LLVM AST infrastructure tests
include exact array-decay and whole-array-address operand checks. Native controls
cover pointer/row/reference/address forms, dependent arrays, partial construction,
ordinary/unwinding exits and throwing callbacks. Full rebuilt SDK acceptance is
separate from these focused production-source checks.
