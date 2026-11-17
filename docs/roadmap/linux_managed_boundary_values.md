# Managed values at concurrency boundaries

Source 2.187 / System 2.113 preserves storage-level cleanup contracts when a
scalar or aggregate is snapshotted for an isolation hop or concurrent task.
A loaded integer descriptor does not become ordinary Sendable data merely
because its resource annotation lives on the variable rather than a typedef.
This applies to `objc_resource`, `objc_cleanup` and native GNU cleanup storage.

A shared bounded value proof follows casts, readonly initialized aliases and
references, comma results, selected generic/choose expressions, conditional
alternatives, `take`, aggregate lists, copy/move construction and native reference
forwarders. It also checks readonly and init-captured values in concurrent Blocks
and C++ lambdas. The proof has a shared 128-node budget, depth 32 and cycle
rejection; exhausted proofs require explicit unsafe admission. Boolean tests and
ordinary computed telemetry remain ordinary data. Constant dead conditional
branches do not transfer their unused values. Mutable aliases, arbitrary helper
results and complete interprocedural provenance remain broader work.

Explicit `objc_unsafe_sendable` wrappers retain their required standalone
unchecked-transfer note. A promise on the callee's parameter cannot silently
launder an unannotated managed caller value. This admission does not itself
extend resource lifetime or supply reference counting; the programmer must retain
the owner or transfer it under the declared resource contract.

Nonisolated actor instance methods and synthesized property getters cannot
promise receiver-owned interior results. Validation runs after attribute and
property processing, so spelling order, categories and inherited actor identity
do not bypass it. An unsafe Sendable result typedef cannot waive this lifetime
rule. Independently owned/raw pointer results without an interior-view contract
retain ordinary signature checks. Strict Concurrency diagnoses an error;
compatibility mode diagnoses a warning.

These checks strengthen existing semantic admission without changing ABI,
interface schema or public layouts. They do not implement arbitrary raw-resource
property copying, effectful key paths, implicit managed union ownership or full
Strict Concurrency conformance. Matching SDK and native-host qualification remain
pending; accepted SDK 2.62 / ABI 21 is unchanged.

Focused evidence: 72 source/PCH commands across C17/C++20/C++23, ARC/MRC
and all four profiles; complete isolation contracts; four C++20/23 ARC/MRC
source-free native configurations with exact replay, provider removal and two
independent consumers. Real Linux pipe descriptors remain open on the destination
and resumed caller executors, then close exactly once. Imported negative controls
reject managed scalar laundering and nonisolated actor interior views. All 100 AST
controls, complete direct-import/source-bridge/codec suites and 26 tooling checks
pass (one optional skip). Maintained patches replay byte-for-byte. These focused
compiler-object links do not qualify a replacement SDK.
