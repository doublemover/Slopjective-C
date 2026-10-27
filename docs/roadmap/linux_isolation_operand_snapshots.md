# Owning operands before awaited isolation entry

Concurrency 2.37.0 evaluates isolated function arguments in source order and
message receivers before their arguments. Each retainable operand obtains an
independent object retain or native Block copy before the next operand runs,
under ARC and MRC. A later argument can clear or release its original owner
without invalidating the value passed into the lazy entry frame.

Isolation entry and key-path applications share the same precise snapshot
acquisition and normal/exception cleanup. MRC retains independently even when
the source convention returns +1; that original claim remains the caller's
responsibility. ARC preserves owned weak loads and already-retained productions.
A temporary destructor throwing during acquisition rolls back a completed
snapshot, and a throwing Block copy releases only completed operands.

The full-expression owns these snapshots through child frame creation and
result construction. The frame then holds its own independent captures.
Cancellation before target entry and frame allocation failure balance both
layers without executing the isolated target. Syntactic calls/messages remain
available for source printing and semantic interface transport.

Native codegen witnesses cover C17/C++20/C++23, ARC/MRC and PCH with retain/copy
before later-argument evaluation. Runtime witnesses cover receiver, class-method
argument, function argument and Block argument replacement, throwing later
arguments, cancellation before target entry and failed frame allocation. The
pre-fix MRC witness detects destruction before frame creation; the fixed ARC and
MRC source probes pass all sixteen scenarios each. Broader matrices remain
batched after implementation. SDK 2.33 remains the accepted archive.

This span does not admit MRC/CF consumed crossings, init-family entry, dynamic
Class qualification or automatic actor association. Runtime/frame ABI layouts
and full profile conformance claims are unchanged.
