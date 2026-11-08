# Native managed union carrier payloads

Source 2.147 / System 2.73 admits explicitly owned native C++ unions as
Optional/Result payloads under ARC. Their public, user-provided, nonthrowing
destructors own the selected native arm. Native copy/move constructors retain
ARC strong/weak/Block fields and C++ subobjects; no inferred whole-union field
walk accesses inactive storage. Complete record definitions are used even when
an earlier forward declaration is the canonical declaration.

Function, message and Block arguments/results, nested carriers, lambda and heap
Block captures, byref forwarding cells, fixed multidimensional arrays and heap
arrays preserve native selected construction and destruction. Implicit carrier
returns now receive Sema construction recipes and C++ implicit move eligibility,
including move-only payloads, without enrolling custom carrier storage in the
ordinary record NRVO protocol. Byref capture analysis accepts absent children in
transfer nodes instead of dereferencing them.

Source-free publication retains the operation and every selected payload
initializer in body identity. A same-type compiler-added xvalue cast on an
implicit return is normalized to the equivalent published move operation;
explicit casts, qualifiers and constructor choices remain bound. The hard cutover
`objc3.native-arc-record-payload.v2` rejects readers that implement only v1.
Throwing/deleted/inaccessible union destruction, packed or volatile unqualified
storage, and MRC managed payloads retain their admission boundaries. Implicit
managed union arm inference remains a separate requirement.

Dedicated compiler controls cover C++20/23, all four profiles, source/PCH replay,
forward declarations, move-only and checked returns, byref initialization and
throwing-union rejection. AST controls retain imported native constructors,
alignment, lifetime and implicit move recipes. Publication checks exact relocation
and reparse identity, missing capabilities and older readers. Production decoding
covers 640 exact replays across 32 independent families; eleven adjacent frontend
groups pass.

Native ASan/UBSan controls exercise present/absent carriers and plain/managed
union arms, copy/move/default-argument-temporary faults across seven routes,
checked/native errors, and exact strong/weak/Block/C++ lifetime balance. Four
release-runtime configurations cover source/PCH, ARC/MRC, disabled copy elision
and C++-only exception handling. A fifth C++23 ARC PCH configuration instruments
both runtime and client and adds constructor-prefix and array-prefix failures.

The [receipt](../support/linux_native_union_carrier_payloads_progress.json)
records focused results and current consolidation. Extension ABI 24/frame
1/services 4/schema 2.2 layouts retain their versions. Remaining implementation
precedes final matched analysis and one SDK delivery.
