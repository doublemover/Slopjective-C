# Composed C++ carrier payloads and fixed member arrays

Source 2.130 / System 2.56 extends pure C++ carrier ownership to source records
that contain carriers, including fixed multidimensional member arrays. Recursive
qualification follows by-value bases, fields and native carrier arms with bounded
depth/work; ordinary pointer targets do not become owned subobjects. Hidden ARC
storage and existing unqualified storage shapes retain their gates.

Explicit member-array initialization and implicit/defaulted copy, move and
destruction now use native selected-payload operations. Sema preserves each
array-loop leaf's C++ constructor recipe instead of converting the carrier into
a primitive value copy. CodeGen excludes carrier arrays from its aggregate-copy
shortcut, establishes fresh array ownership and retains completed-element prefix
cleanup. A completed current leaf enters that prefix before its argument
temporaries are destroyed, so a late temporary destructor fault also destroys
that leaf. Nested loops share the complete row-major prefix.

Records containing these fields can themselves be Optional/Result payloads.
Native constructors and special members preserve their access, deleted-operation,
copy/move and exception contracts. Ordinary field initialization still requires
explicit selected-arm values; support does not supply an implicit payload or
waive mixed ARC/C++ qualification. Existing source class native call boundaries
and the pure C++ carrier call ABI compose with this storage.

Source-free declarations and published bodies require
`objc3.cxx-carrier-composition.v1` when they publish this composition or carrier
array ownership. Type identities cover bases/fields/arrays and nested payload
records; body identities cover local storage, allocation and disposal. Missing or
invented capabilities and older readers are rejected. Four source-free profiles
preserve replay/relocation fingerprints, including implicit member operations.

The maintained native witness covers two-dimensional member arrays, nested scalar
and array payload records, Some/None, defaulted copy/move and exact destruction.
Construction, copy, move and late copy-argument destructor failures are injected
at every element position, with complete owner balance after each unwind. All 49
new compiler commands and four selected ASan/UBSan configurations / fourteen
commands pass, including optimized MRC PCH without elision and C++-only exception
profiles. Consolidation passes 523 executable compiler cases (526 discovered, three
unsupported), all 51 AST controls, 21 frontend groups, codec/isolation and 260 exact
production decoder replays. The previous negative nested-record transfer control
now verifies acceptance; its stale expected rejection was the sole full-suite
failure, and its repaired regression passes. The preceding carrier-call span
retains the complete sixteen-configuration native receipt.

Extension ABI 24, native frame 1, runtime services 4 and schema 2.2 are unchanged.
Final matched analysis and one replacement SDK remain batched after implementation.
