# Mutable managed values at concurrency boundaries

Source 2.193 / System 2.119 preserves cleanup-managed value provenance through
bounded local initializers and writes. A descriptor or aggregate copied into an
unannotated mutable binding does not become ordinary Sendable data. Ordinary
parameters assigned managed values, conditional joins, loop-carried assignments,
clean assignment cycles, native C++ reference aliases and casted reference writes
share the same source union. Record members, local array elements and C++
structured bindings preserve their underlying managed storage provenance.

The value proof follows value-preserving expressions and written sources, with
256 expression visits and depth 32. One complete-body index collects direct
writes and exact local reference cells within 4,096 statement visits and depth 128. Unknown address/reference escapes, shared reference captures, assembly,
gotos and exhausted bounds require explicit unchecked admission for values that
lack native reference ownership. Native object/reference snapshots retain their
independent lifetime holds and type-level Sendable contracts, including when a
later argument clears or replaces the caller slot. Block environment uncertainty
keeps its existing target/capture diagnostics. Compiler-generated C++ copy/move
operations read the source binding, including nontrivial ARC record operations;
their native value and ownership contracts remain responsible for the new copy. Assignment
cycles contribute their reachable sources; they do not create a resource source
by themselves. Boolean conversions and ordinary computed telemetry retain their
value contracts. Constant-dead conditional, short-circuit and loop bodies do not
contribute unexecuted writes. Closure bodies write their copied environments,
while reference captures invalidate a closed outer-binding proof.

Blocks and C++ lambda values check the complete capture source graph rather than
only const/ref initializers. Native isolation frames also recheck their original
operand snapshots once the caller body is complete. This catches writes that
were unavailable when the frame was first lowered, while retaining the original
receiver/argument evaluation and ownership behavior. Same-domain async property
frames keep their ordinary lifetime contracts; the extra Sendable check applies
when the frame crosses an actor or executor boundary.

An explicit unsafe Sendable wrapper remains the source-level admission and emits
its unchecked-transfer note. A promise on the callee's parameter cannot grant
that promise to an unannotated caller copy. The native pipe witness copies a
resource through mutable locals, explicitly admits the transported value, checks
the descriptor on the destination and resumed caller executors, and requires
exactly one final close after the original owner exits. Provider removal and two
independent consumers retain that behavior through source-free publication.

All 216 source/PCH commands pass across C17/C++20/C++23, ARC/MRC and all four
profiles. Four source-free native descriptor configurations, four neighboring
Block ownership/cancellation configurations, the complete isolation and codec
suites, all 107 AST controls and 16 tooling checks pass. The compiler change
builds as C++17 with warnings treated as errors. Maintained patches replay
byte-for-byte and all manifest digests match.

This is a conservative local may-source proof, not a flow-sensitive lifetime or
whole-program alias analysis. Mutable globals/TLS, arbitrary indirect producers,
open reference aliases, broader resource property/key-path transfer and complete
profile enforcement retain their separate obligations. Public runtime layouts,
extension ABI 24 and interface schema 2.2 are unchanged. Matching SDK/archive and
native ARM qualification remain pending.
