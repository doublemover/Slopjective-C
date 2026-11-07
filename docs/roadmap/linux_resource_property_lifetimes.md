# Explicit C++ resource property lifetimes

Source 2.121 applies the same native C++ lifetime policy to isolated property
writes and ordinary isolated frame values. A nontrivial record with an explicit,
nondeleted destructor owns its resource fields and active union arm through its
native copy/move/destruction operations. Composed records and constant member
arrays recurse into this policy. Move-only values keep their native value category.
Typed frame hooks still instantiate the actual operations, enforce access and
deleted-operation rules, and require nonthrowing destruction.

An unsafe-Sendable promise does not erase a borrowed field. Raw resource aliases,
opaque resource records without an explicit lifetime, and volatile record
snapshots remain rejected. Ordinary volatile scalar reads retain their existing
value-snapshot admission. This change needs no new public frame layout or semantic
capability: the existing typed frame contract already describes these lifetimes.

The native fixture checks independent live handles and exact closure counts for
copying, moving, nested resource arrays and both union arms. Copy and move faults,
including a fault after one composed member has acquired ownership, unwind only
completed acquisitions. Setter failures, nil writes/reads, cancelled entries and
unused frame disposal preserve ownership and avoid fabricated resource values.

Consolidation also exposed an interface-publication crash on an uninstantiated
constructor initializer. Such initializers may contain an untyped ParenListExpr.
The capability visitor now skips its absent type and continues visiting children.
A dedicated source-free regression checks both plain templates and templates
accompanied by Block values, including replay, relocation and capability presence.

Focused compiler controls cover Core, Strict, Strict Concurrency and Strict System,
C++20/23, ARC/MRC and source/PCH, with negative controls for the rejected contracts.
The fourteen ASan/UBSan native configurations / 49 commands pass, including two
C++ exception-only configurations. The 43 selected adjacent compiler cases pass;
the final volatile-snapshot refinement passes all 52 dedicated compiler commands.
Both new source-free groups pass all eight C++20/23 ARC/MRC profiles; seven
adjacent frontend groups and codec/isolation controls also pass.

Extension ABI 24, native frame 1, runtime services 4 and schema 2.2 remain unchanged.
These receipts describe development source. Async/checked-effect accessors,
implicit resource-union lifetimes, complete Sendable enforcement, full matched
analysis and the replacement SDK remain separate work.
