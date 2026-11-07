# Managed native runtime scopes

System source 2.37 integrates native `@autoreleasepool` and `@synchronized`
registration into the lifetime and exceptional-control-flow model. Cleanup
follows the existing native lowering: a synchronous pool pops on normal exits;
an exception escaping that pool does not pop it. Native async pools drain before
actual suspension, reopen on resumption and clean up during exceptional exit.
A proven ready C++ await does not drain a pool. A monitor may contain a proven
non-suspending await, but an actual suspension while holding it is diagnosed,
including ordinary C++ task coroutines.

Synchronization evaluates its operand once. ARC holds a separate receiver claim
until after unlock; MRC requires an independently established lifetime. Body
defers and locals clean up before unlock, which precedes the ARC snapshot's
release. Nested snapshots protect matching loans after a source binding changes.
The private CFG representation uses existing scope, lifetime and cleanup nodes;
it adds no public AST layout or native service/frame ABI change.

Known MRC `autorelease` messages queue claims in the innermost active native
pool. Pool exit or async drain consumes these claims while preserving independent
retains and explicit pins. Conditional releases join conservatively, saturated
counts cannot cancel one another, and loop/discovery budgets fail closed. A
callback effect outside a proven active pool remains conservatively consuming.
Foreign family queues do not acquire fabricated native pool semantics. Borrowed
returns are checked after the cleanup sequence that can end their owners.

Published pool/synchronization bodies require
`objc3.managed-runtime-scopes.v1` and the matching canonical semantic identity.
Codec, artifact, module and import readers reject absent, invented or unsupported
claims. Replay reconstructs private cleanup registration from the source body;
synthetic analysis declarations do not leak into published declarations.
Physical extension ABI 24 remains unchanged.

Controls cover C17/C++20/23, ARC/MRC, source/PCH, normal and exceptional exits,
return/goto/break/continue, nested pins, queued and independent claims, callback
effects, pool drains, async exception paths and monitor suspension. Source-free
controls cover six profiles, relocation, exact capability admission and expired
consumers. The maintained optional `runtime-scopes` feature provides a full
24-configuration O0/O2 matrix. Its native witness interposes actual pool and lock
operations, checks exact operand evaluation and destruction, and verifies the
same exception object with an owner still live at unlock.

All 71 selected compiler cases, six source-free profiles and twelve
instrumented native configurations / 42 commands pass. Codec and isolation
controls, five adjacent frontend groups, all nineteen AST controls and four
existing async configurations / nineteen commands also pass.

Broader `@finally` and partial-construction cleanup, open callable and
global/interprocedural ownership precision, complete profile conformance and
the final matched SDK remain on the implementation roadmap.
