# Checked isolated property accessors

The [source 2.123 follow-up](linux_async_property_accessors.md) also completes
async accessors and composition without an executor crossing.

Source 2.122 propagates synchronous accessor errors inside the private isolated
property frame. Each getter and setter gets its own checked call. A getter
failure exits before mutation or setter execution; a setter failure preserves
the getter/RHS result's ordinary cleanup and propagates the exact error object.
Simple reads/writes, compound operations and pre/post updates retain their native
result values. One throwing accessor can pair with a nonthrowing accessor. Two
throwing accessors must have the same typed or untyped error contract; incompatible
contracts reject before native lowering.

The enclosing frame carries the combined error effect. The caller must still use
an explicit checked form around its await. Native actor, executor, class and super
dispatch, receiver/RHS evaluation, nil results and cancellation keep the existing
property-entry policy. The source pseudo-object remains the printed contract;
synthesized accessor checks and thunk declarations do not escape into publication.

All 78 dedicated compiler commands pass across Core, Strict, Strict Concurrency
and Strict System, C17/C++20/23, ARC/MRC and source/PCH. Six new source-free profiles
pass replay, relocation, unsupported/missing capability and unchecked-consumer
controls, together with four adjacent frontend groups. The cumulative native
property fixture checks getter/setter failure, exact error identity and accessor
counts, compound sequence order, preserved values, class/super/actor dispatch,
nil, cancellation and C++ task callers. C++ setter arguments also verify balanced
nontrivial copies, moves and destruction on typed-error paths.

The span uses extension ABI 24, native frame 1, runtime services 4 and schema 2.2.
All twelve ASan/UBSan native configurations / 42 commands and codec/isolation
controls pass. Synchronous checked getters returning nontrivial C++ records, broader error
payloads and full matched SDK consolidation remain separate implementation work.
