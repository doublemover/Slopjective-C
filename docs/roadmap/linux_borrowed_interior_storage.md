# Inline Objective-C storage loans

System source 2.35 extends Strict System ownership inference to addresses into
Objective-C inline ivar storage: scalar addresses, array decay, array elements
and nested native record/array paths. A pointer or reference field names separate
storage and cannot borrow its referent from the containing allocation without a
separate owner contract. Mutable global, weak and temporary owner expressions
need explicit lifetime control. Scalar and independent record data copies retain
their own values after the original object dies.

The flow solver reuses owner identities, operand snapshots, independent aliases
and lifetime pins for inferred loans. C++ reference bindings carry a separate
address-lifetime lane, independent of a pointer value stored in the referent.
That lane survives reference aliases, local reference-field record copies/moves,
conditional/comma lvalues and reference captures, and expires with its owner.
Builtin address-taking preserves the same facts without introducing mutation.
Implicit native member/operator receivers need the existing nonescape contract;
copying plain data to an independent value permits ordinary native operations.
Opaque mutable interior calls retire the affected lifetime proofs.

Owner-origin expansion, address-use walks and inline storage paths have explicit
bounds. Exhaustion retains an unknown obligation or emits a diagnostic; it never
grants a borrowed return or drops an escape check.

Published expressions with Objective-C ivar access require
`objc3.borrowed-interior-storage.v1`. The canonical declaration identity and
sidecar capability must agree. Codec/import/module readers reject absent or
unsupported admission, keeping source-free bodies under the producer's ownership
rules. Public AST layouts and extension ABI 24 are unchanged.

Compiler source/PCH controls cover C17 and C++20/23, ARC/MRC, including actual
inline borrowed returns, wrong-owner and indirection boundaries, expiry, pins,
reference/address/value copies, local reference aggregates, captures, builtin
addresses, implicit receivers, opaque writes and bounded storage paths. Source-free
controls cover replay, relocation, semantic admission and valid/expired consumers
in the same six profiles. The installed catch-binding runtime fixture now returns
the actual object payload address, with inline arrays, nested storage, C++
reference copies and exact construction/destruction under an independent pin.
Focused and consolidated results are recorded when this span finishes verification.

Native managed-cleanup CFG registration, wider assigned-owner correlation and
remaining ownership/profile boundaries remain separate implementation work.
Matched SDK delivery follows feature consolidation.
