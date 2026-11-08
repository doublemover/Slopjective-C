# Native ARC-owning C++ carrier payloads

Source 2.141 / System 2.67 implements native C++ carrier payload records with
ordinary ARC strong, weak and Block fields. Source C++ special members retain,
move and destroy those subobjects. Optional/Result values preserve their selected
native constructors and destructors through functions, messages, Blocks, lambda
captures, escaping byref cells, fixed arrays and direct heap arrays.

A completed native C++ field now has provisional ownership before its initializer
temporaries are destroyed. If a temporary destructor throws before permanent
member cleanup registration, the completed field and earlier members are cleaned
exactly once. Native controls cover leaf constructor/copy/move failures and late
temporary destruction, alongside strong ownership, weak zeroing and heap Block
lifetimes. Existing C++20 ownership admission controls now admit the implemented
record/base/array-field signatures while retaining autoreleasing rejection.

Published signatures, ownership identities and bodies require
`objc3.native-arc-record-payload.v1`. Owned record fields are distinguished from
pointer views; unsafe-only record payloads also retain the new admission contract.
Missing/invented capabilities, older readers, source relocation, generic use and
source-free replay have controls. Native AST import retains field lifetimes and
nontrivial special-member facts. Union, packed/volatile storage and other
unqualified fields retain explicit boundaries; MRC mixed ownership remains open.

The [verification receipt](../support/linux_native_arc_record_payloads_progress.json)
records completed checks: 25 dedicated compiler commands, the updated ownership
admission control, all 58 AST controls, two source-free profiles, 520 production
decoder replays and three native ASan/UBSan configurations / ten commands pass.
A full sweep of 542 discovered compiler cases is running. ABI 24/frame 1/services 4/schema 2.2 retain their
layouts. Full current compiler/analysis consolidation and one matched SDK follow
the remaining implementation batch.
