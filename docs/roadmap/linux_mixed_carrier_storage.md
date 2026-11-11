# Mixed native carrier records and arrays

Source 2.139 / System 2.65 implements mixed ARC/C++ carrier fields, fixed
multidimensional arrays, aggregate and ordinary native C++ enclosing records,
bases, and Optional/Result payloads containing those records. Native source
special members own their subobjects; mixed values are never copied as layout
bytes. Sema preserves selected constructor recipes, default-argument cleanup,
deleted/access checks and throwing-copy exception facts. Unqualified assignment,
union, packed and volatile storage retain their explicit boundaries.

Construction commits one completed subobject at a time. Copy/move constructor
failures and throwing initializer temporaries destroy exactly the completed
prefix, including retained objects and registered weak fields. Native record
allocation, heap arrays, derived records and heap Block captures retain the same
ownership. Composed payloads use that protocol through native functions,
messages, Block calls and heap captures, including empty Optional arms.

Published bodies and type identities require `objc3.cxx-carrier-composition.v2`.
The replaced v1 contract is removed. Missing/invented capabilities, older readers,
source relocation, templates and source-free round trips have controls in C++20
and C++23 ARC profiles. Native controls cover all selected field failure positions,
managed/native arm patterns, weak zeroing and constructor-temporary failures.

All 33 dedicated compiler commands, 56 AST controls, two source-free profiles,
480 decoder replays and three native ASan/UBSan configurations / ten commands
pass. Thirty frontend groups pass before the final nested-payload extension;
the current mixed storage replay also passes.
The [verification receipt](../support/linux_mixed_carrier_storage_progress.json)
records completed checks. Broader compiler/analysis consolidation and one matched
SDK follow the implementation batch. Extension ABI 24, frame 1, services 4 and
schema 2.2 retain their layouts.
