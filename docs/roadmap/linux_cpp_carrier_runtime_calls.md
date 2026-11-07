# C++ carrier runtime calls and implicit parameter construction

Source 2.134 / System 2.60 extends qualified pure C++ carrier arguments/results
to GNUstep Objective-C messages and native Block invocation. Caller-owned
argument slots and fresh indirect results retain their existing selected-payload
construction and destruction. Instance, class, protocol and super dispatch,
checked/optional/discarded results and register exhaustion share that contract.
Nil C++ carrier receivers raise the existing native invalid-argument service
failure after argument evaluation; no C++ payload is manufactured.

Runtime encoding v13 adds a C++ ownership arm and a versioned opaque native
storage description. Its size and alignment match the complete C++ object;
packed and naturally aligned objects through 16-byte alignment have native
reflection controls. Logical type, constructor/destructor and module identity
remain in the static interface contract. Reflection does not grant generic FFI,
forwarding or Foundation adapters permission to copy or destroy C++ bytes.
Mixed payloads, larger alignments and other unqualified leaves remain separate
implementation work. Existing ARC-only encodings retain their current version.

By-value C++ carrier parameters now retain an implicit copy/move recipe in Sema,
including ordinary functions, messages and Blocks. Constructor overload/access
and deleted-operation checks precede CodeGen. The caller keeps a full-expression
cleanup boundary for snapshots, including void-return calls. Direct lvalue copies
preserve the source; xvalues select moves. Existing local/result implicit-transfer
boundaries are unchanged. Copy failures and late default-argument temporary
destruction preserve both the original value and exact snapshot cleanup.

Published runtime signatures and bodies require `objc3.cxx-carrier-runtime.v1`,
including Block typedefs/callbacks, method-only declarations, nested callable
contracts and captured C++ carriers. Four C++20/23 ARC/MRC profiles preserve
source-free replay and relocation identity; older readers and missing/invented
capabilities reject.

All 52 dedicated compiler commands, 55 AST controls, the new replay profiles,
340 production decoder replays and four selected ASan/UBSan configurations /
fourteen commands pass. Native coverage includes Some/None and Result arms,
protocol/class/super sends, nine argument slots, nil, discarded results, heap
Block copy and copy-failure cleanup, implicit lvalue copies and constructor/late
temporary failures through functions, void messages and Blocks. Packed and
16-byte-aligned owners retain exact layout and lifetime counts. The previous
MRC effect-signature AST fixture was corrected to invoke its Block locally
instead of returning a stack Block. Updated broad compiler/frontend consolidation
is running.

Extension ABI 24, native frame 1, services 4 and schema 2.2 retain their layouts.
One matched SDK and final matched analysis follow the implementation batch.
