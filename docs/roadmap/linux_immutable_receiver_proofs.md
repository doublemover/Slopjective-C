# Immutable receiver and capture return proofs

Source 2.170 / System 2.96 extends bounded permanent-result analysis to values
loaded from actual immutable C++ objects and closures. The reader cutover is
`objc3.borrowed-result-proofs.v10`; runtime layouts and extension ABI 24 stay
unchanged. A fresh SDK containing this source is still required for acceptance.

A complete object must be const, constant-initialized, nonvolatile and neither
thread-local nor replaceable through weak/alias linkage. The proof reads its
native constant initializer and follows nonvirtual base, record, fixed-array and
active-union paths. Direct local const objects qualify for their copied values,
without granting permanent lifetime to the local object. A const reference to a
mutable complete object does not qualify. Mutable, volatile and reference fields
remain outside admission.

Known synchronous member/operator calls bind their implicit receiver alongside
explicit parameters. Nested calls, const-record reference forwarding and bounded
receiver alternatives retain the caller's facts. Value captures resolve through
the actual closure field, including init-captures, nested closures, captured
records and pointers to immutable records. The original captured variable's
initializer cannot substitute for an unproved closure field. By-reference
captures, unknown inputs and replaceable/mutable closures retain rejection.
Pointer values must independently prove permanent pointees; function/member
pointer fields must independently select complete admissible target graphs.
Nonconstexpr observer calls in the controls force the return-graph proof rather
than allowing whole-call constant folding to satisfy the test.

Source-free packages also admit externally visible inline namespace closure
objects initialized directly by their lambda expression. Their unnamed native
types bind to the stable variable owner, so aliases and reference results retain
exact identity without source-path coordinates. Distinct closure owners stay
distinct even when their bodies and captured values match. The
`objc3.cxx-inline-closure-objects.v1` reader contract gates this publication.
Initializer dependencies retain export and SPI checks. AST import completes the
closure definition before importing its variable mangling context, preventing
recursive initialization from observing a capture-less lambda class. Non-inline
closure objects, arbitrary closure-producing initialization and broader unnamed
contexts retain separate admission.

Native base-conversion paths now contribute their ordered canonical base types
and virtual-base bits to published expression identity. Selecting the left or
right occurrence of the same repeated base field changes the fingerprint.
`objc3.cxx-base-adjustment-expressions.v1` is required when such expressions are
published; absent/older capabilities and namespace-forged markers are rejected.
This preserves native adjustment semantics without relying on qualifier spelling.

The analysis shares the existing 4096-unit work budget, 32-level recursion bound
and 64-value/target limit. Unknown receiver construction, returned record values,
heap state, arbitrary mutation effects, virtual-base receiver facts, reference
results into receiver fields and general interprocedural object-state analysis
remain separate work. Lazy async/coroutine bodies retain their own lifetime
protocol.

Maintained controls cover direct/PCH C17 and C++20/23 ARC/MRC, source destruction
before AST print/reparse, source-free publication and two native consumers.
Native execution observes getter/capture side effects and selects repeated base
subobjects after removing the provider source. Matching SDK, vendor/profile,
instrumentation and host consolidation remain separate gates.

The focused checkpoint passes all 18 expanded source/PCH commands and all 86
maintained LLVM AST infrastructure tests. Six C17/C++20/23 ARC/MRC publication
profiles pass canonical replay, relocation, changed-base and closure-owner
controls; four source-free native storage configurations pass both consumers,
including explicit static-destruction policy. Decoder controls, adjacent class
dispatch/template-deduction cases and 25 related tooling checks pass. Maintained
patches reproduce the changed compiler/control files exactly and all manifest
digests agree. These checks relink changed LLVM objects against the existing
SDK libraries; they do not qualify a freshly sealed SDK archive.
