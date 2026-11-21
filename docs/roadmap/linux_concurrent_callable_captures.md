# Concurrent callable capture contracts

Current execution checkpoint: the maintained native frontend replay, identity and
consumer controls pass in the ABI 24 development embedding. See the
[2.110 consolidation](linux_publication_and_byte_ranges.md). Source-free package
execution, wider matrices and installed SDK qualification remain pending.

Concurrency source 2.104 applies Part 7 §§7.8.3–7.8.5 to C++ callable storage and
immutable local Block aliases. C++ lambdas preserve the distinction between a
shared `this` pointer and an independent `[*this]` value. Shared `this`, explicit
unsafe object captures and cleanup-managed scalar captures require explicit
unsafe admission; transitively borrowed captures remain forbidden. Reference
fields remain non-Sendable. Weak captures check the referenced type. Nested
closures, copies, init-captures and mutable/generic call operators use the same
classification. Copying a cleanup-managed scalar into an init-capture retains
its source restriction.

ARC ownership qualifiers and MRC's inert unsafe qualifier both participate in
checking. Explicit unsafe admission emits the required transfer note. Blocks
capturing C++ `this` apply that same admission rule to the actual pointee type.
Independent object values and scalar captures retain ordinary value checking.

Local const Block alias chains retain their original literal's capture proof,
including nested aliases captured by another Block. Mutable aliases, function
parameters and external storage cannot supply that proof. Recursive alias walks
are bounded by their visited declarations.

Two new compiler regressions pass thirty source/PCH commands across C17 and
C++20/23, ARC/MRC. The existing unsafe-transfer-note regression also passes.
Controls distinguish safe, weak, unsafe, cleanup-managed, nested, reference,
shared-this and copied-this captures, plus known and unknown Block aliases.
Frontend replay/relocation and positive/negative consumer controls are authored;
frontend execution and final package/SDK consolidation remain pending. These
checks do not claim complete Strict Concurrency or arbitrary dynamic capture
provenance.

The [Concurrency 2.106 span](linux_concurrent_block_values.md) extends these
proofs to conditional/nested Block expressions and bounded shared alias graphs.
It also restores explicit owned capture storage when the source binding is unsafe;
implicit unsafe captures retain their restriction. Focused native compiler and
exact-lifetime runtime controls pass.
