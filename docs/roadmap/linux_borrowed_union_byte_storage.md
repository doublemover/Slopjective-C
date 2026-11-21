# Borrowed union byte storage

Source 2.151 / System 2.77 extends the Strict System local-storage model to native
union byte geometry. Field offsets come from the target record layout. Equal-sized
pointer arms of the same type at the same physical byte range can represent one
known local pointer cell; reads still join overlapping loan facts.

A complete pointer assignment to a plain, trivial union replaces stale overlapping
loan/effect facts. This permits clearing an earlier loan or rebinding the complete
cell to a different owner, directly or through a proven local pointer/byte alias.
The write must cover the whole union. Owner-bearing, volatile, atomic, nontrivial,
partial, unknown-index and open-target storage retains conservative checks.
Representation copies keep their own snapshot/merge protocol.

Published Strict System bodies using union storage carry
`objc3.borrowed-union-byte-storage.v1`. Source/identity and required capability must
agree. Older readers reject the capability; omission and invention fail closed.
Declaration-only union signatures do not claim body semantics.

The regression matrix includes direct and byte writes, nested record/union paths,
owner rebinding, copied loans, conditional destinations, partial writes, unknown
targets, volatile/atomic/nontrivial unions and mutable captured storage. Source and
PCH run across C17/C++20/C++23 with ARC/MRC. Publication tests check canonical replay,
relocation, older/missing/invented capability controls and retained consumer escape
diagnostics. Native source-free consumers exercise direct replacement, typed byte
stores and owner rebinding after provider removal.

The [receipt](../support/linux_borrowed_union_byte_storage_progress.json) records
57 borrowed compiler regressions, six publication profiles, 64 AST controls, nine
adjacent frontend groups, 720 exact production decoder replays and four native
ASan/UBSan configurations (24 operations), all passing.

This is a development source checkpoint. The matched SDK follows the remaining
implementation and final consolidation. Wider global/interprocedural and callable
mutation proofs remain tracked in the [roadmap](linux_forward_development.md).
