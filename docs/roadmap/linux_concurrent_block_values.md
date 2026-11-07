# Concurrent Block expressions and owned snapshots

Current execution checkpoint: the maintained native frontend replay, identity and
consumer controls pass in the ABI 24 development embedding. See the
[2.110 consolidation](linux_publication_and_byte_ranges.md). Source-free package
execution, wider matrices and installed SDK qualification remain pending.

Concurrency source 2.106 preserves concurrent capture proofs through conditional
Block values, immutable local alias graphs, comma and assignment results, GNU
conditional/statement expressions and null alternatives. The checker validates
every possible environment before diagnosing its captures. Side-effect-free
constant conditions select their actual branch; other conditions keep both
alternatives. Unknown, mutable and external storage cannot supply a proof.
Nested immutable Block captures use the same rules.

Completed expression nodes share their proof across diamonds; active nodes
reject cycles. A 256-node budget and 64-level depth bound cannot silently turn
an unknown environment into an empty capture set. Literal environments are
deduplicated before transfer diagnostics. The native lifetime walker also avoids
revisiting identical conditional declaration sources, fixing exponential work
before capture checking in a repeated shared-alias graph. Distinct origins keep
their dangling diagnostics; this does not change arbitrary alias analysis.

Explicit Block capture modes select new storage. Strong/default snapshots of
unsafe source bindings establish owned captures under ARC and MRC, while weak
snapshots still require Sendable referenced types. An unsafe source qualifier
cannot override a new explicit strong storage policy. Implicit unsafe and
explicit unowned captures retain their admission requirement; mutable referenced
types, borrowed provenance and cleanup/move restrictions remain enforced.

The focused selection passes four native compiler regressions in about three
seconds: the existing thirty capture source/PCH commands, eighteen new
C17/C++20/C++23 ARC/MRC source/PCH commands and six ordinary-mode lifetime
commands. Controls include safe/unsafe branches, unknown alternatives, nested
captures, constant and side-effecting conditions, shared diamonds, cycles,
budget exhaustion and explicit strong/weak/unowned modes.

The shared spawn runtime fixture adds thirty-two cases per configuration across
eight expression/capture forms, both selected environments and normal/pre-entry
cancelled completion. It verifies lazy execution, exact operand work, only the
selected object's survival, suspension, exactly-once defers and two final object
destructions. C17 MRC O0 source and C++23 ARC O2 PCH pass the complete existing
spawn suites plus these sixty-four new cases (eleven commands). Frontend
source-free publication/replay, relocation, changed-body and valid/invalid
consumer controls are authored and syntax-checked; execution and remaining
runtime/sanitizer/package/SDK consolidation remain pending. SDK 2.62 is unchanged;
no new public layout or extension ABI is introduced.
