# Awaited isolated property operations

Concurrency source 2.117 implements synchronous property expressions across actor
and executor boundaries. Getters, assignments, compound updates and pre/post
increment/decrement evaluate their caller operands once and execute the accessor
sequence in the required domain. Assignment and prefix/postfix results keep their
ordinary value semantics. Compound actor operations have no suspension between
getter and setter; a queued interloper cannot enter between them.

The lowering preserves custom accessors, class properties, exact instance/class
`super` selection, same-actor access and compatible actor backing-executor
requirements. It reconstructs super operations with the already selected accessor
rather than repeating lookup against the most-derived receiver. Conflicting
accessor executor requirements diagnose. Public syntax retains its parentheses
through emission/replay, including assignments beneath `await` and dependent
C++ property helpers.

Operand snapshots are independent of later effects. Plain record snapshots
survive receiver mutation, while ARC record arguments preserve their owned fields
when a later argument clears the source. Snapshot cleanup uses the native typed
destructor on normal and exceptional exit. Nontrivial C++ call arguments keep
their existing materialized typed snapshots.

The CFG now matches CodeGen's lazy evaluation of unique opaque values. This
preserves property receiver/RHS ordering inside native frames as well as during
isolated lowering. An independent four-profile CFG unit checks call order against
ordinary property semantics. GNUstep subclass ivar metadata also encodes signed
relative offsets when superclass tail padding is reused; oversized offsets
produce a diagnostic rather than an assertion. Ordinary Objective-C/Objective-C++
ARC/MRC controls and the native inherited layout exercise that repair.

Nil operations skip actor/executor lookup, including unregistered destinations.
Reads retain the typed nil policy from Part 7 §7.2.4. Assignments can return their
already valid RHS; constructor-only results are not fabricated. Lazy disposal
and cancellation before parent entry, child entry and accessor execution preserve
argument ownership and skip all accessor work. Both native frames and C++ task
callers use this path.

All thirty focused compiler commands pass, including source/PCH transport,
canonical printing, invalid isolation combinations and signed-offset controls.
All seventeen AST infrastructure tests and six source-free frontend profiles
pass. The complete selected compiler consolidation passes 120 of 123 cases;
three platform cases remain unsupported. Three installed-helper import checks
pass. Twelve expanded ASan/UBSan property configurations pass 42 compile/PCH/
link/execution commands, and all twelve existing nil-result configurations pass
43 commands. These are development-source checks, not matched SDK delivery.

The maintained `isolated_properties` fixture includes independent ordering
oracles, record/ARC snapshots, interloper ordering, custom/class/super accessors,
backing affinity, same-actor calls, exact object destruction, typed nil outcomes
and cancellation. Optional `--feature properties` maintains the complete 24-case
O0/O2 source/PCH matrix and installs with the SDK.

Synchronous accessors must satisfy the existing native frame value/ownership
contracts and the nonthrowing C++ exception boundary. Property writes currently
admit qualified plain values and ObjC references; resource and nontrivial C++
write values retain their explicit diagnostic boundary. Async/checked-effect
accessors and broader ownership/Sendable/profile conformance remain open.
Extension ABI 24, frame ABI 1 and service ABI 4 are unchanged. Source-free
interfaces require `objc3.async-entry-abi.v4`; accepted SDK 2.62 remains historical
while the replacement SDK follows implementation and consolidated verification.
