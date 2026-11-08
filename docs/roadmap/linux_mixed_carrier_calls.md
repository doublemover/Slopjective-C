# Mixed ARC/C++ carrier calls

Source 2.135 / System 2.61 extends native Linux x86-64 carrier call ownership
to selected trees containing both C++ values and ARC strong, weak or unsafe
references. Ordinary functions, function pointers, Objective-C messages and
Block invocation use caller-owned argument snapshots and fresh indirect results.
Implicit by-value copies/moves retain Sema's selected constructor recipe; copy
and late argument-temporary failures destroy only completed snapshots.

Raw and typed checked returns, optional propagation, discarded results and nested
carriers share selected-arm cleanup. Both native exception and typed error paths
preserve exact C++ destruction and ARC ownership, including failure after return
construction. Nil message receivers raise the existing native invalid-argument
service failure after argument evaluation. Weak copies preserve zeroing behavior
without retaining their referent.

Published callable signatures, nested callbacks, method declarations and bodies
require `objc3.mixed-carrier-call.v1`. The production codec and compatibility
validator require the capability to agree exactly with the declaration identity;
missing, invented and unsupported capabilities reject. Source-free replay checks
cover C++20/23, module relocation and method/Block boundaries. Runtime metadata
uses the existing v13 selected-arm layout encoding.

This span retains ARC-only admission for mixed ownership and the existing public,
nondeleted, nonthrowing C++ destructor requirement. Enclosing records/arrays,
assignment, allocation, lambda/MRC captures, ownership-aware FFI and larger or
otherwise unqualified runtime leaves remain separately tracked implementation.
Extension ABI 24, native frame 1, services 4 and schema 2.2 retain their layouts.
All 24 dedicated compiler commands, two source-free publication profiles,
360 production decoder replays and four ARC ASan/UBSan configurations / fourteen
commands pass. Native controls include both active arms, implicit/explicit
copies and moves, nested/default arms, strong/weak ownership, checked/optional
returns, nil receivers and copy/move/late temporary failures. Adjacent frontend
and full compiler consolidation is in progress. This development compiler is
not a matched SDK.
