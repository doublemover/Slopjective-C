# Objective‑C 3.0 — Lowering, ABI, and Runtime Contracts {#c}

_Working draft v0.11 — last updated 2026-02-23_

## C.0 Scope and audience {#c-0}

This document is primarily **normative for implementations** (compiler + runtime + standard library).
It does not change source-level semantics defined elsewhere; it constrains how a conforming implementation supports those semantics under **separate compilation** and across **module boundaries**.

Where this document provides “canonical lowering” patterns, those are:

- **required** when explicitly labeled _normative_; and
- otherwise **recommended** as the default lowering for LLVM/Clang implementations.

### C.0.1 Status tags used in this document {#c-0-1}

Status labels in this document follow [Part 0](#part-0) [§0.3.2](#part-0-3-2):

- **normative** / **minimum** / **normative intent**: contributes to conformance requirements,
- **recommended**: QoI guidance,
- **informative** / **non-normative**: explanatory material.

## C.1 Definitions (informative) {#c-1}

- **ABI**: the binary calling convention and data layout visible to linkers and other languages.
- **Runtime hooks**: functions/types provided by a support library needed to implement language semantics (e.g., executors, task scheduling).
- **Effect**: a property of a callable type that changes call/return behavior (`throws`, `async`).

## C.2 Separate compilation requirements (normative) {#c-2}

A conforming implementation shall ensure:

1. **Effect and isolation consistency is recorded in module metadata.**  
   If a declaration is imported from a module, its:
   - `async`/`throws` effects,
   - executor affinity (`objc_executor(...)`),
   - actor isolation (actor type + isolated/nonisolated members), and
   - any dispatch-affecting attributes (`objc_direct`, `objc_direct_members`, `objc_dynamic`, `objc_sealed`, etc.)
     shall be part of the imported type information.

   The minimum required set is enumerated normatively in **[MODULE_METADATA_AND_ABI_TABLES.md](#d)**.

2. **Mismatched redeclarations are diagnosed.**  
   Redeclaring an imported function/method with a different effect set or incompatible lowering-affecting attributes is ill-formed.

3. **Interface emission is semantics-preserving.**  
   If a textual module interface is emitted, it shall preserve effects and attributes (see [B.7](#b-7)).

## C.2.1 Required module metadata (normative) {#c-2-1}

A conforming implementation shall preserve in module metadata and interface emission the items listed in **[D.3.1](#d-3-1) [Table A](#d-3-1)**.

This requirement is intentionally testable: if importing a module can change whether a call requires `try`/`await`, or can change dispatch legality (`objc_direct` / `objc_direct_members` / `objc_dynamic`), the implementation is non-conforming.

## C.3 Canonical lowering patterns (mixed; see subsection tags) {#c-3}

### C.3.0 Lowering pass-graph scaffold (normative for implementations) {#c-3-0}

Implementations shall maintain a deterministic lowering pass graph from
frontend stage handoff (`lex -> parse -> sema`) through lowering-boundary
normalization and direct IR-emission entrypoint enablement (`lower -> emit`).

At minimum, the pass-graph gate shall validate:

- lexer/parser/semantic diagnostic surfaces are fail-closed clean before emit;
- lowering boundary normalization and runtime dispatch declaration synthesis are
  replay-key stable for the active lowering options; and
- the direct IR emission route remains enabled as the first-class path (no
  implicit fail-open bypass).
- a core-feature readiness surface is emitted with deterministic replay keys
  that can be carried into IR metadata closeout evidence.
- an expansion readiness surface is emitted with deterministic expansion keys,
  and implementations shall fail closed before IR emission when expansion
  accounting/replay anchors drift.
- ownership-aware lowering contracts (ownership qualifiers, retain/release,
  autoreleasepool scope, ARC diagnostics/fixits) remain replay-key stable and
  fail closed when ownership-lowering invariants drift.
- ownership-aware lowering modular split scaffolding shall validate ownership
  qualifier, retain/release, autoreleasepool, and ARC diagnostics lane-contract
  replay keys before direct IR emission proceeds.
- ownership-aware lowering core feature implementation shall remain fail-closed
  for ownership qualifier, retain/release, autoreleasepool, and ARC
  diagnostics/fixit lowering contracts so milestone optimization improvements
  cannot bypass direct LLVM IR emission hardening.
- ownership-aware lowering core feature expansion shall remain fail-closed for
  weak/unowned semantics integration, ownership profile accounting consistency,
  and deterministic expansion replay keys before direct LLVM IR emission.
- direct IR emission completeness for ObjC lowering patterns shall remain
  metadata-stable and fail closed on pass-graph or lowering-boundary drift.
- IR-emission completeness modular split scaffolding shall remain deterministic
  and fail closed while transporting pass-graph core/expansion/edge
  compatibility replay keys into IR metadata evidence surfaces.
- IR-emission core-feature implementation shall remain deterministic and fail
  closed when modular split transport, runtime boundary handoff, or direct IR
  entrypoint readiness drifts.
- object emission and link-path routing (clang/llvm-direct) shall remain
  deterministic and fail closed when backend route selection or object artifact
  generation drifts.
- toolchain/runtime modular split scaffolding shall synthesize deterministic
  backend selection/capability and IR/object compile-route readiness keys
  before backend object dispatch.
- toolchain/runtime core feature implementation shall remain fail-closed on
  backend-output marker path/payload drift and core-feature readiness drift
  after backend object dispatch.
- runtime-facing type metadata semantics governance shall preserve
  deterministic sema parity handoff, runtime-shim default dispatch symbol
  (`objc3_msgsend_i32`), and fail-closed pipeline/artifact metadata projection
  continuity (`M227-D001`).
- runtime-facing type metadata modular split/scaffolding governance shall preserve explicit
  lane-D dependency anchors (`M227-D001`) and fail closed on sema handoff scaffold/pass-flow scaffold
  or runtime-facing metadata projection drift before runtime-facing core-feature validation advances.
- runtime-facing type metadata edge-case expansion and robustness governance shall preserve explicit lane-D dependency anchors (`M227-D006`, `M227-D005`) and fail closed on
  runtime-facing type metadata expansion/robustness consistency, readiness, or robustness-key continuity drift before diagnostics-hardening validation advances.
- runtime-facing type metadata diagnostics hardening governance shall preserve explicit lane-D dependency anchors (`M227-D007`, `M227-D006`) and fail closed on
  runtime-facing type metadata diagnostics-hardening consistency, readiness, key, or alignment continuity drift before recovery/determinism validation advances.
- runtime-facing type metadata recovery/determinism hardening governance shall preserve explicit lane-D dependency anchors (`M227-D008`, `M227-D007`) and fail closed on
  runtime-facing type metadata recovery/determinism consistency, readiness, key, or alignment continuity drift before conformance-matrix validation advances.
- runtime-facing type metadata conformance matrix implementation governance shall preserve explicit lane-D dependency anchors (`M227-D009`, `M227-D008`) and fail closed on
  runtime-facing type metadata conformance matrix consistency, readiness, key, or alignment continuity drift before conformance-corpus validation advances.
- runtime-facing type metadata conformance corpus expansion governance shall preserve explicit lane-D dependency anchors (`M227-D010`, `M227-D009`) and fail closed on
  runtime-facing type metadata conformance corpus consistency, readiness, key, or alignment continuity drift before performance/quality validation advances.
- runtime-facing type metadata performance and quality guardrails governance shall preserve explicit lane-D dependency anchors (`M227-D011`, `M227-D010`) and fail closed on
  runtime-facing type metadata performance/quality consistency, readiness, key, or alignment continuity drift before integration-closeout validation advances.
- runtime-facing type metadata integration closeout and gate sign-off governance shall preserve explicit lane-D dependency anchors (`M227-D012`, `M227-D011`) and fail closed on
  runtime-facing type metadata integration closeout/sign-off consistency, readiness, key, or alignment continuity drift before lane-D readiness sign-off advances.
- toolchain/runtime core feature expansion shall remain fail-closed on backend
  marker-path determinism and backend marker payload-to-route consistency drift
  before core-feature implementation readiness can pass.
- edge-case compatibility completion shall include deterministic compatibility
  handoff and language-version/pragma coordinate ordering gates that fail
  closed before IR emission.
- edge-case expansion and robustness shall include deterministic expansion
  consistency and robustness readiness/key gates that fail closed before IR
  emission.
- diagnostics hardening shall include deterministic diagnostics consistency and
  diagnostics hardening readiness/key gates that fail closed before IR
  emission.
- recovery and determinism hardening shall include deterministic recovery
  consistency and recovery/determinism readiness/key gates that fail closed
  before IR emission.
- conformance matrix implementation shall include deterministic conformance
  consistency and conformance-matrix readiness/key gates that fail closed
  before IR emission.
- conformance corpus expansion shall include deterministic conformance-corpus
  consistency and conformance-corpus readiness/key gates that fail closed
  before IR emission.
- performance and quality guardrails shall include deterministic
  performance-quality consistency and performance-quality readiness/key gates
  that fail closed before IR emission.
- cross-lane integration sync shall preserve deterministic lane dependency
  anchors (`A011`, `B007`, `C005`, `D006`, `E006`) and fail closed when
  contract continuity drifts.
- docs and operator runbook synchronization shall preserve deterministic lane-A
  documentation/runbook anchors (`A011`, `B007`, `C005`, `D006`, `E006`,
  `A012`) and fail closed when operator command sequencing or evidence-path
  continuity drifts.
- release-candidate and replay dry-run wiring shall preserve deterministic
  lane-A release-replay anchors (`A013`), wrapper compile replay artifact
  continuity, and lowering replay-proof command sequencing so readiness fails
  closed when replay evidence drifts.
- advanced core workpack (shard 1) wiring shall preserve deterministic
  toolchain/runtime GA advanced-core consistency/readiness and
  advanced-core-key continuity (`A014`) so lane-A advanced shard closure fails
  closed when advanced-core evidence drifts.
- integration closeout and gate sign-off wiring shall preserve deterministic
  toolchain/runtime GA integration-closeout sign-off
  consistency/readiness/key continuity (`A015`) so lane-A closeout fails
  closed when integration sign-off evidence drifts.
- IR-emission core-feature expansion shall remain deterministic, preserve
  expansion readiness/key continuity, and fail closed when pass-graph
  expansion continuity or expansion metadata transport drifts.
- IR-emission edge-case compatibility completion shall remain deterministic,
  preserve compatibility consistency/readiness and compatibility-key
  continuity, and fail closed when pass-graph compatibility evidence or
  metadata transport continuity drifts.
- IR-emission recovery and determinism hardening shall remain deterministic,
  preserve recovery consistency/readiness and recovery-determinism-key
  continuity, and fail closed when pass-graph or parse-artifact
  recovery-determinism evidence drifts.
- IR-emission conformance matrix implementation shall remain deterministic,
  preserve conformance consistency/readiness and conformance-matrix-key
  continuity, and fail closed when pass-graph or parse-artifact
  conformance-matrix evidence drifts.
- IR-emission conformance corpus expansion shall remain deterministic,
  preserve conformance-corpus consistency/readiness and
  conformance-corpus-key continuity, and fail closed when pass-graph or
  parse-artifact conformance-corpus evidence drifts.
- IR-emission performance and quality guardrails shall remain deterministic,
  preserve performance/quality consistency/readiness and
  performance-quality-key continuity, and fail closed when pass-graph or
  parse-artifact performance-quality evidence drifts.
- IR-emission cross-lane integration sync shall remain deterministic,
  preserve cross-lane integration consistency/readiness and
  cross-lane-integration-key continuity, and fail closed when pass-graph or
  parse-artifact cross-lane integration evidence drifts.
- IR-emission docs and operator runbook synchronization governance shall
  preserve explicit lane-C dependency anchors (`M228-C013`, `M228-C012`) and
  fail closed on docs/runbook command sequencing, evidence path continuity, or
  dependency-anchor drift before release-candidate dry-run gates advance.
- IR-emission release-candidate and replay dry-run governance shall preserve
  explicit lane-C dependency anchors (`M228-C014`, `M228-C013`) and fail
  closed on release/replay command sequencing, evidence path continuity, or
  dependency-anchor drift before advanced workpack gates advance.
- IR-emission advanced core workpack (shard 1) governance shall preserve
  explicit lane-C dependency anchors (`M228-C015`, `M228-C014`) and fail
  closed on advanced-core shard1 consistency, key-transport continuity, or
  dependency-anchor drift before advanced edge-compatibility gates advance.
- IR-emission advanced edge compatibility workpack (shard 1) governance shall
  preserve explicit lane-C dependency anchors (`M228-C016`, `M228-C015`) and
  fail closed on advanced-edge-compatibility shard1 consistency,
  key-transport continuity, or dependency-anchor drift before advanced
  diagnostics gates advance.
- IR-emission advanced diagnostics workpack (shard 1) governance shall
  preserve explicit lane-C dependency anchors (`M228-C017`, `M228-C016`) and
  fail closed on advanced-diagnostics shard1 consistency, key-transport
  continuity, or dependency-anchor drift before advanced conformance gates
  advance.
- IR-emission advanced conformance workpack (shard 1) governance shall
  preserve explicit lane-C dependency anchors (`M228-C018`, `M228-C017`) and
  fail closed on advanced-conformance shard1 consistency, key-transport
  continuity, or dependency-anchor drift before advanced integration gates
  advance.
- IR-emission advanced integration workpack (shard 1) governance shall
  preserve explicit lane-C dependency anchors (`M228-C019`, `M228-C018`) and
  fail closed on advanced-integration shard1 consistency, key-transport
  continuity, or dependency-anchor drift before advanced performance gates
  advance.
- ownership-aware lowering edge-case and compatibility completion shall include
  deterministic compatibility consistency/readiness and compatibility-key
  transport gates that fail closed before IR emission.
- ownership-aware lowering edge-case expansion and robustness shall include
  deterministic robustness expansion/readiness and robustness-key transport
  gates that fail closed before IR emission.
- ownership-aware lowering diagnostics hardening shall include deterministic
  diagnostics consistency/readiness and diagnostics-key transport gates that
  fail closed before IR emission.
- ownership-aware lowering recovery and determinism hardening shall include
  deterministic recovery consistency/readiness and recovery-key transport gates
  that fail closed before IR emission.
- ownership-aware lowering conformance matrix implementation shall include
  deterministic conformance consistency/readiness and conformance-matrix-key
  transport gates that fail closed before IR emission.
- ownership-aware lowering conformance corpus expansion shall include
  deterministic conformance-corpus consistency/readiness and
  conformance-corpus-key transport gates that fail closed before IR emission.
- ownership-aware lowering performance and quality guardrails shall include
  deterministic performance/quality consistency/readiness and
  performance-quality-key transport gates derived from parse-lowering
  performance/quality case accounting
  (`parse_lowering_performance_quality_guardrails_*`) and shall fail closed
  before IR emission when guardrail consistency, readiness, key continuity, or
  case-pass continuity drifts.
- ownership-aware lowering cross-lane integration sync shall include
  deterministic lane-B/lane-A integration consistency/readiness and
  cross-lane-integration-key transport gates derived from ownership-aware
  performance/quality guardrails and lowering pass-graph conformance/performance
  continuity, and shall fail closed before IR emission when integration
  continuity or key continuity drifts.
- ownership-aware lowering docs and operator runbook synchronization governance
  shall preserve explicit lane-B dependency anchors (`M228-B013`, `M228-B012`)
  and fail closed on docs/runbook synchronization continuity, command
  sequencing, or evidence-path drift before lane-B release validation advances.
- ownership-aware lowering release-candidate and replay dry-run governance
  shall preserve explicit lane-B dependency anchors (`M228-B014`, `M228-B013`)
  and fail closed on release/replay continuity, command sequencing, or
  evidence-path drift before lane-B advanced-core validation advances.
- ownership-aware lowering advanced core workpack (shard 1) governance shall
  preserve explicit lane-B dependency anchors (`M228-B015`, `M228-B014`) and
  fail closed on advanced-core-shard1 continuity, command sequencing, or
  evidence-path drift before lane-B advanced edge-compatibility validation
  advances.
- ownership-aware lowering advanced edge compatibility workpack (shard 1)
  governance shall preserve explicit lane-B dependency anchors (`M228-B016`,
  `M228-B015`) and fail closed on advanced-edge-compatibility-shard1
  continuity, command sequencing, or evidence-path drift before lane-B
  advanced diagnostics validation advances.
- ownership-aware lowering advanced diagnostics workpack (shard 1) governance
  shall preserve explicit lane-B dependency anchors (`M228-B017`, `M228-B016`)
  and fail closed on advanced-diagnostics-shard1 continuity, command
  sequencing, or evidence-path drift before lane-B advanced conformance
  validation advances.
- ownership-aware lowering advanced conformance workpack (shard 1) governance
  shall preserve explicit lane-B dependency anchors (`M228-B018`, `M228-B017`)
  and fail closed on advanced-conformance-shard1 continuity, command
  sequencing, or evidence-path drift before lane-B advanced shard-2
  validation advances.
- ownership-aware lowering advanced integration workpack (shard 1) governance
  shall preserve explicit lane-B dependency anchors (`M228-B019`, `M228-B018`)
  and fail closed on advanced-integration-shard1 continuity, command
  sequencing, or evidence-path drift before lane-B advanced performance
  validation advances.
- ownership-aware lowering advanced performance workpack (shard 1) governance
  shall preserve explicit lane-B dependency anchors (`M228-B020`, `M228-B019`)
  and fail closed on advanced-performance-shard1 continuity, command
  sequencing, or evidence-path drift before lane-B advanced shard-2
  validation advances.
- ownership-aware lowering advanced core workpack (shard 2) governance shall
  preserve explicit lane-B dependency anchors (`M228-B021`, `M228-B020`) and
  fail closed on advanced-core-shard2 continuity, command sequencing, or
  evidence-path drift before lane-B integration closeout validation advances.
- ownership-aware lowering integration closeout and gate sign-off governance
  shall preserve explicit lane-B dependency anchors (`M228-B022`, `M228-B021`)
  and fail closed on integration-closeout-signoff continuity, command
  sequencing, or evidence-path drift before lane-B closeout readiness advances.
- toolchain/runtime edge-case compatibility completion shall remain
  deterministic, preserve compatibility consistency/readiness and
  compatibility-key continuity, and fail closed when backend route/output
  compatibility evidence drifts.
- toolchain/runtime edge-case expansion and robustness shall remain
  deterministic, preserve robustness consistency/readiness and robustness-key
  continuity, and fail closed when backend route/output robustness evidence
  drifts.
- toolchain/runtime recovery and determinism hardening shall remain
  deterministic, preserve recovery consistency/readiness and
  recovery-determinism-key continuity, and fail closed when backend route/output
  recovery-determinism evidence drifts.
- toolchain/runtime conformance matrix implementation shall remain
  deterministic, preserve conformance consistency/readiness and
  conformance-matrix-key continuity, and fail closed when backend route/output
  conformance-matrix evidence drifts.
- toolchain/runtime conformance corpus expansion shall remain deterministic,
  preserve conformance-corpus consistency/readiness and
  conformance-corpus-key continuity, and fail closed when backend route/output
  conformance-corpus evidence drifts.
- toolchain/runtime performance and quality guardrails shall remain
  deterministic, preserve performance/quality consistency/readiness and
  performance-quality-key continuity, and fail closed when backend route/output
  quality guardrail evidence drifts.
- replay-proof/performance closeout gate wiring shall preserve explicit lane-E
  dependency anchors (`M228-A001`, `M228-B001`, `M228-C002`, `M228-D001`) and
  fail closed when dependency references or closeout evidence commands drift.
- replay-proof/performance modular split and scaffolding closeout gate wiring
  shall preserve explicit lane-E dependency anchors (`M228-E001`, `M228-A002`,
  `M228-B002`, `M228-C004`, `M228-D002`) and fail closed when dependency
  references or closeout evidence commands drift.
- replay-proof/performance core-feature implementation closeout gate wiring
  shall preserve explicit lane-E dependency anchors (`M228-E002`, `M228-A003`,
  `M228-B003`, `M228-C003`, `M228-D003`) and fail closed when dependency
  references or closeout evidence commands drift.
- replay-proof/performance core-feature expansion closeout gate wiring shall
  preserve explicit lane-E dependency anchors (`M228-E003`, `M228-A003`, `M228-B004`,
  `M228-C003`, `M228-D003`, and pending token `M228-C008`) and
  fail closed when dependency references, readiness chaining, or closeout
  evidence commands drift.
- replay-proof/performance edge-case compatibility completion closeout wiring
  shall preserve explicit lane-E dependency anchors (`M228-E004`, `M228-A004`,
  `M228-B006`, `M228-C004`, `M228-D005`, and pending token `M228-C010`) and
  fail closed when dependency references, readiness chaining, pending-token
  continuity, or closeout evidence commands drift.
- replay-proof/performance lane-E edge-case expansion and robustness closeout
  wiring shall preserve explicit lane-E dependency anchors (`M228-E005`,
  `M228-A006`, `M228-B006`, `M228-C006`, `M228-D006`) and fail closed when
  dependency references, readiness chaining, or closeout evidence commands
  drift.
- replay-proof/performance diagnostics hardening closeout wiring shall preserve
  explicit lane-E dependency anchors (`M228-E006`, `M228-A007`, `M228-B007`,
  `M228-D007`, and pending token `M228-C007`) and fail closed when dependency
  references, readiness chaining, pending-token continuity, or closeout
  evidence commands drift.
- replay-proof/performance recovery and determinism hardening closeout wiring
  shall preserve explicit lane-E dependency anchors (`M228-E007`, `M228-A008`,
  `M228-B008`, `M228-D008`, and pending token `M228-C008`) and fail closed when
  dependency references, readiness chaining, pending-token continuity, or
  closeout evidence commands drift.
- replay-proof/performance conformance matrix implementation closeout wiring
  shall preserve explicit lane-E dependency anchors (`M228-E008`, `M228-A009`,
  `M228-B009`, `M228-C008`, `M228-D009`, and pending tokens `M228-A007`,
  `M228-B010`, `M228-C017`, `M228-D007`) and fail closed when dependency
  references, readiness chaining, pending-token continuity, or closeout
  evidence commands drift.
- lowering/codegen cost profiling and controls contract-freeze wiring shall preserve explicit lane-C dependency anchors (`none`) and fail closed when dependency
  references, compile-route proof hooks, or perf-budget evidence commands drift.
- lowering/codegen modular split/scaffolding wiring shall preserve explicit lane-C dependency anchor (`M247-C001`) and fail closed when dependency references,
  readiness chaining, or modular split/scaffolding evidence commands drift.
- lowering/codegen core feature implementation wiring shall preserve explicit lane-C dependency anchor (`M247-C002`) and fail closed when dependency references,
  readiness chaining, or core feature implementation evidence commands drift.
- lowering/codegen core feature expansion wiring shall preserve explicit lane-C dependency anchor (`M247-C003`) and fail closed when dependency references,
  readiness chaining, or core feature expansion evidence commands drift.
- lowering/codegen edge-case and compatibility completion wiring shall preserve explicit lane-C dependency anchor (`M247-C004`) and fail closed when dependency references,
  readiness chaining, or edge-case compatibility evidence commands drift.
- lowering/codegen edge-case expansion and robustness wiring shall preserve explicit lane-C dependency anchor (`M247-C005`) and fail closed when dependency references,
  readiness chaining, or edge-case robustness evidence commands drift.
- lowering/codegen diagnostics hardening wiring shall preserve explicit lane-C dependency anchor (`M247-C006`) and fail closed when dependency references,
  readiness chaining, or edge-case robustness evidence commands drift.
- lowering/codegen recovery and determinism hardening wiring shall preserve explicit lane-C dependency anchor (`M247-C007`) and fail closed when dependency references,
  readiness chaining, or recovery/determinism evidence commands drift.
- lowering/codegen conformance matrix implementation wiring shall preserve explicit lane-C dependency anchor (`M247-C008`) and fail closed when dependency references,
  readiness chaining, or conformance-matrix continuity evidence commands drift.
- performance SLO gate/reporting wiring shall preserve explicit lane-E
  dependency anchors (`M247-A001`, `M247-B001`, `M247-C001`, `M247-D001`) and
  fail closed when dependency references, compile-route proof hooks, or
  perf-budget evidence commands drift.
- performance SLO modular split/scaffolding wiring shall preserve explicit
  lane-E dependency anchors (`M247-E001`, `M247-A002`, `M247-B002`, `M247-C002`,
  `M247-D002`) and fail closed when dependency references or modular split
  evidence commands drift.
- performance SLO core feature implementation wiring shall preserve explicit
  lane-E dependency anchors (`M247-E002`, `M247-A003`, `M247-B003`, `M247-C003`,
  `M247-D002`) and fail closed when dependency references, readiness chaining,
  pending-token continuity, or core implementation evidence commands drift.
- performance SLO edge-case expansion and robustness wiring shall preserve explicit
  lane-E dependency anchors (`M247-E005`, `M247-A006`, `M247-B007`, `M247-C006`,
  `M247-D005`) and fail closed when dependency references, readiness chaining,
  pending-token continuity, or edge-case robustness evidence commands drift.
- performance SLO diagnostics hardening wiring shall preserve explicit
  lane-E dependency anchors (`M247-E006`, `M247-A007`, `M247-B007`, `M247-C007`,
  `M247-D007`) and fail closed when dependency references, readiness chaining,
  pending-token continuity, or diagnostics hardening evidence commands drift.
- semantic hot-path analysis/budgeting recovery and determinism hardening wiring
  shall preserve explicit lane-B dependency anchor (`M247-B007`) and fail closed
  when dependency references, pending-token continuity, or contract-gating
  evidence commands drift.
- semantic hot-path analysis/budgeting conformance matrix implementation wiring
  shall preserve explicit lane-B dependency anchor (`M247-B008`) and fail closed
  when dependency references, conformance-matrix continuity, or contract-gating
  evidence commands drift.
- semantic hot-path analysis/budgeting conformance corpus expansion wiring
  shall preserve explicit lane-B dependency anchor (`M247-B009`) and fail closed
  when dependency references, conformance-corpus continuity, or contract-gating
  evidence commands drift.
- semantic hot-path analysis/budgeting cross-lane integration sync wiring
  shall preserve explicit lane-B dependency anchor (`M247-B011`) and fail closed
  when dependency references, cross-lane synchronization continuity, or
  contract-gating evidence commands drift.
- semantic hot-path analysis/budgeting docs and operator runbook synchronization wiring
  shall preserve explicit lane-B dependency anchor (`M247-B012`) and fail closed when dependency references, docs-runbook-synchronization consistency/readiness, docs-runbook-synchronization-key continuity, or contract-gating evidence commands drift.
- semantic hot-path analysis/budgeting release-candidate and replay dry-run wiring
  shall preserve explicit lane-B dependency anchor (`M247-B013`) and fail closed when dependency references, release-candidate/replay command sequencing continuity, release-candidate-replay-key continuity, or contract-gating evidence commands drift.
- semantic hot-path analysis/budgeting advanced core workpack (shard 1) wiring
  shall preserve explicit lane-B dependency anchor (`M247-B014`) and fail closed when dependency references, advanced-core-workpack command sequencing continuity, advanced-core-workpack-shard-1-key continuity, or contract-gating evidence commands drift.
- frontend profiling and hot-path decomposition contract and architecture freeze wiring shall preserve explicit lane-A dependency anchor (`none`) and fail closed when dependency references,
  deterministic lane-A parser/AST profiling and hot-path decomposition anchors and fail closed on contract-freeze drift.
- frontend profiling and hot-path decomposition modular split/scaffolding governance shall preserve explicit lane-A dependency anchors (`M247-A001`) and fail closed on scaffolding evidence drift.
- frontend profiling and hot-path decomposition edge-case expansion and
  robustness wiring shall preserve explicit lane-A dependency anchor
  (`M247-A005`) and fail closed when dependency references, parser-boundary
  replay hooks, or profiling evidence commands drift.
- frontend profiling and hot-path decomposition diagnostics hardening wiring
  shall preserve explicit lane-A dependency anchor (`M247-A006`) and fail closed
  when dependency references, profiling diagnostics, or compile-time budget
  evidence commands drift.
- frontend profiling and hot-path decomposition recovery and determinism hardening wiring
  shall preserve explicit lane-A dependency anchor (`M247-A007`) and fail closed
  when dependency references, recovery replay proofs, or compile-time budget
  evidence commands drift.
- frontend profiling and hot-path decomposition conformance matrix implementation wiring
  shall preserve explicit lane-A dependency anchor (`M247-A008`) and fail closed
  when dependency references, conformance-matrix continuity, or contract-gating
  evidence commands drift.
- frontend profiling and hot-path decomposition conformance corpus expansion wiring
  shall preserve explicit lane-A dependency anchor (`M247-A009`) and fail closed
  when dependency references, conformance-corpus continuity, or contract-gating
  evidence commands drift.
- frontend profiling and hot-path decomposition performance and quality guardrails wiring
  shall preserve explicit lane-A dependency anchor (`M247-A010`) and fail closed
  when dependency references, performance-quality continuity, or contract-gating
  evidence commands drift.
- frontend profiling and hot-path decomposition cross-lane integration sync wiring
  shall preserve explicit lane-A dependency anchor (`M247-A011`) and fail closed
  when dependency references, cross-lane synchronization continuity, or
  contract-gating evidence commands drift.
- frontend profiling and hot-path decomposition docs and operator runbook synchronization wiring
  shall preserve explicit lane-A dependency anchor (`M247-A012`) and fail closed
  when dependency references, docs/runbook synchronization continuity, or
  contract-gating evidence commands drift.
- frontend profiling and hot-path decomposition release-candidate and replay dry-run wiring
  shall preserve explicit lane-A dependency anchor (`M247-A013`) and fail closed
  when dependency references, release-candidate/replay command sequencing continuity, release_candidate_replay_key continuity, or contract-gating evidence commands drift.
- frontend profiling and hot-path decomposition advanced edge compatibility workpack (shard 1) wiring
  shall preserve explicit lane-A dependency anchor (`M247-A015`) and fail closed
  when dependency references, advanced-edge-compatibility-workpack command sequencing continuity, advanced-edge-compatibility-workpack-shard-1-key continuity, or contract-gating evidence commands drift.
- runtime/link/build throughput optimization core-feature expansion wiring
  shall preserve explicit lane-D dependency anchor (`M247-D003`) and fail
  closed when dependency references, pending-token continuity, or throughput
  contract-gating evidence commands drift.
- runtime/link/build throughput optimization edge-case and compatibility completion wiring
  shall preserve explicit lane-D dependency anchor (`M247-D004`) and fail
  closed when dependency references or throughput edge-compatibility evidence
  commands drift.
- runtime/link/build throughput optimization conformance matrix implementation wiring
  shall preserve explicit lane-D dependency anchor (`M247-D008`) and fail
  closed when dependency references, conformance-matrix continuity, or throughput conformance-matrix evidence
  commands drift.
- interop surface syntax and declaration forms governance shall preserve
  deterministic lane-A anchors, explicit dependency tokens (`none` for `M244-A001`),
  and fail closed on declaration-form boundary drift before downstream interop
  lowering/runtime integration advances.
- interop surface syntax/declaration-form modular split scaffolding governance shall preserve explicit
  lane-A dependency anchor (`M244-A001`) and fail closed when modular split
  dependency references or readiness evidence commands drift.
- interop surface syntax/declaration-form core-feature implementation governance shall preserve explicit
  lane-A dependency anchor (`M244-A002`) and fail closed when core-feature
  dependency references or readiness evidence commands drift.
- interop surface syntax/declaration-form core-feature expansion governance shall preserve explicit
  lane-A dependency anchor (`M244-A003`) and fail closed when core-feature
  expansion dependency references or readiness evidence commands drift.
- interop surface syntax/declaration-form edge-case and compatibility completion governance shall preserve explicit
  lane-A dependency anchor (`M244-A004`) and fail closed when edge-case
  completion dependency references or readiness evidence commands drift.
- interop surface syntax/declaration-form edge-case expansion and robustness governance shall preserve explicit
  lane-A dependency anchor (`M244-A005`) and fail closed when edge-case
  expansion dependency references or readiness evidence commands drift.
- interop surface syntax/declaration-form diagnostics hardening governance shall preserve explicit
  lane-A dependency anchor (`M244-A006`) and fail closed when diagnostics
  dependency references or readiness evidence commands drift.
- interop surface syntax/declaration-form recovery and determinism hardening governance shall preserve explicit
  lane-A dependency anchor (`M244-A007`) and fail closed when recovery
  dependency references or readiness evidence commands drift.
- interop surface syntax/declaration-form conformance matrix implementation governance shall preserve explicit
  lane-A dependency anchor (`M244-A008`) and fail closed when conformance
  dependency references or readiness evidence commands drift.
- interop surface syntax/declaration-form conformance corpus expansion governance shall preserve explicit
  lane-A dependency anchor (`M244-A009`) and fail closed when conformance
  dependency references or readiness evidence commands drift.
- interop surface syntax/declaration-form performance and quality guardrails governance shall preserve explicit
  lane-A dependency anchor (`M244-A010`) and fail closed when quality
  dependency references or readiness evidence commands drift.
- interop surface syntax/declaration-form cross-lane integration sync governance shall preserve explicit
  lane-A dependency anchors (`M244-A011`, `M244-B007`, `M244-C007`, `M244-D004`, and `M244-E006`) and fail closed
  when integration dependency references or readiness evidence commands drift.
- interop surface syntax/declaration-form integration closeout and gate sign-off governance shall preserve explicit
  lane-A dependency anchor (`M244-A012`) and fail closed when integration closeout
  dependency references or readiness evidence commands drift.
- interop lowering and ABI conformance governance shall preserve
  deterministic lane-C anchors, explicit dependency tokens (`none` for `M244-C001`),
  and fail closed on lowering and ABI conformance boundary drift before
  downstream runtime projection and cross-lane conformance expansion advances.
- interop lowering and ABI conformance modular split scaffolding governance shall preserve explicit
  lane-C dependency anchor (`M244-C001`) and fail closed when modular split
  dependency references or readiness evidence commands drift.
- interop lowering and ABI conformance core-feature implementation governance shall preserve explicit
  lane-C dependency anchor (`M244-C002`) and fail closed when core-feature
  dependency references or readiness evidence commands drift.
- interop lowering and ABI conformance core-feature expansion governance shall preserve explicit
  lane-C dependency anchor (`M244-C003`) and fail closed when core-feature
  expansion dependency references or readiness evidence commands drift.
- interop lowering and ABI conformance edge-case and compatibility completion governance shall preserve explicit
  lane-C dependency anchor (`M244-C004`) and fail closed when edge-case
  completion dependency references or readiness evidence commands drift.
- interop lowering and ABI conformance edge-case expansion and robustness governance shall preserve explicit
  lane-C dependency anchor (`M244-C005`) and fail closed when edge-case
  expansion dependency references or readiness evidence commands drift.
- interop lowering and ABI conformance diagnostics hardening governance shall preserve explicit
  lane-C dependency anchor (`M244-C006`) and fail closed when diagnostics
  dependency references or readiness evidence commands drift.
- interop lowering and ABI conformance recovery and determinism hardening governance shall preserve explicit
  lane-C dependency anchor (`M244-C007`) and fail closed when recovery
  dependency references or readiness evidence commands drift.
- interop lowering and ABI conformance conformance matrix implementation governance shall preserve explicit
  lane-C dependency anchor (`M244-C008`) and fail closed when conformance matrix
  dependency references or readiness evidence commands drift.
- interop lowering and ABI conformance conformance corpus expansion governance shall preserve explicit
  lane-C dependency anchor (`M244-C009`) and fail closed when conformance corpus
  dependency references or readiness evidence commands drift.
- interop lowering and ABI conformance performance and quality guardrails governance shall preserve explicit
  lane-C dependency anchor (`M244-C010`) and fail closed when performance/quality
  dependency references or readiness evidence commands drift.
- interop lowering and ABI conformance cross-lane integration sync governance shall preserve explicit
  lane-C dependency anchor (`M244-C011`) and fail closed when cross-lane integration
  dependency references or readiness evidence commands drift.
- interop lowering and ABI conformance docs and operator runbook synchronization governance shall preserve explicit
  lane-C dependency anchor (`M244-C012`) and fail closed when docs/runbook synchronization
  dependency references or readiness evidence commands drift.
- interop lowering and ABI conformance release-candidate and replay dry-run governance shall preserve explicit
  lane-C dependency anchor (`M244-C013`) and fail closed when release-candidate/replay dry-run
  dependency references or readiness evidence commands drift.
- runtime/link bridge-path governance shall preserve explicit lane-D dependency anchors (`M244-A001`) and fail closed on bridge-path boundary drift before
  downstream runtime projection and metadata integration advances.
- runtime/link bridge-path modular split scaffolding governance shall preserve explicit lane-D dependency anchors (`M244-D001`) and fail closed on
  modular split scaffolding evidence drift before downstream runtime projection and metadata integration advances.
- runtime/link bridge-path core feature implementation governance shall preserve explicit lane-D dependency anchors (`M244-D002`) and fail closed on
  core-feature implementation evidence drift before downstream runtime projection and metadata integration advances.
- runtime/link bridge-path core feature expansion governance shall preserve explicit lane-D dependency anchors (`M244-D003`) and fail closed on
  core-feature expansion evidence drift before downstream runtime projection and metadata integration advances.
- runtime/link bridge-path edge-case and compatibility completion governance shall preserve explicit lane-D dependency anchors (`M244-D004`) and fail closed on
  edge-case and compatibility completion evidence drift before downstream runtime projection and metadata integration advances.
- runtime/link bridge-path edge-case expansion and robustness governance shall preserve explicit lane-D dependency anchors (`M244-D005`) and fail closed on
  edge-case expansion and robustness evidence drift before downstream runtime projection and metadata integration advances.
- runtime/link bridge-path diagnostics hardening governance shall preserve explicit lane-D dependency anchors (`M244-D006`) and fail closed on
  diagnostics hardening evidence drift before downstream runtime projection and metadata integration advances.
- runtime/link bridge-path recovery and determinism hardening governance shall preserve explicit lane-D dependency anchors (`M244-D007`) and fail closed on
  recovery and determinism hardening evidence drift before downstream runtime projection and metadata integration advances.
- interop semantic contracts and type mediation governance shall preserve
  deterministic lane-B anchors, explicit dependency tokens (`none` for `M244-B001`),
  and fail closed on semantic/type mediation drift before downstream interop
  lowering/runtime integration advances.
- interop semantic/type mediation modular split scaffolding governance shall preserve explicit
  lane-B dependency anchor (`M244-B001`) and fail closed when modular split
  dependency references or readiness evidence commands drift.
- interop semantic/type mediation core-feature implementation governance shall preserve explicit
  lane-B dependency anchor (`M244-B002`) and fail closed when core-feature
  dependency references or readiness evidence commands drift.
- interop semantic/type mediation core-feature expansion governance shall preserve explicit
  lane-B dependency anchor (`M244-B003`) and fail closed when core-feature
  expansion dependency references or readiness evidence commands drift.
- interop semantic/type mediation edge-case and compatibility completion governance shall preserve explicit
  lane-B dependency anchor (`M244-B004`) and fail closed when edge-case
  completion dependency references or readiness evidence commands drift.
- interop semantic/type mediation edge-case expansion and robustness governance shall preserve explicit
  lane-B dependency anchor (`M244-B005`) and fail closed when edge-case
  expansion dependency references or readiness evidence commands drift.
- interop semantic/type mediation cross-lane integration sync governance shall preserve explicit
  lane-B dependency anchor (`M244-B011`) and fail closed when cross-lane integration
  dependency references or readiness evidence commands drift.
- interop semantic/type mediation docs and operator runbook synchronization governance shall preserve explicit
  lane-B dependency anchor (`M244-B012`) and fail closed when docs/runbook synchronization
  dependency references or readiness evidence commands drift.
- interop semantic/type mediation advanced core workpack (shard 1) governance shall preserve explicit
  lane-B dependency anchor (`M244-B014`) and fail closed when advanced core workpack (shard 1)
  dependency references or readiness evidence commands drift.
- interop conformance gate and operations contract and architecture freeze wiring shall preserve explicit
  lane-E dependency anchors (`M244-A001`, `M244-B001`, `M244-C001`, and `M244-D001`),
  preserve `npm run --if-present` dependency-reference continuity for lane-B/C/D readiness hooks,
  and fail closed when dependency token/reference continuity, interop evidence commands,
  or lane-E readiness hooks drift.
- interop conformance gate and operations modular split/scaffolding wiring shall preserve explicit
  lane-E dependency anchors (`M244-E001`, `M244-A002`, `M244-B002`, `M244-C002`, and `M244-D002`),
  preserve `npm run --if-present` dependency-reference continuity for lane-B/C/D modular split readiness hooks,
  and fail closed when dependency token/reference continuity, interop evidence commands,
  or lane-E modular split readiness hooks drift.
- interop conformance gate and operations core-feature implementation wiring shall preserve explicit
  lane-E dependency anchors (`M244-E002`, `M244-A002`, `M244-B003`, `M244-C004`, and `M244-D004`),
  preserve `npm run --if-present` dependency-reference continuity for lane-B/C/D core-feature readiness hooks,
  and fail closed when dependency token/reference continuity, interop evidence commands,
  or lane-E core-feature readiness hooks drift.
- interop conformance gate and operations core-feature expansion wiring shall preserve explicit
  lane-E dependency anchors (`M244-E003`, `M244-A003`, `M244-B004`, `M244-C005`, and `M244-D005`),
  preserve `npm run --if-present` dependency-reference continuity for lane-B/C/D core-feature expansion readiness hooks,
  and fail closed when dependency token/reference continuity, interop evidence commands,
  or lane-E core-feature expansion readiness hooks drift.
- interop conformance gate and operations edge-case and compatibility completion wiring shall preserve explicit
  lane-E dependency anchors (`M244-E004`, `M244-A004`, `M244-B006`, `M244-C007`, and `M244-D006`),
  preserve `npm run --if-present` dependency-reference continuity for lane-B/C/D edge-case and compatibility completion readiness hooks,
  and fail closed when dependency token/reference continuity, interop evidence commands,
  or lane-E edge-case and compatibility completion readiness hooks drift.
- interop conformance gate and operations edge-case expansion and robustness wiring shall preserve explicit
  lane-E dependency anchors (`M244-E005`, `M244-A005`, `M244-B007`, `M244-C008`, and `M244-D008`),
  preserve `npm run --if-present` dependency-reference continuity for lane-B/C/D edge-case expansion and robustness readiness hooks,
  and fail closed when dependency token/reference continuity, interop evidence commands,
  or lane-E edge-case expansion and robustness readiness hooks drift.
- interop conformance gate and operations diagnostics hardening wiring shall preserve explicit
  lane-E dependency anchors (`M244-E006`, `M244-A005`, `M244-B008`, `M244-C009`, and `M244-D009`),
  preserve `npm run --if-present` dependency-reference continuity for lane-B/C/D diagnostics hardening readiness hooks,
  and fail closed when dependency token/reference continuity, interop evidence commands,
  or lane-E diagnostics hardening readiness hooks drift.
- interop conformance gate and operations recovery and determinism hardening wiring shall preserve explicit
  lane-E dependency anchors (`M244-E007`, `M244-A006`, `M244-B009`, `M244-C011`, and `M244-D010`),
  preserve `npm run --if-present` dependency-reference continuity for lane-B/C/D recovery and determinism hardening readiness hooks,
  and fail closed when dependency token/reference continuity, interop evidence commands,
  or lane-E recovery and determinism hardening readiness hooks drift.
- interop conformance gate and operations conformance matrix implementation wiring shall preserve explicit
  lane-E dependency anchors (`M244-E008`, `M244-A007`, `M244-B010`, `M244-C012`, and `M244-D012`),
  preserve `npm run --if-present` dependency-reference continuity for lane-B/C/D conformance matrix implementation readiness hooks,
  and fail closed when dependency token/reference continuity, interop evidence commands,
  or lane-E conformance matrix implementation readiness hooks drift.
- suite partitioning and fixture ownership governance shall preserve explicit
  lane-A dependency boundary anchors and fail closed on fixture partition drift
  before parser and recovery replay validation advances.
- suite partitioning modular split/scaffolding shall preserve explicit lane-A
  dependency anchors (`M248-A001`) and fail closed on scaffolding evidence drift
  before parser replay readiness advances.
- suite partitioning and fixture ownership recovery and determinism hardening governance
  shall preserve explicit lane-A dependency anchors (`M248-A007`) and fail
  closed on recovery and determinism hardening evidence drift before downstream
  parser replay and lane-e conformance matrix advances.
- frontend behavior parity across toolchains governance shall preserve
  deterministic lane-A boundary anchors and fail closed on toolchain portability drift
  before semantic parity and lowering portability validation advances.
- property and ivar syntax surface completion governance shall preserve
  deterministic lane-A boundary anchors and fail closed on property/ivar semantics drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic grammar normalization governance shall preserve
  deterministic lane-A boundary anchors and fail closed on nullability/generics/qualifier semantics drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic semantic inference governance shall preserve
  deterministic lane-B boundary anchors and fail closed on nullability/generics/qualifier semantic-inference drift
  before semantic parity and lowering portability validation advances.
- qualified type lowering and ABI representation governance shall preserve deterministic lane-C
  boundary anchors and fail closed on nullability/generics/qualifier lowering and ABI representation drift
  before semantic parity and lowering portability validation advances.
- qualified type lowering and ABI representation modular split/scaffolding governance shall preserve explicit
  lane-C dependency anchors (`M235-C001`) and fail closed on modular split/scaffolding evidence drift
  before semantic parity and lowering portability validation advances.
- qualified type lowering and ABI representation core feature implementation governance shall preserve explicit
  lane-C dependency anchors (`M235-C002`) and fail closed on core feature implementation evidence drift
  before semantic parity and lowering portability validation advances.
- qualified type lowering and ABI representation core feature expansion governance shall preserve explicit
  lane-C dependency anchors (`M235-C003`) and fail closed on core feature expansion evidence drift
  before semantic parity and lowering portability validation advances.
- qualified type lowering and ABI representation edge-case and compatibility completion governance shall preserve explicit
  lane-C dependency anchors (`M235-C004`) and fail closed on edge-case and compatibility completion evidence drift
  before semantic parity and lowering portability validation advances.
- interop behavior for qualified generic APIs governance shall preserve explicit
  lane-D dependency anchors (`M235-C001`) and fail closed on interop contract and architecture evidence drift
  before semantic parity and lowering portability validation advances.
- interop behavior for qualified generic APIs modular split/scaffolding governance shall preserve explicit
  lane-D dependency anchors (`M235-D001`) and fail closed on modular split/scaffolding interop evidence drift
  before semantic parity and lowering portability validation advances.
- interop behavior for qualified generic APIs core feature implementation governance shall preserve explicit
  lane-D dependency anchors (`M235-D002`) and fail closed on core feature implementation interop evidence drift
  before semantic parity and lowering portability validation advances.
- interop behavior for qualified generic APIs core feature expansion governance shall preserve explicit
  lane-D dependency anchors (`M235-D003`) and fail closed on core feature expansion interop evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic conformance gate governance shall preserve explicit
  lane-E dependency anchors (`M235-A001`, `M235-B001`, `M235-C001`) and fail closed on lane-E contract freeze evidence drift
  before cross-lane conformance gate expansion validation advances.
- qualifier/generic conformance gate modular split/scaffolding governance shall preserve explicit
  lane-E dependency anchors (`M235-E001`, `M235-A002`, `M235-B004`, `M235-C003`, `M235-D001`) and fail closed on lane-E modular split/scaffolding evidence drift
  before cross-lane conformance gate expansion validation advances.
- qualifier/generic conformance gate core feature implementation governance shall preserve explicit
  lane-E dependency anchors (`M235-E002`, `M235-A003`, `M235-B006`, `M235-C004`, `M235-D002`) and fail closed on lane-E core feature implementation evidence drift
  before cross-lane conformance gate expansion validation advances.
- qualifier/generic semantic inference modular split/scaffolding governance shall preserve explicit
  lane-B dependency anchors (`M235-B001`) and fail closed on scaffolding evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic semantic inference core feature implementation governance shall preserve explicit
  lane-B dependency anchors (`M235-B002`) and fail closed on core-feature evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic semantic inference core feature expansion governance shall preserve explicit
  lane-B dependency anchors (`M235-B003`) and fail closed on core-feature expansion evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic semantic inference edge-case and compatibility completion governance shall preserve explicit
  lane-B dependency anchors (`M235-B004`) and fail closed on edge-case and compatibility completion evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic semantic inference edge-case expansion and robustness governance shall preserve explicit
  lane-B dependency anchors (`M235-B005`) and fail closed on edge-case expansion and robustness evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic semantic inference diagnostics hardening governance shall preserve explicit
  lane-B dependency anchors (`M235-B006`) and fail closed on diagnostics hardening evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic semantic inference recovery and determinism hardening governance shall preserve explicit
  lane-B dependency anchors (`M235-B007`) and fail closed on recovery and determinism hardening evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic semantic inference conformance matrix implementation governance shall preserve explicit
  lane-B dependency anchors (`M235-B008`) and fail closed on conformance matrix implementation evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic semantic inference conformance corpus expansion governance shall preserve explicit
  lane-B dependency anchors (`M235-B009`) and fail closed on conformance corpus expansion evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic semantic inference performance and quality guardrails governance shall preserve explicit
  lane-B dependency anchors (`M235-B010`) and fail closed on performance and quality guardrails evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic semantic inference cross-lane integration sync governance shall preserve explicit
  lane-B dependency anchors (`M235-B011`) and fail closed on cross-lane integration sync evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic semantic inference docs and operator runbook synchronization governance shall preserve explicit
  lane-B dependency anchors (`M235-B012`) and fail closed on docs and operator runbook synchronization evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic semantic inference release-candidate and replay dry-run governance shall preserve explicit
  lane-B dependency anchors (`M235-B013`) and fail closed on release-candidate and replay dry-run evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic semantic inference advanced core workpack (shard 1) governance shall preserve explicit
  lane-B dependency anchors (`M235-B014`) and fail closed on advanced core workpack (shard 1) evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic semantic inference advanced edge compatibility workpack (shard 1) governance shall preserve explicit
  lane-B dependency anchors (`M235-B015`) and fail closed on advanced edge compatibility workpack (shard 1) evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic semantic inference advanced diagnostics workpack (shard 1) governance shall preserve explicit
  lane-B dependency anchors (`M235-B016`) and fail closed on advanced diagnostics workpack (shard 1) evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic semantic inference advanced conformance workpack (shard 1) governance shall preserve explicit
  lane-B dependency anchors (`M235-B017`) and fail closed on advanced conformance workpack (shard 1) evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic semantic inference advanced integration workpack (shard 1) governance shall preserve explicit
  lane-B dependency anchors (`M235-B018`) and fail closed on advanced integration workpack (shard 1) evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic semantic inference advanced performance workpack (shard 1) governance shall preserve explicit
  lane-B dependency anchors (`M235-B019`) and fail closed on advanced performance workpack (shard 1) evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic semantic inference advanced core workpack (shard 2) governance shall preserve explicit
  lane-B dependency anchors (`M235-B020`) and fail closed on advanced core workpack (shard 2) evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic semantic inference advanced edge compatibility workpack (shard 2) governance shall preserve explicit
  lane-B dependency anchors (`M235-B021`) and fail closed on advanced edge compatibility workpack (shard 2) evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic semantic inference advanced diagnostics workpack (shard 2) governance shall preserve explicit
  lane-B dependency anchors (`M235-B022`) and fail closed on advanced diagnostics workpack (shard 2) evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic semantic inference advanced conformance workpack (shard 2) governance shall preserve explicit
  lane-B dependency anchors (`M235-B023`) and fail closed on advanced conformance workpack (shard 2) evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic semantic inference advanced integration workpack (shard 2) governance shall preserve explicit
  lane-B dependency anchors (`M235-B024`) and fail closed on advanced integration workpack (shard 2) evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic semantic inference advanced performance workpack (shard 2) governance shall preserve explicit
  lane-B dependency anchors (`M235-B025`) and fail closed on advanced performance workpack (shard 2) evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic semantic inference advanced core workpack (shard 3) governance shall preserve explicit
  lane-B dependency anchors (`M235-B026`) and fail closed on advanced core workpack (shard 3) evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic semantic inference advanced edge compatibility workpack (shard 3) governance shall preserve explicit
  lane-B dependency anchors (`M235-B027`) and fail closed on advanced edge compatibility workpack (shard 3) evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic semantic inference advanced diagnostics workpack (shard 3) governance shall preserve explicit
  lane-B dependency anchors (`M235-B028`) and fail closed on advanced diagnostics workpack (shard 3) evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic semantic inference integration closeout and gate sign-off governance shall preserve explicit
  lane-B dependency anchors (`M235-B029`) and fail closed on integration closeout and gate sign-off evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic grammar normalization modular split/scaffolding governance shall preserve explicit
  lane-A dependency anchors (`M235-A001`) and fail closed on scaffolding evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic grammar normalization core feature implementation governance shall preserve explicit
  lane-A dependency anchors (`M235-A002`) and fail closed on core-feature evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic grammar normalization core feature expansion governance shall preserve explicit
  lane-A dependency anchors (`M235-A003`) and fail closed on core-feature expansion evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic grammar normalization edge-case and compatibility completion governance shall preserve explicit
  lane-A dependency anchors (`M235-A004`) and fail closed on edge-case and compatibility completion evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic grammar normalization edge-case expansion and robustness governance shall preserve explicit
  lane-A dependency anchors (`M235-A005`) and fail closed on edge-case expansion and robustness evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic grammar normalization diagnostics hardening governance shall preserve explicit
  lane-A dependency anchors (`M235-A006`) and fail closed on diagnostics hardening evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic grammar normalization recovery and determinism hardening governance shall preserve explicit
  lane-A dependency anchors (`M235-A007`) and fail closed on recovery and determinism hardening evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic grammar normalization conformance matrix implementation governance shall preserve explicit
  lane-A dependency anchors (`M235-A008`) and fail closed on conformance matrix implementation evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic grammar normalization conformance corpus expansion governance shall preserve explicit
  lane-A dependency anchors (`M235-A009`) and fail closed on conformance corpus expansion evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic grammar normalization performance and quality guardrails wiring shall preserve explicit
  lane-A dependency anchor (`M235-A010`) and fail closed on performance and quality guardrails evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic grammar normalization cross-lane integration sync wiring shall preserve explicit
  lane-A dependency anchor (`M235-A011`) and fail closed on cross-lane integration sync evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic grammar normalization docs and operator runbook synchronization wiring shall preserve explicit
  lane-A dependency anchor (`M235-A012`) and fail closed on docs and operator runbook synchronization evidence drift
  before semantic parity and lowering portability validation advances.
- qualifier/generic grammar normalization release-candidate and replay dry-run wiring shall preserve explicit
  lane-A dependency anchor (`M235-A013`) and fail closed when
  release-candidate/replay command sequencing continuity, release_candidate_replay_key continuity, or contract-gating evidence commands drift.
- qualifier/generic grammar normalization advanced edge compatibility workpack (shard 1) wiring shall preserve explicit
  lane-A dependency anchor (`M235-A015`) and fail closed when
  advanced-edge-compatibility-workpack command sequencing continuity, advanced-edge-compatibility-workpack-shard-1-key continuity, or contract-gating evidence commands drift.
- qualifier/generic grammar normalization integration closeout and gate sign-off wiring shall preserve explicit
  lane-A dependency anchor (`M235-A016`) and fail closed when
  integration-closeout-and-gate-signoff command sequencing continuity, integration-closeout-and-gate-signoff-key continuity, or contract-gating evidence commands drift.
- accessor and ivar lowering contracts governance shall preserve deterministic lane-C
  boundary anchors and fail closed on property/ivar lowering semantics drift before
  runtime property metadata and integration validation advances.
- accessor and ivar lowering modular split/scaffolding governance shall preserve explicit lane-C
  dependency anchors (`M234-C001`) and fail closed on modular split evidence drift
  before runtime property metadata and integration validation advances.
- accessor and ivar lowering contracts core feature implementation shall preserve explicit
  lane-C dependency anchors (`M234-C001`, `M234-C002`) and fail closed on core-feature
  evidence drift before runtime property metadata and integration validation advances.
- accessor and ivar lowering contracts core feature expansion shall preserve explicit lane-C dependency token (`M234-C003`) and fail closed on core-feature expansion evidence drift
  before runtime property metadata and integration validation advances.
- accessor and ivar lowering contracts edge-case and compatibility completion governance shall preserve explicit
  lane-C dependency anchors (`M234-C004`) and fail closed on edge-case and compatibility completion evidence drift
  before runtime property metadata and integration validation advances.
- accessor and ivar lowering contracts edge-case expansion and robustness governance shall preserve explicit
  lane-C dependency anchors (`M234-C005`) and fail closed on edge-case expansion and robustness evidence drift
  before runtime property metadata and integration validation advances.
- accessor and ivar lowering contracts diagnostics hardening governance shall preserve explicit
  lane-C dependency anchors (`M234-C006`) and fail closed on diagnostics hardening evidence drift
  before runtime property metadata and integration validation advances.
- accessor and ivar lowering contracts recovery and determinism hardening governance shall preserve explicit
  lane-C dependency anchors (`M234-C007`) and fail closed on recovery and determinism hardening evidence drift
  before runtime property metadata and integration validation advances.
- accessor and ivar lowering contracts conformance matrix implementation governance shall preserve explicit
  lane-C dependency anchors (`M234-C008`) and fail closed on conformance matrix implementation evidence drift
  before runtime property metadata and integration validation advances.
- accessor and ivar lowering contracts conformance corpus expansion governance shall preserve explicit
  lane-C dependency anchors (`M234-C009`) and fail closed on conformance corpus expansion evidence drift
  before runtime property metadata and integration validation advances.
- accessor and ivar lowering contracts performance and quality guardrails governance shall preserve explicit
  lane-C dependency anchors (`M234-C010`) and fail closed on performance and quality guardrails evidence drift
  before runtime property metadata and integration validation advances.
- accessor and ivar lowering contracts cross-lane integration sync governance shall preserve explicit
  lane-C dependency anchors (`M234-C011`) and fail closed on cross-lane integration sync evidence drift
  before runtime property metadata and integration validation advances.
- accessor and ivar lowering contracts docs and operator runbook synchronization governance shall preserve explicit
  lane-C dependency anchors (`M234-C012`) and fail closed on docs and operator runbook synchronization evidence drift
  before runtime property metadata and integration validation advances.
- accessor and ivar lowering contracts release-candidate and replay dry-run governance shall preserve explicit
  lane-C dependency anchors (`M234-C013`) and fail closed on release-candidate and replay dry-run evidence drift
  before runtime property metadata and integration validation advances.
- accessor and ivar lowering contracts advanced core workpack (shard 1) governance shall preserve explicit
  lane-C dependency anchors (`M234-C014`) and fail closed on advanced core workpack (shard 1) evidence drift
  before runtime property metadata and integration validation advances.
- accessor and ivar lowering contracts advanced edge compatibility workpack (shard 1) governance shall preserve explicit
  lane-C dependency anchors (`M234-C015`) and fail closed on advanced edge compatibility workpack (shard 1) evidence drift
  before runtime property metadata and integration validation advances.
- accessor and ivar lowering contracts integration closeout and gate sign-off governance shall preserve explicit
  lane-C dependency anchors (`M234-C016`) and fail closed on integration closeout and gate sign-off evidence drift
  before runtime property metadata and integration validation advances.
- runtime property metadata integration governance shall preserve deterministic lane-D
  boundary anchors and fail closed on runtime property metadata integration drift before
  runtime property metadata conformance and runtime validation advances.
- runtime property metadata integration modular split scaffolding shall
  preserve explicit lane-D dependency anchors (`M234-D001`) and fail closed on
  modular split/scaffolding evidence drift before runtime property metadata
  conformance and runtime validation advances.
- runtime property metadata integration core feature implementation shall
  preserve explicit lane-D dependency anchors (`M234-D002`) and fail closed on
  core-feature evidence drift before architecture freeze readiness advances.
- runtime property metadata integration core feature expansion shall
  preserve explicit lane-D dependency anchors (`M234-D003`) and fail closed on
  core-feature expansion evidence drift before edge-case compatibility completion readiness advances.
- property and ivar syntax surface completion modular split/scaffolding governance shall preserve explicit
  lane-A dependency anchors (`M234-A001`) and fail closed on scaffolding evidence drift
  before semantic parity and lowering portability validation advances.
- property and ivar syntax surface completion core feature implementation governance shall preserve explicit
  lane-A dependency anchors (`M234-A002`) and fail closed on core-feature evidence drift
  before semantic parity and lowering portability validation advances.
- property and ivar syntax surface completion core feature expansion governance shall preserve explicit
  lane-A dependency anchors (`M234-A003`) and fail closed on core-feature expansion evidence drift
  before semantic parity and lowering portability validation advances.
- property and ivar syntax surface completion edge-case and compatibility completion governance shall preserve explicit
  lane-A dependency anchors (`M234-A004`) and fail closed on edge-case and compatibility completion evidence drift
  before semantic parity and lowering portability validation advances.
- property and ivar syntax surface completion edge-case expansion and robustness governance shall preserve explicit
  lane-A dependency anchors (`M234-A005`) and fail closed on edge-case expansion and robustness evidence drift
  before semantic parity and lowering portability validation advances.
- property and ivar syntax surface completion diagnostics hardening governance shall preserve explicit
  lane-A dependency anchors (`M234-A006`) and fail closed on diagnostics hardening evidence drift
  before semantic parity and lowering portability validation advances.
- property and ivar syntax surface completion recovery and determinism hardening governance shall preserve explicit
  lane-A dependency anchors (`M234-A007`) and fail closed on recovery and determinism hardening evidence drift
  before semantic parity and lowering portability validation advances.
- property and ivar syntax surface completion conformance matrix implementation governance shall preserve explicit
  lane-A dependency anchors (`M234-A008`) and fail closed on conformance matrix implementation evidence drift
  before semantic parity and lowering portability validation advances.
- property and ivar syntax surface completion conformance corpus expansion governance shall preserve explicit
  lane-A dependency anchors (`M234-A009`) and fail closed on conformance corpus expansion evidence drift
  before semantic parity and lowering portability validation advances.
- property and ivar syntax surface completion performance and quality guardrails wiring shall preserve explicit
  lane-A dependency anchor (`M234-A010`) and fail closed on guardrail evidence drift
  before semantic parity and lowering portability validation advances.
- property and ivar syntax surface completion cross-lane integration sync wiring shall preserve explicit
  lane-A dependency anchor (`M234-A011`) and fail closed on cross-lane integration evidence drift
  before semantic parity and lowering portability validation advances.
- property and ivar syntax surface completion docs and operator runbook synchronization wiring shall preserve explicit
  lane-A dependency anchor (`M234-A012`) and fail closed on docs/runbook synchronization drift
  before semantic parity and lowering portability validation advances.
- property and ivar syntax surface completion release-candidate and replay dry-run wiring shall preserve explicit
  lane-A dependency anchor (`M234-A013`) and fail closed when release-candidate/replay command sequencing continuity, release_candidate_replay_key continuity, or contract-gating evidence commands drift.
- property and ivar syntax surface completion advanced core workpack (shard 1) wiring shall preserve explicit
  lane-A dependency anchor (`M234-A014`) and fail closed when advanced-core command sequencing continuity or shard-1 evidence drift occurs.
- property and ivar syntax surface completion integration closeout and gate sign-off wiring shall preserve explicit
  lane-A dependency anchor (`M234-A015`) and fail closed when integration-closeout-and-gate-signoff command sequencing continuity, integration-closeout-and-gate-signoff-key continuity, or contract-gating evidence commands drift.
- feature packaging surface and compatibility governance shall preserve
  deterministic lane-A boundary anchors and fail closed on release packaging drift
  before semantic migration and IR/object policy validation advances.
- feature packaging modular split/scaffolding governance shall preserve explicit
  lane-A dependency anchors (`M249-A001`) and fail closed on scaffolding evidence drift
  before semantic migration and IR/object policy validation advances.
- feature packaging core feature implementation governance shall preserve explicit
  lane-A dependency anchors (`M249-A002`) and fail closed on core-feature evidence drift
  before semantic migration and IR/object policy validation advances.
- semantic/lowering test architecture governance shall preserve explicit lane-B
  diagnostic replay anchors and fail closed on semantic fixture drift before
  lowering matrix and conformance expansion validation advances.
- semantic/lowering modular split scaffolding shall preserve explicit lane-B
  dependency anchors (`M248-B001`) and fail closed on modular split evidence
  drift before semantic replay readiness advances.
- type-system completeness for ObjC3 forms governance shall preserve explicit
  lane-B dependency anchors (`M227-B001`) and fail closed on canonical ObjC
  type-form contract drift before semantic compatibility and migration
  validation advances.
- type-system diagnostics hardening governance shall preserve explicit lane-B dependency anchors (`M227-B007`, `M227-B006`) and fail closed on canonical ObjC type-form diagnostics consistency/readiness or diagnostics-key continuity drift before semantic compatibility and migration validation advances.
- type-system recovery/determinism hardening governance shall preserve explicit lane-B dependency anchors (`M227-B008`, `M227-B007`) and fail closed on canonical ObjC type-form recovery consistency/readiness or recovery-key continuity drift before semantic compatibility and migration validation advances.
- type-system conformance matrix implementation governance shall preserve explicit lane-B dependency anchors (`M227-B009`, `M227-B008`) and fail closed on canonical ObjC type-form conformance matrix consistency/readiness or conformance-matrix-key continuity drift before semantic compatibility and migration validation advances.
- type-system conformance corpus expansion governance shall preserve explicit lane-B dependency anchors (`M227-B010`, `M227-B009`) and fail closed on canonical ObjC type-form conformance corpus consistency/readiness, case-accounting continuity, or conformance-corpus-key continuity drift before semantic compatibility and migration validation advances.
- type-system performance and quality guardrails governance shall preserve explicit lane-B dependency anchors (`M227-B011`, `M227-B010`) and fail closed on canonical ObjC type-form performance/quality guardrail accounting, consistency/readiness, or performance-quality-key continuity drift before semantic compatibility and migration validation advances.
- type-system docs and operator runbook synchronization governance shall preserve explicit lane-B dependency anchors (`M227-B013`, `M227-B012`) and fail closed on canonical ObjC type-form docs/runbook command sequencing or evidence-path continuity drift before semantic compatibility and migration validation advances.
- type-system release-candidate replay dry-run governance shall preserve explicit lane-B dependency anchors (`M227-B014`, `M227-B013`) and fail closed on canonical ObjC type-form release/replay command sequencing or evidence-path continuity drift before semantic compatibility and migration validation advances.
- type-system advanced core workpack (shard 1) governance shall preserve explicit lane-B dependency anchors (`M227-B015`, `M227-B014`) and fail closed on canonical ObjC type-form advanced-core command sequencing, evidence-path continuity, or advanced-core continuity drift before semantic compatibility and migration validation advances.
- type-system advanced edge compatibility workpack (shard 1) governance shall preserve explicit lane-B dependency anchors (`M227-B016`, `M227-B015`) and fail closed on canonical ObjC type-form advanced-edge-compatibility command sequencing, evidence-path continuity, or advanced-edge-compatibility continuity drift before semantic compatibility and migration validation advances.
- type-system advanced diagnostics workpack (shard 1) governance shall preserve explicit lane-B dependency anchors (`M227-B017`, `M227-B016`) and fail closed on canonical ObjC type-form advanced-diagnostics command sequencing, evidence-path continuity, or advanced-diagnostics continuity drift before semantic compatibility and migration validation advances.
- type-system advanced conformance workpack (shard 1) governance shall preserve explicit lane-B dependency anchors (`M227-B018`, `M227-B017`) and fail closed on canonical ObjC type-form advanced-conformance command sequencing, evidence-path continuity, or advanced-conformance continuity drift before semantic compatibility and migration validation advances.
- type-system advanced integration workpack (shard 1) governance shall preserve explicit lane-B dependency anchors (`M227-B019`, `M227-B018`) and fail closed on canonical ObjC type-form advanced-integration command sequencing, evidence-path continuity, or advanced-integration continuity drift before semantic compatibility and migration validation advances.
- type-system advanced performance workpack (shard 1) governance shall preserve explicit lane-B dependency anchors (`M227-B020`, `M227-B019`) and fail closed on canonical ObjC type-form advanced-performance command sequencing, evidence-path continuity, or advanced-performance continuity drift before semantic compatibility and migration validation advances.
- type-system advanced core workpack (shard 2) governance shall preserve explicit lane-B dependency anchors (`M227-B021`, `M227-B020`) and fail closed on canonical ObjC type-form advanced-core-shard2 command sequencing, evidence-path continuity, or advanced-core-shard2 continuity drift before semantic compatibility and migration validation advances.
- type-system advanced edge compatibility workpack (shard 2) governance shall preserve explicit lane-B dependency anchors (`M227-B022`, `M227-B021`) and fail closed on canonical ObjC type-form advanced-edge-compatibility-shard2 command sequencing, evidence-path continuity, or advanced-edge-compatibility-shard2 continuity drift before semantic compatibility and migration validation advances.
- type-system advanced diagnostics workpack (shard 2) governance shall preserve explicit lane-B dependency anchors (`M227-B023`, `M227-B022`) and fail closed on canonical ObjC type-form advanced-diagnostics-shard2 command sequencing, evidence-path continuity, or advanced-diagnostics-shard2 continuity drift before semantic compatibility and migration validation advances.
- type-system advanced conformance workpack (shard 2) governance shall preserve explicit lane-B dependency anchors (`M227-B024`, `M227-B023`) and fail closed on canonical ObjC type-form advanced-conformance-shard2 command sequencing, evidence-path continuity, or advanced-conformance-shard2 continuity drift before semantic compatibility and migration validation advances.
- type-system advanced integration workpack (shard 2) governance shall preserve explicit lane-B dependency anchors (`M227-B025`, `M227-B024`) and fail closed on canonical ObjC type-form advanced-integration-shard2 command sequencing, evidence-path continuity, or advanced-integration-shard2 continuity drift before semantic compatibility and migration validation advances.
- type-system advanced performance workpack (shard 2) governance shall preserve explicit lane-B dependency anchors (`M227-B026`, `M227-B025`) and fail closed on canonical ObjC type-form advanced-performance-shard2 command sequencing, evidence-path continuity, or advanced-performance-shard2 continuity drift before semantic compatibility and migration validation advances.
- type-system advanced core workpack (shard 3) governance shall preserve explicit lane-B dependency anchors (`M227-B027`, `M227-B026`) and fail closed on canonical ObjC type-form advanced-core-shard3 command sequencing, evidence-path continuity, or advanced-core-shard3 continuity drift before semantic compatibility and migration validation advances.
- type-system advanced edge compatibility workpack (shard 3) governance shall preserve explicit lane-B dependency anchors (`M227-B028`, `M227-B027`) and fail closed on canonical ObjC type-form advanced-edge-compatibility-shard3 command sequencing, evidence-path continuity, or advanced-edge-compatibility-shard3 continuity drift before semantic compatibility and migration validation advances.
- type-system advanced diagnostics workpack (shard 3) governance shall preserve explicit lane-B dependency anchors (`M227-B029`, `M227-B028`) and fail closed on canonical ObjC type-form advanced-diagnostics-shard3 command sequencing, evidence-path continuity, or advanced-diagnostics-shard3 continuity drift before semantic compatibility and migration validation advances.
- type-system advanced conformance workpack (shard 3) governance shall preserve explicit lane-B dependency anchors (`M227-B030`, `M227-B029`) and fail closed on canonical ObjC type-form advanced-conformance-shard3 command sequencing, evidence-path continuity, or advanced-conformance-shard3 continuity drift before semantic compatibility and migration validation advances.
- type-system advanced integration workpack (shard 3) governance shall preserve explicit lane-B dependency anchors (`M227-B031`, `M227-B030`) and fail closed on canonical ObjC type-form advanced-integration-shard3 command sequencing, evidence-path continuity, or advanced-integration-shard3 continuity drift before semantic compatibility and migration validation advances.
- type-system advanced performance workpack (shard 3) governance shall preserve explicit lane-B dependency anchors (`M227-B032`, `M227-B031`) and fail closed on canonical ObjC type-form advanced-performance-shard3 command sequencing, evidence-path continuity, or advanced-performance-shard3 continuity drift before semantic compatibility and migration validation advances.
- type-system advanced core workpack (shard 4) governance shall preserve explicit lane-B dependency anchors (`M227-B033`, `M227-B032`) and fail closed on canonical ObjC type-form advanced-core-shard4 command sequencing, evidence-path continuity, or advanced-core-shard4 continuity drift before semantic compatibility and migration validation advances.
- type-system advanced edge compatibility workpack (shard 4) governance shall preserve explicit lane-B dependency anchors (`M227-B034`, `M227-B033`) and fail closed on canonical ObjC type-form advanced-edge-compatibility-shard4 command sequencing, evidence-path continuity, or advanced-edge-compatibility-shard4 continuity drift before semantic compatibility and migration validation advances.
- type-system advanced diagnostics workpack (shard 4) governance shall preserve explicit lane-B dependency anchors (`M227-B035`, `M227-B034`) and fail closed on canonical ObjC type-form advanced-diagnostics-shard4 command sequencing, evidence-path continuity, or advanced-diagnostics-shard4 continuity drift before semantic compatibility and migration validation advances.
- type-system advanced conformance workpack (shard 4) governance shall preserve explicit lane-B dependency anchors (`M227-B036`, `M227-B035`) and fail closed on canonical ObjC type-form advanced-conformance-shard4 command sequencing, evidence-path continuity, or advanced-conformance-shard4 continuity drift before semantic compatibility and migration validation advances.
- type-system advanced integration workpack (shard 4) governance shall preserve explicit lane-B dependency anchors (`M227-B037`, `M227-B036`) and fail closed on canonical ObjC type-form advanced-integration-shard4 command sequencing, evidence-path continuity, or advanced-integration-shard4 continuity drift before semantic compatibility and migration validation advances.
- type-system advanced performance workpack (shard 4) governance shall preserve explicit lane-B dependency anchors (`M227-B038`, `M227-B037`) and fail closed on canonical ObjC type-form advanced-performance-shard4 command sequencing, evidence-path continuity, or advanced-performance-shard4 continuity drift before semantic compatibility and migration validation advances.
- type-system integration closeout and gate sign-off governance shall preserve explicit lane-B dependency anchors (`M227-B039`, `M227-B038`) and fail closed on canonical ObjC type-form integration-closeout-and-gate-signoff command sequencing, evidence-path continuity, or integration-closeout-and-gate-signoff continuity drift before semantic compatibility and migration validation advances.
- typed sema-to-lowering contracts governance shall preserve explicit lane-C typed sema handoff anchors,
  lane-C dependency anchors (`M227-C001`) and fail closed on typed sema transport or lowering metadata drift
  before semantic compatibility and runtime-facing metadata validation advances.
- typed sema-to-lowering modular split scaffolding governance shall preserve explicit
  lane-C dependency anchors (`M227-C001`) and fail closed on typed sema/lowering modular split handoff drift
  before semantic compatibility and runtime-facing metadata validation advances.
- typed sema-to-lowering core feature implementation governance shall preserve explicit lane-C dependency anchors (`M227-C002`) and fail closed on typed sema core-feature case accounting or typed core-feature key continuity drift before semantic compatibility and runtime-facing metadata validation advances.
- semantic-pass conformance matrix implementation governance shall preserve explicit
  lane-A dependency anchor (`M227-A009`) and fail closed on parser/sema
  conformance-matrix or corpus replay drift before conformance expansion
  validation advances.
- semantic-pass conformance corpus expansion governance shall preserve explicit
  lane-A dependency anchor (`M227-A010`) and fail closed on parser/sema
  conformance-corpus accounting or replay continuity drift before lane-A
  integration closeout validation advances.
- semantic-pass performance and quality guardrails governance shall preserve explicit
  lane-A dependency anchor (`M227-A011`) and fail closed on parser/sema
  performance-quality guardrail drift before cross-lane synchronization
  validation advances.
- semantic conformance lane-E quality-gate contract and architecture freeze wiring shall preserve explicit lane-E dependency anchors (`M227-A001`, `M227-B002`, `M227-C001`, and `M227-D001`),
  preserve readiness continuity across `check:objc3c:m227-a001-lane-a-readiness`, `check:objc3c:m227-b002-lane-b-readiness`, `check:objc3c:m227-c001-lane-c-readiness`, and `check:objc3c:m227-d001-lane-d-readiness`,
  and fail closed when dependency tokens, package readiness hooks, or lane-E gate evidence drift.
- semantic conformance lane-E modular split/scaffolding wiring shall preserve explicit lane-E dependency anchors (`M227-E001`, `M227-A002`, `M227-B004`, `M227-C003`, and `M227-D002`),
  preserve readiness continuity across `check:objc3c:m227-e001-lane-e-quality-gate-readiness`, `check:objc3c:m227-a002-lane-a-readiness`, `check:objc3c:m227-b004-lane-b-readiness`, `check:objc3c:m227-c003-lane-c-readiness`, and `check:objc3c:m227-d002-lane-d-readiness`,
  and fail closed when dependency tokens, package readiness hooks, or lane-E modular split/scaffolding evidence drift.
- semantic conformance lane-E edge-case expansion and robustness wiring shall preserve explicit lane-E dependency anchors (`M227-E005`, `M227-A006`, `M227-B006`, `M227-C006`, and `M227-D006`),
  preserve readiness continuity across direct `M227-E005` and `M227-A006` checker/test commands plus `check:objc3c:m227-b006-lane-b-readiness`, `check:objc3c:m227-c006-lane-c-readiness`, and `check:objc3c:m227-d006-lane-d-readiness`,
  and fail closed when dependency tokens, dependency-reference commands, or lane-E edge-case expansion/robustness evidence drift.
- semantic conformance lane-E diagnostics hardening wiring shall preserve explicit lane-E dependency anchors (`M227-E006`, `M227-A007`, `M227-B007`, `M227-C007`, and `M227-D007`),
  preserve readiness continuity across direct `M227-E006` and `M227-A007` checker/test commands plus `check:objc3c:m227-b007-lane-b-readiness`, `check:objc3c:m227-c007-lane-c-readiness`, and `check:objc3c:m227-d007-lane-d-readiness`,
  and fail closed when dependency tokens, dependency-reference commands, or lane-E diagnostics hardening evidence drift.
- semantic conformance lane-E recovery and determinism hardening wiring shall preserve explicit lane-E dependency anchors (`M227-E007`, `M227-A008`, `M227-B008`, `M227-C008`, and `M227-D008`),
  preserve readiness continuity across direct `M227-E007` and `M227-A008` checker/test commands plus `check:objc3c:m227-b008-lane-b-readiness`, `check:objc3c:m227-c008-lane-c-readiness`, and `check:objc3c:m227-d008-lane-d-readiness`,
  and fail closed when dependency tokens, dependency-reference commands, or lane-E recovery and determinism hardening evidence drift.
- semantic conformance lane-E conformance matrix implementation wiring shall preserve explicit lane-E dependency anchors (`M227-E008`, `M227-A009`, `M227-B018`, `M227-C012`, and `M227-D005`),
  preserve readiness continuity across direct `M227-E008`, `M227-A009`, and `M227-D005` checker/test commands plus `check:objc3c:m227-b018-lane-b-readiness` and `check:objc3c:m227-c012-lane-c-readiness`,
  and fail closed when dependency tokens, dependency-reference commands, or lane-E conformance matrix implementation evidence drift.
- semantic conformance lane-E conformance corpus expansion wiring shall preserve explicit lane-E dependency anchors (`M227-E009`, `M227-A011`, `M227-B020`, `M227-C013`, and `M227-D006`),
  preserve readiness continuity across direct `M227-E009`, `M227-A011`, and `M227-D006` checker/test commands plus `check:objc3c:m227-b020-lane-b-readiness` and `check:objc3c:m227-c013-lane-c-readiness`,
  and fail closed when dependency tokens, dependency-reference commands, or lane-E conformance corpus expansion evidence drift.
- semantic conformance lane-E performance and quality guardrails wiring shall preserve explicit lane-E dependency anchors (`M227-E010`, `M227-A012`, `M227-B021`, `M227-C014`, and `M227-D007`),
  preserve readiness continuity across direct `M227-E010`, `M227-A012`, and `M227-D007` checker/test commands plus `check:objc3c:m227-b021-lane-b-readiness` and `check:objc3c:m227-c014-lane-c-readiness`,
  and fail closed when dependency tokens, dependency-reference commands, or lane-E performance and quality guardrails evidence drift.
- semantic conformance lane-E cross-lane integration sync wiring shall preserve explicit lane-E dependency anchors (`M227-E011`, `M227-A013`, `M227-B023`, `M227-C016`, and `M227-D007`),
  preserve readiness continuity across direct `M227-E011`, `M227-A013`, and `M227-D007` checker/test commands plus `check:objc3c:m227-b023-lane-b-readiness` and `check:objc3c:m227-c016-lane-c-readiness`,
  and fail closed when dependency tokens, dependency-reference commands, or lane-E cross-lane integration sync evidence drift.
- semantic conformance lane-E docs and operator runbook synchronization wiring shall preserve explicit lane-E dependency anchors (`M227-E012`, `M227-A014`, `M227-B025`, `M227-C017`, and `M227-D008`),
  preserve readiness continuity across direct `M227-E012`, `M227-A014`, and `M227-D008` checker/test commands plus `check:objc3c:m227-b025-lane-b-readiness` and `check:objc3c:m227-c017-lane-c-readiness`,
  and fail closed when dependency tokens, dependency-reference commands, or lane-E docs and operator runbook synchronization evidence drift.
- semantic conformance lane-E release-candidate and replay dry-run wiring shall preserve explicit lane-E dependency anchors (`M227-E013`, `M227-A015`, `M227-B027`, `M227-C018`, and `M227-D008`),
  preserve readiness continuity across direct `M227-E013`, `M227-A015`, and `M227-D008` checker/test commands plus `check:objc3c:m227-b027-lane-b-readiness` and `check:objc3c:m227-c018-lane-c-readiness`,
  and fail closed when dependency tokens, dependency-reference commands, or lane-E release-candidate and replay dry-run evidence drift.
- semantic conformance lane-E advanced core workpack (shard 1) wiring shall preserve explicit lane-E dependency anchors (`M227-E014`, `M227-A016`, `M227-B029`, `M227-C020`, and `M227-D009`),
  preserve readiness continuity across direct `M227-E014`, `M227-A016`, and `M227-D009` checker/test commands plus `check:objc3c:m227-b029-lane-b-readiness` and `check:objc3c:m227-c020-lane-c-readiness`,
  and fail closed when dependency tokens, dependency-reference commands, or lane-E advanced core workpack (shard 1) evidence drift.
- semantic conformance lane-E advanced edge compatibility workpack (shard 1) wiring shall preserve explicit lane-E dependency anchors (`M227-E015`, `M227-A017`, `M227-B031`, `M227-C021`, and `M227-D010`),
  preserve readiness continuity across direct `M227-E015`, `M227-A017`, and `M227-D010` checker/test commands plus `check:objc3c:m227-b031-lane-b-readiness` and `check:objc3c:m227-c021-lane-c-readiness`,
  and fail closed when dependency tokens, dependency-reference commands, or lane-E advanced edge compatibility workpack (shard 1) evidence drift.
- semantic conformance lane-E advanced diagnostics workpack (shard 1) wiring shall preserve explicit lane-E dependency anchors (`M227-E016`, `M227-A018`, `M227-B033`, `M227-C022`, and `M227-D010`),
  preserve readiness continuity across direct `M227-E016`, `M227-A018`, and `M227-D010` checker/test commands plus `check:objc3c:m227-b033-lane-b-readiness` and `check:objc3c:m227-c022-lane-c-readiness`,
  and fail closed when dependency tokens, dependency-reference commands, or lane-E advanced diagnostics workpack (shard 1) evidence drift.
- semantic conformance lane-E advanced conformance workpack (shard 1) wiring shall preserve explicit lane-E dependency anchors (`M227-E017`, `M227-A019`, `M227-B035`, `M227-C023`, and `M227-D011`),
  preserve readiness continuity across direct `M227-E017`, `M227-A019`, and `M227-D011` checker/test commands plus `check:objc3c:m227-b035-lane-b-readiness` and `check:objc3c:m227-c023-lane-c-readiness`,
  and fail closed when dependency tokens, dependency-reference commands, or lane-E advanced conformance workpack (shard 1) evidence drift.
- semantic conformance lane-E advanced integration workpack (shard 1) wiring shall preserve explicit lane-E dependency anchors (`M227-E018`, `M227-A020`, `M227-B037`, `M227-C025`, and `M227-D011`),
  preserve readiness continuity across direct `M227-E018`, `M227-A020`, and `M227-D011` checker/test commands plus `check:objc3c:m227-b037-lane-b-readiness` and `check:objc3c:m227-c025-lane-c-readiness`,
  and fail closed when dependency tokens, dependency-reference commands, or lane-E advanced integration workpack (shard 1) evidence drift.
- semantic conformance lane-E integration closeout and gate sign-off wiring shall preserve explicit lane-E dependency anchors (`M227-E019`, `M227-A021`, `M227-B039`, `M227-C026`, and `M227-D012`),
  preserve readiness continuity across direct `M227-E019`, `M227-A021`, and `M227-D012` checker/test commands plus `check:objc3c:m227-b039-lane-b-readiness` and `check:objc3c:m227-c026-lane-c-readiness`,
  and fail closed when dependency tokens, dependency-reference commands, or lane-E integration closeout and gate sign-off evidence drift.
- semantic-pass cross-lane integration sync governance shall preserve explicit
  lane-A dependency anchor (`M227-A012`) and fail closed when semantic-pass
  lane dependency contracts (`M227-A011`, `M227-B007`, `M227-C002`, `M227-D001`,
  `M227-E001`) drift before docs/runbook synchronization validation advances.
- semantic-pass docs and operator runbook synchronization governance shall
  preserve explicit lane-A dependency anchor (`M227-A013`) and fail closed when
  operator command sequencing, dependency contract anchors, or readiness wiring
  drift before release-candidate replay validation advances.
- semantic-pass release-candidate and replay dry-run governance shall preserve explicit lane-A dependency anchor (`M227-A014`),
  preserve deterministic replay artifacts (`module.manifest.json`, `module.diagnostics.json`, `module.ll`, `module.object-backend.txt`),
  and fail closed when replay evidence, runbook command sequencing, or readiness wiring drifts before advanced shard validation advances.
- semantic-pass advanced core workpack (shard 1) wiring shall preserve deterministic advanced-core dependency anchors (`M227-A015`)
  and fail closed when `toolchain_runtime_ga_operations_advanced_core_consistent`,
  `toolchain_runtime_ga_operations_advanced_core_ready`, or
  `toolchain_runtime_ga_operations_advanced_core_key` drift before advanced
  edge/diagnostics/conformance validation advances.
- semantic-pass advanced edge compatibility workpack (shard 1) wiring shall preserve deterministic
  edge-compatibility dependency anchors (`M227-A016`)
  and fail closed when
  `toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent`,
  `toolchain_runtime_ga_operations_advanced_edge_compatibility_ready`, or
  `toolchain_runtime_ga_operations_advanced_edge_compatibility_key` drift
  before advanced diagnostics/conformance validation advances.
- semantic-pass advanced diagnostics workpack (shard 1) wiring shall preserve deterministic
  diagnostics dependency anchors (`M227-A017`) and fail closed
  when `toolchain_runtime_ga_operations_advanced_diagnostics_consistent`,
  `toolchain_runtime_ga_operations_advanced_diagnostics_ready`, or
  `toolchain_runtime_ga_operations_advanced_diagnostics_key` drift before
  advanced conformance validation advances.
- semantic-pass advanced conformance workpack (shard 1) wiring shall preserve deterministic
  conformance dependency anchors (`M227-A018`) and fail closed
  when `toolchain_runtime_ga_operations_advanced_conformance_consistent`,
  `toolchain_runtime_ga_operations_advanced_conformance_ready`, or
  `toolchain_runtime_ga_operations_advanced_conformance_key` drift before
  advanced integration validation advances.
- semantic-pass advanced integration workpack (shard 1) wiring shall preserve deterministic
  integration dependency anchors (`M227-A019`) and fail closed
  when `toolchain_runtime_ga_operations_advanced_integration_consistent`,
  `toolchain_runtime_ga_operations_advanced_integration_ready`, or
  `toolchain_runtime_ga_operations_advanced_integration_key` drift before
  advanced performance validation advances.
- semantic-pass advanced performance workpack (shard 1) wiring shall preserve deterministic
  performance dependency anchors (`M227-A020`) and fail closed
  when `toolchain_runtime_ga_operations_advanced_performance_consistent`,
  `toolchain_runtime_ga_operations_advanced_performance_ready`, or
  `toolchain_runtime_ga_operations_advanced_performance_key` drift before
  integration closeout validation advances.
- semantic-pass integration closeout and gate sign-off wiring shall preserve deterministic
  sign-off dependency anchors (`M227-A021`) and fail closed
  when `toolchain_runtime_ga_operations_integration_closeout_signoff_consistent`,
  `toolchain_runtime_ga_operations_integration_closeout_signoff_ready`, or
  `toolchain_runtime_ga_operations_integration_closeout_signoff_key` drift before
  lane-E integration gate validation advances.
- semantic compatibility and migration checks governance shall preserve explicit
  lane-B compatibility-mode and migration-assist handoff anchors and fail
  closed on sema pass-flow or parse/lowering compatibility drift before
  semantic stability expansion validation advances.
- semantic compatibility and migration checks modular split scaffolding shall
  preserve explicit lane-B dependency anchors (`M249-B001`) and fail closed on
  modular split evidence drift before semantic stability expansion validation
  advances.
- lowering/IR portability contracts governance shall preserve explicit
  lane-C portability and deterministic emission anchors and fail closed on
  lowering portability drift before runtime execution and conformance
  validation advances.
- lowering/IR portability modular split/scaffolding governance shall preserve explicit lane-C dependency anchors (`M245-C001`) and fail closed on modular split evidence drift before runtime portability scaffolding advances.
- lowering/IR portability contracts core feature implementation shall preserve explicit lane-C dependency anchors (`M245-C001`, `M245-C002`) and fail closed on core-feature evidence drift before runtime portability implementation advances.
- lowering/IR portability contracts core feature expansion shall preserve explicit lane-C dependency token (`M245-C003`) and fail closed on core-feature expansion evidence drift before runtime portability edge-case compatibility advances.
- lowering/IR portability contracts edge-case and compatibility completion shall preserve explicit lane-C dependency anchors (`M245-C004`) and fail closed on edge-case and compatibility completion evidence drift before runtime portability robustness validation advances.
- lowering/IR portability contracts edge-case expansion and robustness shall preserve explicit lane-C dependency anchors (`M245-C005`) and fail closed on edge-case expansion and robustness evidence drift before runtime portability diagnostics-hardening validation advances.
- lowering/IR portability contracts diagnostics hardening shall preserve explicit lane-C dependency anchors (`M245-C006`) and fail closed on diagnostics hardening evidence drift before runtime portability recovery-and-determinism-hardening validation advances.
- lowering/IR portability contracts recovery and determinism hardening shall preserve explicit lane-C dependency anchors (`M245-C007`) and fail closed on recovery and determinism hardening evidence drift before runtime portability conformance-matrix validation advances.
- lowering/IR portability contracts conformance matrix implementation shall preserve explicit lane-C dependency anchors (`M245-C008`) and fail closed on conformance matrix evidence drift before runtime portability conformance-corpus expansion validation advances.
- lowering/IR portability contracts conformance corpus expansion shall preserve explicit lane-C dependency anchors (`M245-C009`) and fail closed on conformance corpus evidence drift before runtime portability performance-and-quality-guardrails validation advances.
- lowering/IR portability contracts performance and quality guardrails shall preserve explicit lane-C dependency anchors (`M245-C010`) and fail closed on performance and quality evidence drift before runtime portability cross-lane-integration-sync validation advances.
- lowering/IR portability contracts cross-lane integration sync shall preserve explicit lane-C dependency anchors (`M245-C011`) and fail closed on cross-lane integration sync evidence drift before runtime portability docs-and-operator-runbook-synchronization validation advances.
- lowering/IR portability contracts docs and operator runbook synchronization shall preserve explicit lane-C dependency anchors (`M245-C012`) and fail closed on docs and operator runbook synchronization evidence drift before runtime portability release-candidate-and-replay-dry-run validation advances.
- lowering/IR portability contracts release-candidate and replay dry-run shall preserve explicit lane-C dependency anchors (`M245-C013`) and fail closed on release-candidate and replay dry-run evidence drift before runtime portability advanced-core-workpack validation advances.
- lowering/IR portability contracts advanced core workpack (shard 1) shall preserve explicit lane-C dependency anchors (`M245-C014`) and fail closed on advanced core workpack evidence drift before runtime portability integration-closeout-and-gate-sign-off validation advances.
- lowering/IR portability contracts integration closeout and gate sign-off shall preserve explicit lane-C dependency anchors (`M245-C015`) and fail closed on integration closeout and gate sign-off evidence drift before runtime portability milestone closeout validation advances.
- frontend behavior parity modular split/scaffolding governance shall preserve explicit lane-A dependency anchors (`M245-A001`) and fail closed on scaffolding evidence drift before parser portability scaffolding advances.
- frontend behavior parity core feature implementation governance shall preserve explicit lane-A dependency anchors (`M245-A002`) and fail closed on core-feature evidence drift before parser portability implementation advances.
- frontend behavior parity core feature expansion governance shall preserve explicit
  lane-A dependency anchors (`M245-A003`) and fail closed on core-feature expansion evidence drift
  before parser portability edge-case compatibility advances.
- frontend behavior parity edge-case and compatibility completion governance shall preserve explicit
  lane-A dependency anchors (`M245-A004`) and fail closed on edge-case and compatibility completion evidence drift
  before parser portability robustness validation advances.
- frontend behavior parity edge-case expansion and robustness governance shall preserve explicit
  lane-A dependency anchors (`M245-A005`) and fail closed on edge-case expansion and robustness evidence drift
  before parser portability diagnostics-hardening validation advances.
- frontend behavior parity diagnostics hardening governance shall preserve explicit
  lane-A dependency anchors (`M245-A006`) and fail closed on diagnostics hardening evidence drift
  before parser portability recovery-and-determinism-hardening validation advances.
- frontend behavior parity recovery and determinism hardening governance shall preserve explicit
  lane-A dependency anchors (`M245-A007`) and fail closed on recovery and determinism hardening evidence drift
  before parser portability conformance-matrix validation advances.
- frontend behavior parity conformance matrix implementation governance shall preserve explicit
  lane-A dependency anchors (`M245-A008`) and fail closed on conformance matrix implementation evidence drift
  before parser portability conformance-corpus expansion validation advances.
- frontend behavior parity conformance corpus expansion governance shall preserve explicit
  lane-A dependency anchors (`M245-A009`) and fail closed on conformance corpus expansion evidence drift
  before parser portability performance-and-quality-guardrails validation advances.
- frontend behavior parity integration closeout and gate sign-off governance shall preserve explicit
  lane-A dependency anchors (`M245-A010`) and fail closed on integration closeout and gate sign-off evidence drift
  before parser portability cross-lane-integration-sync validation advances.
- semantic parity and platform constraints core feature implementation shall
  preserve explicit lane-B dependency anchors (`M245-B002`) and fail closed on core-feature evidence drift
  before portability gate and semantic stability expansion validation advances.
- semantic parity and platform constraints core feature expansion shall
  preserve explicit lane-B dependency anchors (`M245-B003`) and fail closed on core-feature expansion evidence drift
  before portability gate and semantic stability edge-case compatibility validation advances.
- semantic parity and platform constraints edge-case and compatibility completion shall
  preserve explicit lane-B dependency anchors (`M245-B004`) and fail closed on edge-case and compatibility completion evidence drift
  before portability gate and semantic stability robustness validation advances.
- semantic parity and platform constraints edge-case expansion and robustness shall
  preserve explicit lane-B dependency anchors (`M245-B005`) and fail closed on edge-case expansion and robustness evidence drift
  before portability gate and semantic stability diagnostics-hardening validation advances.
- semantic parity and platform constraints diagnostics hardening shall
  preserve explicit lane-B dependency anchors (`M245-B006`) and fail closed on diagnostics hardening evidence drift
  before portability gate and semantic stability recovery-and-determinism-hardening validation advances.
- semantic parity and platform constraints recovery and determinism hardening shall
  preserve explicit lane-B dependency anchors (`M245-B007`) and fail closed on recovery and determinism hardening evidence drift
  before portability gate and semantic stability conformance-matrix validation advances.
- semantic parity and platform constraints conformance matrix implementation shall
  preserve explicit lane-B dependency anchors (`M245-B008`) and fail closed on conformance matrix evidence drift
  before portability gate and semantic stability conformance-corpus expansion validation advances.
- semantic parity and platform constraints conformance corpus expansion shall
  preserve explicit lane-B dependency anchors (`M245-B009`) and fail closed on conformance corpus evidence drift
  before portability gate and semantic stability performance-and-quality-guardrails validation advances.
- semantic parity and platform constraints performance and quality guardrails shall
  preserve explicit lane-B dependency anchors (`M245-B010`) and fail closed on performance and quality evidence drift
  before portability gate and semantic stability cross-lane-integration-sync validation advances.
- semantic parity and platform constraints cross-lane integration sync shall
  preserve explicit lane-B dependency anchors (`M245-B011`) and fail closed on cross-lane integration sync evidence drift
  before portability gate and semantic stability integration-closeout-and-gate-sign-off validation advances.
- semantic parity and platform constraints integration closeout and gate sign-off shall
  preserve explicit lane-B dependency anchors (`M245-B012`) and fail closed on integration closeout and gate sign-off evidence drift
  before portability gate and semantic stability release integration advances.
- build/link/runtime reproducibility modular split/scaffolding governance shall preserve explicit
  lane-D dependency anchors (`M245-D001`) and fail closed on modular split evidence drift
  before runtime reproducibility scaffolding advances.
- build/link/runtime reproducibility core feature implementation governance shall preserve explicit
  lane-D dependency anchors (`M245-D002`) and fail closed on core-feature evidence drift
  before runtime reproducibility implementation advances.
- build/link/runtime reproducibility core feature expansion governance shall preserve explicit
  lane-D dependency anchors (`M245-D003`) and fail closed on core-feature expansion evidence drift
  before runtime reproducibility edge-compatibility advances.
- build/link/runtime reproducibility edge-case and compatibility completion governance shall preserve explicit
  lane-D dependency anchors (`M245-D004`) and fail closed on edge-case and compatibility completion evidence drift
  before runtime reproducibility robustness validation advances.
- build/link/runtime reproducibility edge-case expansion and robustness governance shall preserve explicit
  lane-D dependency anchors (`M245-D005`) and fail closed on edge-case expansion and robustness evidence drift
  before runtime reproducibility diagnostics-hardening validation advances.
- build/link/runtime reproducibility diagnostics hardening governance shall preserve explicit
  lane-D dependency anchors (`M245-D006`) and fail closed on diagnostics hardening evidence drift
  before runtime reproducibility recovery-and-determinism-hardening validation advances.
- build/link/runtime reproducibility recovery and determinism hardening governance shall preserve explicit
  lane-D dependency anchors (`M245-D007`) and fail closed on recovery and determinism hardening evidence drift
  before runtime reproducibility conformance-matrix validation advances.
- build/link/runtime reproducibility conformance matrix implementation governance shall preserve explicit
  lane-D dependency anchors (`M245-D008`) and fail closed on conformance matrix evidence drift
  before runtime reproducibility conformance-corpus expansion validation advances.
- build/link/runtime reproducibility conformance corpus expansion governance shall preserve explicit
  lane-D dependency anchors (`M245-D009`) and fail closed on conformance corpus evidence drift
  before runtime reproducibility performance-and-quality-guardrails validation advances.
- build/link/runtime reproducibility performance and quality guardrails governance shall preserve explicit
  lane-D dependency anchors (`M245-D010`) and fail closed on performance and quality evidence drift
  before runtime reproducibility cross-lane-integration-sync validation advances.
- build/link/runtime reproducibility cross-lane integration sync governance shall preserve explicit
  lane-D dependency anchors (`M245-D011`) and fail closed on cross-lane integration sync evidence drift
  before runtime reproducibility docs-and-operator-runbook-synchronization validation advances.
- build/link/runtime reproducibility docs and operator runbook synchronization governance shall preserve explicit
  lane-D dependency anchors (`M245-D012`) and fail closed on docs and operator runbook synchronization evidence drift
  before runtime reproducibility release-candidate-and-replay-dry-run validation advances.
- build/link/runtime reproducibility release-candidate and replay dry-run governance shall preserve explicit
  lane-D dependency anchors (`M245-D013`) and fail closed on release-candidate and replay dry-run evidence drift
  before runtime reproducibility advanced-core-workpack validation advances.
- build/link/runtime reproducibility advanced core workpack (shard 1) governance shall preserve explicit
  lane-D dependency anchors (`M245-D014`) and fail closed on advanced core workpack evidence drift
  before runtime reproducibility advanced-edge-compatibility-workpack validation advances.
- build/link/runtime reproducibility advanced edge compatibility workpack (shard 1) governance shall preserve explicit
  lane-D dependency anchors (`M245-D015`) and fail closed on advanced edge compatibility evidence drift
  before runtime reproducibility advanced-diagnostics-workpack validation advances.
- build/link/runtime reproducibility advanced diagnostics workpack (shard 1) governance shall preserve explicit
  lane-D dependency anchors (`M245-D016`) and fail closed on advanced diagnostics evidence drift
  before runtime reproducibility advanced-conformance-workpack validation advances.
- build/link/runtime reproducibility advanced conformance workpack (shard 1) governance shall preserve explicit
  lane-D dependency anchors (`M245-D017`) and fail closed on advanced conformance evidence drift
  before runtime reproducibility advanced-integration-workpack validation advances.
- build/link/runtime reproducibility advanced integration workpack (shard 1) governance shall preserve explicit
  lane-D dependency anchors (`M245-D018`) and fail closed on advanced integration evidence drift
  before runtime reproducibility advanced-performance-workpack validation advances.
- build/link/runtime reproducibility advanced performance workpack (shard 1) governance shall preserve explicit
  lane-D dependency anchors (`M245-D019`) and fail closed on advanced performance evidence drift
  before runtime reproducibility milestone closeout validation advances.
- portability gate/release checklist contract and architecture freeze wiring shall
  preserve lane-E dependency freeze anchors (`M245-A001`, `M245-B001`,
  `M245-C001`, and `M245-D001`) and fail closed on lane handoff drift.
- portability gate/release checklist modular split/scaffolding wiring shall preserve
  explicit lane-E dependency anchors (`M245-E001`, `M245-A002`, `M245-B002`,
  `M245-C002`, and `M245-D002`) and fail closed on modular split handoff drift.
- portability gate/release checklist core feature implementation wiring shall preserve
  explicit lane-E dependency anchors (`M245-E002`, `M245-A001`, `M245-B001`,
  `M245-C002`, and `M245-D002`) and fail closed on core-feature handoff drift.
- portability gate/release checklist core feature expansion wiring shall preserve
  explicit lane-E dependency anchors (`M245-E003`, `M245-A002`, `M245-B002`,
  `M245-C002`, and `M245-D003`) and fail closed on core-feature expansion
  handoff drift.
- portability gate/release checklist edge-case and compatibility completion wiring shall preserve
  explicit lane-E dependency anchors (`M245-E004`, `M245-A002`, `M245-B002`,
  `M245-C003`, and `M245-D004`) and fail closed on edge-case and compatibility completion
  handoff drift.
- portability gate/release checklist edge-case expansion and robustness wiring shall preserve
  explicit lane-E dependency anchors (`M245-E005`, `M245-A002`, `M245-B003`,
  `M245-C003`, and `M245-D004`) and fail closed on edge-case expansion and robustness
  handoff drift.
- portability gate/release checklist diagnostics hardening wiring shall preserve
  explicit lane-E dependency anchors (`M245-E006`, `M245-A003`, `M245-B003`,
  `M245-C004`, and `M245-D005`) and fail closed on diagnostics hardening
  handoff drift.
- portability gate/release checklist recovery and determinism hardening wiring shall preserve
  explicit lane-E dependency anchors (`M245-E007`, `M245-A003`, `M245-B004`,
  `M245-C004`, and `M245-D006`) and fail closed on recovery and determinism hardening
  handoff drift.
- portability gate/release checklist conformance matrix implementation wiring shall preserve
  explicit lane-E dependency anchors (`M245-E008`, `M245-A003`, `M245-B004`,
  `M245-C005`, and `M245-D007`) and fail closed on conformance matrix
  handoff drift.
- portability gate/release checklist conformance corpus expansion wiring shall preserve
  explicit lane-E dependency anchors (`M245-E009`, `M245-A004`, `M245-B004`,
  `M245-C006`, and `M245-D007`) and fail closed on conformance corpus
  handoff drift.
- portability gate/release checklist performance and quality guardrails wiring shall preserve
  explicit lane-E dependency anchors (`M245-E010`, `M245-A004`, `M245-B005`,
  `M245-C006`, and `M245-D008`) and fail closed on performance and quality
  handoff drift.
- portability gate/release checklist cross-lane integration sync wiring shall preserve
  explicit lane-E dependency anchors (`M245-E011`, `M245-A005`, `M245-B005`,
  `M245-C007`, and `M245-D009`) and fail closed on cross-lane integration sync
  handoff drift.
- portability gate/release checklist docs and operator runbook synchronization wiring shall preserve
  explicit lane-E dependency anchors (`M245-E012`, `M245-A005`, `M245-B006`,
  `M245-C007`, and `M245-D009`) and fail closed on docs and operator runbook
  synchronization handoff drift.
- portability gate/release checklist release-candidate and replay dry-run wiring shall preserve
  explicit lane-E dependency anchors (`M245-E013`, `M245-A005`, `M245-B006`,
  `M245-C008`, and `M245-D010`) and fail closed on release-candidate and replay
  dry-run handoff drift.
- portability gate/release checklist advanced core workpack (shard 1) wiring shall preserve
  explicit lane-E dependency anchors (`M245-E014`, `M245-A006`, `M245-B007`,
  `M245-C008`, and `M245-D011`) and fail closed on advanced core workpack
  handoff drift.
- portability gate/release checklist advanced edge compatibility workpack (shard 1) wiring shall preserve
  explicit lane-E dependency anchors (`M245-E015`, `M245-A006`, `M245-B007`,
  `M245-C009`, and `M245-D012`) and fail closed on advanced edge compatibility
  handoff drift.
- portability gate/release checklist advanced diagnostics workpack (shard 1) wiring shall preserve
  explicit lane-E dependency anchors (`M245-E016`, `M245-A006`, `M245-B008`,
  `M245-C009`, and `M245-D012`) and fail closed on advanced diagnostics
  handoff drift.
- portability gate/release checklist advanced conformance workpack (shard 1) wiring shall preserve
  explicit lane-E dependency anchors (`M245-E017`, `M245-A007`, `M245-B008`,
  `M245-C010`, and `M245-D013`) and fail closed on advanced conformance
  handoff drift.
- portability gate/release checklist advanced integration workpack (shard 1) wiring shall preserve
  explicit lane-E dependency anchors (`M245-E018`, `M245-A007`, `M245-B009`,
  `M245-C010`, and `M245-D014`) and fail closed on advanced integration
  handoff drift.
- portability gate/release checklist advanced performance workpack (shard 1) wiring shall preserve
  explicit lane-E dependency anchors (`M245-E019`, `M245-A008`, `M245-B009`,
  `M245-C011`, and `M245-D014`) and fail closed on advanced performance
  handoff drift.
- frontend optimization hint capture governance shall preserve explicit
  deterministic lane-A parser/AST hint-capture anchors and fail closed on optimization hint drift
  before optimizer pipeline integration and invariants validation advances.
- frontend optimization hint capture modular split/scaffolding governance shall preserve explicit
  lane-A dependency anchors (`M246-A001`) and fail closed on scaffolding evidence drift
  before optimizer pipeline modular split/scaffolding advances.
- semantic invariants for optimization legality governance shall preserve explicit
  deterministic lane-B semantic legality anchors and fail closed on optimization legality drift
  before optimizer pipeline integration and invariants validation advances.
- semantic invariants for optimization legality modular split/scaffolding governance shall preserve explicit
  lane-B dependency anchors (`M246-B001`) and fail closed on modular split handoff drift
  before optimizer semantic legality scaffolding advances.
- IR optimization pass wiring and validation governance shall preserve explicit
  deterministic lane-C pass-wiring anchors and fail closed on optimizer IR validation drift
  before optimizer pipeline integration and invariants validation advances.
- IR optimization pass wiring and validation modular split/scaffolding governance shall preserve explicit
  lane-C dependency anchors (`M246-C001`) and fail closed on modular split handoff drift
  before optimizer pass-wiring scaffolding advances.
- toolchain integration and optimization controls governance shall preserve explicit
  deterministic lane-D control anchors and fail closed on toolchain optimization control drift
  before optimizer pipeline integration and invariants validation advances.
- optimization gate and perf evidence contract-freeze wiring shall preserve
  explicit lane-E dependency anchors (`M246-A001`, `M246-B001`, `M246-C002`,
  and `M246-D001`) and fail closed on contract-freeze handoff drift.
- optimization gate and perf evidence modular split/scaffolding wiring shall preserve
  explicit lane-E dependency anchors (`M246-E001`, `M246-A002`, `M246-B002`,
  `M246-C004`, and `M246-D002`) and fail closed on modular split handoff drift.
- optimization gate and perf evidence core feature implementation wiring shall preserve
  explicit lane-E dependency anchors (`M246-E002`, `M246-A002`, `M246-B003`,
  `M246-C005`, and `M246-D002`) and fail closed on core-feature handoff drift.
- optimization gate and perf evidence core feature expansion wiring shall preserve
  explicit lane-E dependency anchors (`M246-E003`, `M246-A003`, `M246-B004`,
  `M246-C007`, and `M246-D003`) and fail closed on expansion handoff drift.
- optimization gate and perf evidence edge-case and compatibility completion wiring shall preserve
  explicit lane-E dependency anchors (`M246-E004`, `M246-A004`, `M246-B005`,
  `M246-C009`, and `M246-D004`) and fail closed on compatibility-completion handoff drift.
- optimization gate and perf evidence edge-case expansion and robustness wiring shall preserve
  explicit lane-E dependency anchors (`M246-E005`, `M246-A005`, `M246-B006`,
  `M246-C011`, and `M246-D005`) and fail closed on robustness handoff drift.
- semantic compatibility and migration checks core feature implementation shall
  preserve explicit lane-B dependency anchors (`M249-B002`) and fail closed on core-feature evidence drift
  before semantic stability expansion validation advances.
- replay harness/artifact governance shall preserve explicit lane-C artifact
  replay anchors and fail closed on replay evidence drift before execution and
  conformance replay validation advances.
- replay harness/artifact modular split scaffolding shall preserve explicit
  lane-C dependency anchors (`M248-C001`) and fail closed on modular split
  evidence drift before replay readiness advances.
- IR/object packaging and symbol policy governance shall preserve explicit
  lane-C object packaging and symbol policy anchors and fail closed on
  artifact packaging or symbol-policy drift before execution and conformance
  replay validation advances.
- IR/object packaging and symbol policy modular split/scaffolding shall preserve explicit
  lane-C dependency anchors (`M249-C001`) and fail closed on modular split
  evidence drift before execution and conformance replay validation advances.
- IR/object packaging and symbol policy core feature implementation shall preserve explicit
  lane-C dependency anchors (`M249-C001`, `M249-C002`) and fail closed on core-feature
  evidence drift before execution and conformance replay validation advances.
- installer/runtime operations and support tooling governance shall preserve
  explicit lane-D installer/runtime operation anchors and fail closed on
  support-tooling replay drift before architecture freeze readiness advances.
- installer/runtime operations and support tooling modular split scaffolding shall
  preserve explicit lane-D dependency anchors (`M249-D001`) and fail closed on
  scaffolding evidence drift before architecture freeze readiness advances.
- installer/runtime operations and support tooling core feature implementation shall
  preserve explicit lane-D dependency anchors (`M249-D002`) and fail closed on
  core-feature evidence drift before architecture freeze readiness advances.
- installer/runtime operations and support tooling core feature expansion shall
  preserve explicit lane-D dependency anchors (`M249-D003`) and fail closed on
  core-feature expansion evidence drift before edge-case compatibility completion readiness advances.
- runtime metadata and lookup plumbing governance shall preserve
  explicit lane-D installer/runtime operation anchors and fail closed on
  lookup-plumbing replay drift before architecture freeze readiness advances.
- runtime metadata and lookup plumbing modular split scaffolding shall
  preserve explicit lane-D dependency anchors (`M233-D001`) and fail closed on
  scaffolding evidence drift before architecture freeze readiness advances.
- runtime metadata and lookup plumbing core feature implementation shall
  preserve explicit lane-D dependency anchors (`M233-D002`) and fail closed on
  core-feature evidence drift before architecture freeze readiness advances.
- runtime metadata and lookup plumbing core feature expansion shall
  preserve explicit lane-D dependency anchors (`M233-D003`) and fail closed on
  core-feature expansion evidence drift before edge-case compatibility completion readiness advances.
- runtime metadata and lookup plumbing release-candidate replay dry-run governance shall preserve
  explicit lane-D dependency anchors (`M233-D014`, `M233-D013`) and fail closed on
  release/replay command sequencing or replay evidence continuity drift before lane-D closeout readiness advances.
- runtime metadata and lookup plumbing advanced core workpack (shard 1) governance shall preserve
  explicit lane-D dependency anchors (`M233-D015`, `M233-D014`) and fail closed on
  advanced-core evidence continuity drift before lane-D closeout readiness advances.
- runtime metadata and lookup plumbing advanced edge compatibility workpack (shard 1) governance shall preserve
  explicit lane-D dependency anchors (`M233-D016`, `M233-D015`) and fail closed on
  advanced edge-compatibility evidence continuity drift before lane-D closeout readiness advances.
- runtime metadata and lookup plumbing advanced diagnostics workpack (shard 1) governance shall preserve
  explicit lane-D dependency anchors (`M233-D017`, `M233-D016`) and fail closed on
  advanced diagnostics evidence continuity drift before lane-D closeout readiness advances.
- runtime metadata and lookup plumbing advanced conformance workpack (shard 1) governance shall preserve
  explicit lane-D dependency anchors (`M233-D018`, `M233-D017`) and fail closed on
  advanced conformance evidence continuity drift before lane-D closeout readiness advances.
- runtime metadata and lookup plumbing advanced integration workpack (shard 1) governance shall preserve
  explicit lane-D dependency anchors (`M233-D019`, `M233-D018`) and fail closed on
  advanced integration evidence continuity drift before lane-D closeout readiness advances.
- runtime metadata and lookup plumbing advanced performance workpack (shard 1) governance shall preserve
  explicit lane-D dependency anchors (`M233-D020`, `M233-D019`) and fail closed on
  advanced performance evidence continuity drift before lane-D closeout readiness advances.
- runtime metadata and lookup plumbing advanced edge compatibility workpack (shard 2) governance shall preserve
  explicit lane-D dependency anchors (`M233-D022`, `M233-D021`) and fail closed on
  advanced edge-compatibility evidence continuity drift before lane-D closeout readiness advances.
- runtime metadata and lookup plumbing advanced diagnostics workpack (shard 2) governance shall preserve
  explicit lane-D dependency anchors (`M233-D023`, `M233-D022`) and fail closed on
  advanced diagnostics evidence continuity drift before lane-D closeout readiness advances.
- runtime metadata and lookup plumbing advanced conformance workpack (shard 2) governance shall preserve
  explicit lane-D dependency anchors (`M233-D024`, `M233-D023`) and fail closed on
  advanced conformance evidence continuity drift before lane-D closeout readiness advances.
- runtime metadata and lookup plumbing advanced integration workpack (shard 2) governance shall preserve
  explicit lane-D dependency anchors (`M233-D025`, `M233-D024`) and fail closed on
  advanced integration evidence continuity drift before lane-D closeout readiness advances.
- runtime metadata and lookup plumbing advanced performance workpack (shard 2) governance shall preserve
  explicit lane-D dependency anchors (`M233-D026`, `M233-D025`) and fail closed on
  advanced performance evidence continuity drift before lane-D closeout readiness advances.
- runtime metadata and lookup plumbing advanced core workpack (shard 3) governance shall preserve
  explicit lane-D dependency anchors (`M233-D027`, `M233-D026`) and fail closed on
  advanced core evidence continuity drift before lane-D closeout readiness advances.
- runtime metadata and lookup plumbing integration closeout and gate sign-off governance shall preserve
  explicit lane-D dependency anchors (`M233-D028`, `M233-D027`) and fail closed on
  integration-closeout evidence continuity drift before lane-D completion sign-off.
- conformance corpus and gate closeout contract and architecture freeze wiring shall preserve
  explicit lane-E dependency anchors (`M233-A001`, `M233-B001`, `M233-C001`, and
  `M233-D002`) and fail closed on dependency tokens, package readiness hooks, or
  lane-E conformance corpus and gate closeout evidence drift.
- conformance corpus and gate closeout modular split/scaffolding wiring shall preserve
  explicit lane-E dependency anchors (`M233-E001`, `M233-A001`, `M233-B002`,
  `M233-C003`, and `M233-D003`) and fail closed on dependency tokens,
  dependency-reference commands, or lane-E modular split/scaffolding evidence drift.
- conformance corpus and gate closeout core feature implementation wiring shall preserve
  explicit lane-E dependency anchors (`M233-E002`, `M233-A002`, `M233-B003`,
  `M233-C004`, and `M233-D005`) and fail closed on dependency tokens,
  dependency-reference commands, or lane-E core-feature evidence drift.
- conformance corpus and gate closeout core feature expansion wiring shall preserve
  explicit lane-E dependency anchors (`M233-E003`, `M233-A003`, `M233-B004`,
  `M233-C005`, and `M233-D007`) and fail closed on dependency tokens,
  dependency-reference commands, or lane-E core-feature expansion evidence drift.
- installer/runtime operations and support tooling release-candidate replay dry-run governance shall preserve
  explicit lane-D dependency anchors (`M249-D014`, `M249-D013`) and fail closed on
  release/replay command sequencing or replay evidence continuity drift before lane-D closeout readiness advances.
- installer/runtime operations and support tooling advanced core workpack (shard 1) governance shall preserve
  explicit lane-D dependency anchors (`M249-D015`, `M249-D014`) and fail closed on
  advanced-core evidence continuity drift before lane-D closeout readiness advances.
- installer/runtime operations and support tooling advanced edge compatibility workpack (shard 1) governance shall preserve
  explicit lane-D dependency anchors (`M249-D016`, `M249-D015`) and fail closed on
  advanced edge-compatibility evidence continuity drift before lane-D closeout readiness advances.
- installer/runtime operations and support tooling advanced diagnostics workpack (shard 1) governance shall preserve
  explicit lane-D dependency anchors (`M249-D017`, `M249-D016`) and fail closed on
  advanced diagnostics evidence continuity drift before lane-D closeout readiness advances.
- installer/runtime operations and support tooling advanced conformance workpack (shard 1) governance shall preserve
  explicit lane-D dependency anchors (`M249-D018`, `M249-D017`) and fail closed on
  advanced conformance evidence continuity drift before lane-D closeout readiness advances.
- installer/runtime operations and support tooling advanced integration workpack (shard 1) governance shall preserve
  explicit lane-D dependency anchors (`M249-D019`, `M249-D018`) and fail closed on
  advanced integration evidence continuity drift before lane-D closeout readiness advances.
- installer/runtime operations and support tooling integration closeout and gate sign-off governance shall preserve
  explicit lane-D dependency anchors (`M249-D020`, `M249-D019`) and fail closed on
  integration closeout/sign-off evidence continuity drift before lane-D closeout readiness advances.
- CLI/reporting and output contract integration governance shall preserve
  explicit lane-D diagnostics UX and fix-it engine reporting anchors and fail
  closed on summary payload, diagnostics artifact path, or stage-report output
  contract drift before lane-E integration readiness advances.
- CLI/reporting and output modular split scaffolding governance shall preserve
  explicit lane-D dependency anchors (`M243-D001`) and fail closed on
  scaffold key, summary mode, diagnostics schema, or stage-report output
  contract drift before lane-E modular split readiness advances.
- CLI/reporting and output core feature implementation governance shall preserve
  explicit lane-D dependency anchors (`M243-D002`) and fail closed on
  core-feature key continuity, diagnostics output path determinism, summary
  output path determinism, or stage-report output contract drift before lane-E
  core-feature readiness advances.
- CLI/reporting and output core feature expansion governance shall preserve
  explicit lane-D dependency anchors (`M243-D003`) and fail closed on
  core-feature expansion key continuity, summary/diagnostics output payload
  consistency, diagnostics emit-prefix filename contract continuity, or
  stage-report output contract drift before lane-E core-feature expansion
  readiness advances.
- CLI/reporting and output edge-case compatibility completion governance shall preserve
  explicit lane-D dependency anchors (`M243-D004`) and fail closed on
  edge-case compatibility key continuity, case-folded output-path distinctness,
  output-path control-character hygiene, or summary/diagnostics compatibility
  contract drift before lane-E edge-case readiness advances.
- CLI/reporting and output edge-case expansion and robustness governance shall preserve
  explicit lane-D dependency anchors (`M243-D005`) and fail closed on
  edge-case robustness key continuity, output-path length-budget continuity,
  output-path trailing-space hygiene, or summary/diagnostics expansion
  continuity drift before lane-E robustness readiness advances.
- CLI/reporting and output recovery and determinism hardening governance shall preserve
  explicit lane-D dependency anchors (`M243-D007`) and fail closed on
  recovery/determinism key continuity, diagnostics-hardening continuity, or
  summary/diagnostics output replay drift before lane-E recovery readiness
  advances.
- CLI/reporting and output conformance matrix implementation governance shall preserve
  explicit lane-D dependency anchors (`M243-D008`) and fail closed on
  conformance-matrix key continuity, recovery/determinism continuity, or
  summary/diagnostics output contract drift before lane-E recovery readiness
  advances.
- CLI/reporting and output conformance corpus expansion governance shall preserve
  explicit lane-D dependency anchors (`M243-D009`) and fail closed on
  conformance-corpus key continuity, conformance-corpus case-accounting
  continuity, or summary/diagnostics output contract drift before lane-E
  recovery readiness advances.
- CLI/reporting and output performance and quality guardrails governance shall preserve
  explicit lane-D dependency anchors (`M243-D010`) and fail closed on
  performance/quality guardrails consistency/readiness, guardrails-key
  continuity, or summary/diagnostics output contract drift before lane-E
  recovery readiness advances.
- CLI/reporting and output cross-lane integration sync governance shall preserve
  explicit lane-D dependency anchors (`M243-D011`) and fail closed on
  cross-lane-integration-sync consistency/readiness,
  cross-lane-integration-sync-key continuity, or summary/diagnostics output
  contract drift before lane-E recovery readiness advances.
- CLI/reporting and output docs and operator runbook synchronization governance shall preserve
  explicit lane-D dependency anchors (`M243-D012`) and fail closed on
  docs-runbook-synchronization consistency/readiness,
  docs-runbook-synchronization-key continuity, or summary/diagnostics output
  contract drift before lane-E recovery readiness advances.
- release gate/docs/runbooks contract and architecture freeze wiring shall
  preserve explicit lane-E dependency anchors (`M249-A001`, `M249-B001`,
  `M249-C001`, `M249-D001`) and fail closed when dependency references,
  docs/runbook evidence commands, or release gate readiness hooks drift.
- release gate/docs/runbooks modular split/scaffolding wiring shall preserve
  explicit lane-E dependency anchors (`M249-E001`, `M249-A002`, `M249-B002`,
  `M249-C002`, and `M249-D002`) and fail closed when dependency references,
  docs/runbook modular split/scaffolding evidence commands, or release gate
  readiness hooks drift.
- release gate/docs/runbooks core feature implementation wiring shall preserve
  explicit lane-E dependency anchors (`M249-E002`, `M249-A003`, `M249-B003`,
  `M249-C003`, and `M249-D003`) and fail closed when dependency references,
  docs/runbook core-feature evidence commands, or release gate readiness hooks drift.
- release gate/docs/runbooks advanced core workpack (shard 1) wiring shall preserve
  explicit lane-E dependency anchors (`M249-E014`, `M249-A006`, `M249-B007`,
  `M249-C008`, and `M249-D015`) and fail closed when dependency references,
  docs/runbook advanced-core evidence commands, or release gate readiness hooks drift.
- release gate/docs/runbooks advanced edge compatibility workpack (shard 1) governance shall preserve
  explicit lane-E dependency anchors (`M249-E016`, `M249-E015`) and fail closed on
  advanced edge-compatibility command sequencing, evidence-path continuity, or
  release gate/docs/runbooks advanced edge-compatibility continuity drift before lane-E readiness advances.
- release gate/docs/runbooks advanced diagnostics workpack (shard 1) governance shall preserve
  explicit lane-E dependency anchors (`M249-E017`, `M249-E016`, `M249-A007`, `M249-B008`,
  `M249-C009`, and `M249-D017`) and fail closed on
  advanced diagnostics command sequencing, dependency evidence continuity, or
  release gate/docs/runbooks advanced diagnostics continuity drift before lane-E readiness advances.
- release gate/docs/runbooks advanced conformance workpack (shard 1) governance shall preserve
  explicit lane-E dependency anchors (`M249-E018`, `M249-E017`, `M249-A007`, `M249-B008`,
  `M249-C009`, and `M249-D015`) and fail closed on
  advanced conformance command sequencing, dependency evidence continuity, or
  release gate/docs/runbooks advanced conformance continuity drift before lane-E readiness advances.
- release gate/docs/runbooks advanced integration workpack (shard 1) governance shall preserve
  explicit lane-E dependency anchors (`M249-E019`, `M249-E018`, `M249-A007`, `M249-B009`,
  `M249-C010`, and `M249-D016`) and fail closed on
  advanced integration command sequencing, dependency evidence continuity, or
  release gate/docs/runbooks advanced integration continuity drift before lane-E readiness advances.
- release gate/docs/runbooks advanced performance workpack (shard 1) governance shall preserve
  explicit lane-E dependency anchors (`M249-E020`, `M249-E019`, `M249-A008`, `M249-B009`,
  `M249-C010`, and `M249-D017`) and fail closed on
  advanced performance command sequencing, dependency evidence continuity, or
  release gate/docs/runbooks advanced performance continuity drift before lane-E readiness advances.
- release gate/docs/runbooks advanced core workpack (shard 2) governance shall preserve
  explicit lane-E dependency anchors (`M249-E021`, `M249-E020`, `M249-A008`, `M249-B010`,
  `M249-C011`, and `M249-D018`) and fail closed on
  advanced core (shard 2) command sequencing, dependency evidence continuity, or
  release gate/docs/runbooks advanced core (shard 2) continuity drift before lane-E readiness advances.
- release gate/docs/runbooks advanced edge compatibility workpack (shard 2) governance shall preserve
  explicit lane-E dependency anchors (`M249-E022`, `M249-E021`, `M249-A008`, `M249-B010`,
  `M249-C011`, and `M249-D018`) and fail closed on
  advanced edge compatibility (shard 2) command sequencing, dependency evidence continuity, or
  release gate/docs/runbooks advanced edge compatibility (shard 2) continuity drift before lane-E readiness advances.
- release gate/docs/runbooks advanced diagnostics workpack (shard 2) governance shall preserve
  explicit lane-E dependency anchors (`M249-E023`, `M249-E022`, `M249-A009`, `M249-B011`,
  `M249-C012`, and `M249-D019`) and fail closed on
  advanced diagnostics (shard 2) command sequencing, dependency evidence continuity, or
  release gate/docs/runbooks advanced diagnostics (shard 2) continuity drift before lane-E readiness advances.
- release gate/docs/runbooks integration closeout and gate signoff governance shall preserve
  explicit lane-E dependency anchors (`M249-E024`, `M249-E023`, `M249-A009`, `M249-B011`,
  `M249-C012`, and `M249-D020`) and fail closed on
  integration closeout/gate-signoff command sequencing, dependency evidence continuity, or
  release gate/docs/runbooks integration closeout/gate-signoff continuity drift before lane-E readiness advances.
- diagnostics quality gate and replay policy wiring shall preserve explicit
  lane-E dependency anchors (`M243-A001`, `M243-B001`, `M243-C001`, and
  `M243-D001`) and fail closed when dependency references,
  diagnostics/replay evidence commands, or lane-E readiness hooks drift.
- diagnostics quality gate and replay policy modular split/scaffolding wiring shall preserve explicit
  lane-E dependency anchors (`M243-E001`, `M243-A001`, `M243-B001`, `M243-C001`, and
  `M243-D001`) and fail closed when dependency references,
  diagnostics/replay modular split evidence commands, or lane-E readiness hooks drift.
- diagnostics quality gate and replay policy core feature implementation wiring shall preserve explicit
  lane-E dependency anchors (`M243-E002`, `M243-A003`, `M243-B003`, `M243-C002`, and
  `M243-D002`) and fail closed when dependency references,
  diagnostics/replay core-feature evidence commands, or lane-E readiness hooks drift.
- diagnostics quality gate and replay policy core feature expansion wiring shall preserve explicit
  lane-E dependency anchors (`M243-E003`, `M243-A004`, `M243-B004`, `M243-C003`, and
  `M243-D003`) and fail closed when dependency references,
  diagnostics/replay core-feature expansion evidence commands, or lane-E readiness hooks drift.
- diagnostics quality gate and replay policy edge-case and compatibility completion wiring shall preserve explicit
  lane-E dependency anchors (`M243-E004`, `M243-A005`, `M243-B005`, `M243-C005`, and
  `M243-D005`) and fail closed when dependency references,
  diagnostics/replay edge-case compatibility evidence commands, or lane-E readiness hooks drift.
- diagnostics quality gate and replay policy edge-case expansion and robustness wiring shall preserve explicit
  lane-E dependency anchors (`M243-E005`, `M243-A002`, `M243-B003`, `M243-C003`, and
  `M243-D004`) and fail closed when dependency references,
  diagnostics/replay edge-case expansion and robustness evidence commands, or lane-E readiness hooks drift.
- diagnostics quality gate and replay policy diagnostics hardening wiring shall preserve explicit
  lane-E dependency anchors (`M243-E006`, `M243-A003`, `M243-B003`, `M243-C004`, and
  `M243-D005`) and fail closed when dependency references,
  diagnostics/replay diagnostics hardening evidence commands, or lane-E readiness hooks drift.
- diagnostics quality gate and replay policy recovery and determinism hardening wiring shall preserve explicit
  lane-E dependency anchors (`M243-E007`, `M243-A003`, `M243-B004`, `M243-C004`, and
  `M243-D006`) and fail closed when dependency references,
  diagnostics/replay recovery and determinism hardening evidence commands, or lane-E readiness hooks drift.
- diagnostics quality gate and replay policy conformance matrix implementation wiring shall preserve explicit
  lane-E dependency anchors (`M243-E008`, `M243-A003`, `M243-B004`, `M243-C005`, and
  `M243-D006`) and fail closed when dependency references,
  diagnostics/replay conformance matrix implementation evidence commands, or lane-E readiness hooks drift.
- diagnostics quality gate and replay policy conformance corpus expansion wiring shall preserve explicit
  lane-E dependency anchors (`M243-E009`, `M243-A004`, `M243-B005`, `M243-C005`, and
  `M243-D007`) and fail closed when dependency references,
  diagnostics/replay conformance corpus expansion evidence commands, or lane-E readiness hooks drift.
- diagnostics quality gate and replay policy performance and quality guardrails wiring shall preserve explicit
  lane-E dependency anchors (`M243-E010`, `M243-A004`, `M243-B005`, `M243-C006`, and
  `M243-D008`) and fail closed when dependency references,
  diagnostics/replay performance and quality guardrail evidence commands, or lane-E readiness hooks drift.
- diagnostics quality gate and replay policy cross-lane integration sync wiring shall preserve explicit
  lane-E dependency anchors (`M243-E011`, `M243-A012`, `M243-B012`, `M243-C011`, and
  `M243-D012`) and fail closed when dependency references,
  diagnostics/replay cross-lane integration sync evidence commands, or lane-E readiness hooks drift.
- lowering/runtime diagnostics surfacing modular split scaffolding shall
  preserve explicit lane-C dependency anchors (`M243-C001`) and fail closed on
  scaffolding evidence drift before diagnostics quality gate and replay policy
  validation advances.
- lowering/runtime diagnostics surfacing core feature implementation shall
  preserve explicit lane-C dependency anchors (`M243-C002`) and fail closed on
  core-feature evidence drift before diagnostics quality gate and replay policy
  validation advances.
- lowering/runtime diagnostics surfacing core feature expansion shall preserve
  explicit lane-C dependency anchors (`M243-C003`) and fail closed on
  core-feature expansion evidence drift before diagnostics quality gate and
  replay policy validation advances.
- lowering/runtime diagnostics surfacing edge-case compatibility completion shall preserve
  explicit lane-C dependency anchors (`M243-C004`) and fail closed on
  compatibility continuity, edge-case surface readiness, or replay-key
  transport drift before diagnostics quality gate and replay policy validation
  advances.
- lowering/runtime diagnostics surfacing edge-case expansion and robustness shall preserve
  explicit lane-C dependency anchors (`M243-C005`) and fail closed on
  expansion consistency, robustness readiness, or robustness-key transport
  drift before diagnostics quality gate and replay policy validation advances.
- lowering/runtime diagnostics surfacing diagnostics hardening shall preserve
  explicit lane-C dependency anchors (`M243-C006`) and fail closed on
  diagnostics-hardening consistency, diagnostics-hardening readiness, or
  diagnostics-hardening-key continuity drift before diagnostics quality gate
  and replay policy validation advances.
- lowering/runtime diagnostics surfacing recovery and determinism hardening shall preserve
  explicit lane-C dependency anchors (`M243-C007`) and fail closed on
  recovery-determinism consistency, recovery-determinism readiness, or
  recovery-determinism-key continuity drift before diagnostics quality gate
  and replay policy validation advances.
- lowering/runtime diagnostics surfacing conformance matrix implementation shall preserve
  explicit lane-C dependency anchors (`M243-C008`) and fail closed on
  conformance-matrix consistency, conformance-matrix readiness, or
  conformance-matrix-key continuity drift before diagnostics quality gate and
  replay policy validation advances.
- lowering/runtime diagnostics surfacing conformance corpus expansion shall preserve
  explicit lane-C dependency anchors (`M243-C009`) and fail closed on
  conformance-corpus consistency, conformance-corpus readiness, or
  conformance-corpus-key continuity drift before diagnostics quality gate and
  replay policy validation advances.
- lowering/runtime diagnostics surfacing performance and quality guardrails shall preserve
  explicit lane-C dependency anchors (`M243-C010`) and fail closed on
  performance/quality guardrails consistency/readiness, guardrails-key
  continuity, or diagnostics surfacing evidence contract drift before lane-C
  readiness advances.
- lowering/runtime diagnostics surfacing cross-lane integration sync shall preserve
  explicit lane-C dependency anchors (`M243-C011`) and fail closed on
  cross-lane-integration-sync consistency/readiness,
  cross-lane-integration-sync-key continuity, or diagnostics surfacing
  evidence contract drift before lane-C readiness advances.
- diagnostic grammar hooks and source precision recovery and determinism
  hardening shall preserve lane-A dependency anchors (`M243-A007`) and fail
  closed on parser diagnostic grammar-hook recovery-determinism consistency,
  readiness, or replay-key continuity drift before conformance matrix
  expansions advance.
- diagnostic grammar hooks and source precision performance and quality guardrails shall preserve lane-A dependency anchors (`M243-A010`)
  and fail closed on readiness-chain continuity, architecture/spec anchor
  continuity, or evidence-summary continuity drift before downstream lane-A
  integration closures advance.
- diagnostic grammar hooks and source precision integration closeout and gate sign-off shall preserve lane-A dependency anchors (`M243-A011`)
  and fail closed on readiness-chain continuity, architecture/spec anchor
  continuity, or sign-off evidence continuity drift before downstream lane-A
  integration-closeout gates advance.
- semantic diagnostic taxonomy and fix-it synthesis core feature implementation
  shall preserve lane-B dependency anchors (`M243-B002`) and fail closed on
  core-feature evidence drift before diagnostics hardening and conformance
  expansions advance.
- semantic diagnostic taxonomy and fix-it synthesis core feature expansion
  shall preserve lane-B dependency anchors (`M243-B003`) and fail closed on
  typed handoff-key continuity, replay-key readiness, or payload accounting
  drift before diagnostics hardening and conformance expansions advance.
- semantic diagnostic taxonomy and fix-it synthesis edge-case compatibility completion
  shall preserve lane-B dependency anchors (`M243-B004`) and fail closed on
  compatibility-handoff continuity, parse edge robustness continuity, or replay
  determinism drift before diagnostics hardening and conformance expansions advance.
- semantic diagnostic taxonomy and fix-it synthesis edge-case expansion and robustness
  shall preserve lane-B dependency anchors (`M243-B005`) and fail closed on
  edge-case expansion consistency, robustness readiness, or robustness-key
  continuity drift before diagnostics hardening and conformance expansions advance.
- semantic diagnostic taxonomy and fix-it synthesis diagnostics hardening
  shall preserve lane-B dependency anchors (`M243-B006`) and fail closed on
  diagnostics-hardening consistency, diagnostics-hardening readiness, or
  diagnostics-hardening-key continuity drift before recovery and conformance expansions advance.
- semantic diagnostic taxonomy and fix-it synthesis recovery and determinism hardening
  shall preserve lane-B dependency anchors (`M243-B007`) and fail closed on
  recovery-determinism consistency, recovery-determinism readiness, or
  recovery-determinism-key continuity drift before conformance expansions advance.
- semantic diagnostic taxonomy and fix-it synthesis conformance corpus expansion
  shall preserve lane-B dependency anchors (`M243-B009`) and fail closed on
  conformance-corpus consistency, conformance-corpus readiness, or
  conformance-corpus-key continuity drift before downstream lane-B guardrail
  expansions advance.
- semantic diagnostic taxonomy and fix-it synthesis performance and quality guardrails
  shall preserve lane-B dependency anchors (`M243-B010`) and fail closed on
  performance-quality-guardrails consistency, performance-quality-guardrails readiness,
  or performance-quality-guardrails-key continuity drift before downstream lane-B
  integration closures advance.
- semantic diagnostic taxonomy and fix-it synthesis cross-lane integration sync
  shall preserve lane-B dependency anchors (`M243-B011`) and fail closed on
  cross-lane-integration-sync consistency, cross-lane-integration-sync readiness,
  or cross-lane-integration-sync-key continuity drift before downstream lane-B
  integration closures advance.
- semantic diagnostic taxonomy and fix-it synthesis docs and operator runbook synchronization
  shall preserve lane-B dependency anchors (`M243-B012`) and fail closed on
  docs-runbook-synchronization consistency, docs-runbook-synchronization readiness,
  or docs-runbook-synchronization-key continuity drift before downstream lane-B
  integration closures advance.
- semantic diagnostic taxonomy and fix-it synthesis integration closeout and gate sign-off
  shall preserve lane-B dependency anchors (`M243-B013`) and fail closed on
  integration-closeout consistency, integration-closeout readiness,
  integration-closeout-signoff-key continuity, or downstream lane-B readiness
  chain drift before cross-lane release integration advances.
- runner/platform operations governance shall preserve explicit lane-D
  compile-route anchors and fail closed on platform replay drift before
  performance budget and execution replay validation advances.
- runner/platform operations modular split scaffolding shall preserve explicit
  lane-D dependency anchors (`M248-D001`) and fail closed on scaffolding
  evidence drift before platform replay readiness advances.
- runner/platform operations core feature implementation shall preserve
  explicit lane-D dependency anchors (`M248-D002`) and fail closed on
  nullable-tool-path safety drift before runner/platform lane-D readiness
  advances.
- runner/platform operations core feature expansion governance shall preserve explicit lane-D dependency anchors (`M248-D003`)
  and fail closed on core-feature expansion evidence drift before downstream
  platform replay and lane-e gate integration advances.
- runner/platform operations edge-case and compatibility completion governance
  shall preserve explicit lane-D dependency anchors (`M248-D004`) and fail
  closed on compatibility completion evidence drift before downstream platform
  replay and lane-e edge-case hardening advances.
- runner/platform operations edge-case expansion and robustness governance
  shall preserve explicit lane-D dependency anchors (`M248-D005`) and fail
  closed on robustness expansion evidence drift before downstream platform
  replay and lane-e diagnostics hardening advances.
- runner/platform operations diagnostics hardening governance
  shall preserve explicit lane-D dependency anchors (`M248-D006`) and fail
  closed on diagnostics hardening evidence drift before downstream platform
  replay and lane-e recovery and determinism hardening advances.
- runner/platform operations recovery and determinism hardening governance
  shall preserve explicit lane-D dependency anchors (`M248-D007`) and fail
  closed on recovery and determinism hardening evidence drift before downstream
  platform replay and lane-e conformance matrix advances.
- runner/platform operations conformance matrix implementation governance
  shall preserve explicit lane-D dependency anchors (`M248-D008`) and fail
  closed on conformance matrix implementation evidence drift before downstream
  platform replay and lane-e conformance corpus advances.
- runner/platform operations conformance corpus expansion governance
  shall preserve explicit lane-D dependency anchors (`M248-D009`) and fail
  closed on conformance corpus expansion evidence drift before downstream
  platform replay and lane-e performance guardrail advances.
- runner/platform operations performance and quality guardrails governance
  shall preserve explicit lane-D dependency anchors (`M248-D010`) and fail
  closed on performance and quality guardrails evidence drift before downstream
  platform replay and lane-e conformance matrix advances.
- runner/platform operations cross-lane integration sync governance
  shall preserve explicit lane-D dependency anchors (`M248-D011`) and fail
  closed on cross-lane integration sync evidence drift before downstream
  platform replay and lane-e conformance matrix advances.
- runner/platform operations docs and operator runbook synchronization governance shall preserve
  explicit lane-D dependency anchors (`M248-D012`) and fail closed on
  docs/runbook synchronization evidence drift before downstream platform replay
  and lane-e conformance matrix advances.
- runner/platform operations release-candidate replay dry-run governance shall preserve
  explicit lane-D dependency anchors (`M248-D013`) and fail closed on replay
  dry-run evidence drift before downstream platform replay and lane-e
  conformance matrix advances.
- runner/platform operations advanced core workpack (shard 1) governance shall preserve
  explicit lane-D dependency anchors (`M248-D014`) and fail closed on advanced
  core evidence drift before downstream platform replay and lane-e conformance
  matrix advances.
- runner/platform operations advanced edge compatibility workpack (shard 1) governance shall preserve
  explicit lane-D dependency anchors (`M248-D015`) and fail closed on advanced
  edge compatibility evidence drift before downstream platform replay and lane-e
  conformance matrix advances.
- runner/platform operations advanced diagnostics workpack (shard 1) governance shall preserve
  explicit lane-D dependency anchors (`M248-D016`) and fail closed on advanced
  diagnostics evidence drift before downstream platform replay and lane-e
  conformance matrix advances.

### C.3.1 Optional message send `[receiver? ...]` (normative) {#c-3-1}

Optional message send semantics are defined in [Part 3](#part-3). Conforming implementations shall lower:

```objc
[receiver? sel:arg1 other:arg2]
```

as if by the following abstract steps:

1. Evaluate `receiver` exactly once into a temporary.
2. If the receiver temporary is `nil`, produce the “nil case” result:
   - object/block return: `nil`
   - `void` return: no effect
3. Otherwise, evaluate argument expressions in source order and perform the message send.

**Key requirement:** argument expressions shall **not** be evaluated in the `nil` case.

### C.3.2 Nil-coalescing `??` (recommended) {#c-3-2}

`a ?? b` should lower to a single evaluation of `a` with a conditional branch.
Implementations should avoid duplicating retains/releases when `a` is an object pointer.

### C.3.3 Postfix propagation `?` (recommended) {#c-3-3}

The postfix propagation operator in Parts 3 and 6 should lower to a structured early-exit:

- `Optional<T>` carrier: `if (!x) return nil; else use *x;`
- `Result<T,E>` carrier: `if (isErr(x)) return Err(e); else use t;`

Implementations should preserve left-to-right evaluation and should not introduce hidden temporaries with observable lifetimes beyond what ARC already requires.

## C.4 `throws` ABI and lowering (normative for implementations) {#c-4}

### C.4.1 Canonical ABI shape for throwing functions (normative for implementations) {#c-4-1}

For a function declared:

```c
R f(A1 a1, A2 a2) throws;
```

a conforming implementation shall provide a stable calling convention that allows the caller to receive either:

- a normal return of type `R`, or
- an error value of type `id<Error>`.

**Canonical ABI (recommended and permitted as normative):** the implementation uses an additional trailing _error-out parameter_:

```c
R f(A1 a1, A2 a2, id<Error> _Nullable * _Nullable outError);
```

Rules:

- On success: if `outError != NULL`, store `nil` into `*outError`, then return the value.
- On error: if `outError != NULL`, store the thrown error into `*outError`, then return an unspecified value of type `R` (or zero-initialized as QoI).

> Note: For `void` return, the return value is omitted and the outError store alone indicates failure.

An implementation may choose a different ABI shape, but only if it is:

- stable under separate compilation, and
- representable in module metadata so importers use the same ABI.

### C.4.2 Throwing Objective‑C methods (recommended) {#c-4-2}

For Objective‑C methods, the same “trailing error-out parameter” approach is recommended: the selector’s parameter list includes the implicit trailing outError at the ABI level.
The language surface does not expose that parameter; it is introduced/consumed by the compiler as part of the `throws` effect.

### C.4.3 Lowering of `try` (recommended) {#c-4-3}

### C.4.4 Objective‑C selectors vs ABI signatures (normative intent) {#c-4-4}

For Objective‑C methods, the `throws` effect is **not** part of the selector spelling. The selector used for lookup remains the source-level selector.

However, the _call ABI_ of the method’s implementation may include the canonical trailing error-out parameter described in [C.4.1](#c-4-1).

A conforming implementation shall ensure that:

- when a call site is type-checked as calling a `throws` method, the generated call passes the error-out parameter in the canonical position required by that implementation’s ABI; and
- when forming or calling an `IMP` for a `throws` method (e.g., via `methodForSelector:`), the compiler uses a function pointer type whose parameter list includes the error-out parameter.

**Reflection note (non-normative):** baseline Objective‑C runtime type-encoding strings do not represent `throws`. This draft does not require extending the runtime type encoding in v1. Toolchains may provide extended metadata for reflective invocation as an extension.

A `try` call should lower to:

- allocate a local `id<Error> err = nil;`
- perform the call with `&err`
- if `err != nil`, branch to the enclosing error propagation/handler path.

`try?` and `try!` should be implemented as specified in [Part 6](#part-6), using the same underlying error value.

## C.5 `async` ABI and lowering (normative for implementations) {#c-5}

### C.5.1 Coroutine model (normative for implementations) {#c-5-1}

A conforming implementation shall implement `async` functions such that:

- each `await` is a potential suspension point,
- local variables required after an `await` have stable storage across suspension,
- ARC + cleanup semantics are preserved across suspension (see Parts 4 and 7).

**Recommended lowering:** lower `async` functions to LLVM coroutine state machines (e.g., using LLVM’s coroutine intrinsics), with resumption scheduled by the active executor.

### C.5.2 Task context (normative) {#c-5-2}

While executing an `async` function, there exists a _current task context_ that provides at least:

- cancellation state,
- current executor identity,
- and (if actors are used) current actor isolation context.

The concrete representation is implementation-defined, but the values must be queryable by the standard library and usable by the runtime to enforce executor hops.

### C.5.3 Executor hops (recommended) {#c-5-3}

When entering a declaration annotated with `objc_executor(X)`, implementations should:

- check whether the current executor is `X`;
- if not, suspend and schedule resumption on `X`.

Implementations should provide (directly or via the standard library) a primitive equivalent to:

- `await ExecutorHop(X)`

so that the compiler can lower hops in a uniform way.

### C.5.4 Call-site lowering for isolation boundaries (normative intent) {#c-5-4}

[Part 7](#part-7) defines `await` as the marker for **potential suspension** ([Decision D-011](#decisions-d-011)), not merely “calling explicitly-async functions.”

A conforming implementation shall treat the following as potentially-suspending at call sites when the relevant metadata indicates an isolation boundary is being crossed:

- entering an `objc_executor(X)` declaration from code not proven to already be on executor `X`;
- entering an actor-isolated member from outside that actor’s executor.

**Recommended lowering pattern (informative):**

1. If already on the required executor, call directly.
2. Otherwise:
   - suspend the current task,
   - enqueue the continuation onto the required executor (or actor executor),
   - resume and perform the call,
   - then continue.

This may be implemented either as:

- an implicit compiler-inserted hop around the call (still requiring `await` in the source), or
- a lowering through an explicit hop primitive plus a direct call.

The observable requirement is: the call **may suspend**, and the work executes on the correct executor.

## C.6 Actors: runtime representation and dispatch (normative for implementations) {#c-6}

### C.6.1 Actor executor (normative) {#c-6-1}

Each actor instance shall be associated with a serial executor used to protect isolated mutable state.
Cross-actor access requires enqueueing work onto that executor.

### C.6.2 Actor method entry (recommended) {#c-6-2}

A non-reentrant actor method entry should:

- hop to the actor’s executor (if not already on it),
- then execute until the next suspension point,
- preserving the actor’s invariants.

Reentrancy behavior is defined by [Part 7](#part-7); this document only constrains that runtime scheduling is consistent with that behavior.

## C.7 Autorelease pools at suspension points (normative for ObjC runtimes) {#c-7}

On Objective‑C runtimes with autorelease semantics, implementations shall ensure:

- Each _task execution slice_ (from resume to next suspension or completion) runs inside an implicit autorelease pool.
- The pool is drained:
  - before suspending at an `await`, and
  - when the task completes.

This rule exists to make async code’s memory behavior predictable for Foundation-heavy code.

## C.8 Direct/final/sealed/dynamic lowering (recommended) {#c-8}

- `objc_direct` methods should lower to direct function calls when statically referenced, and may omit dynamic method table entries as permitted by [Part 9](#part-9).
- Methods made effectively direct by `objc_direct_members` should lower identically to explicit `objc_direct`, except for members explicitly marked `objc_dynamic`.
- `objc_dynamic` methods should lower through selector-based dynamic dispatch even when surrounding declarations are eligible for direct/devirtualized lowering.
- `objc_final` should enable devirtualization and dispatch optimization but shall not change observable message lookup semantics unless paired with `objc_direct`.
- `objc_sealed` should enable whole-module reasoning about subclass sets; implementations should treat cross-module subclassing attempts as ill-formed or diagnose at link time where possible.

## C.9 Macro/derive expansion and ABI (normative for implementations) {#c-9}

If macro expansion or derives synthesize declarations that affect layout or vtables/dispatch surfaces (including generated ivars, properties, methods), then:

- the synthesis must occur before ABI layout is finalized for the type;
- the synthesized declarations must appear in any emitted module interface or API dump.

## C.10 System programming analysis hooks (recommended) {#c-10}

This section collects implementation guidance for **[Part 8](#part-8)** features that are primarily enforced by diagnostics and analysis rather than runtime hooks.

### C.10.1 Resources and cleanup {#c-10-1}

`objc_resource(...)` ([Part 8](#part-8)) should lower equivalently to a scope-exit cleanup action (similar to `cleanup(...)`), but implementations should additionally:

- perform use-after-move and double-close diagnostics in strict-system profiles;
- treat the “invalid sentinel” as the moved-from / reset state.

### C.10.2 Borrowed pointers {#c-10-2}

`borrowed T *` and `objc_returns_borrowed(owner_index=...)` ([Part 8](#part-8)) should be represented in the AST and module metadata ([D Table A](#d-3-1)) so that:

- call sites can enforce escape analysis in strict-system profiles;
- importers preserve the qualifier for downstream tooling.

No runtime support is required by these features in v1; the contract is primarily compile-time diagnostics and toolability.

## C.11 Conformance tests (non-normative) {#c-11}

Implementations are encouraged to provide a conformance suite that includes:

- effect mismatch diagnostics across modules,
- correct argument evaluation semantics for optional sends,
- `throws` propagation behavior in nested `do/catch`,
- executor hop correctness and actor isolation enforcement across module boundaries.

## C.12 Lane-C Edge-Case Compatibility Governance (implementation anchor) {#c-12}

typed sema-to-lowering edge-case and compatibility completion governance shall preserve explicit lane-C dependency anchors (`M227-C005`, `M227-C004`) so readiness stays deterministic and fail-closed when typed handoff and parse/lowering surfaces drift.

typed sema-to-lowering edge-case expansion and robustness governance shall preserve explicit lane-C dependency anchors (`M227-C006`, `M227-C005`) so typed robustness and parse/lowering alignment stay deterministic and fail-closed when expansion continuity drifts.

typed sema-to-lowering diagnostics hardening governance shall preserve explicit lane-C dependency anchors (`M227-C007`, `M227-C006`) so typed diagnostics-hardening and parse/lowering alignment stay deterministic and fail-closed when diagnostics continuity drifts.

typed sema-to-lowering recovery and determinism hardening governance shall preserve explicit lane-C dependency anchors (`M227-C008`, `M227-C007`) so typed recovery/determinism and parse/lowering alignment stay deterministic and fail-closed when replay continuity drifts.

typed sema-to-lowering conformance matrix implementation governance shall preserve explicit lane-C dependency anchors (`M227-C009`, `M227-C008`) so typed conformance-matrix and parse/lowering alignment stay deterministic and fail-closed when matrix continuity drifts.

typed sema-to-lowering conformance corpus expansion governance shall preserve explicit lane-C dependency anchors (`M227-C010`, `M227-C009`) so typed conformance-corpus and parse/lowering alignment stay deterministic and fail-closed when corpus continuity drifts.

typed sema-to-lowering performance/quality guardrails governance shall preserve explicit lane-C dependency anchors (`M227-C011`, `M227-C010`) so typed guardrail accounting and parse/lowering alignment stay deterministic and fail-closed when guardrail continuity drifts.

typed sema-to-lowering cross-lane integration sync governance shall preserve explicit lane-C dependency anchors (`M227-C012`, `M227-C011`) so typed integration-sync continuity and parse/lowering alignment stay deterministic and fail-closed when cross-lane drift occurs.

typed sema-to-lowering docs/runbook synchronization governance shall preserve explicit lane-C dependency anchors (`M227-C013`, `M227-C012`) so typed docs/runbook continuity and parse/lowering alignment stay deterministic and fail-closed when synchronization drift occurs.

typed sema-to-lowering release-candidate and replay dry-run governance shall preserve explicit lane-C dependency anchors (`M227-C014`, `M227-C013`) so typed release-candidate/replay continuity and parse/lowering alignment stay deterministic and fail-closed when dry-run drift occurs.

typed sema-to-lowering advanced core workpack (shard 1) governance shall preserve explicit lane-C dependency anchors (`M227-C015`, `M227-C014`) so typed advanced-core continuity and parse/lowering alignment stay deterministic and fail-closed when shard drift occurs.

typed sema-to-lowering advanced edge compatibility workpack (shard 1) governance shall preserve explicit lane-C dependency anchors (`M227-C016`, `M227-C015`) so typed advanced-edge compatibility continuity and parse/lowering alignment stay deterministic and fail-closed when shard drift occurs.

typed sema-to-lowering advanced diagnostics workpack (shard 1) governance shall preserve explicit lane-C dependency anchors (`M227-C017`, `M227-C016`) so typed advanced-diagnostics continuity and parse/lowering alignment stay deterministic and fail-closed when shard drift occurs.

typed sema-to-lowering advanced conformance workpack (shard 1) governance shall preserve explicit lane-C dependency anchors (`M227-C018`, `M227-C017`) so typed advanced-conformance continuity and parse/lowering alignment stay deterministic and fail-closed when shard drift occurs.

typed sema-to-lowering advanced integration workpack (shard 1) governance shall preserve explicit lane-C dependency anchors (`M227-C019`, `M227-C018`) so typed advanced-integration continuity and parse/lowering alignment stay deterministic and fail-closed when shard drift occurs.

typed sema-to-lowering advanced performance workpack (shard 1) governance shall preserve explicit lane-C dependency anchors (`M227-C020`, `M227-C019`) so typed advanced-performance continuity and parse/lowering alignment stay deterministic and fail-closed when shard drift occurs.

typed sema-to-lowering advanced core workpack (shard 2) governance shall preserve explicit lane-C dependency anchors (`M227-C021`, `M227-C020`) so typed advanced-core-shard2 continuity and parse/lowering alignment stay deterministic and fail-closed when shard drift occurs.

typed sema-to-lowering advanced edge compatibility workpack (shard 2) governance shall preserve explicit lane-C dependency anchors (`M227-C022`, `M227-C021`) so typed advanced-edge-compatibility-shard2 continuity and parse/lowering alignment stay deterministic and fail-closed when shard drift occurs.

typed sema-to-lowering advanced diagnostics workpack (shard 2) governance shall preserve explicit lane-C dependency anchors (`M227-C023`, `M227-C022`) so typed advanced-diagnostics-shard2 continuity and parse/lowering alignment stay deterministic and fail-closed when shard drift occurs.

typed sema-to-lowering advanced conformance workpack (shard 2) governance shall preserve explicit lane-C dependency anchors (`M227-C024`, `M227-C023`) so typed advanced-conformance-shard2 continuity and parse/lowering alignment stay deterministic and fail-closed when shard drift occurs.

typed sema-to-lowering advanced integration workpack (shard 2) governance shall preserve explicit lane-C dependency anchors (`M227-C025`, `M227-C024`) so typed advanced-integration-shard2 continuity and parse/lowering alignment stay deterministic and fail-closed when shard drift occurs.

typed sema-to-lowering integration closeout and gate sign-off governance shall preserve explicit lane-C dependency anchors (`M227-C026`, `M227-C025`) so typed integration-closeout/sign-off continuity and parse/lowering alignment stay deterministic and fail-closed when sign-off drift occurs.

message send lowering and call emission contract and architecture freeze governance shall preserve explicit lane-C dependency anchors (`M232-C001`) and fail closed on semantic-to-lowering handoff continuity, operator command sequencing, or architecture/spec anchor drift before lane-C expansion workpacks advance.

message send lowering and call emission modular split and scaffolding governance shall preserve explicit lane-C dependency anchors (`M232-C002`, `M232-C001`) and fail closed on modular split/scaffolding continuity, operator command sequencing, or architecture/spec anchor drift before lane-C core-feature workpacks advance.

message send lowering and call emission core feature implementation governance shall preserve explicit lane-C dependency anchors (`M232-C003`, `M232-C002`) and fail closed on core-feature continuity, operator command sequencing, or architecture/spec anchor drift before lane-C expansion workpacks advance.

message send lowering and call emission core feature expansion governance shall preserve explicit lane-C dependency anchors (`M232-C004`, `M232-C003`) and fail closed on core-feature-expansion continuity, operator command sequencing, or architecture/spec anchor drift before lane-C edge-case workpacks advance.

message send lowering and call emission edge-case and compatibility completion governance shall preserve explicit lane-C dependency anchors (`M232-C005`, `M232-C004`) and fail closed on edge-case/compatibility continuity, operator command sequencing, or architecture/spec anchor drift before lane-C robustness workpacks advance.

message send lowering and call emission edge-case expansion and robustness governance shall preserve explicit lane-C dependency anchors (`M232-C006`, `M232-C005`) and fail closed on edge-case expansion/robustness continuity, operator command sequencing, or architecture/spec anchor drift before lane-C diagnostics workpacks advance.

message send lowering and call emission diagnostics hardening governance shall preserve explicit lane-C dependency anchors (`M232-C007`, `M232-C006`) and fail closed on diagnostics-hardening continuity, operator command sequencing, or architecture/spec anchor drift before lane-C recovery workpacks advance.

message send lowering and call emission recovery and determinism hardening governance shall preserve explicit lane-C dependency anchors (`M232-C008`, `M232-C007`) and fail closed on recovery/determinism-hardening continuity, operator command sequencing, or architecture/spec anchor drift before lane-C conformance workpacks advance.

message send lowering and call emission conformance matrix implementation governance shall preserve explicit lane-C dependency anchors (`M232-C009`, `M232-C008`) and fail closed on conformance-matrix continuity, operator command sequencing, or architecture/spec anchor drift before lane-C corpus-expansion workpacks advance.

message send lowering and call emission conformance corpus expansion governance shall preserve explicit lane-C dependency anchors (`M232-C010`, `M232-C009`) and fail closed on conformance-corpus continuity, operator command sequencing, or architecture/spec anchor drift before lane-C guardrail workpacks advance.

message send lowering and call emission performance and quality guardrails governance shall preserve explicit lane-C dependency anchors (`M232-C011`, `M232-C010`) and fail closed on performance/quality continuity, operator command sequencing, or architecture/spec anchor drift before lane-C cross-lane sync workpacks advance.

message send lowering and call emission cross-lane integration sync governance shall preserve explicit lane-C dependency anchors (`M232-C012`, `M232-C011`) and fail closed on cross-lane-sync continuity, operator command sequencing, or architecture/spec anchor drift before lane-C docs/runbook workpacks advance.

message send lowering and call emission docs and operator runbook synchronization governance shall preserve explicit lane-C dependency anchors (`M232-C013`, `M232-C012`) and fail closed on docs/runbook synchronization continuity, operator command sequencing, or architecture/spec anchor drift before lane-C release-replay workpacks advance.

message send lowering and call emission release-candidate and replay dry-run governance shall preserve explicit lane-C dependency anchors (`M232-C014`, `M232-C013`) and fail closed on release/replay continuity, operator command sequencing, or architecture/spec anchor drift before lane-C advanced workpack stages advance.

message send lowering and call emission advanced core workpack (shard 1) governance shall preserve explicit lane-C dependency anchors (`M232-C015`, `M232-C014`) and fail closed on advanced-core-shard1 continuity, operator command sequencing, or architecture/spec anchor drift before lane-C advanced edge-compatibility stages advance.

message send lowering and call emission advanced edge compatibility workpack (shard 1) governance shall preserve explicit lane-C dependency anchors (`M232-C016`, `M232-C015`) and fail closed on advanced-edge-compatibility-shard1 continuity, operator command sequencing, or architecture/spec anchor drift before lane-C advanced edge-compatibility shard-2 stages advance.

message send lowering and call emission advanced diagnostics workpack (shard 1) governance shall preserve explicit lane-C dependency anchors (`M232-C017`, `M232-C016`) and fail closed on advanced-diagnostics-shard1 continuity, operator command sequencing, or architecture/spec anchor drift before lane-C advanced diagnostics shard-2 stages advance.

message send lowering and call emission advanced conformance workpack (shard 1) governance shall preserve explicit lane-C dependency anchors (`M232-C018`, `M232-C017`) and fail closed on advanced-conformance-shard1 continuity, operator command sequencing, or architecture/spec anchor drift before lane-C advanced conformance shard-2 stages advance.

property conformance gate and docs contract and architecture freeze wiring shall preserve explicit lane-E dependency anchors (`M234-A001`, `M234-B001`, `M234-C001`, and `M234-D001`) and fail closed when lane-E contract-freeze dependency continuity drifts.

property conformance gate and docs modular split/scaffolding wiring shall preserve explicit lane-E dependency anchors (`M234-E001`, `M234-A002`, `M234-B002`, `M234-C002`, and `M234-D002`) and fail closed when lane-E modular split/scaffolding dependency continuity drifts.

property conformance gate and docs core feature implementation wiring shall preserve explicit lane-E dependency anchors (`M234-E002`, `M234-A003`, `M234-B003`, `M234-C003`, and `M234-D002`) and fail closed when lane-E core feature implementation dependency continuity drifts.

property conformance gate and docs core feature expansion wiring shall preserve explicit lane-E dependency anchors (`M234-E003`, `M234-A004`, `M234-B004`, `M234-C004`, and `M234-D003`) and fail closed when lane-E core feature expansion dependency continuity drifts.

class/protocol/category metadata generation governance shall preserve explicit lane-A dependency anchors (`M229-A001`) and fail closed on class/protocol/category metadata linkage drift before lane-A modular split/scaffolding workpacks advance.

class/protocol/category metadata generation contract freeze shall preserve deterministic lane-A boundary anchors and fail closed on class/protocol/category metadata linkage drift, parser declaration continuity drift, or typed sema-to-lowering handoff drift before runtime ABI expansion stages advance.

class/protocol/category metadata generation modular split/scaffolding governance shall preserve explicit lane-A dependency anchors (`M229-A001`) and fail closed on scaffolding evidence drift before lane-A core feature implementation workpacks advance.

class/protocol/category metadata generation core feature implementation governance shall preserve explicit lane-A dependency anchors (`M229-A002`) and fail closed on core-feature evidence drift before lane-A core-feature expansion workpacks advance.

class/protocol/category metadata generation core feature expansion governance shall preserve explicit lane-A dependency anchors (`M229-A003`) and fail closed on core-feature-expansion evidence drift before lane-A edge-case and compatibility workpacks advance.

conformance corpus governance and sharding contract-freeze governance shall preserve explicit lane-A deterministic boundary anchors (`M230-A001`) and fail closed on contract-freeze evidence drift before lane-A modular split/scaffolding workpacks advance.

conformance corpus governance and sharding modular split/scaffolding governance shall preserve explicit lane-A dependency anchors (`M230-A001`) and fail closed on scaffolding evidence drift before lane-A core feature implementation workpacks advance.

conformance corpus governance and sharding core feature implementation governance shall preserve explicit lane-A dependency anchors (`M230-A002`) and fail closed on core-feature evidence drift before lane-A core-feature expansion workpacks advance.

conformance corpus governance and sharding core feature expansion governance shall preserve explicit lane-A dependency anchors (`M230-A003`) and fail closed on core-feature-expansion evidence drift before lane-A edge-case and compatibility workpacks advance.

conformance corpus governance and sharding edge-case and compatibility completion governance shall preserve explicit lane-A dependency anchors (`M230-A004`) and fail closed on edge-case-and-compatibility-completion evidence drift before lane-A robustness workpacks advance.

conformance corpus governance and sharding edge-case expansion and robustness governance shall preserve explicit
lane-A dependency anchors (`M230-A005`) and fail closed on edge-case-expansion-and-robustness evidence drift


conformance corpus governance and sharding diagnostics hardening governance shall preserve explicit
lane-A dependency anchors (`M230-A006`) and fail closed on diagnostics-hardening evidence drift


conformance corpus governance and sharding recovery and determinism hardening governance shall preserve explicit
lane-A dependency anchors (`M230-A007`) and fail closed on recovery-and-determinism-hardening evidence drift


conformance corpus governance and sharding conformance matrix implementation governance shall preserve explicit
lane-A dependency anchors (`M230-A008`) and fail closed on conformance-matrix-implementation evidence drift


conformance corpus governance and sharding conformance corpus expansion governance shall preserve explicit
lane-A dependency anchors (`M230-A009`) and fail closed on conformance-corpus-expansion evidence drift


conformance corpus governance and sharding performance and quality guardrails governance shall preserve explicit
lane-A dependency anchors (`M230-A010`) and fail closed on performance-and-quality-guardrails evidence drift


conformance corpus governance and sharding integration closeout and gate sign-off governance shall preserve explicit
lane-A dependency anchors (`M230-A011`) and fail closed on integration-closeout-and-gate-sign-off evidence drift


declaration grammar expansion and normalization contract-freeze governance shall preserve explicit
lane-A deterministic boundary anchors (`M231-A001`) and fail closed on contract-freeze evidence drift


declaration grammar expansion and normalization modular split/scaffolding governance shall preserve explicit
lane-A dependency anchors (`M231-A001`) and fail closed on scaffolding evidence drift


declaration grammar expansion and normalization core feature implementation governance shall preserve explicit
lane-A dependency anchors (`M231-A002`) and fail closed on core-feature evidence drift


declaration grammar expansion and normalization core feature expansion governance shall preserve explicit
lane-A dependency anchors (`M231-A003`) and fail closed on core-feature evidence drift


declaration grammar expansion and normalization edge-case and compatibility completion governance shall preserve explicit
lane-A dependency anchors (`M231-A004`) and fail closed on core-feature evidence drift


declaration grammar expansion and normalization edge-case expansion and robustness governance shall preserve explicit
lane-A dependency anchors (`M231-A005`) and fail closed on core-feature evidence drift


declaration grammar expansion and normalization diagnostics hardening governance shall preserve explicit
lane-A dependency anchors (`M231-A006`) and fail closed on core-feature evidence drift


declaration grammar expansion and normalization recovery and determinism hardening governance shall preserve explicit
lane-A dependency anchors (`M231-A007`) and fail closed on core-feature evidence drift


declaration grammar expansion and normalization conformance matrix implementation governance shall preserve explicit
lane-A dependency anchors (`M231-A008`) and fail closed on core-feature evidence drift


declaration grammar expansion and normalization conformance corpus expansion governance shall preserve explicit
lane-A dependency anchors (`M231-A009`) and fail closed on core-feature evidence drift


declaration grammar expansion and normalization performance and quality guardrails governance shall preserve explicit
lane-A dependency anchors (`M231-A010`) and fail closed on core-feature evidence drift


declaration grammar expansion and normalization cross-lane integration sync governance shall preserve explicit
lane-A dependency anchors (`M231-A011`) and fail closed on core-feature evidence drift


declaration grammar expansion and normalization docs and operator runbook synchronization governance shall preserve explicit
lane-A dependency anchors (`M231-A012`) and fail closed on core-feature evidence drift


declaration grammar expansion and normalization release-candidate and replay dry-run governance shall preserve explicit
lane-A dependency anchors (`M231-A013`) and fail closed on core-feature evidence drift


declaration grammar expansion and normalization advanced core workpack (shard 1) governance shall preserve explicit
lane-A dependency anchors (`M231-A014`) and fail closed on core-feature evidence drift


declaration grammar expansion and normalization advanced edge compatibility workpack (shard 1) governance shall preserve explicit
lane-A dependency anchors (`M231-A015`) and fail closed on core-feature evidence drift


declaration grammar expansion and normalization advanced diagnostics workpack (shard 1) governance shall preserve explicit
lane-A dependency anchors (`M231-A016`) and fail closed on core-feature evidence drift


declaration grammar expansion and normalization advanced conformance workpack (shard 1) governance shall preserve explicit
lane-A dependency anchors (`M231-A017`) and fail closed on core-feature evidence drift


declaration grammar expansion and normalization advanced integration workpack (shard 1) governance shall preserve explicit
lane-A dependency anchors (`M231-A018`) and fail closed on core-feature evidence drift


declaration grammar expansion and normalization advanced performance workpack (shard 1) governance shall preserve explicit
lane-A dependency anchors (`M231-A019`) and fail closed on core-feature evidence drift


declaration grammar expansion and normalization advanced core workpack (shard 2) governance shall preserve explicit
lane-A dependency anchors (`M231-A020`) and fail closed on core-feature evidence drift


declaration grammar expansion and normalization integration closeout and gate sign-off governance shall preserve explicit
lane-A dependency anchors (`M231-A021`) and fail closed on core-feature evidence drift


message expression grammar and selector forms contract-freeze governance shall preserve explicit
lane-A deterministic boundary anchors (`M232-A001`) and fail closed on contract-freeze evidence drift


lane-A deterministic boundary anchors (`M232-A002`) and fail closed on contract-freeze evidence drift


lane-A deterministic boundary anchors (`M232-A003`) and fail closed on contract-freeze evidence drift


lane-A deterministic boundary anchors (`M232-A004`) and fail closed on contract-freeze evidence drift


lane-A deterministic boundary anchors (`M232-A005`) and fail closed on contract-freeze evidence drift


lane-A deterministic boundary anchors (`M232-A006`) and fail closed on contract-freeze evidence drift


lane-A deterministic boundary anchors (`M232-A007`) and fail closed on contract-freeze evidence drift


lane-A deterministic boundary anchors (`M232-A008`) and fail closed on contract-freeze evidence drift


lane-A deterministic boundary anchors (`M232-A009`) and fail closed on contract-freeze evidence drift


lane-A deterministic boundary anchors (`M232-A010`) and fail closed on contract-freeze evidence drift


lane-A deterministic boundary anchors (`M232-A011`) and fail closed on contract-freeze evidence drift


lane-A deterministic boundary anchors (`M232-A012`) and fail closed on contract-freeze evidence drift


lane-A deterministic boundary anchors (`M232-A013`) and fail closed on contract-freeze evidence drift


lane-A deterministic boundary anchors (`M232-A014`) and fail closed on contract-freeze evidence drift


lane-A deterministic boundary anchors (`M232-A015`) and fail closed on contract-freeze evidence drift


lane-A deterministic boundary anchors (`M232-A016`) and fail closed on contract-freeze evidence drift


message send lowering and call emission advanced integration workpack (shard 1) governance shall preserve explicit lane-C dependency anchors (`M232-C019`, `M232-C018`)


message send lowering and call emission integration closeout and gate sign-off governance shall preserve explicit lane-C dependency anchors (`M232-C020`, `M232-C019`)


runtime selector binding integration contract-freeze governance shall preserve explicit
lane-D deterministic boundary anchors (`M232-D001`) and fail closed on contract-freeze evidence drift

lane-D deterministic boundary anchors (`M232-D002`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M232-D003`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M232-D004`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M232-D005`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M232-D006`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M232-D007`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M232-D008`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M232-D009`) and fail closed on contract-freeze evidence drift


message semantics gate and replay evidence contract-freeze governance shall preserve explicit
lane-E deterministic boundary anchors (`M232-E001`) and fail closed on contract-freeze evidence drift

lane-E deterministic boundary anchors (`M232-E002`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M232-E003`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M232-E004`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M232-E005`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M232-E006`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M232-E007`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M232-E008`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M232-E009`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M232-E010`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M232-E011`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M232-E012`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M232-E013`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M232-E014`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M232-E015`) and fail closed on contract-freeze evidence drift


method lookup and overload semantics contract-freeze governance shall preserve explicit
lane-B deterministic boundary anchors (`M232-B001`) and fail closed on contract-freeze evidence drift

lane-B deterministic boundary anchors (`M232-B002`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M232-B003`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M232-B004`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M232-B005`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M232-B006`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M232-B007`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M232-B008`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M232-B009`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M232-B010`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M232-B011`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M232-B012`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M232-B013`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M232-B014`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M232-B015`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M232-B016`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M232-B017`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M232-B018`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M232-B019`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M232-B020`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M232-B021`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M232-B022`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M232-B023`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M232-B024`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M232-B025`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M232-B026`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M232-B027`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M232-B028`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M232-B029`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M232-B030`) and fail closed on contract-freeze evidence drift


preprocessor semantic model and expansion rules contract-freeze governance shall preserve explicit
lane-B deterministic boundary anchors (`M242-B008`) and fail closed on contract-freeze evidence drift


protocol/category grammar and AST shape contract-freeze governance shall preserve explicit
lane-A deterministic boundary anchors (`M233-A001`) and fail closed on contract-freeze evidence drift


lane-A deterministic boundary anchors (`M233-A002`) and fail closed on contract-freeze evidence drift


lane-A deterministic boundary anchors (`M233-A003`) and fail closed on contract-freeze evidence drift


lane-A deterministic boundary anchors (`M233-A004`) and fail closed on contract-freeze evidence drift


lane-A deterministic boundary anchors (`M233-A005`) and fail closed on contract-freeze evidence drift


lane-A deterministic boundary anchors (`M233-A006`) and fail closed on contract-freeze evidence drift


lane-A deterministic boundary anchors (`M233-A007`) and fail closed on contract-freeze evidence drift


lane-A deterministic boundary anchors (`M233-A008`) and fail closed on contract-freeze evidence drift


lane-A deterministic boundary anchors (`M233-A009`) and fail closed on contract-freeze evidence drift


lane-A deterministic boundary anchors (`M233-A010`) and fail closed on contract-freeze evidence drift


lane-A deterministic boundary anchors (`M233-A011`) and fail closed on contract-freeze evidence drift


lane-A deterministic boundary anchors (`M233-A012`) and fail closed on contract-freeze evidence drift


conformance checking and diagnostics contract-freeze governance shall preserve explicit
lane-B deterministic boundary anchors (`M233-B001`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M233-B002`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M233-B003`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M233-B004`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M233-B005`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M233-B006`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M233-B007`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M233-B008`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M233-B009`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M233-B010`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M233-B011`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M233-B012`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M233-B013`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M233-B014`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M233-B015`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M233-B016`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M233-B017`) and fail closed on contract-freeze evidence drift


lowering of protocol/category artifacts contract-freeze governance shall preserve explicit
lane-C deterministic boundary anchors (`M233-C001`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M233-C002`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M233-C003`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M233-C004`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M233-C005`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M233-C006`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M233-C007`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M233-C008`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M233-C009`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M233-C010`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M233-C011`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M233-C012`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M233-C013`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M233-C014`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M233-C015`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M233-C016`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M233-C017`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M233-C018`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M233-C019`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M233-C020`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M233-C021`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M233-C022`) and fail closed on contract-freeze evidence drift


semantic flow analysis and invariants contract-freeze governance shall preserve explicit
lane-B deterministic boundary anchors (`M239-B003`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M239-B004`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M239-B005`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M239-B006`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M239-B007`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M239-B008`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M239-B009`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M239-B010`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M239-B011`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M239-B012`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M239-B013`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M239-B014`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M239-B015`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M239-B016`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M239-B017`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M239-B018`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M239-B019`) and fail closed on contract-freeze evidence drift


IR-emission advanced performance workpack (shard 1) governance shall


lane-C dependency anchors (`M228-C020`, `M228-C019`)


IR-emission advanced core workpack (shard 2) governance shall


lane-C dependency anchors (`M228-C021`, `M228-C020`)


IR-emission advanced edge compatibility workpack (shard 2) governance shall


lane-C dependency anchors (`M228-C022`, `M228-C021`)


IR-emission advanced diagnostics workpack (shard 2) governance shall


lane-C dependency anchors (`M228-C023`, `M228-C022`)


IR-emission advanced conformance workpack (shard 2) governance shall


lane-C dependency anchors (`M228-C024`, `M228-C023`)


IR-emission advanced integration workpack (shard 2) governance shall


lane-C dependency anchors (`M228-C025`, `M228-C024`)


IR-emission advanced performance workpack (shard 2) governance shall


lane-C dependency anchors (`M228-C026`, `M228-C025`)


IR-emission advanced core workpack (shard 3) governance shall


lane-C dependency anchors (`M228-C027`, `M228-C026`)


IR-emission advanced edge compatibility workpack (shard 3) governance shall


lane-C dependency anchors (`M228-C028`, `M228-C027`)


IR-emission advanced diagnostics workpack (shard 3) governance shall


lane-C dependency anchors (`M228-C029`, `M228-C028`)


IR-emission advanced conformance workpack (shard 3) governance shall


lane-C dependency anchors (`M228-C030`, `M228-C029`)


IR-emission advanced integration workpack (shard 3) governance shall


lane-C dependency anchors (`M228-C031`, `M228-C030`)


IR-emission advanced performance workpack (shard 3) governance shall


lane-C dependency anchors (`M228-C032`, `M228-C031`)


IR-emission advanced core workpack (shard 4) governance shall


lane-C dependency anchors (`M228-C033`, `M228-C032`)


IR-emission advanced edge compatibility workpack (shard 4) governance shall


lane-C dependency anchors (`M228-C034`, `M228-C033`)


IR-emission advanced diagnostics workpack (shard 4) governance shall


lane-C dependency anchors (`M228-C035`, `M228-C034`)


IR-emission advanced conformance workpack (shard 4) governance shall


lane-C dependency anchors (`M228-C036`, `M228-C035`)


IR-emission advanced integration workpack (shard 4) governance shall


lane-C dependency anchors (`M228-C037`, `M228-C036`)


IR-emission integration closeout and gate sign-off governance shall


lane-C dependency anchors (`M228-C038`, `M228-C037`)


toolchain/runtime cross-lane integration sync shall remain



toolchain/runtime docs and operator runbook synchronization shall remain



toolchain/runtime release-candidate and replay dry-run shall remain



toolchain/runtime advanced core workpack (shard 1) shall remain



toolchain/runtime integration closeout and gate sign-off shall remain



replay-proof/performance conformance corpus expansion closeout wiring



replay-proof/performance performance and quality guardrails closeout wiring



replay-proof/performance cross-lane integration sync closeout wiring



replay-proof/performance docs and operator runbook synchronization closeout wiring



replay-proof/performance release-candidate and replay dry-run closeout wiring



replay-proof/performance advanced core workpack (shard 1) closeout wiring



replay-proof/performance advanced edge compatibility workpack (shard 1) closeout wiring



replay-proof/performance advanced diagnostics workpack (shard 1) closeout wiring



replay-proof/performance advanced conformance workpack (shard 1) closeout wiring



replay-proof/performance advanced integration workpack (shard 1) closeout wiring



replay-proof/performance integration closeout and gate sign-off closeout wiring



class/protocol/category metadata generation edge-case and compatibility completion governance shall preserve explicit
lane-A dependency anchors (`M229-A004`) and fail closed on edge-case-and-compatibility-completion evidence drift


class/protocol/category metadata generation edge-case expansion and robustness governance shall preserve explicit
lane-A dependency anchors (`M229-A005`) and fail closed on edge-case-expansion-and-robustness evidence drift


class/protocol/category metadata generation diagnostics hardening governance shall preserve explicit
lane-A dependency anchors (`M229-A006`) and fail closed on diagnostics-hardening evidence drift


class/protocol/category metadata generation recovery and determinism hardening governance shall preserve explicit
lane-A dependency anchors (`M229-A007`) and fail closed on recovery-and-determinism-hardening evidence drift


class/protocol/category metadata generation conformance matrix implementation governance shall preserve explicit
lane-A dependency anchors (`M229-A008`) and fail closed on conformance-matrix-implementation evidence drift


class/protocol/category metadata generation conformance corpus expansion governance shall preserve explicit
lane-A dependency anchors (`M229-A009`) and fail closed on conformance-corpus-expansion evidence drift


class/protocol/category metadata generation integration closeout and gate sign-off governance shall preserve explicit
lane-A dependency anchors (`M229-A010`) and fail closed on integration-closeout-and-gate-sign-off evidence drift


dispatch ABI and selector resolution contract-freeze governance shall preserve explicit
lane-B deterministic boundary anchors (`M229-B001`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M229-B002`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M229-B003`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M229-B004`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M229-B005`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M229-B006`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M229-B007`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M229-B008`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M229-B009`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M229-B010`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M229-B011`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M229-B012`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M229-B013`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M229-B014`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M229-B015`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M229-B016`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M229-B017`) and fail closed on contract-freeze evidence drift


interop boundary ABI handling contract-freeze governance shall preserve explicit
lane-C deterministic boundary anchors (`M229-C001`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M229-C002`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M229-C003`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M229-C004`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M229-C005`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M229-C006`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M229-C007`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M229-C008`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M229-C009`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M229-C010`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M229-C011`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M229-C012`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M229-C013`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M229-C014`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M229-C015`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M229-C016`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M229-C017`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M229-C018`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M229-C019`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M229-C020`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M229-C021`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M229-C022`) and fail closed on contract-freeze evidence drift


packaging and runtime launch ergonomics contract-freeze governance shall preserve explicit
lane-D deterministic boundary anchors (`M229-D001`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M229-D002`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M229-D003`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M229-D004`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M229-D005`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M229-D006`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M229-D007`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M229-D008`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M229-D009`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M229-D010`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M229-D011`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M229-D012`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M229-D013`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M229-D014`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M229-D015`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M229-D016`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M229-D017`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M229-D018`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M229-D019`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M229-D020`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M229-D021`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M229-D022`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M229-D023`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M229-D024`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M229-D025`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M229-D026`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M229-D027`) and fail closed on contract-freeze evidence drift


runtime release gate and operational docs contract-freeze governance shall preserve explicit
lane-E deterministic boundary anchors (`M229-E001`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M229-E002`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M229-E003`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M229-E004`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M229-E005`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M229-E006`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M229-E007`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M229-E008`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M229-E009`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M229-E010`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M229-E011`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M229-E012`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M229-E013`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M229-E014`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M229-E015`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M229-E016`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M229-E017`) and fail closed on contract-freeze evidence drift


CI matrix simplification and flake elimination contract-freeze governance shall preserve explicit
lane-A deterministic boundary anchors (`M230-B001`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M230-B001`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M230-B002`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M230-B003`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M230-B004`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M230-B005`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M230-B006`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M230-B007`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M230-B008`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M230-B009`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M230-B010`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M230-B011`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M230-B012`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M230-B013`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M230-B014`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M230-B015`) and fail closed on contract-freeze evidence drift


Documentation generation as source-of-truth contract-freeze governance shall preserve explicit
lane-C deterministic boundary anchors (`M230-C001`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M230-C002`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M230-C003`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M230-C004`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M230-C005`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M230-C006`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M230-C007`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M230-C008`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M230-C009`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M230-C010`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M230-C011`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M230-C012`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M230-C013`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M230-C014`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M230-C015`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M230-C016`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M230-C017`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M230-C018`) and fail closed on contract-freeze evidence drift


Developer CLI and diagnostics ergonomics contract-freeze governance shall preserve explicit
lane-D deterministic boundary anchors (`M230-D001`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M230-D002`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M230-D003`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M230-D004`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M230-D005`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M230-D006`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M230-D007`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M230-D008`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M230-D009`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M230-D010`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M230-D011`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M230-D012`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M230-D013`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M230-D014`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M230-D015`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M230-D016`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M230-D017`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M230-D018`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M230-D019`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M230-D020`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M230-D021`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M230-D022`) and fail closed on contract-freeze evidence drift


Program control and release readiness governance contract-freeze governance shall preserve explicit
lane-E deterministic boundary anchors (`M230-E001`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E002`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E003`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E004`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E005`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E006`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E007`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E008`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E009`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E010`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E011`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E012`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E013`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E014`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E015`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E016`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E017`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E018`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E019`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E020`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E021`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E022`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E023`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E024`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E025`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E026`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E027`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E028`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E029`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E030`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M230-E031`) and fail closed on contract-freeze evidence drift


Declaration semantic validation rules contract-freeze governance shall preserve explicit
lane-B deterministic boundary anchors (`M231-B001`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M231-B002`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M231-B003`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M231-B004`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M231-B005`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M231-B006`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M231-B007`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M231-B008`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M231-B009`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M231-B010`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M231-B011`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M231-B012`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M231-B013`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M231-B014`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M231-B015`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M231-B016`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M231-B017`) and fail closed on contract-freeze evidence drift


Declaration lowering contract updates contract-freeze governance shall preserve explicit
lane-C deterministic boundary anchors (`M231-C001`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M231-C002`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M231-C003`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M231-C004`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M231-C005`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M231-C006`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M231-C007`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M231-C008`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M231-C009`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M231-C010`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M231-C011`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M231-C012`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M231-C013`) and fail closed on contract-freeze evidence drift


lane-C deterministic boundary anchors (`M231-C014`) and fail closed on contract-freeze evidence drift


Frontend/runtime declaration metadata linkage contract-freeze governance shall preserve explicit
lane-D deterministic boundary anchors (`M231-D001`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M231-D002`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M231-D003`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M231-D004`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M231-D005`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M231-D006`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M231-D007`) and fail closed on contract-freeze evidence drift


lane-D deterministic boundary anchors (`M231-D008`) and fail closed on contract-freeze evidence drift


Declaration coverage gate and docs contract-freeze governance shall preserve explicit
lane-E deterministic boundary anchors (`M231-E001`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M231-E002`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M231-E003`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M231-E004`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M231-E005`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M231-E006`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M231-E007`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M231-E008`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M231-E009`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M231-E010`) and fail closed on contract-freeze evidence drift


lane-E deterministic boundary anchors (`M231-E011`) and fail closed on contract-freeze evidence drift


property semantic rules and synthesis analysis governance shall preserve
deterministic lane-B boundary anchors and fail closed on property/ivar semantics drift


qualified type lowering and ABI representation edge-case expansion and robustness governance shall preserve explicit

lane-C dependency anchors (`M235-C005`) and fail closed on edge-case expansion and robustness evidence drift


qualified type lowering and ABI representation Edge-case expansion and robustness governance shall preserve explicit

lane-C dependency anchors (`M235-C005`) and fail closed on Edge-case expansion and robustness evidence drift


qualified type lowering and ABI representation diagnostics hardening governance shall preserve explicit

lane-C dependency anchors (`M235-C006`) and fail closed on diagnostics hardening evidence drift


qualified type lowering and ABI representation recovery and determinism hardening governance shall preserve explicit

lane-C dependency anchors (`M235-C007`) and fail closed on recovery and determinism hardening evidence drift


qualified type lowering and ABI representation conformance matrix implementation governance shall preserve explicit

lane-C dependency anchors (`M235-C008`) and fail closed on conformance matrix implementation evidence drift


qualified type lowering and ABI representation conformance corpus expansion governance shall preserve explicit

lane-C dependency anchors (`M235-C009`) and fail closed on conformance corpus expansion evidence drift


qualified type lowering and ABI representation performance and quality guardrails governance shall preserve explicit

lane-C dependency anchors (`M235-C010`) and fail closed on performance and quality guardrails evidence drift


qualified type lowering and ABI representation cross-lane integration sync governance shall preserve explicit

lane-C dependency anchors (`M235-C011`) and fail closed on cross-lane integration sync evidence drift


qualified type lowering and ABI representation docs and operator runbook synchronization governance shall preserve explicit

lane-C dependency anchors (`M235-C012`) and fail closed on docs and operator runbook synchronization evidence drift


qualified type lowering and ABI representation release-candidate and replay dry-run governance shall preserve explicit

lane-C dependency anchors (`M235-C013`) and fail closed on release-candidate and replay dry-run evidence drift


qualified type lowering and ABI representation advanced core workpack (shard 1) governance shall preserve explicit

lane-C dependency anchors (`M235-C014`) and fail closed on advanced core workpack (shard 1) evidence drift


qualified type lowering and ABI representation advanced edge compatibility workpack (shard 1) governance shall preserve explicit

lane-C dependency anchors (`M235-C015`) and fail closed on advanced edge compatibility workpack (shard 1) evidence drift


qualified type lowering and ABI representation advanced diagnostics workpack (shard 1) governance shall preserve explicit

lane-C dependency anchors (`M235-C016`) and fail closed on advanced diagnostics workpack (shard 1) evidence drift


qualified type lowering and ABI representation advanced conformance workpack (shard 1) governance shall preserve explicit

lane-C dependency anchors (`M235-C017`) and fail closed on advanced conformance workpack (shard 1) evidence drift


qualified type lowering and ABI representation advanced integration workpack (shard 1) governance shall preserve explicit

lane-C dependency anchors (`M235-C018`) and fail closed on advanced integration workpack (shard 1) evidence drift


qualified type lowering and ABI representation integration closeout and gate sign-off governance shall preserve explicit

lane-C dependency anchors (`M235-C019`) and fail closed on integration closeout and gate sign-off evidence drift


interop behavior for qualified generic APIs edge-case and compatibility completion governance shall preserve explicit

lane-D dependency anchors (`M235-D004`) and fail closed on edge-case and compatibility completion interop evidence drift


interop behavior for qualified generic APIs edge-case expansion and robustness governance shall preserve explicit

lane-D dependency anchors (`M235-D005`) and fail closed on edge-case expansion and robustness interop evidence drift


interop behavior for qualified generic APIs diagnostics hardening governance shall preserve explicit

lane-D dependency anchors (`M235-D006`) and fail closed on diagnostics hardening interop evidence drift


interop behavior for qualified generic APIs recovery and determinism hardening governance shall preserve explicit

lane-D dependency anchors (`M235-D007`) and fail closed on recovery and determinism hardening interop evidence drift


interop behavior for qualified generic APIs integration closeout and gate sign-off governance shall preserve explicit

lane-D dependency anchors (`M235-D008`) and fail closed on integration closeout and gate sign-off interop evidence drift


lane-B deterministic boundary anchors (`M242-B009`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M242-B010`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M242-B011`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M242-B012`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M242-B013`) and fail closed on contract-freeze evidence drift


lane-B deterministic boundary anchors (`M242-B014`) and fail closed on contract-freeze evidence drift


lowering/runtime diagnostics surfacing docs and operator runbook synchronization shall preserve

lane-C dependency anchors (`M243-C012`)


lowering/runtime diagnostics surfacing release-candidate and replay dry-run shall preserve

lane-C dependency anchors (`M243-C013`)


lowering/runtime diagnostics surfacing advanced core workpack (shard 1) shall preserve

lane-C dependency anchors (`M243-C014`)


lowering/runtime diagnostics surfacing advanced edge compatibility workpack (shard 1) shall preserve

lane-C dependency anchors (`M243-C015`)


lowering/runtime diagnostics surfacing integration closeout and gate sign-off shall preserve

lane-C dependency anchors (`M243-C016`)


CLI/reporting and output release-candidate and replay dry-run governance shall preserve


CLI/reporting and output advanced core workpack (shard 1) governance shall preserve


CLI/reporting and output advanced edge compatibility workpack (shard 1) governance shall preserve


CLI/reporting and output advanced diagnostics workpack (shard 1) governance shall preserve


CLI/reporting and output advanced conformance workpack (shard 1) governance shall preserve


CLI/reporting and output advanced integration workpack (shard 1) governance shall preserve


CLI/reporting and output advanced performance workpack (shard 1) governance shall preserve


CLI/reporting and output advanced core workpack (shard 2) governance shall preserve


CLI/reporting and output integration closeout and gate sign-off governance shall preserve


diagnostics quality gate and replay policy docs and operator runbook synchronization wiring shall preserve explicit

lane-E dependency anchors (`M243-E012`, `M243-A012`, `M243-B012`, `M243-C011`, and


diagnostics quality gate and replay policy release-candidate and replay dry-run wiring shall preserve explicit

lane-E dependency anchors (`M243-E013`, `M243-A012`, `M243-B012`, `M243-C011`, and


diagnostics quality gate and replay policy advanced core workpack (shard 1) wiring shall preserve explicit

lane-E dependency anchors (`M243-E014`, `M243-A012`, `M243-B012`, `M243-C011`, and


diagnostics quality gate and replay policy advanced edge compatibility workpack (shard 1) wiring shall preserve explicit

lane-E dependency anchors (`M243-E015`, `M243-A012`, `M243-B012`, `M243-C011`, and


diagnostics quality gate and replay policy advanced diagnostics workpack (shard 1) wiring shall preserve explicit

lane-E dependency anchors (`M243-E016`, `M243-A012`, `M243-B012`, `M243-C011`, and


diagnostics quality gate and replay policy advanced conformance workpack (shard 1) wiring shall preserve explicit

lane-E dependency anchors (`M243-E017`, `M243-A012`, `M243-B012`, `M243-C011`, and


diagnostics quality gate and replay policy advanced integration workpack (shard 1) wiring shall preserve explicit

lane-E dependency anchors (`M243-E018`, `M243-A012`, `M243-B012`, `M243-C011`, and


diagnostics quality gate and replay policy advanced performance workpack (shard 1) wiring shall preserve explicit

lane-E dependency anchors (`M243-E019`, `M243-A012`, `M243-B012`, `M243-C011`, and


diagnostics quality gate and replay policy advanced core workpack (shard 2) wiring shall preserve explicit

lane-E dependency anchors (`M243-E020`, `M243-A012`, `M243-B012`, `M243-C011`, and


diagnostics quality gate and replay policy advanced edge compatibility workpack (shard 2) wiring shall preserve explicit

lane-E dependency anchors (`M243-E021`, `M243-A012`, `M243-B012`, `M243-C011`, and


diagnostics quality gate and replay policy advanced diagnostics workpack (shard 2) wiring shall preserve explicit

lane-E dependency anchors (`M243-E022`, `M243-A012`, `M243-B012`, `M243-C011`, and


diagnostics quality gate and replay policy advanced conformance workpack (shard 2) wiring shall preserve explicit

lane-E dependency anchors (`M243-E023`, `M243-A012`, `M243-B012`, `M243-C011`, and


diagnostics quality gate and replay policy advanced integration workpack (shard 2) wiring shall preserve explicit

lane-E dependency anchors (`M243-E024`, `M243-A012`, `M243-B012`, `M243-C011`, and


diagnostics quality gate and replay policy advanced performance workpack (shard 2) wiring shall preserve explicit

lane-E dependency anchors (`M243-E025`, `M243-A012`, `M243-B012`, `M243-C011`, and


diagnostics quality gate and replay policy advanced core workpack (shard 3) wiring shall preserve explicit

lane-E dependency anchors (`M243-E026`, `M243-A012`, `M243-B012`, `M243-C011`, and


diagnostics quality gate and replay policy advanced edge compatibility workpack (shard 3) wiring shall preserve explicit

lane-E dependency anchors (`M243-E027`, `M243-A012`, `M243-B012`, `M243-C011`, and


diagnostics quality gate and replay policy advanced diagnostics workpack (shard 3) wiring shall preserve explicit

lane-E dependency anchors (`M243-E028`, `M243-A012`, `M243-B012`, `M243-C011`, and


diagnostics quality gate and replay policy advanced conformance workpack (shard 3) wiring shall preserve explicit

lane-E dependency anchors (`M243-E029`, `M243-A012`, `M243-B012`, `M243-C011`, and


diagnostics quality gate and replay policy integration closeout and gate sign-off wiring shall preserve explicit

lane-E dependency anchors (`M243-E030`, `M243-A012`, `M243-B012`, `M243-C011`, and


interop semantic/type mediation advanced edge compatibility workpack (shard 1) governance shall preserve explicit

lane-B dependency anchor (`M244-B015`)


interop semantic/type mediation advanced diagnostics workpack (shard 1) governance shall preserve explicit

lane-B dependency anchor (`M244-B016`)


interop semantic/type mediation advanced conformance workpack (shard 1) governance shall preserve explicit

lane-B dependency anchor (`M244-B017`)


interop semantic/type mediation integration closeout and gate sign-off governance shall preserve explicit

lane-B dependency anchor (`M244-B018`)


interop lowering and ABI conformance advanced core workpack (shard 1) governance shall preserve explicit

lane-C dependency anchor (`M244-C014`)


interop lowering and ABI conformance advanced edge compatibility workpack (shard 1) governance shall preserve explicit

lane-C dependency anchor (`M244-C015`)


interop lowering and ABI conformance advanced diagnostics workpack (shard 1) governance shall preserve explicit

lane-C dependency anchor (`M244-C016`)


interop lowering and ABI conformance advanced conformance workpack (shard 1) governance shall preserve explicit

lane-C dependency anchor (`M244-C017`)


interop lowering and ABI conformance advanced integration workpack (shard 1) governance shall preserve explicit

lane-C dependency anchor (`M244-C018`)


interop lowering and ABI conformance advanced performance workpack (shard 1) governance shall preserve explicit

lane-C dependency anchor (`M244-C019`)


interop lowering and ABI conformance advanced core workpack (shard 2) governance shall preserve explicit

lane-C dependency anchor (`M244-C020`)


interop lowering and ABI conformance advanced edge compatibility workpack (shard 2) governance shall preserve explicit

lane-C dependency anchor (`M244-C021`)


interop lowering and ABI conformance integration closeout and gate sign-off governance shall preserve explicit

lane-C dependency anchor (`M244-C022`)


runtime/link bridge-path conformance matrix implementation governance shall preserve explicit lane-D dependency anchors (`M244-D008`)

conformance matrix implementation evidence drift before downstream runtime projection and metadata integration advances.


runtime/link bridge-path conformance corpus expansion governance shall preserve explicit lane-D dependency anchors (`M244-D009`)

conformance corpus expansion evidence drift before downstream runtime projection and metadata integration advances.


runtime/link bridge-path performance and quality guardrails governance shall preserve explicit lane-D dependency anchors (`M244-D010`)

performance and quality guardrails evidence drift before downstream runtime projection and metadata integration advances.


runtime/link bridge-path cross-lane integration sync governance shall preserve explicit lane-D dependency anchors (`M244-D011`)

cross-lane integration sync evidence drift before downstream runtime projection and metadata integration advances.


runtime/link bridge-path docs and operator runbook synchronization governance shall preserve explicit lane-D dependency anchors (`M244-D012`)

docs and operator runbook synchronization evidence drift before downstream runtime projection and metadata integration advances.


runtime/link bridge-path release-candidate and replay dry-run governance shall preserve explicit lane-D dependency anchors (`M244-D013`)

release-candidate and replay dry-run evidence drift before downstream runtime projection and metadata integration advances.


runtime/link bridge-path advanced core workpack (shard 1) governance shall preserve explicit lane-D dependency anchors (`M244-D014`)

advanced core workpack (shard 1) evidence drift before downstream runtime projection and metadata integration advances.


runtime/link bridge-path advanced edge compatibility workpack (shard 1) governance shall preserve explicit lane-D dependency anchors (`M244-D015`)

advanced edge compatibility workpack (shard 1) evidence drift before downstream runtime projection and metadata integration advances.


runtime/link bridge-path advanced diagnostics workpack (shard 1) governance shall preserve explicit lane-D dependency anchors (`M244-D016`)

advanced diagnostics workpack (shard 1) evidence drift before downstream runtime projection and metadata integration advances.


runtime/link bridge-path advanced conformance workpack (shard 1) governance shall preserve explicit lane-D dependency anchors (`M244-D017`)

advanced conformance workpack (shard 1) evidence drift before downstream runtime projection and metadata integration advances.


runtime/link bridge-path advanced integration workpack (shard 1) governance shall preserve explicit lane-D dependency anchors (`M244-D018`)

advanced integration workpack (shard 1) evidence drift before downstream runtime projection and metadata integration advances.


runtime/link bridge-path advanced performance workpack (shard 1) governance shall preserve explicit lane-D dependency anchors (`M244-D019`)

advanced performance workpack (shard 1) evidence drift before downstream runtime projection and metadata integration advances.


runtime/link bridge-path advanced core workpack (shard 2) governance shall preserve explicit lane-D dependency anchors (`M244-D020`)

advanced core workpack (shard 2) evidence drift before downstream runtime projection and metadata integration advances.


runtime/link bridge-path integration closeout and gate sign-off governance shall preserve explicit lane-D dependency anchors (`M244-D021`)

integration closeout and gate sign-off evidence drift before downstream runtime projection and metadata integration advances.


interop conformance gate and operations conformance corpus expansion wiring shall preserve explicit

lane-E dependency anchors (`M244-E009`, `M244-A007`, `M244-B010`, `M244-C012`, and `M244-D012`)

preserve `npm run --if-present` dependency-reference continuity for lane-B/C/D conformance corpus expansion readiness hooks

or lane-E conformance corpus expansion readiness hooks drift.


interop conformance gate and operations performance and quality guardrails wiring shall preserve explicit

lane-E dependency anchors (`M244-E010`, `M244-A007`, `M244-B010`, `M244-C012`, and `M244-D012`)

preserve `npm run --if-present` dependency-reference continuity for lane-B/C/D performance and quality guardrails readiness hooks

or lane-E performance and quality guardrails readiness hooks drift.


interop conformance gate and operations cross-lane integration sync wiring shall preserve explicit

lane-E dependency anchors (`M244-E011`, `M244-A007`, `M244-B010`, `M244-C012`, and `M244-D012`)

preserve `npm run --if-present` dependency-reference continuity for lane-B/C/D cross-lane integration sync readiness hooks

or lane-E cross-lane integration sync readiness hooks drift.


interop conformance gate and operations docs and operator runbook synchronization wiring shall preserve explicit

lane-E dependency anchors (`M244-E012`, `M244-A007`, `M244-B010`, `M244-C012`, and `M244-D012`)

preserve `npm run --if-present` dependency-reference continuity for lane-B/C/D docs and operator runbook synchronization readiness hooks

or lane-E docs and operator runbook synchronization readiness hooks drift.


interop conformance gate and operations release-candidate and replay dry-run wiring shall preserve explicit

lane-E dependency anchors (`M244-E013`, `M244-A007`, `M244-B010`, `M244-C012`, and `M244-D012`)

preserve `npm run --if-present` dependency-reference continuity for lane-B/C/D release-candidate and replay dry-run readiness hooks

or lane-E release-candidate and replay dry-run readiness hooks drift.


interop conformance gate and operations advanced core workpack (shard 1) wiring shall preserve explicit

lane-E dependency anchors (`M244-E014`, `M244-A007`, `M244-B010`, `M244-C012`, and `M244-D012`)

preserve `npm run --if-present` dependency-reference continuity for lane-B/C/D advanced core workpack (shard 1) readiness hooks

or lane-E advanced core workpack (shard 1) readiness hooks drift.


interop conformance gate and operations advanced edge compatibility workpack (shard 1) wiring shall preserve explicit

lane-E dependency anchors (`M244-E015`, `M244-A007`, `M244-B010`, `M244-C012`, and `M244-D012`)

preserve `npm run --if-present` dependency-reference continuity for lane-B/C/D advanced edge compatibility workpack (shard 1) readiness hooks

or lane-E advanced edge compatibility workpack (shard 1) readiness hooks drift.


interop conformance gate and operations integration closeout and gate sign-off wiring shall preserve explicit

lane-E dependency anchors (`M244-E016`, `M244-A007`, `M244-B010`, `M244-C012`, and `M244-D012`)

preserve `npm run --if-present` dependency-reference continuity for lane-B/C/D integration closeout and gate sign-off readiness hooks

or lane-E integration closeout and gate sign-off readiness hooks drift.

## M251 runtime metadata source ownership freeze (A001)

Runtime metadata source ownership governance shall preserve deterministic
lane-A boundary anchors and fail closed on runtime metadata source drift before
native metadata section emission begins.

The canonical freeze for `M251-A001` is:

- contract id: `objc3c-runtime-metadata-source-ownership-freeze/m251-a001-v1`
- canonical source schema: `objc3-runtime-metadata-source-boundary-v1`
- class records remain frontend-owned through
  `Objc3InterfaceDecl` / `Objc3ImplementationDecl`
- protocol records remain frontend-owned through `Objc3ProtocolDecl`
- category records remain frontend-owned through category-bearing interface /
  implementation declarations
- property and method records remain frontend-owned through
  `Objc3PropertyDecl` / `Objc3MethodDecl`
- ivar records remain frontend-owned through property-synthesis ivar binding
  symbols until native ivar declaration extraction exists

For `M251-A001`, lowering/runtime must treat the emitted boundary as authoritative
ownership evidence while runtime metadata source records remain not yet ready
for lowering and the test shim topology remains explicitly non-production.

## M251 runtime metadata source record extraction (A002)

Lane-A runtime metadata extraction shall preserve
`Objc3RuntimeMetadataSourceRecordSet` as the canonical frontend handoff for
runtime-owned declaration packets.

`M251-A002` requires:

- deterministic lexicographic vectors for class, protocol, category, property,
  method, and ivar source records,
- category owner-name normalization as `Class(Category)`,
- manifest projection sourced from the extracted record set rather than
  count-only sema/type summaries,
- frontend C API runner proof that declaration fixtures stay lex/parse/sema
  clean before any downstream emit gate is considered.

Current emit-only downstream `O3L300` remains acceptable during `M251-A002`
because `M251-A003` is responsible for carrying runtime record projection
through manifest-oriented lowering/emit boundaries.

## M251 runtime record manifest handoff normalization (A003)

Lane-A manifest projection shall preserve runtime-record handoff availability
before later LLVM IR/object fail-closed diagnostics are surfaced.

`M251-A003` requires:

- explicit frontend emit intent for manifest, IR, and object outputs,
- deterministic runtime-record manifest projection to remain available when
  later lowering/runtime readiness is still incomplete,
- manifest-only frontend runs to succeed without reporting downstream emit
  failure,
- full compile/emit workflows to remain fail-closed while still preserving the
  manifest handoff artifact for downstream consumers.

## M251 runtime export legality freeze (B001)

Lane-B shall freeze a canonical runtime-export legality packet before metadata
section emission and runtime registration work begin.

`M251-B001` requires:

- `Objc3RuntimeExportLegalityBoundary` to remain the single semantic legality
  freeze packet for runtime-exported declarations,
- the packet to be synthesized from deterministic runtime metadata ownership,
  protocol/category linking, selector normalization, property attribute,
  object-pointer spelling, symbol-graph, and property-synthesis/ivar-binding
  surfaces,
- the packet to be ready while metadata export enforcement remains pending,
  provided it stays fail-closed and carries the pending enforcement bits for
  duplicate identities, incomplete declarations, and illegal redeclaration
  mixes.

## M251 runtime export enforcement semantics (B002)

Lane-B shall promote the frozen runtime-export legality packet into a real
fail-closed semantic barrier before metadata section emission and runtime
registration work begin.

`M251-B002` requires:

- `Objc3RuntimeExportEnforcementSummary` to remain the single semantic
  enforcement packet for runtime-exported declarations,
- the enforcement packet to be synthesized from deterministic runtime metadata
  source records plus the B001 legality packet,
- duplicate runtime identities, incomplete export units, illegal redeclaration
  mixes, and metadata-shape drift to block lowering when present,
- forward protocol declarations to remain legal dependency hints rather than
  incomplete export candidates.

Duplicate runtime identities, incomplete export units, illegal redeclaration
mixes, and metadata-shape drift must therefore fail closed before lowering
begins. Forward protocol declarations must remain legal dependency hints rather
than incomplete export candidates.

## M251 runtime export diagnostic precision (B003)

Lane-B shall preserve the B002 fail-closed blocker code while making incomplete
runtime export messages precise for class and category interface declarations
that parse successfully but still cannot participate in runtime export.

`M251-B003` requires:

- deterministic synthesis of source-anchored runtime export blocker diagnostics
  from runtime metadata source records,
- interface-only runtime export units to name the missing class
  `@implementation`,
- category-interface-only runtime export units to name the missing category
  `@implementation`,
- the generic B002 blocker to remain as the fallback when no more precise
  declaration-specific explanation is available.

B003 shall keep the B002 fail-closed blocker code stable while making incomplete runtime export messages precise so class and category interface declarations that parse successfully but still cannot participate in runtime export fail closed with deterministic explanations.

## M251 runtime metadata section ABI and symbol policy freeze (C001)

Lane-C shall freeze the canonical runtime metadata section ABI and symbol policy
before LLVM global reservation and physical object-section emission begin.

`M251-C001` requires:

- `Objc3RuntimeMetadataSectionAbiFreezeSummary` to remain the single lane-C
  freeze packet for runtime metadata section inventory and symbol policy,
- the logical section inventory to remain frozen for image info, class,
  protocol, category, property, and ivar metadata descriptors,
- descriptor and aggregate symbol prefixes, linkage policy, visibility policy,
  and retention-root policy to remain deterministic and published through
  manifest/IR surfaces,
- the boundary to remain fail-closed until the runtime metadata source
  ownership, legality, and enforcement packets are all ready.

Runtime metadata section ABI and symbol policy governance must therefore remain
deterministic and fail closed before `M251-C002` starts reserving LLVM globals
and physical object sections.

## M251 runtime metadata section scaffold emission (C002)

Lane-C shall emit retained placeholder runtime metadata globals and aggregates
into the canonical logical metadata sections before executable runtime payload
layouts land.

`M251-C002` requires:

- `Objc3RuntimeMetadataSectionScaffoldSummary` to remain the single lane-C
  emitted scaffold packet,
- image info, aggregate section symbols, and per-record descriptor placeholders
  to be emitted as real LLVM globals rather than manifest-only accounting,
- `@llvm.used` to retain the scaffolded metadata globals so later object
  inspection and runtime registration work can observe them,
- manifest JSON and LLVM IR metadata to publish the same scaffold contract
  through `runtime_metadata_section_scaffold_contract_id` and
  `!objc3.objc_runtime_metadata_section_scaffold`,
- emission to remain fail-closed until the `M251-C001` section ABI and
  `M251-B002` runtime export enforcement packets are both ready.

Runtime metadata section scaffold governance must therefore remain deterministic
and fail closed before runtime registration, live lookup, and executable
object-model payload lowering land.

## M251 runtime metadata object inspection harness (C003)

Lane-C shall publish a deterministic object inspection matrix for emitted
runtime metadata scaffold objects.

`M251-C003` requires:

- `Objc3RuntimeMetadataObjectInspectionHarnessSummary` to remain the single lane-C inspection matrix packet,
- manifest JSON and LLVM IR metadata to publish the same inspection contract
  through `runtime_metadata_object_inspection_contract_id` and
  `!objc3.objc_runtime_metadata_object_inspection`,
- the zero-descriptor fixture and emitted object relative path to remain frozen
  as `tests/tooling/fixtures/native/m251_runtime_metadata_object_inspection_zero_descriptor.objc3`
  and `module.obj`,
- matrix rows `zero-descriptor-section-inventory` and
  `zero-descriptor-symbol-inventory` to remain concrete `llvm-readobj` and
  `llvm-objdump` commands rather than ad hoc operator guidance,
- the inspection harness to remain fail-closed until the `M251-C002` scaffold
  packet is ready.

## M251 native runtime-library surface and build contract (D001)

Lane-D shall freeze the canonical in-tree native runtime-library surface before
the library skeleton and driver link wiring land.

`M251-D001` requires:

- `Objc3RuntimeSupportLibraryContractSummary` to remain the single lane-D
  freeze packet for native runtime-library surface and build integration
  constraints,
- manifest JSON and LLVM IR metadata to publish the same runtime-library
  contract through `runtime_support_library_contract_id` and
  `!objc3.objc_runtime_support_library`,
- the in-tree runtime-library surface to remain frozen as target
  `objc3_runtime`, source root `native/objc3c/src/runtime`, public header
  `native/objc3c/src/runtime/objc3_runtime.h`, and static archive basename
  `objc3_runtime`,
- the exported entrypoint family to remain frozen as
  `objc3_runtime_register_image`, `objc3_runtime_lookup_selector`,
  `objc3_runtime_dispatch_i32`, and `objc3_runtime_reset_for_testing`,
- compiler ownership to remain limited to emitted metadata while runtime
  ownership remains limited to registration, lookup, and dispatch state,
- the driver link mode to remain fail-closed as `not-linked-until-m251-d003`
  until the runtime library is actually wired in.

Runtime-library surface governance must therefore remain deterministic and fail
closed before `M251-D002` lands the library skeleton and `M251-D003` rewires
the driver to consume it.

Runtime metadata object inspection governance must therefore remain
deterministic and fail closed before runtime registration, live lookup, and
executable object-model payload lowering land.

## M251 native runtime-library core feature (D002)

Lane-D shall instantiate the frozen D001 surface as a real in-tree native
runtime library artifact before D003 links the driver against it.

`M251-D002` requires:

- `Objc3RuntimeSupportLibraryCoreFeatureSummary` to publish the real native
  runtime-library skeleton independently from the frozen D001 packet,
- manifest JSON and LLVM IR metadata to publish the same D002 contract through
  `runtime_support_library_core_feature_contract_id` and
  `!objc3.objc_runtime_support_library_core_feature`,
- `native/objc3c/src/runtime/objc3_runtime.cpp` to implement
  `objc3_runtime_register_image`, `objc3_runtime_lookup_selector`,
  `objc3_runtime_dispatch_i32`, and `objc3_runtime_reset_for_testing`,
- `npm run build:objc3c-native` to emit `artifacts/lib/objc3_runtime.lib`,
- `objc3_runtime_dispatch_i32` to preserve the deterministic
  `objc3_msgsend_i32` arithmetic formula while driver link mode remains
  `not-linked-until-m251-d003`,
- `tests/tooling/runtime/m251_d002_runtime_library_probe.cpp` to link against
  the real archive and prove registration, selector lookup, dispatch, and
  reset-for-testing on the happy path.

D002 therefore proves the native runtime library exists as an executable
artifact without claiming driver-link integration or full object-model runtime
registration have landed yet.

## M251 native runtime-library link wiring (D003)

Lane-D shall wire emitted-object execution consumers to the real native runtime
library without mutating the frozen D001 surface or the D002 implementation
packet.

`M251-D003` requires:

- `Objc3RuntimeSupportLibraryLinkWiringSummary` to publish the canonical
  emitted-object runtime-consumption contract,
- manifest JSON and emitted LLVM IR metadata to publish the same D003 contract
  through `runtime_support_library_link_wiring_contract_id` and
  `!objc3.objc_runtime_support_library_link_wiring`,
- emitted-object runtime link mode to become
  `emitted-object-links-against-objc3_runtime-lib`,
- `native/objc3c/src/runtime/objc3_runtime.cpp` to export the compatibility
  bridge symbol `objc3_msgsend_i32` while keeping
  `objc3_runtime_dispatch_i32` canonical,
- `scripts/check_objc3c_native_execution_smoke.ps1` to consume the emitted
  manifest/runtime archive contract and link runtime-requiring fixtures against
  `artifacts/lib/objc3_runtime.lib`,
- negative unresolved-symbol coverage to remain fail closed by omitting the
  runtime library on link-failure fixtures.

D003 therefore proves emitted objects can consume the real runtime archive in
deterministic smoke/checker paths without yet claiming metadata registration,
class realization, or property/category runtime execution are complete.

## M251 runnable-runtime foundation gate and evidence contract (E001)

Lane-E shall freeze one aggregate foundation gate over the completed M251
runtime-foundation tranche.

`M251-E001` requires:

- the canonical upstream evidence set to remain explicit:
  - `tmp/reports/m251/M251-A003/runtime_record_manifest_handoff_contract_summary.json`
  - `tmp/reports/m251/M251-B003/illegal_runtime_exposed_declaration_diagnostics_summary.json`
  - `tmp/reports/m251/M251-C003/runtime_metadata_object_inspection_harness_summary.json`
  - `tmp/reports/m251/M251-D003/runtime_support_library_link_wiring_summary.json`
  - `tmp/artifacts/objc3c-native/execution-smoke/m251_d003_runtime_library_link_wiring/summary.json`
- `M251-A003` to remain the canonical manifest/runtime-record handoff proof,
- `M251-B003` to remain the canonical fail-closed runtime-export diagnostic
  proof,
- `M251-C003` to remain the canonical object-file inspection proof for runtime
  metadata sections and retained symbols,
- `M251-D003` to remain the canonical emitted-object runtime-link proof,
- the D003 smoke summary to remain `PASS` while reporting
  `runtime_library = artifacts/lib/objc3_runtime.lib`,
- `artifacts/lib/objc3_runtime.lib` to exist when the gate runs.

E001 therefore freezes the aggregate foundation proof that `M252+` must
preserve without claiming startup registration, class realization, property
execution, blocks, ARC, or cross-module runtime support are complete.

## M251 cross-lane runtime-foundation gate and bootstrap proof (E002)

Lane-E shall add the first integrated runtime-foundation gate over the finished
M251 lane outputs.

`M251-E002` requires:

- `M251-E001` to remain the canonical aggregate dependency contract,
- the gate to exercise the real native toolchain path through
  `artifacts/bin/objc3c-native.exe`,
- a metadata-rich native compile probe to preserve `module.manifest.json` and
  `runtime_metadata_source_records` on the integrated path,
- an incomplete runtime-export probe to fail closed with the precise `O3S260`
  diagnostic,
- a zero-descriptor object-emission probe to succeed and prove the runtime
  metadata sections/symbols through `llvm-readobj --sections` and
  `llvm-objdump --syms`,
- a fresh execution-smoke run to report `status = PASS` and
  `runtime_library = artifacts/lib/objc3_runtime.lib` under run id
  `m251_e002_cross_lane_runtime_foundation_gate`.

E002 therefore proves the source-record, semantic-diagnostic,
metadata-section-scaffold, and runtime-library execution layers line up on the
same native toolchain path before M251 publishes operator runbooks in E003.

## M251 developer runbooks and environment publication for runtime foundation (E003)

Lane-E shall publish one canonical runtime-foundation runbook at
`docs/runbooks/m251_runtime_foundation_developer_runbook.md`.

`M251-E003` requires:

- the documented build command `npm run build:objc3c-native`,
- the documented native object-emission command through
  `.\artifacts\bin\objc3c-native.exe`,
- the documented object-inspection commands through `llvm-readobj.exe` and
  `llvm-objdump.exe`,
- the documented execution-smoke replay under run id
  `m251_e003_runtime_foundation_runbook_smoke`,
- the dependency summary
  `tmp/reports/m251/M251-E002/cross_lane_runtime_foundation_gate_summary.json`
  to remain `ok = true`,
- the documented command path to keep producing the published runtime archive,
  object-inspection artifacts, and smoke summary without hidden setup drift.

E003 therefore publishes the operator-facing build/inspection/smoke path for a
fresh clone on this machine without claiming new runtime feature coverage.

## M252 executable metadata source graph freeze (A001)

Lane-A shall freeze one canonical source-graph contract before metadata
semantic closure work begins.

`M252-A001` requires:

- one deterministic executable metadata source graph contract id:
  `objc3c-executable-metadata-source-graph-freeze/m252-a001-v1`,
- one owner-identity model:
  `semantic-link-symbol-lexicographic-owner-identity`,
- one metaclass policy:
  `metaclass-nodes-derived-from-resolved-interface-symbols`,
- parser/semantic/pipeline anchors to preserve class, metaclass, protocol,
  category, property, ivar, and method graph coverage,
- lexicographic owner/edge ordering to remain frozen and fail closed before
  `M252-A002` graph-completion work begins.

A001 therefore freezes the executable metadata source graph boundary and
evidence only. It does not claim semantic closure, lowering readiness, runtime
ingest packaging, or startup registration are complete.

## M252 executable metadata graph completeness (A002)

`M252-A002` promotes the executable metadata graph into one real frontend
packet with:

- contract id `objc3c-executable-metadata-source-graph-completeness/m252-a002-v1`,
- first-class interface, implementation, class, and metaclass node entries,
- canonical runtime owner identities on `class:` / `metaclass:` surfaces,
- deterministic owner-edge ordering on `lexicographic-kind-source-target`,
- explicit superclass and super-metaclass edges,
- fail-closed `ready_for_semantic_closure == true` and
  `ready_for_lowering == false` on the happy path.

`M252-A002` does not complete protocol/category/property/ivar graph closure,
semantic ambiguity diagnostics, or lowering/runtime ingest readiness.

## M252 executable metadata export graph completion (A003)

`M252-A003` extends the executable metadata graph closure with:

- explicit `protocol_node_entries`,
- explicit `category_node_entries`,
- explicit `property_node_entries`,
- explicit `method_node_entries`,
- explicit `ivar_node_entries`,
- canonical category node owners on `category:Class(Category)`,
- declaration-owner versus export-owner separation for member nodes,
- deterministic protocol-inheritance, category-attachment, property/method
  ownership, and ivar/property binding edges,
- fail-closed `source_graph_complete == true`,
  `ready_for_semantic_closure == true`, and
  `ready_for_lowering == false` on class/protocol/property/ivar and
  category/protocol/property happy-path fixtures.

`M252-A003` still does not claim semantic ambiguity diagnostics, lowering
readiness, runtime ingest packaging, or startup registration are complete.

## M252 executable metadata semantic consistency freeze (B001)

`M252-B001` freezes one canonical lane-B semantic-consistency packet:

- contract id `objc3c-executable-metadata-semantic-consistency-freeze/m252-b001-v1`,
- packet type `Objc3ExecutableMetadataSemanticConsistencyBoundary`,
- manifest publication under
  `frontend.pipeline.semantic_surface.objc_executable_metadata_semantic_consistency_boundary`,
- deterministic dependence on the ready executable metadata source graph and
  deterministic protocol/category, class-linking, selector, property, and
  scope-resolution handoffs,
- explicit pending enforcement bits for semantic conflict diagnostics,
  duplicate export-owner enforcement, and lowering admission.

`M252-B001` is allowed to be `ready=true` while those enforcement lanes remain
pending, as long as the boundary is frozen, fail-closed, and
`lowering_admission_ready == false`.

## M252 inheritance override protocol composition validation (B002)

`M252-B002` adds one executable-metadata semantic-validation packet:

- contract id `objc3c-executable-metadata-semantic-validation/m252-b002-v1`,
- packet type `Objc3ExecutableMetadataSemanticValidationSurface`,
- manifest publication under
  `frontend.pipeline.semantic_surface.objc_executable_metadata_semantic_validation_surface`,
- graph-backed `method-to-overridden-method` owner edges for legal
  class-interface overrides,
- deterministic validation of inheritance chains, override legality,
  protocol-composition accounting, and metaclass relationships.

`M252-B002` must remain fail-closed and `lowering_admission_ready == false`
even when the happy path is fully ready.

## M252 category attachment duplication ambiguity diagnostics (B003)

`M252-B003` hardens the existing runtime metadata export blocker with
deterministic category conflict diagnostics:

- `O3S261` for category attachment collisions,
- `O3S262` for duplicate runtime members,
- `O3S263` for ambiguous runtime metadata graph resolution.

The expansion must preserve the valid class-plus-category happy path, keep
`O3S260` for incomplete declarations, and remain fail-closed without claiming
lowering admission or runtime ingest readiness.

## M252 property ivar export legality and synthesis preconditions (B004)

`M252-B004` freezes the property/ivar export legality surface so the
property-synthesis/ivar-binding lowering contract is built from
`Objc3SemaParityContractSurface` instead of the older property-attribute
fallback.

The packet-free hardening must preserve:

- one canonical property-synthesis/ivar-binding summary published through
  `frontend.pipeline.sema_pass_manager`,
- the same counts under
  `frontend.pipeline.semantic_surface.objc_property_synthesis_ivar_binding_surface`,
- and a lowering replay key derived from those same counts.

The expansion must keep missing interface properties and incompatible property
signatures fail-closed under `O3S206`, and category-only property export keep
the synthesis counters at zero.

## M252 metadata graph lowering handoff freeze (C001)

`M252-C001` freezes one canonical lane-C lowering-handoff packet:
`Objc3ExecutableMetadataLoweringHandoffSurface`.

The frozen packet must preserve:

- contract id `objc3c-executable-metadata-lowering-handoff-freeze/m252-c001-v1`,
- deterministic dependence on the ready executable metadata source graph,
  semantic-consistency boundary, and semantic-validation surface,
- deterministic dependence on typed sema metadata, protocol/category,
  class/protocol/category linking, selector normalization, property attribute,
  symbol-graph/scope resolution, and property-synthesis/ivar-binding handoffs,
- manifest publication under
  `frontend.pipeline.semantic_surface.objc_executable_metadata_lowering_handoff_surface`,
- typed/parse projection of
  `executable_metadata_lowering_handoff_ready`,
  `executable_metadata_lowering_handoff_deterministic`, and
  `executable_metadata_lowering_handoff_key`,
- fail-closed `ready_for_lowering == false`.

`M252-C001` does not emit object metadata sections, perform runtime ingest
packaging, or claim executable metadata programs are globally lowering-ready.

## M252 typed metadata graph handoff and manifest schema (C002)

`M252-C002` promotes the frozen C001 boundary into one concrete lowering-ready
packet:
`Objc3ExecutableMetadataTypedLoweringHandoff`.

The packet must preserve:

- contract id `objc3c-executable-metadata-typed-lowering-handoff/m252-c002-v1`,
- dependency continuity with:
  - `objc3c-executable-metadata-source-graph-completeness/m252-a002-v1`,
  - `objc3c-executable-metadata-semantic-consistency-freeze/m252-b001-v1`,
  - `objc3c-executable-metadata-semantic-validation/m252-b002-v1`,
  - `objc3c-executable-metadata-lowering-handoff-freeze/m252-c001-v1`,
- manifest schema ordering model
  `contract-header-then-source-graph-payload-v1`,
- publication under
  `frontend.pipeline.semantic_surface.objc_executable_metadata_typed_lowering_handoff`,
- direct typed payload publication of the ordered metadata graph under
  `source_graph`,
- parse/lowering projection of
  `executable_metadata_typed_lowering_handoff_ready`,
  `executable_metadata_typed_lowering_handoff_deterministic`, and
  `executable_metadata_typed_lowering_handoff_key`,
- fail-closed `ready_for_lowering == true` on the typed handoff packet.

`M252-C002` does not emit object-file metadata sections yet, but it does freeze
the lowering-ready payload schema that those later section-emission issues must
consume.

## M252 metadata debug projection and replay anchors (C003)

`M252-C003` publishes the canonical lane-C metadata inspection matrix:
`Objc3ExecutableMetadataDebugProjectionSummary`.

The packet must preserve:

- contract id `objc3c-executable-metadata-debug-projection/m252-c003-v1`,
- typed-handoff dependency continuity with
  `objc3c-executable-metadata-typed-lowering-handoff/m252-c002-v1`,
- manifest semantic-surface publication under
  `frontend.pipeline.semantic_surface.objc_executable_metadata_debug_projection`,
- IR named-metadata publication under
  `!objc3.objc_executable_metadata_debug_projection`,
- deterministic matrix rows:
  - `class-protocol-property-ivar-manifest-projection`,
  - `category-protocol-property-manifest-projection`,
  - `hello-ir-named-metadata-anchor`,
- replay-anchor continuity between the debug-projection packet and the active
  C002 typed handoff whenever the current input actually materializes a typed
  metadata payload.

`M252-C003` intentionally proves the manifest/debug projection on the
metadata-rich fixtures and the IR named-metadata anchor on the runnable hello
fixture before runtime section emission and runtime ingest packaging land.

## M252 runtime ingest packaging for metadata graphs (D001)

`M252-D001` freezes the canonical lane-D runtime-ingest packaging boundary:
`Objc3ExecutableMetadataRuntimeIngestPackagingContractSummary`.

The packet must preserve:

- contract id
  `objc3c-executable-metadata-runtime-ingest-packaging-boundary/m252-d001-v1`,
- typed-handoff dependency continuity with
  `objc3c-executable-metadata-typed-lowering-handoff/m252-c002-v1`,
- debug-projection dependency continuity with
  `objc3c-executable-metadata-debug-projection/m252-c003-v1`,
- manifest semantic-surface publication under
  `frontend.pipeline.semantic_surface.objc_executable_metadata_runtime_ingest_packaging_contract`,
- payload model `typed-handoff-plus-debug-projection-manifest-v1`,
- transport artifact `module.manifest.json`,
- replay-key continuity with both the active C002 typed handoff and the C003
  debug-projection packet,
- fail-closed non-goals declaring that runtime section emission, startup
  registration, and runtime-loader registration have not landed yet.

`M252-D001` does not implement the binary section payload or startup
registration path. It freezes the manifest transport contract that those later
lane-D and M253 milestones must preserve.

## M252 artifact packaging and binary boundary for metadata payloads (D002)

`M252-D002` materializes the canonical lane-D runtime-ingest binary boundary:
`Objc3ExecutableMetadataRuntimeIngestBinaryBoundarySummary`.

The packet must preserve:

- contract id
  `objc3c-executable-metadata-runtime-ingest-binary-boundary/m252-d002-v1`,
- packaging-contract continuity with
  `objc3c-executable-metadata-runtime-ingest-packaging-boundary/m252-d001-v1`,
- typed-handoff continuity with
  `objc3c-executable-metadata-typed-lowering-handoff/m252-c002-v1`,
- debug-projection continuity with
  `objc3c-executable-metadata-debug-projection/m252-c003-v1`,
- manifest semantic-surface publication under
  `frontend.pipeline.semantic_surface.objc_executable_metadata_runtime_ingest_binary_boundary`,
- emitted artifact `module.runtime-metadata.bin`,
- binary envelope format `objc3-runtime-metadata-envelope-v1`,
- binary magic `OBJC3RM1`,
- deterministic chunk set:
  - `runtime_ingest_packaging_contract`,
  - `typed_lowering_handoff`,
  - `debug_projection`,
- replay-key continuity with the active D001/C002/C003 packets,
- fail-closed readiness for later section-emission/bootstrap handoff.

`M252-D002` still stops short of object-file section emission and startup
registration. It lands the standalone runtime-facing binary artifact that those
later milestones must consume.

## M252 metadata semantic-closure gate (E001)

`M252-E001` freezes the aggregate lane-E metadata semantic-closure gate:
`Objc3ExecutableMetadataSemanticClosureGateSummary`.

The gate must preserve:

- contract id
  `objc3c-executable-metadata-semantic-closure-gate/m252-e001-v1`,
- upstream closure proofs
  `M252-A003`, `M252-B004`, `M252-C003`, and `M252-D002`,
- evidence that the executable metadata source graph is complete while
  property/ivar export legality, debug/replay projection, and runtime-facing
  binary packaging all remain deterministic and fail closed,
- the stable evidence path
  `tmp/reports/m252/M252-E001/metadata_semantic_closure_gate_summary.json`,
- the requirement that the aggregate boundary remains synchronized before
  `M253-A001` section emission begins.

`M252-E001` still does not implement object-file section emission, startup
registration, or runtime metadata loader bootstrap. It freezes the aggregate
pre-section-emission proof that the next implementation issue must preserve.

## M252 conformance corpus and docs sync for metadata graph closure (E002)

`M252-E002` adds the first representative integrated corpus gate for metadata
graph closure on the real frontend runner path.

The gate must preserve:

- contract id
  `objc3c-metadata-graph-closure-conformance-corpus-doc-sync/m252-e002-v1`,
- dependency continuity through `M252-E001`,
- representative cases
  `class-protocol-property-ivar-runtime-graph`,
  `category-protocol-property-runtime-graph`,
  `class-property-synthesis-ready`,
  `category-property-export-only`,
  `missing-interface-property-diagnostic`, and
  `incompatible-property-signature-diagnostic`,
- a direct readiness runner that exercises the real frontend runner path rather
  than recursively nesting the whole lane stack,
- evidence path
  `tmp/reports/m252/M252-E002/conformance_corpus_and_docs_sync_for_metadata_graph_closure_summary.json`.

## M253 emitted metadata inventory freeze (A001)

`M253-A001` freezes the emitted runtime metadata inventory boundary before the
lane-A completeness matrix and later real payload work land.

The freeze must preserve:

- contract id
  `objc3c-emitted-metadata-inventory-freeze/m253-a001-v1`,
- the canonical emitted inventory families:
  `objc3.runtime.image_info`,
  `objc3.runtime.class_descriptors`,
  `objc3.runtime.protocol_descriptors`,
  `objc3.runtime.category_descriptors`,
  `objc3.runtime.property_descriptors`, and
  `objc3.runtime.ivar_descriptors`,
- the frozen symbol policies:
  `__objc3_meta_`,
  `__objc3_sec_`,
  `__objc3_image_info`,
  descriptor linkage `private`,
  aggregate linkage `internal`,
  metadata visibility `hidden`,
  retention root `llvm.used`,
- the requirement that lowering-boundary replay keys and backend object
  emission preserve this inventory rather than inferring or rewriting another
  metadata layout,
- the stable evidence path
  `tmp/reports/m253/M253-A001/emitted_metadata_inventory_contract_summary.json`.

`M253-A001` does not yet publish the source-to-section completeness matrix,
does not introduce concrete descriptor layouts, and does not add standalone
method/selector/string-pool emitted sections.

## M253 source-to-section mapping completeness matrix (A002)

`M253-A002` publishes the deterministic completeness matrix that maps every
currently supported executable metadata graph node kind to its emitted runtime
metadata section payload, symbol family, relocation behavior, and proof anchor.

The matrix must preserve:

- contract id
  `objc3c-runtime-metadata-source-to-section-matrix/m253-a002-v1`,
- manifest surface
  `frontend.pipeline.semantic_surface.objc_runtime_metadata_source_to_section_matrix`,
- source-graph node kind ordering
  `interface`, `implementation`, `class`, `metaclass`, `protocol`, `category`,
  `property`, `method`, `ivar`,
- concrete emitted rows for:
  `class`, `protocol`, `category`, `property`, and `ivar`,
- explicit no-standalone-emission rows for:
  `interface`, `implementation`, `metaclass`, and `method`,
- the frozen relocation model
  `zero-sentinel-or-count-plus-pointer-vector` for emitted descriptor families,
- proof binding to metadata-rich source fixtures plus emitted object inspection
  commands,
- evidence path
  `tmp/reports/m253/M253-A002/source_to_section_mapping_completeness_matrix_summary.json`.

`M253-A002` still does not introduce real descriptor payload layouts, startup
registration, runtime loader bootstrap, or standalone emitted
method/selector/string-pool sections.

## M253 layout ordering and visibility policy freeze (B001)

`M253-B001` freezes one emitted metadata layout/visibility policy surface
before semantic finalization and object-format expansion land.

The freeze must preserve:

- contract id
  `objc3c-runtime-metadata-layout-ordering-visibility-policy-freeze/m253-b001-v1`,
- family ordering model
  `image-info-then-class-protocol-category-property-ivar`,
- within-family ordering model
  `ascending-descriptor-ordinal-then-family-aggregate`,
- relocation model
  `zero-sentinel-or-count-plus-pointer-vector`,
- COMDAT policy `disabled`,
- visibility spelling policy
  `local-linkage-omits-explicit-ir-visibility`,
- retention ordering `llvm.used-emission-order`,
- object-format policy model `object-format-neutral-until-m253-b003`,
- the requirement that lowering replay keys and llvm-direct object emission do
  not infer or rewrite another ordering/visibility model.

`M253-B001` does not yet implement semantic finalization of layout decisions;
semantic finalization of layout decisions remains deferred to `M253-B002`.
It also does not add COFF/ELF/Mach-O policy variants; that remains deferred to
`M253-B003`.

## M253 deterministic ordering, visibility, and relocation semantics (B002)

`M253-B002` normalizes one metadata layout policy before IR emission so the
emitter consumes a shared lowering decision instead of re-hardcoding order and
relocation locally.

The implementation must preserve:

- contract id `objc3c-runtime-metadata-layout-policy/m253-b002-v1`,
- normalized layout-policy packets in `lower/objc3_lowering_contract.h`,
- fail-closed builder logic in `lower/objc3_lowering_contract.cpp`,
- named metadata `!objc3.objc_runtime_metadata_layout_policy`,
- metadata node `!55`,
- replay comment `; runtime_metadata_layout_policy = ...`,
- family order `image-info`, `class`, `protocol`, `category`, `property`,
  `ivar`,
- relocation model `zero-sentinel-or-count-plus-pointer-vector`,
- visibility spelling policy `local-linkage-omits-explicit-ir-visibility`,
- retention ordering `llvm.used-emission-order`.

`M253-B002` still does not add COFF/ELF/Mach-O policy variants; that remains
`M253-B003`.

## M253 COFF, ELF, and Mach-O metadata policy surface (B003)

`M253-B003` expands the normalized `M253-B002` packet into one explicit
host-format mapping surface while preserving the existing logical metadata ABI.

The implementation must preserve:

- contract id `objc3c-runtime-metadata-object-format-policy/m253-b003-v1`,
- explicit host-format mapping surface for `coff`, `elf`, and `mach-o`,
- explicit section-spelling models
  `coff-logical-section-spellings`,
  `elf-logical-section-spellings`, and
  `mach-o-data-segment-comma-section-spellings`,
- explicit retention-anchor models
  `llvm.used-appending-global+coff-timestamp-normalization`,
  `llvm.used-appending-global+elf-stable-sections`, and
  `llvm.used-appending-global+mach-o-data-segment-sections`,
- layout-policy fields that carry logical and emitted section names,
- emitter consumption of emitted section spellings rather than logical section
  names directly,
- process-level produced-object detection before post-write determinism.

`M253-B003` does not add new metadata families, runtime registration, or
bootstrap. It only expands the lowering-side emitted-format surface.

## M253 metadata section emission freeze (C001)

`M253-C001` freezes the current real-section emission boundary so later lane-C
implementation issues can replace placeholder bytes without reopening the
contract question.

The implementation must preserve:

- contract id `objc3c-runtime-metadata-section-emission-freeze/m253-c001-v1`,
- payload model `scaffold-placeholder-payloads-until-m253-c002`,
- inventory model
  `image-info-plus-class-protocol-category-property-ivar-sections`,
- image-info payload model
  `internal-{i32,i32}-zeroinitializer-image-info`,
- descriptor payload model
  `private-[1xi8]-zeroinitializer-per-descriptor`,
- aggregate payload model
  `i64-count-plus-pointer-vector-aggregates`,
- the emitted IR comment
  `; runtime_metadata_section_emission_boundary = ...`,
- fail-closed non-goals:
  no method, selector, or string-pool payload emission yet.

## M253 class and metaclass data emission (C002)

Lane-C shall replace the class-family placeholder byte model with real
class/metaclass descriptor bundles while preserving the frozen emitted
inventory, ordering, visibility, object-format, and scaffold boundaries.

`M253-C002` requires:

- contract id `objc3c-runtime-class-metaclass-data-emission/m253-c002-v1`,
- payload model
  `class-source-record-descriptor-bundles-with-inline-metaclass-records`,
- name model `shared-class-name-cstring-per-bundle`,
- super-link model `nullable-super-source-record-bundle-pointer`,
- method-list reference model
  `count-plus-owner-identity-pointer-method-list-ref`,
- emitted IR publication through
  `; runtime_metadata_class_metaclass_emission = ...` and
  `!objc3.objc_runtime_class_metaclass_emission`,
- metadata-only IR admission on ready metadata fixtures so native IR/object
  emission no longer depends on the older global parse/lowering gate for this
  payload family,
- declaration-owner-ordered bundle emission over the current typed metadata
  handoff, runtime-export class accounting, and scaffold class-descriptor
  counts,
- llvm-direct object emission preserving the inline bundle payloads verbatim.

`M253-C002` does not add standalone metaclass sections, selector/string-pool
payloads, standalone method/property/ivar list sections, or runtime
registration/bootstrap. The class family alone advances from scaffold
placeholders to real bundle payloads in this issue.

## M253 protocol and category data emission (C003)

Lane-C shall replace the protocol/category placeholder byte model with real
protocol/category descriptor bundles while preserving the frozen emitted
inventory, ordering, visibility, object-format, and scaffold boundaries.

`M253-C003` requires:

- contract id `objc3c-runtime-protocol-category-data-emission/m253-c003-v1`,
- protocol payload model
  `protocol-descriptor-bundles-with-inherited-protocol-ref-lists`,
- category payload model
  `category-descriptor-bundles-with-attachment-and-protocol-ref-lists`,
- protocol-reference model
  `count-plus-descriptor-pointer-protocol-ref-lists`,
- category-attachment model
  `count-plus-owner-identity-pointer-attachment-lists`,
- emitted IR publication through
  `; runtime_metadata_protocol_category_emission = ...` and
  `!objc3.objc_runtime_protocol_category_emission`,
- fail-closed bundle emission over the current typed metadata handoff, protocol
  source graph, runtime-export protocol/category accounting, and scaffold
  protocol/category descriptor counts,
- expansion of one combined category graph node into explicit
  interface/implementation record bundles before emission,
- llvm-direct object emission preserving the inline protocol/category payloads
  verbatim.

`M253-C003` does not add selector/string-pool payloads, standalone
property/ivar payload sections, or runtime registration/bootstrap. The
protocol/category families alone advance from scaffold placeholders to real
bundle payloads in this issue.

## M253 method/property/ivar payload emission (C004)

Lane-C shall emit deterministic owner-scoped method tables plus real property
and ivar descriptor payload families while preserving the frozen class,
protocol, and category descriptor shapes established in `M253-C002` and
`M253-C003`.

`M253-C004` requires:

- contract id `objc3c-runtime-member-table-emission/m253-c004-v1`,
- method-list payload model
  `owner-scoped-method-table-globals-with-inline-entry-records`,
- method-list grouping model
  `declaration-owner-plus-class-kind-lexicographic`,
- property payload model
  `property-descriptor-records-with-accessor-and-binding-strings`,
- ivar payload model
  `ivar-descriptor-records-with-property-binding-strings`,
- emitted IR publication through
  `; runtime_metadata_member_table_emission = ...` and
  `!objc3.objc_runtime_member_table_emission`,
- fail-closed emission over the current typed metadata handoff, property/ivar
  runtime-export accounting, method graph expansion, and scaffold
  property/ivar descriptor counts,
- deterministic lexicographic ordering of:
  - method list bundles by owner family, declaration-owner identity, and list kind,
  - method entries by selector, owner identity, parameter count, return type,
    and body presence,
  - property/ivar descriptor records by declaration-owner identity, property
    name, and owner identity,
- llvm-direct object emission preserving adjacent owner-scoped method table
  bytes plus the standalone property/ivar descriptor payload sections verbatim.

`M253-C004` does not add selector pools, runtime registration/bootstrap, or
runtime consumption/dispatch over the emitted member tables. It advances the
method/property/ivar families from scaffolds/placeholders to real payload
records only.

## M253 selector/string pool emission (C005)

Lane-C shall replace the older selector-only global scheme with canonical
selector and runtime string pool sections while preserving the frozen class,
protocol, category, property, and ivar descriptor payload contracts from
`M253-C002` through `M253-C004`.

`M253-C005` requires:

- contract id `objc3c-runtime-selector-string-pool-emission/m253-c005-v1`,
- selector pool payload model
  `canonical-selector-cstring-pool-with-stable-ordinal-aggregate`,
- string pool payload model
  `canonical-runtime-string-cstring-pool-with-stable-ordinal-aggregate`,
- emitted IR publication through
  `; runtime_metadata_selector_string_pool_emission = ...` and
  `!objc3.objc_runtime_selector_string_pool_emission`,
- deterministic lexicographic canonicalization of pooled selector cstrings and
  pooled runtime strings,
- retained aggregate globals `@__objc3_sec_selector_pool` and
  `@__objc3_sec_string_pool`,
- message-send lowering sourcing selector pointers from the canonical selector
  pool instead of the older `@.objc3.sel.*` global family,
- llvm-direct object emission preserving `objc3.runtime.selector_pool` and
  `objc3.runtime.string_pool` verbatim.

`M253-C005` does not rewire existing descriptor bundles to pooled string
pointers, nor does it add runtime registration/bootstrap or mutable runtime
interning tables. It introduces canonical selector/string pool families only.

## M253 binary inspection harness for emitted metadata (C006)

Lane-C shall freeze one shared emitted-metadata inspection corpus so object-file
structure is asserted against the current section families instead of inferred
from earlier scaffold-only probes.

`M253-C006` requires:

- contract id `objc3c-runtime-binary-inspection-harness/m253-c006-v1`,
- positive corpus model
  `positive-structural-section-and-symbol-corpus-with-case-specific-absence-checks`,
- negative corpus model
  `negative-compile-failure-gating-with-no-object-inspection`,
- emitted IR publication through
  `; runtime_metadata_binary_inspection_harness = ...` and
  `!objc3.objc_runtime_binary_inspection_harness`,
- shared inspection commands:
  - `llvm-readobj --sections module.obj`
  - `llvm-objdump --syms module.obj`,
- a positive corpus spanning scaffold-only, class-heavy, category-heavy, and
  selector-pool-heavy emitted objects,
- a fail-closed negative compile case that produces diagnostics but no
  `module.obj` and no `module.manifest.json`,
- section-family assertions covering
  `objc3.runtime.image_info`,
  `objc3.runtime.class_descriptors`,
  `objc3.runtime.protocol_descriptors`,
  `objc3.runtime.category_descriptors`,
  `objc3.runtime.property_descriptors`,
  `objc3.runtime.ivar_descriptors`,
  `objc3.runtime.selector_pool`, and
  `objc3.runtime.string_pool`,
- aggregate symbol assertions covering the corresponding `__objc3_*` section
  anchors and zero/nonzero offsets where applicable.

`M253-C006` does not add new metadata families, runtime registration/bootstrap,
or descriptor-family rewiring. It expands binary inspection evidence only.

## M253 object packaging and retention boundary (D001)

Lane-D shall freeze the current produced-object handoff so later archive, link,
and startup-registration work extends one explicit module.obj boundary rather
than redefining retention and discoverability anchors.

`M253-D001` requires:

- contract id `objc3c-runtime-object-packaging-retention-boundary/m253-d001-v1`,
- boundary model
  `current-object-file-boundary-with-retained-metadata-section-aggregates`,
- retention-anchor model `llvm.used-plus-aggregate-section-symbols`,
- emitted IR publication through
  `; runtime_metadata_object_packaging_retention = ...` and
  `!objc3.objc_runtime_object_packaging_retention`,
- frozen current object artifact `module.obj`,
- frozen aggregate symbol prefix `__objc3_sec_`,
- shared discovery commands:
  - `llvm-readobj --sections module.obj`
  - `llvm-objdump --syms module.obj`,
- one positive proof over the current metadata-rich emitted object path, and
- one fail-closed negative proof that compile failure emits diagnostics but no
  manifest/object/backend marker.

`M253-D001` does not add archive packaging, link-registration, or startup
registration/bootstrap behavior. It freezes the current produced-object
boundary only.

## M253 linker retention anchors and dead-strip resistance (D002)

Lane-D shall keep emitted metadata discoverable after the object is packaged
into one library/archive by publishing one public linker anchor, one public
discovery root, and one deterministic driver response-file payload for the
current object format.

`M253-D002` requires:

- contract id
  `objc3c-runtime-linker-retention-and-dead-strip-resistance/m253-d002-v1`,
- anchor model `public-linker-anchor-rooted-in-discovery-table`,
- discovery model `public-discovery-root-over-retained-metadata-aggregates`,
- emitted IR publication through
  `; runtime_metadata_linker_retention = ...` and
  `!objc3.objc_runtime_linker_retention`,
- one hashed public linker-anchor symbol
  `objc3_runtime_metadata_link_anchor_<hash>`,
- one hashed public discovery-root symbol
  `objc3_runtime_metadata_discovery_root_<hash>`,
- one emitted response artifact
  `module.runtime-metadata-linker-options.rsp`,
- one emitted discovery artifact
  `module.runtime-metadata-discovery.json`,
- current-format driver linker flag models:
  - COFF: `-Wl,/include:<symbol>`
  - ELF: `-Wl,--undefined=<symbol>`
  - Mach-O: `-Wl,-u,_<symbol>`,
- one happy-path proof that static-library packaging drops metadata without the
  response file and retains metadata with it, and
- one fail-closed negative proof that compile failure produces no object,
  backend marker, linker response file, or discovery artifact.

`M253-D002` does not yet claim multi-archive fan-in or cross-translation-unit
anchor-merging behavior. Those edge cases stay deferred to `M253-D003`.

## M253 archive/static-link metadata discovery behavior (D003)

Lane-D shall close the remaining archive/static-link metadata discovery gaps by
making public linker/discovery symbols translation-unit-stable, by keeping
metadata-only library objects from exporting a colliding public `objc3c_entry`,
and by standardizing one merged discovery/response artifact pair for
multi-archive static-link fan-in.

`M253-D003` requires:

- contract id
  `objc3c-runtime-metadata-archive-and-static-link-discovery/m253-d003-v1`,
- anchor-seed model
  `module-and-metadata-replay-plus-translation-unit-identity`,
- translation-unit identity model
  `input-path-plus-parse-and-lowering-replay`,
- merge model `deduplicated-driver-flag-fan-in`,
- emitted IR publication through
  `; runtime_metadata_archive_static_link_discovery = ...` and
  `!objc3.objc_runtime_archive_static_link_discovery`,
- object-level discovery JSON fields
  `translation_unit_identity_model` and
  `translation_unit_identity_key`,
- canonical merged artifacts
  `module.merged.runtime-metadata-linker-options.rsp` and
  `module.merged.runtime-metadata-discovery.json`,
- one positive proof that identical module/metadata source compiled from
  distinct translation units yields distinct public anchor/discovery symbols,
- one positive proof that multi-archive plain link drops metadata while the
  merged response artifact retains it, and
- one fail-closed negative proof that malformed or colliding discovery inputs
  do not produce a false-success merged artifact.

`M253-D003` does not add runtime registration or startup bootstrap behavior.

## M253 metadata emission gate (E001)

Lane-E shall freeze one fail-closed evidence gate over the implemented
source-to-section matrix, object-format policy, binary inspection corpus, and
archive/static-link discovery proof before `M253-E002` cross-lane closeout
begins.

`M253-E001` requires:

- contract id `objc3c-runtime-metadata-emission-gate/m253-e001-v1`,
- evidence model `a002-b003-c006-d003-summary-chain`,
- failure model `fail-closed-on-upstream-summary-drift`,
- emitted IR publication through
  `; runtime_metadata_emission_gate = ...` and
  `!objc3.objc_runtime_metadata_emission_gate`,
- direct validation of the canonical upstream summaries:
  - `M253-A002`
  - `M253-B003`
  - `M253-C006`
  - `M253-D003`,
- one checker/test pair that rejects drift in any upstream evidence file, and
- an explicit handoff that `M253-E002` is the next closeout issue.

`M253-E001` does not add new object-emission behavior or runtime registration.

## M253 cross-lane object-emission gate and closeout (E002)

Lane-E shall close the emitted-metadata tranche by replaying one integrated
native object-emission gate over the canonical `M253-E001` summary plus fresh
class/category/message-send object probes before `M254` startup registration
work begins.

`M253-E002` requires:

- contract id
  `objc3c-runtime-cross-lane-object-emission-closeout/m253-e002-v1`,
- evidence model
  `e001-summary-plus-integrated-native-object-emission-probes`,
- failure model `fail-closed-on-summary-or-integrated-probe-drift`,
- emitted IR publication through
  `; runtime_metadata_object_emission_closeout = ...` and
  `!objc3.objc_runtime_metadata_object_emission_closeout`,
- direct validation of the canonical upstream summaries:
  - `M253-A002`
  - `M253-B003`
  - `M253-C006`
  - `M253-D003`
  - `M253-E001`,
- integrated native object probes over class, category, and message-send cases
  that keep source graph closure, object-format policy, binary-inspection
  inventories, and linker/discovery artifacts aligned on the same outputs, and
- one fail-closed negative compile case that preserves `O3S206` diagnostics and
  blocks object/discovery emission.

`M253-E002` is a closeout gate only; it adds no runtime startup registration,
class registration, or new metadata section families.

## M254 translation-unit registration surface freeze (A001)

`M254-A001` freezes one manifest-published preregistration boundary over the
already-emitted metadata/object artifacts.

- contract id `objc3c-translation-unit-registration-surface-freeze/m254-a001-v1`
- surface path
  `frontend.pipeline.semantic_surface.objc_runtime_translation_unit_registration_contract`
- payload model `runtime-metadata-binary-plus-linker-retention-sidecars-v1`
- canonical runtime-owned payload inventory:
  - `module.runtime-metadata.bin`
  - `module.runtime-metadata-linker-options.rsp`
  - `module.runtime-metadata-discovery.json`
- constructor-root ownership model
  `compiler-emits-constructor-root-runtime-owns-registration-state`
- reserved constructor root `__objc3_runtime_register_image_ctor`
- runtime-owned registration entrypoint `objc3_runtime_register_image`
- translation-unit identity model `input-path-plus-parse-and-lowering-replay`

Non-goals:

- no constructor-root emission yet
- no startup registration yet
- no runtime bootstrap yet

`M254-A002` must preserve this contract while materializing registration
manifests and constructor-root ownership.

## M254 translation-unit registration manifest implementation (A002)

`M254-A002` turns the frozen `M254-A001` preregistration boundary into one real
emitted translation-unit registration manifest artifact.

- contract id `objc3c-translation-unit-registration-manifest/m254-a002-v1`
- surface path
  `frontend.pipeline.semantic_surface.objc_runtime_translation_unit_registration_manifest`
- payload model `translation-unit-registration-manifest-json-v1`
- emitted registration-manifest artifact
  `module.runtime-registration-manifest.json`
- preserved runtime-owned payload inventory:
  - `module.runtime-metadata.bin`
  - `module.runtime-metadata-linker-options.rsp`
  - `module.runtime-metadata-discovery.json`
- constructor-root ownership model
  `compiler-emits-constructor-root-runtime-owns-registration-state`
- manifest authority model
  `registration-manifest-authoritative-for-constructor-root-shape`
- init-stub symbol prefix `__objc3_runtime_register_image_init_stub_`
- init-stub ownership model
  `lowering-emits-init-stub-from-registration-manifest`
- constructor priority policy `deferred-until-m254-c001`
- runtime-owned registration entrypoint `objc3_runtime_register_image`
- translation-unit identity model `input-path-plus-parse-and-lowering-replay`

Non-goals:

- no init-stub emission yet
- no constructor-root emission yet
- no automatic startup registration yet
- no runtime bootstrap execution yet

## M254 startup bootstrap invariants (B001)

`M254-B001` freezes the semantic invariants that later startup-registration and
runtime-bootstrap implementation must preserve.

- contract id `objc3c-runtime-startup-bootstrap-invariants/m254-b001-v1`
- surface path
  `frontend.pipeline.semantic_surface.objc_runtime_startup_bootstrap_invariants`
- upstream manifest contract id
  `objc3c-translation-unit-registration-manifest/m254-a002-v1`
- duplicate-registration policy
  `fail-closed-by-translation-unit-identity-key`
- realization-order policy
  `constructor-root-then-registration-manifest-order`
- failure mode
  `abort-before-user-main-no-partial-registration-commit`
- image-local initialization scope
  `runtime-owned-image-local-registration-state`
- constructor-root uniqueness policy
  `one-startup-root-per-translation-unit-identity`
- constructor-root consumption model
  `startup-root-consumes-registration-manifest`
- startup execution mode `deferred-until-m254-c001`

Non-goals:

- no live startup execution yet
- no duplicate-registration enforcement yet
- no image-local realization/runtime bootstrap execution yet

## M254 live startup bootstrap semantics (B002)

`M254-B002` lands the first live runtime-side startup/bootstrap enforcement
surface while constructor-root emission is still deferred to `M254-C001`.

- contract id `objc3c-runtime-startup-bootstrap-semantics/m254-b002-v1`
- surface path
  `frontend.pipeline.semantic_surface.objc_runtime_startup_bootstrap_semantics`
- duplicate-registration policy
  `fail-closed-by-translation-unit-identity-key`
- realization-order policy
  `constructor-root-then-registration-manifest-order`
- failure mode
  `abort-before-user-main-no-partial-registration-commit`
- runtime result model `zero-success-negative-fail-closed`
- runtime state snapshot symbol
  `objc3_runtime_copy_registration_state_for_testing`
- registration order ordinal model
  `strictly-monotonic-positive-registration-order-ordinal`

Required behavior:

- duplicate translation-unit identity keys are rejected with status `-2`
- non-monotonic registration ordinals are rejected with status `-3`
- invalid descriptors are rejected with status `-1`
- failed registrations do not partially commit runtime-owned state
- emitted `module.runtime-registration-manifest.json` payloads carry the same
  status-code and policy surface consumed by the native runtime probe

## M254 bootstrap lowering freeze (C001)

`M254-C001` freezes the lowering boundary that later startup materialization
must implement from the emitted registration manifest plus the live
`M254-B002` bootstrap semantics packet.

- contract id `objc3c-runtime-bootstrap-lowering-freeze/m254-c001-v1`
- surface path
  `frontend.pipeline.semantic_surface.objc_runtime_bootstrap_lowering_contract`
- boundary model
  `registration-manifest-driven-constructor-root-init-stub-and-registration-table-lowering`
- constructor root symbol `__objc3_runtime_register_image_ctor`
- init-stub symbol prefix `__objc3_runtime_register_image_init_stub_`
- registration-table symbol prefix `__objc3_runtime_registration_table_`
- future global-ctor list model `llvm.global_ctors-single-root-priority-65535`
- constructor-root emission state `deferred-until-m254-c002`
- init-stub emission state `deferred-until-m254-c002`
- registration-table emission state `deferred-until-m254-c002`

Non-goals:

- no emitted `@llvm.global_ctors` yet
- no emitted constructor-root global yet
- no emitted init-stub global yet
- no emitted registration-table global yet

## M254 constructor and init-stub emission (C002)

`M254-C002` materializes the first real lowering-owned startup registration
path from the emitted registration manifest plus the live `M254-B002`
semantics surface.

- contract id `objc3c-runtime-constructor-init-stub-emission/m254-c002-v1`
- emitted ctor root `__objc3_runtime_register_image_ctor`
- emitted init-stub prefix `__objc3_runtime_register_image_init_stub_`
- emitted registration-table prefix `__objc3_runtime_registration_table_`
- emitted image descriptor prefix `__objc3_runtime_image_descriptor_`
- emitted `@llvm.global_ctors` model
  `llvm.global_ctors-single-root-priority-65535`
- init stub calls `objc3_runtime_register_image`
- non-zero registration status fails closed through `abort()`
- emitted registration manifest must publish the exact derived init-stub and
  registration-table symbols
- COFF object emission must materialize the startup constructor path in
  `.CRT$XCU`

## M254 registration-table emission and image-local initialization (C003)

`M254-C003` expands the lowering-owned startup path so later runtime image
walks consume one self-describing per-image registration boundary.

- contract id
  `objc3c-runtime-registration-table-image-local-initialization/m254-c003-v1`
- registration-table layout model
  `abi-version-field-count-image-descriptor-discovery-root-linker-anchor-family-aggregates-selector-string-pools-image-local-init-state`
- image-local init-state prefix
  `__objc3_runtime_image_local_init_state_`
- registration-table ABI version `1`
- registration-table pointer-field count `11`
- emitted init stub guards startup with the image-local init-state cell before
  runtime registration
- emitted registration tables now include section-root and pool-root pointers
  for later runtime image-walk stages

## M254 runtime bootstrap API freeze (D001)

`M254-D001` freezes the runtime-owned bootstrap API surface that later
registrar/image-walk and deterministic-reset work must preserve.

- contract id `objc3c-runtime-bootstrap-api-freeze/m254-d001-v1`
- semantic surface path
  `frontend.pipeline.semantic_surface.objc_runtime_bootstrap_api_contract`
- public header path `native/objc3c/src/runtime/objc3_runtime.h`
- archive path `artifacts/lib/objc3_runtime.lib`
- registration status enum type `objc3_runtime_registration_status_code`
- image descriptor type `objc3_runtime_image_descriptor`
- selector handle type `objc3_runtime_selector_handle`
- registration snapshot type `objc3_runtime_registration_state_snapshot`
- preserved entrypoints:
  - `objc3_runtime_register_image`
  - `objc3_runtime_lookup_selector`
  - `objc3_runtime_dispatch_i32`
  - `objc3_runtime_copy_registration_state_for_testing`
  - `objc3_runtime_reset_for_testing`
- startup invocation model
  `generated-init-stub-calls-runtime-register-image`
- runtime locking model `process-global-mutex-serialized-runtime-state`

Non-goals:

- no emitted-metadata image walk yet
- no class/protocol/category realization graph yet

## M254 registrar implementation and image walk (D002)

`M254-D002` lands the private registrar/image-walk bridge that preserves the
frozen D001 public runtime API while letting emitted startup code stage and walk
the live registration table.

- contract id `objc3c-runtime-bootstrap-registrar-image-walk/m254-d002-v1`
- semantic surface path
  `frontend.pipeline.semantic_surface.objc_runtime_bootstrap_registrar_contract`
- private bootstrap header
  `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- private stage-registration symbol
  `objc3_runtime_stage_registration_table_for_bootstrap`
- private image-walk snapshot symbol
  `objc3_runtime_copy_image_walk_state_for_testing`
- image-walk model
  `registration-table-roots-validated-and-staged-before-realization`
- discovery-root validation model
  `linker-anchor-must-point-at-discovery-root`
- selector-pool interning model
  `canonical-selector-pool-preinterned-during-startup-image-walk`
- realization staging model
  `registration-table-roots-retained-for-later-realization`
- no expanded deterministic reset coverage beyond the current runtime-owned
  testing hooks
## M254 realization sequencing and deterministic reset hooks (D003)

`M254-D003` preserves the frozen public runtime bootstrap API while extending
the private bootstrap boundary with deterministic same-process reset/replay:

- contract id `objc3c-runtime-bootstrap-reset-replay/m254-d003-v1`
- semantic surface
  `frontend.pipeline.semantic_surface.objc_runtime_bootstrap_reset_contract`
- private replay hook
  `objc3_runtime_replay_registered_images_for_testing`
- private reset/replay snapshot hook
  `objc3_runtime_copy_reset_replay_state_for_testing`
- reset lifecycle model
  `reset-clears-live-runtime-state-and-zeroes-image-local-init-cells`
- replay order model
  `replay-re-registers-retained-images-in-original-registration-order`

## M254 driver, link, and runtime launch integration (D004)

`M254-D004` keeps the emitted bootstrap/runtime registration surface unchanged
while freezing the operator launch contract consumed by compile, proof, and
smoke:

- contract id `objc3c-runtime-launch-integration/m254-d004-v1`
- emitted authority remains
  `module.runtime-registration-manifest.json`
- runtime archive resolution model
  `registration-manifest-runtime-archive-path-is-authoritative`
- linker-flag consumption model
  `registration-manifest-driver-linker-flags-feed-proof-and-smoke-link-commands`
- compile wrapper script `scripts/objc3c_native_compile.ps1`
- compile proof script `scripts/run_objc3c_native_compile_proof.ps1`
- execution smoke script `scripts/check_objc3c_native_execution_smoke.ps1`
- launch integration ready flag `launch_integration_ready`

## M254 startup registration gate (E001)

`M254-E001` introduces one fail-closed lane-E gate over the live startup
registration/bootstrap evidence chain without adding new runtime behavior:

- contract id `objc3c-runtime-startup-registration-gate/m254-e001-v1`
- evidence model `a002-b002-c003-d003-d004-summary-chain`
- failure model `fail-closed-on-bootstrap-evidence-drift`
- upstream evidence must stay rooted in the canonical summaries for `M254-A002`,
  `M254-B002`, `M254-C003`, `M254-D003`, and `M254-D004`
- any drift in manifest authority, bootstrap status codes, registration-table
  realization, deterministic reset/replay, or launch integration must fail
  closed before `M254-E002`

## M254 replay and bootstrap proof plus runbook closeout (E002)

`M254-E002` closes the milestone by composing the already-landed `M254-E001`
gate with one published operator runbook and one live smoke replay of that
runbook:

- contract id `objc3c-runtime-replay-bootstrap-closeout/m254-e002-v1`
- closeout model `e001-gate-plus-live-operator-runbook-smoke`
- canonical runbook
  `docs/runbooks/m254_bootstrap_replay_operator_runbook.md`
- canonical closeout evidence
  `tmp/reports/m254/M254-E002/replay_bootstrap_runbook_closeout_summary.json`

## M255 dispatch surface classification (A001)

`M255-A001` freezes the live dispatch taxonomy without widening runtime
behavior yet:

- contract id `objc3c-dispatch-surface-classification/m255-a001-v1`
- instance/class/super/dynamic dispatch remain classified against the live
  runtime family
  `objc3_runtime_dispatch_i32-objc3_msgsend_i32-compat`
- direct dispatch remains a reserved non-goal in `M255-A001`
- the freeze exists to hand off a deterministic starting point to `M255-A002`

## M255 dispatch-site modeling implementation (A002)

`M255-A002` materializes the frozen dispatch taxonomy into one real frontend,
semantic, and lowering handoff for the current native path:

- known-class identifiers plus implicit `self`/`super` must be normalized before
  semantic validation and LLVM lowering
- semantic surface `frontend.pipeline.semantic_surface.objc_dispatch_surface_classification_surface`
  publishes the live dispatch counts and entrypoint families
- lowering handoff `lowering_dispatch_surface_classification` must replay the
  same contract state into IR emission
- the canonical proof fixture
  `tests/tooling/fixtures/native/m255_dispatch_surface_modeling.objc3` must
  produce instance `2`, class `2`, super `1`, direct `0`, dynamic `1`
- `objc3c-native` must emit the textual profile line
  `frontend_objc_dispatch_surface_classification_profile` and named metadata
  `!objc3.objc_dispatch_surface_classification`
- the live lowering handoff contract remains
  `objc3c-dispatch-surface-classification/m255-a001-v1`

## M255 dispatch legality and selector resolution (B001)

`M255-B001` freezes the live legality and selector-resolution rules that the
current native dispatch path consumes:

- contract id `objc3c-dispatch-legality-selector-resolution/m255-b001-v1`
- boundary model
  `selector-normalized-arity-checked-receiver-required-no-overload`
- ambiguity policy
  `fail-closed-on-unresolved-or-ambiguous-selector-resolution`
- selector forms stay normalized unary/keyword selectors only
- receiver presence remains mandatory before dispatch lowering consumes the site
- direct dispatch remains reserved
- this freeze exists to hand off one deterministic legality packet to
  `M255-B002`

## M255 selector resolution and ambiguity implementation (B002)

`M255-B002` implements the first live selector-resolution rules that sit between
dispatch-site classification and runtime dispatch emission:

- contract id `objc3c-selector-resolution-ambiguity/m255-b002-v1`
- concrete receiver policy
  `self-super-known-class-receivers-resolve-concretely`
- dynamic fallback policy
  `non-concrete-receivers-remain-runtime-dynamic`
- overload policy
  `no-overload-recovery-exact-signature-or-fail-closed`
- concrete `self`, `super`, and known-class receivers now resolve against the
  semantic surface before lowering continues
- missing concrete selectors emit `O3S216`
- incompatible concrete interface/implementation signatures emit `O3S217`
- lowering still uses the live runtime dispatch family and does not add a
  second overload-recovery pass

## M255 super/direct/dynamic legality and method-family expansion (B003)

`M255-B003` closes the remaining legality/runtime-rule edges that sit between
dispatch-site classification and live runtime dispatch lowering:

- contract id `objc3c-super-dynamic-method-family/m255-b003-v1`
- super legality policy
  `super-requires-enclosing-method-and-real-superclass`
- direct dispatch policy
  `direct-dispatch-remains-reserved-non-goal`
- dynamic dispatch policy
  `dynamic-dispatch-preserves-runtime-resolution-and-method-family-accounting`
- runtime-visible method-family policy
  `super-and-dynamic-sites-preserve-method-family-runtime-visibility`
- super outside an implementation method fails closed with `O3S216`
- root-class `super` dispatch also fails closed with `O3S216`
- admitted super/dynamic sites continue to lower through the live runtime
  entrypoint family rather than inventing a second direct-dispatch path

## M255 dispatch lowering ABI freeze (C001)

`M255-C001` freezes the lane-C lowering boundary that will later switch native
IR off the compatibility bridge and onto the canonical runtime ABI:

- contract id `objc3c-runtime-dispatch-lowering-abi-freeze/m255-c001-v1`
- canonical runtime entrypoint `objc3_runtime_dispatch_i32`
- compatibility bridge entrypoint `objc3_msgsend_i32`
- selector lookup symbol `objc3_runtime_lookup_selector`
- selector handle type `objc3_runtime_selector_handle`
- receiver/result ABI remain `i32`
- selector operand remains a lowered cstring pointer until `M255-C002`
- fixed argument marshalling remains `4` `i32` slots with zero padding
- default lowering target remains the compatibility bridge until `M255-C002`
- `super`, nil, and direct runtime-entrypoint cutover stay deferred until
  `M255-C003`

## M255 runtime call ABI generation for instance and class sends (C002)

`M255-C002` applies the frozen lane-C ABI to real emitted call sites:

- contract id `objc3c-runtime-call-abi-instance-class-dispatch/m255-c002-v1`
- normalized instance sends lower to `objc3_runtime_dispatch_i32`
- normalized class sends lower to `objc3_runtime_dispatch_i32`
- normalized super/dynamic/deferred sends stay on `objc3_msgsend_i32` until
  `M255-C003`
- selector operands remain lowered cstring pointers
- the fixed four-slot `i32` argument vector is preserved unchanged

## M255 super, nil, and direct runtime call ABI cutover (C003)

`M255-C003` continues the lane-C cutover while keeping dynamic compatibility
separate:

- contract id `objc3c-runtime-call-abi-super-nil-direct-dispatch/m255-c003-v1`
- normalized super sends now lower to `objc3_runtime_dispatch_i32`
- canonical nil-receiver sends no longer lower to local IR elision; they lower
  through `objc3_runtime_dispatch_i32`, which owns the nil `0` result
- normalized dynamic sends remain on `objc3_msgsend_i32` until `M255-C004`
- reserved direct-dispatch surfaces fail closed if they reach IR emission

## M255 live dispatch cutover and shim-removal boundary (C004)

`M255-C004` removes the final live compatibility-bridge dependency:

- contract id `objc3c-runtime-call-abi-live-dispatch-cutover/m255-c004-v1`
- normalized dynamic sends now lower to `objc3_runtime_dispatch_i32`
- all supported live sends lower to `objc3_runtime_dispatch_i32`
- `objc3_msgsend_i32` remains exported only as compatibility/test evidence and
  is no longer emitted by live-path IR
- reserved direct-dispatch surfaces remain fail closed

## M255 lookup and dispatch runtime freeze (D001)

`M255-D001` freezes the runtime-owned boundary that later selector interning,
lookup-table, cache, and slow-path issues must preserve:

- contract id `objc3c-runtime-lookup-dispatch-freeze/m255-d001-v1`
- canonical selector lookup symbol `objc3_runtime_lookup_selector`
- canonical dispatch symbol `objc3_runtime_dispatch_i32`
- canonical selector handle type `objc3_runtime_selector_handle`
- selector interning model
  `process-global-selector-intern-table-stable-id-per-canonical-selector-spelling`
- metadata-backed selector lookup tables remain deferred to `M255-D002`
- method-cache and runtime slow-path lookup remain deferred to `M255-D003`
- protocol/category-aware method resolution remains deferred to `M255-D004`
- unsupported runtime-resolution surfaces remain fail closed until later lane-D
  implementation issues materialize them explicitly

## M255 selector interning and lookup tables (D002)

`M255-D002` materializes the first runtime-owned selector table built from
registration-table selector pools while preserving the frozen D001 public ABI:

- contract id `objc3c-runtime-selector-lookup-tables/m255-d002-v1`
- selector table state snapshot symbol
  `objc3_runtime_copy_selector_lookup_table_state_for_testing`
- per-selector lookup entry snapshot symbol
  `objc3_runtime_copy_selector_lookup_entry_for_testing`
- interning model
  `registered-selector-pools-materialize-process-global-stable-id-table`
- merge model
  `per-image-selector-pools-deduplicated-and-merged-across-registration-order`
- dynamic fallback model
  `unknown-selector-lookups-remain-dynamic-until-m255-d003`
- replay model
  `reset-replay-rebuilds-metadata-backed-selector-table-in-registration-order`

## M255 method-cache and slow-path lookup (D003)

`M255-D003` turns runtime dispatch into a live method-cache surface backed by
registered class/metaclass records and emitted callable method tables:

- contract id `objc3c-runtime-method-cache-slow-path-lookup/m255-d003-v1`
- method cache state snapshot symbol
  `objc3_runtime_copy_method_cache_state_for_testing`
- method cache entry snapshot symbol
  `objc3_runtime_copy_method_cache_entry_for_testing`
- receiver normalization model
  `known-class-and-class-self-receivers-normalize-to-one-metaclass-cache-key`
- slow-path resolution model
  `registered-class-and-metaclass-records-drive-deterministic-slow-path-method-resolution`
- cache model
  `normalized-receiver-plus-selector-stable-id-positive-and-negative-cache`
- fallback model
  `unsupported-or-ambiguous-runtime-resolution-falls-back-to-compatibility-dispatch-formula`

## M255 protocol and category-aware method resolution (D004)

`M255-D004` extends the D003 live runtime path beyond class/metaclass bodies:

- contract id `objc3c-runtime-protocol-category-method-resolution/m255-d004-v1`
- preserved dependencies:
  - `objc3c-runtime-lookup-dispatch-freeze/m255-d001-v1`
  - `objc3c-runtime-selector-lookup-tables/m255-d002-v1`
  - `objc3c-runtime-method-cache-slow-path-lookup/m255-d003-v1`
- category resolution model
  `class-bodies-win-first-category-implementation-records-supply-next-live-method-tier`
- protocol declaration model
  `adopted-and-inherited-protocol-method-lists-provide-declaration-aware-negative-resolution`
- fallback model
  `conflicting-category-or-protocol-resolution-fails-closed-to-compatibility-dispatch`
- runtime snapshots stay on the preserved D003 boundary:
  - `objc3_runtime_copy_method_cache_state_for_testing`
  - `objc3_runtime_copy_method_cache_entry_for_testing`

## M255 live dispatch gate (E001)

`M255-E001` freezes one fail-closed lane-E gate over the already-landed live
dispatch path:

- contract id `objc3c-runtime-live-dispatch-gate/m255-e001-v1`
- evidence model `a002-b003-c004-d004-summary-chain`
- shim boundary model
  `live-runtime-dispatch-required-compatibility-shim-evidence-only`
- failure model `fail-closed-on-live-dispatch-evidence-drift`
- the gate requires `M255-C004` to keep all supported live sends on
  `objc3_runtime_dispatch_i32`
- the gate requires `M255-D004` to keep live category-backed resolution and
  protocol-backed negative lookup evidence on the runtime-owned slow path
- `objc3_msgsend_i32` remains exported only as compatibility/test evidence and
  is not an acceptable substitute for live-dispatch proof
- `M255-E002` is the explicit handoff for replacing shim-based smoke and
  closeout gates with integrated live-dispatch evidence

## M255 live dispatch smoke and replay closeout (E002)

`M255-E002` closes the live-dispatch tranche by making the integrated smoke and
replay proof authoritative:

- contract id `objc3c-runtime-live-dispatch-smoke-replay-closeout/m255-e002-v1`
- execution smoke now publishes `requires_live_runtime_dispatch`
- the canonical live smoke summary path is
  `tmp/artifacts/objc3c-native/execution-smoke/m255_e002_live_dispatch_smoke/summary.json`
- execution replay proof now canonicalizes `runtime_library`,
  `compatibility_runtime_shim`, and `live_runtime_dispatch_default_symbol`
- supported message-send and runtime-dispatch negative fixtures now assert
  `objc3_runtime_dispatch_i32`
- `objc3_msgsend_i32` remains compatibility/test evidence only and is not an
  acceptable substitute for live smoke/replay proof

## M256 executable class/protocol/category source closure (A001)

`M256-A001` freezes the parser/sema/IR source-closure boundary that later
realization work must preserve:

- contract id
  `objc3c-executable-class-protocol-category-source-closure/m256-a001-v1`
- parser-owned interface/implementation/category identities remain canonical
  for:
  - superclass inheritance input
  - metaclass derivation input
  - adopted protocol composition ordering
  - category attachment ownership
- sema must preserve one deterministic closure over:
  - `interface_implementation_summary`
  - `protocol_category_composition_summary`
  - `class_protocol_category_linking_summary`
- IR must continue to publish
  `!objc3.objc_interface_implementation`,
  `!objc3.objc_protocol_category`, and
  `!objc3.objc_class_protocol_category_linking`
  as the canonical proof surface for this frozen source boundary
- this issue is freeze/evidence only and therefore does not claim:
  - runtime class realization
  - category merge semantics
  - protocol conformance runtime enforcement
  - instance storage/layout behavior
  - executable method binding
- the next implementation issue is `M256-A002`

## M256 class/metaclass declaration completeness plus inheritance modeling (A002)

`M256-A002` upgrades the source closure from the earlier freeze into a
runtime-facing declaration model:

- contract id
  `objc3c-executable-class-metaclass-source-closure/m256-a002-v1`
- the executable metadata source graph now carries explicit declaration-owned:
  - class object identities
  - metaclass object identities
  - superclass and super-metaclass identities
  - instance/class method-owner identities
- IR republishes the same closure through
  `; executable_class_metaclass_source_closure = ...`
- the canonical identity models are:
  - `declaration-owned-class-parent-plus-metaclass-parent-identities`
  - `declaration-owned-instance-class-method-owner-identities`
  - `declaration-owned-class-and-metaclass-object-identities`
- this issue remains source-model work only and therefore does not yet claim:
  - live runtime class realization
  - root-class bootstrapping
  - category merge or protocol conformance runtime behavior
- the next implementation issue is `M256-A003`

## M256 protocol/category source-surface completion for executable runtime (A003)

`M256-A003` upgrades the source closure into a runtime-facing
protocol/category declaration model:

- contract id
  `objc3c-executable-protocol-category-source-closure/m256-a003-v1`
- the executable metadata source graph now carries explicit declaration-owned:
  - protocol inheritance identities
  - category attachment identities
  - adopted-protocol conformance identities
- IR republishes the same closure through
  `; executable_protocol_category_source_closure = ...`
- the canonical identity models are:
  - `protocol-declaration-owned-inherited-protocol-identities`
  - `category-declaration-owned-class-interface-implementation-attachment-identities`
  - `category-declaration-owned-adopted-protocol-conformance-identities`
- this issue remains source-model work only and therefore does not yet claim:
  - live runtime protocol conformance enforcement
  - category merge behavior
  - object-model semantic rule enforcement
- the next implementation issue is `M256-B001`

## M256 object-model semantic rules (B001)

`M256-B001` freezes the semantic-rule boundary consumed by later executable
class/protocol/category runtime work:

- contract id `objc3c-object-model-semantic-rules/m256-b001-v1`
- sema remains authoritative for:
  - realization legality
  - inheritance legality
  - override compatibility
  - declared protocol conformance policy
  - deterministic category merge policy
- parser remains source-only for superclass, adoption, and category-owner
  identities
- IR remains proof-only for the frozen semantic boundary and does not yet claim
  executable enforcement
- the frozen semantic models are:
  - `interface-plus-implementation-pair-required-before-runtime-realization`
  - `single-superclass-no-cycles-rooted-in-source-closure-parent-identities`
  - `selector-kind-and-instance-class-ownership-must-remain-compatible-before-runtime-binding`
  - `declared-adoption-requires-required-member-coverage-optional-members-are-non-blocking`
- `deterministic-declaration-order-with-fail-closed-conflict-detection-before-runtime-installation`
- the next implementation issue is `M256-B002`

## M256 protocol conformance and required/optional member enforcement (B002)

`M256-B002` implements the first live protocol-conformance legality pass over
the frozen M256 source graph:

- contract id
  `objc3c-protocol-conformance-required-optional-enforcement/m256-b002-v1`
- parser owns required/optional partitioning for protocol methods and
  properties
- sema owns required-member closure construction and conformance enforcement
- required methods and required properties are enforced
- optional members remain non-blocking
- inherited protocol requirements fail closed on incompatible required members
- deterministic conformance diagnostics use `O3S218`
- IR remains a downstream proof consumer of the sema-owned conformance result

## M256 category merge and conflict semantics (B003)

`M256-B003` turns category attachment on realized classes into a live semantic
surface:

- contract id
  `objc3c-category-merge-conflict-semantics/m256-b003-v1`
- parser preserves category attachment order/identity for sema-owned merge
  ordering
- sema owns deterministic merge-surface construction for realized classes only
- concrete message resolution consumes the merged category surface before base
  class fallback
- declared protocol conformance consumes the same merged category surface
- realized-class category interface/implementation pairs fail closed when one
  side is missing
- incompatible attached category members fail closed with `O3S219`
- IR remains a downstream proof consumer of the sema-owned merge legality

## M256 inheritance, override, and realization legality (B004)

`M256-B004` turns the remaining realized-class inheritance and override rules
into a live semantic surface:

- contract id
  `objc3c-inheritance-override-realization-legality/m256-b004-v1`
- parser preserves raw superclass spellings and member identities only
- sema owns live fail-closed legality for:
  - missing superclass interfaces on realized classes
  - superclass cycles on realized classes
  - missing realized superclass implementation closure
  - inherited method override compatibility
  - selector-kind drift across superclass chains
  - inherited property compatibility
- deterministic failures collapse onto `O3S220`
- IR remains a downstream proof consumer of the sema-owned realized-class
  legality result and must not reinterpret superclass closure or override
  compatibility

## M256 executable object artifact lowering (C001)

`M256-C001` freezes the lane-C lowering boundary that binds realized-object
metadata records to executable method bodies already emitted by the native
frontend:

- contract id `objc3c-executable-object-artifact-lowering/m256-c001-v1`
- parser preserves raw implementation method bodies, selectors, and owner
  identities only
- sema preserves realization legality and canonical owner identities only
- IR/object lowering binds:
  - implementation-owned method entries to LLVM definition symbols by owner
    identity
  - class/metaclass descriptor bundles to owner-scoped method-list refs
  - category descriptor bundles to owner-scoped method-list refs
- fail-closed model
  `no-synthetic-implementation-symbols-no-rebound-legality-no-new-section-families`
- non-goals:
  - no new descriptor payload families
  - no bootstrap/runtime-registration rebinding
  - no protocol executable-realization path

## M256 executable method-body binding (C002)

`M256-C002` turns the frozen `M256-C001` object-artifact boundary into a live
fail-closed executable binding capability:

- contract id `objc3c-executable-method-body-binding/m256-c002-v1`
- lowering now requires:
  - every implementation-owned executable method entry binds to exactly one
    concrete LLVM definition symbol
  - missing bindings fail closed during IR/object emission
  - duplicate bindings for the same canonical method owner identity fail closed
- emitted IR publishes:
  - `; executable_method_body_binding = ...`
- runtime consumption model:
  - emitted method-entry implementation pointers dispatch through
    `objc3_runtime_dispatch_i32`
  - no runtime-side body rediscovery from source or manifests
- proof surface:
  - `tests/tooling/fixtures/native/m256_c002_method_body_binding.objc3`
  - `tests/tooling/runtime/m256_c002_method_binding_probe.cpp`
  - `tmp/reports/m256/M256-C002/method_body_binding_summary.json`

## M256 executable realization records (C003)

`M256-C003` expands the executable lowering surface so emitted realization
records preserve the owner and graph edges that the runtime tranche will
consume directly:

- contract id `objc3c-executable-realization-records/m256-c003-v1`
- class record model
  `class-and-metaclass-records-carry-bundle-object-and-super-owner-identities-plus-method-list-refs`
- protocol record model
  `protocol-records-carry-owner-inherited-protocol-edges-and-split-instance-class-method-counts`
- category record model
  `category-records-carry-explicit-class-and-category-owner-identities-plus-attachment-and-adopted-protocol-edges`
- fail-closed model
  `no-identity-edge-elision-no-out-of-band-graph-reconstruction`
- emitted IR publishes:
  - `; executable_realization_records = ...`
- proof surface:
  - `tests/tooling/fixtures/native/m256_inheritance_override_realization_positive.objc3`
  - `tests/tooling/fixtures/native/m256_protocol_conformance_positive.objc3`
  - `tests/tooling/fixtures/native/m256_category_merge_positive.objc3`
  - `tmp/reports/m256/M256-C003/realization_records_summary.json`

## M256 class realization runtime freeze (D001)

`M256-D001` freezes the runtime-owned class realization boundary that consumes
the already-emitted `M256-C003` realization records.

- contract id `objc3c-runtime-class-realization-freeze/m256-d001-v1`
- class realization model
  `registered-class-bundles-realize-one-deterministic-class-metaclass-chain-per-class-name`
- metaclass graph model
  `known-class-and-class-self-receivers-normalize-onto-the-metaclass-record-chain`
- category attachment model
  `preferred-category-implementation-records-attach-after-class-bundle-resolution`
- protocol check model
  `adopted-and-inherited-protocol-method-lists-provide-declaration-aware-negative-runtime-checks`
- fail-closed model
  `invalid-bundle-graphs-category-conflicts-and-ambiguous-runtime-resolution-fail-closed`
- non-goals:
  - property/ivar storage realization
  - accessor synthesis
  - executable protocol-body dispatch
  - cross-image class coalescing beyond the current ordered image walk
- architecture/spec/checker anchors for this issue are:
  - `docs/contracts/m256_class_realization_runtime_contract_and_architecture_freeze_d001_expectations.md`
  - `spec/planning/compiler/m256/m256_d001_class_realization_runtime_contract_and_architecture_freeze_packet.md`
  - `check:objc3c:m256-d001-class-realization-runtime-contract`
  - `check:objc3c:m256-d001-lane-d-readiness`

## M256 metaclass graph and root-class baseline (D002)

`M256-D002` turns that frozen runtime boundary into a runtime-owned realized
class graph with explicit root-class publication and stable receiver-base
identity bindings.

- contract id `objc3c-runtime-metaclass-graph-root-class-baseline/m256-d002-v1`
- realized class graph model
  `runtime-owned-realized-class-nodes-bind-receiver-base-identities-to-class-and-metaclass-records`
- root-class baseline model
  `root-classes-realize-with-null-superclass-links-and-live-instance-plus-class-dispatch`
- fail-closed model
  `missing-receiver-bindings-or-broken-realized-superclass-links-fall-closed-to-compatibility-dispatch`
- non-goals:
  - object allocation
  - instance storage / ivar layout
  - executable protocol-body dispatch
  - category attachment runtime checks beyond the already frozen D001 surface
- architecture/spec/checker anchors for this issue are:
  - `docs/contracts/m256_metaclass_graph_and_root_class_baseline_core_feature_implementation_d002_expectations.md`
  - `spec/planning/compiler/m256/m256_d002_metaclass_graph_and_root_class_baseline_core_feature_implementation_packet.md`
  - `check:objc3c:m256-d002-metaclass-graph-root-class-baseline`
  - `check:objc3c:m256-d002-lane-d-readiness`

## M256 category attachment and protocol conformance runtime checks (D003)

`M256-D003` consumes the D002 realized class graph directly and proves live
category attachment plus runtime protocol-conformance queries without falling
back to manifest-only summaries.

- contract id `objc3c-runtime-category-attachment-protocol-conformance/m256-d003-v1`
- category attachment model
  `realized-class-nodes-own-preferred-category-attachments-after-registration`
- protocol conformance query model
  `runtime-protocol-conformance-queries-walk-class-category-and-inherited-protocol-closures`
- fail-closed model
  `invalid-attachment-owner-identities-or-broken-protocol-refs-disable-runtime-attachment-queries`
- non-goals:
  - object allocation
  - property / ivar storage realization
  - executable protocol-body dispatch
  - cross-image attachment coalescing beyond the current registration order
- architecture/spec/checker anchors for this issue are:
  - `docs/contracts/m256_category_attachment_and_protocol_conformance_runtime_checks_core_feature_implementation_d003_expectations.md`
  - `spec/planning/compiler/m256/m256_d003_category_attachment_and_protocol_conformance_runtime_checks_core_feature_implementation_packet.md`
  - `check:objc3c:m256-d003-category-attachment-protocol-conformance-runtime-checks`
  - `check:objc3c:m256-d003-lane-d-readiness`

## M256 canonical runnable class and object sample support (D004)

`M256-D004` turns the realized graph into a truthful executable object-sample
surface by admitting runtime-owned builtin `alloc`/`new`/`init` while keeping
metadata-rich object-model cases on a library-plus-probe proof path until the
runtime export gate is wider.

- contract id `objc3c-runtime-canonical-runnable-object-sample-support/m256-d004-v1`
- execution model
  `canonical-object-samples-use-runtime-owned-alloc-new-init-and-realized-class-dispatch`
- probe split model
  `metadata-rich-object-samples-prove-category-and-protocol-runtime-behavior-through-library-plus-probe-splits`
- fail-closed model
  `metadata-heavy-executable-samples-stay-library-probed-until-runtime-export-gates-open`
- non-goals:
  - property / ivar storage realization
  - metadata-heavy all-in-one executable samples that still trip the runtime export gate
  - cross-image object allocation or coalescing semantics beyond the current single-image runtime path
- architecture/spec/checker anchors for this issue are:
  - `docs/contracts/m256_canonical_runnable_class_and_object_sample_support_core_feature_expansion_d004_expectations.md`
  - `spec/planning/compiler/m256/m256_d004_canonical_runnable_class_and_object_sample_support_core_feature_expansion_packet.md`
  - `check:objc3c:m256-d004-canonical-runnable-class-and-object-sample-support`
  - `check:objc3c:m256-d004-lane-d-readiness`

## M256 class/protocol/category conformance gate (E001)

`M256-E001` freezes the first lane-E proof gate over the current executable
class/protocol/category surface without broadening the runtime matrix yet.

- contract id
  `objc3c-executable-class-protocol-category-conformance-gate/m256-e001-v1`
- evidence model
  `a003-b004-c003-d004-summary-chain`
- execution boundary model
  `runnable-class-protocol-category-evidence-consumes-source-sema-lowering-and-runtime-proofs`
- failure model
  `fail-closed-on-class-protocol-category-conformance-evidence-drift`
- canonical upstream evidence:
  - `tmp/reports/m256/M256-A003/protocol_category_source_surface_completion_for_executable_runtime_summary.json`
  - `tmp/reports/m256/M256-B004/inheritance_override_realization_legality_summary.json`
  - `tmp/reports/m256/M256-C003/realization_records_summary.json`
  - `tmp/reports/m256/M256-D004/canonical_runnable_object_sample_support_summary.json`
- the next implementation issue is `M256-E002`

## M256 runnable class/protocol/category execution matrix (E002)

`M256-E002` broadens the frozen `M256-E001` gate into the first live runnable
execution matrix for executable classes, protocols, and categories.

- contract id
  `objc3c-runnable-class-protocol-category-execution-matrix/m256-e002-v1`
- evidence model
  `a003-b004-c003-d004-e001-summary-chain-plus-live-inheritance-execution`
- execution matrix model
  `runnable-class-protocol-category-matrix-composes-upstream-summaries-with-live-inheritance-and-runtime-dispatch-proof`
- failure model
  `fail-closed-on-runnable-object-matrix-drift-or-missing-live-runtime-proof`
- canonical upstream evidence:
  - `tmp/reports/m256/M256-A003/protocol_category_source_surface_completion_for_executable_runtime_summary.json`
  - `tmp/reports/m256/M256-B004/inheritance_override_realization_legality_summary.json`
  - `tmp/reports/m256/M256-C003/realization_records_summary.json`
  - `tmp/reports/m256/M256-D004/canonical_runnable_object_sample_support_summary.json`
  - `tmp/reports/m256/M256-E001/class_protocol_category_conformance_gate_summary.json`
- live matrix case:
  - `tests/tooling/fixtures/native/m256_inheritance_override_realization_positive.objc3`
  - `artifacts/lib/objc3_runtime.lib`
  - expected exit code `4`
- the next implementation issue is `M257-A001`

## M257 property and ivar executable source closure (A001)

`M257-A001` freezes one fail-closed lowering/runtime boundary above the
existing property-synthesis/ivar-binding summary surface and below the later
layout/accessor realization work.

- contract id
  `objc3c-executable-property-ivar-source-closure/m257-a001-v1`
- source-surface model
  `property-ivar-executable-source-closure-freezes-decls-synthesis-bindings-and-accessor-selectors-before-storage-realization`
- evidence model
  `class-protocol-property-ivar-fixture-manifest-and-ir-replay-key`
- failure model
  `fail-closed-on-property-ivar-source-surface-drift-before-layout-and-accessor-expansion`
- canonical anchors:
  - `Objc3PropertyDecl`
  - `Objc3PropertyDecl.ivar_binding_symbol`
  - `Objc3InterfaceDecl.property_synthesis_symbols_lexicographic`
  - `Objc3InterfaceDecl.ivar_binding_symbols_lexicographic`
  - `Objc3ImplementationDecl.property_synthesis_symbols_lexicographic`
  - `Objc3ImplementationDecl.ivar_binding_symbols_lexicographic`
  - `frontend.pipeline.sema_pass_manager.lowering_property_synthesis_ivar_binding_replay_key`
- evidence path:
  - `tmp/reports/m257/M257-A001/property_ivar_executable_source_closure_summary.json`
- `M257-A002`
  broadens this freeze into ivar layout and property attribute/source-model
  completion

## M257 ivar layout and property attribute source-model completion (A002)

`M257-A002` broadens the frozen `M257-A001` source surface into one
deterministic source-model completion step:

- contract id
  `objc3c-executable-property-ivar-source-model-completion/m257-a002-v1`
- layout model
  `property-ivar-source-model-computes-deterministic-layout-slots-sizes-and-alignment-before-runtime-storage-realization`
- attribute model
  `property-attribute-and-effective-accessor-source-model-publishes-deterministic-ownership-and-selector-profiles`
- failure model
  `fail-closed-on-property-attribute-accessor-ownership-or-layout-drift-before-storage-realization`
- emitted IR summary
  - `; property_ivar_source_model_completion = ...`
- next handoff
  - `M257-B001`

## M257 property and ivar executable semantics (B001)

`M257-B001` freezes the runtime-meaningful semantic rules above the completed
`M257-A002` source model:

- contract id
  `objc3c-executable-property-ivar-semantics/m257-b001-v1`
- synthesis semantics model
  `non-category-class-interface-properties-own-deterministic-implicit-ivar-and-synthesized-binding-identities-until-explicit-synthesize-lands`
- accessor semantics model
  `readonly-and-attribute-driven-accessor-selectors-resolve-to-one-declaration-level-profile-before-body-emission`
- storage semantics model
  `interface-owned-property-layout-slots-sizes-and-alignment-remain-deterministic-before-runtime-allocation`
- compatibility semantics model
  `protocol-and-inheritance-compatibility-compare-declaration-level-attribute-accessor-ownership-profiles-not-storage-local-layout-symbols`
- failure model
  `fail-closed-on-property-runtime-semantic-boundary-drift-before-accessor-body-or-storage-realization`
- next handoff
  - `M257-B002`

## M257 property synthesis and default ivar binding full semantics (B002)

`M257-B002` promotes the frozen `M257-B001` synthesis boundary into a live
compiler rule:

- contract id
  `objc3c-property-default-ivar-binding-semantics/m257-b002-v1`
- default binding resolution model
  `matched-class-implementations-resolve-interface-declared-properties-through-authoritative-default-ivar-bindings-with-or-without-implementation-redeclaration`
- authoritative synthesis rule
  - matched class implementations synthesize from interface-declared properties first
  - implementation redeclarations remain optional compatibility overlays
  - category implementations remain outside default ivar synthesis
- lowering consequence
  - lowering consumes the sema-owned interface-driven property synthesis counts and bindings without re-deriving them from implementation-local redeclarations
- next handoff
  - `M257-B003`

## M257 accessor legality and ownership or atomicity attribute interactions (B003)

`M257-B003` promotes the `M257-B002` property surface into a stricter live
compiler rule:

- contract id
  `objc3c-property-accessor-attribute-interactions/m257-b003-v1`
- accessor-selector uniqueness model
  `effective-getter-and-setter-selectors-must-be-unique-within-each-property-container-before-runtime-accessor-binding`
- ownership or atomicity interaction model
  `runtime-managed-property-ownership-and-atomicity-combinations-fail-closed-until-executable-accessor-storage-semantics-land`
- lowering consequence
  - lane-B rejects duplicate effective accessor selectors before executable accessor binding
  - lowering consumes only sema-approved property accessor selectors and ownership profiles
  - atomic ownership-aware properties remain fail-closed until later runtime storage/accessor work lands
- next handoff
  - `M257-B004`

## M257 accessor and layout lowering (C001)

`M257-C001` freezes the current lane-C lowering boundary above the completed
`M257-A002` source model and the `M257-B001..B003` semantic closure:

- contract id
  `objc3c-executable-property-accessor-layout-lowering/m257-c001-v1`
- property-table model
  `property-descriptor-bundles-carry-sema-approved-attribute-accessor-binding-and-layout-records`
- ivar-layout model
  `ivar-descriptor-bundles-carry-sema-approved-layout-symbol-slot-size-alignment-records`
- accessor-binding model
  `effective-accessor-selectors-and-synthesized-binding-identities-pass-through-lowering-without-body-synthesis`
- scope model
  `ast-sema-property-layout-handoff-ir-object-metadata-publication`
- failure model
  `no-synthesized-accessor-bodies-no-runtime-storage-allocation-no-layout-rederivation`
- emitted IR summary
  - `; executable_property_accessor_layout_lowering = ...`
- non-goals
  - no synthesized accessor body emission
  - no runtime storage allocation
  - no instance layout realization
- next handoff
  - `M257-C002`

## M257 ivar offset and layout emission (C002)

`M257-C002` extends the frozen `M257-C001` lowering boundary into real emitted
ivar layout payloads. Lane-C now materializes byte offsets from the
sema-approved slot/size/alignment packet and publishes retained per-owner
layout tables in object artifacts without yet allocating runtime instances.

- contract id
  `objc3c-executable-ivar-layout-emission/m257-c002-v1`
- descriptor model
  `ivar-descriptor-records-carry-layout-symbol-offset-global-slot-offset-size-alignment`
- offset-global model
  `one-retained-i64-offset-global-per-emitted-ivar-binding`
- layout-table model
  `declaration-owner-layout-tables-order-ivars-by-slot-and-publish-instance-size`
- scope model
  `sema-approved-layout-shape-lowers-into-ivar-section-payloads-without-runtime-allocation`
- failure model
  `no-runtime-instance-allocation-no-layout-rederivation-no-accessor-body-synthesis`
- emitted IR summary
  - `; executable_ivar_layout_emission = ...`
- non-goals
  - no runtime instance allocation
  - no runtime layout re-derivation
  - no synthesized accessor body emission
- next handoff
  - `M257-C003`

## M263 registration descriptor and image-root source surface (A001)

`M263-A001` freezes the frontend-visible naming surface that closes the
remaining bootstrap source-model gap above the already-emitted `M254`
registration manifest.

- contract id
  `objc3c-bootstrap-registration-descriptor-image-root-source-surface/m263-a001-v1`
- canonical frontend prelude contract path
  `frontend.bootstrap_registration_source_pragma_contract`
- canonical semantic surface path
  `frontend.pipeline.semantic_surface.objc_runtime_registration_descriptor_image_root_source_surface`
- canonical file-scope prelude pragmas:
  - `objc_registration_descriptor`
  - `objc_image_root`
- canonical identity-source vocabulary:
  - `module-declaration-or-default`
  - `source-pragma`
  - `module-derived-default`
- canonical ownership model
  `image-root-owns-registration-descriptor-runtime-owns-bootstrap-state`

Non-goals:

- no bootstrap-table lowering yet
- no multi-image root emission yet
- no runtime replay/discovery execution yet

`M263-A002` must preserve this contract while broadening frontend closure and
registration-manifest consumption.

## M263 registration manifest and descriptor frontend closure (A002)

`M263-A002` turns the frozen `M263-A001` source packet into one emitted
frontend-owned descriptor artifact that is derived from the already-emitted
`M254` registration manifest.

- contract id
  `objc3c-runtime-registration-descriptor-frontend-closure/m263-a002-v1`
- canonical semantic surface path
  `frontend.pipeline.semantic_surface.objc_runtime_registration_descriptor_frontend_closure`
- emitted artifact
  `module.runtime-registration-descriptor.json`
- payload model
  `runtime-registration-descriptor-json-v1`
- authority model
  `registration-descriptor-artifact-derived-from-source-surface-and-registration-manifest`
- payload ownership model
  `compiler-emits-registration-descriptor-artifact-runtime-consumes-bootstrap-identity`

Non-goals:

- no bootstrap-table lowering yet
- no multi-image replay behavior yet
- no runtime bootstrap execution yet

## M263 bootstrap legality, duplicate policy, and failure contract (B001)

`M263-B001` freezes the semantic legality packet that sits above the emitted
`M263-A002` descriptor closure and above the live `M254-B002` bootstrap
semantics surface.

- contract id
  `objc3c-runtime-bootstrap-legality-duplicate-order-failure-contract/m263-b001-v1`
- canonical semantic surface path
  `frontend.pipeline.semantic_surface.objc_runtime_bootstrap_legality_failure_contract`
- upstream contract ids:
  - `objc3c-runtime-registration-descriptor-frontend-closure/m263-a002-v1`
  - `objc3c-runtime-startup-bootstrap-semantics/m254-b002-v1`
- frozen legality policies:
  - duplicate registration
    `fail-closed-by-translation-unit-identity-key`
  - image order invariant
    `strictly-monotonic-positive-registration-order-ordinal`
  - failure mode
    `abort-before-user-main-no-partial-registration-commit`
  - restart lifecycle
    `reset-clears-live-runtime-state-and-zeroes-image-local-init-cells`
  - replay order
    `replay-re-registers-retained-images-in-original-registration-order`
  - image-local init reset
    `retained-bootstrap-image-local-init-cells-reset-to-zero-before-replay`
  - catalog retention
    `bootstrap-catalog-retained-across-reset-for-deterministic-replay`
- identifier and ordinal continuity flows through from `M263-A002`; later
  runtime work must not rebuild bootstrap identity/order from scratch.

Non-goals:

- no multi-image bootstrap execution yet
- no runtime replay implementation yet

## M263 duplicate-registration and image-order semantics (B002)

`M263-B002` turns the `M263-B001` freeze into a live semantic bridge that
publishes duplicate-registration and cross-image legality using the emitted
translation-unit identity key and registration-order ordinal.

- contract id
  `objc3c-runtime-bootstrap-legality-duplicate-order-semantics/m263-b002-v1`
- canonical semantic surface path
  `frontend.pipeline.semantic_surface.objc_runtime_bootstrap_legality_semantics`
- upstream contract ids:
  - `objc3c-runtime-bootstrap-legality-duplicate-order-failure-contract/m263-b001-v1`
  - `objc3c-runtime-registration-descriptor-frontend-closure/m263-a002-v1`
  - `objc3c-runtime-startup-bootstrap-semantics/m254-b002-v1`
- canonical models:
  - duplicate registration
    `fail-closed-by-translation-unit-identity-key`
  - image order invariant
    `strictly-monotonic-positive-registration-order-ordinal`
  - cross-image legality
    `translation-unit-identity-key-and-registration-order-ordinal-govern-bootstrap-legality`
  - semantic diagnostics
    `fail-closed-bootstrap-legality-before-runtime-handoff`
- deterministic proof minimums:
  - recompiling the same translation unit preserves the same
    `translation_unit_identity_key`
  - peer translation units with identical visible bootstrap identifiers receive
    different `translation_unit_identity_key` values
- no API widening beyond the already-frozen bootstrap/runtime contracts

## M263 bootstrap failure-mode and restart semantics (B003)

`M263-B003` closes the residual restart/recovery semantics by publishing a live
bridge over the frozen bootstrap legality packet, the live `M254-B002`
bootstrap semantics, and the `M254-D003` deterministic reset/replay contract.

- contract id
  `objc3c-runtime-bootstrap-failure-restart-semantics/m263-b003-v1`
- canonical semantic surface path
  `frontend.pipeline.semantic_surface.objc_runtime_bootstrap_failure_restart_semantics`
- upstream/runtime contract ids:
  - `objc3c-runtime-bootstrap-legality-duplicate-order-semantics/m263-b002-v1`
  - `objc3c-runtime-startup-bootstrap-semantics/m254-b002-v1`
  - `objc3c-runtime-bootstrap-reset-replay/m254-d003-v1`
- canonical models:
  - failure mode `abort-before-user-main-no-partial-registration-commit`
  - restart lifecycle `reset-clears-live-runtime-state-and-zeroes-image-local-init-cells`
  - replay order `replay-re-registers-retained-images-in-original-registration-order`
  - unsupported topology
    `replay-requires-empty-live-runtime-state-and-retained-bootstrap-catalog`
- deterministic proof minimums:
  - replay while live state is still populated fails closed with the invalid
    descriptor status path
  - reset preserves retained catalog state and zeroes the live init-state cells
  - reset plus replay restores the retained image in canonical order

## M263 constructor-root and init-array lowering contract (C001)

`M263-C001` freezes the live lowering boundary above the emitted
`M263-A002` registration-descriptor artifact and the emitted translation-unit
registration manifest.

- contract id
  `objc3c-runtime-constructor-root-init-array-lowering/m263-c001-v1`
- canonical semantic surface path
  `frontend.pipeline.semantic_surface.objc_runtime_bootstrap_lowering_contract`
- descriptor handoff contract id
  `objc3c-runtime-registration-descriptor-frontend-closure/m263-a002-v1`
- descriptor artifact
  `module.runtime-registration-descriptor.json`
- lowering boundary model
  `registration-descriptor-and-registration-manifest-drive-constructor-root-init-stub-registration-table-and-platform-init-array-lowering`
- emitted lowering guarantees:
  - constructor root `__objc3_runtime_register_image_ctor`
  - init-stub prefix `__objc3_runtime_register_image_init_stub_`
  - registration-table prefix `__objc3_runtime_registration_table_`
  - image-local-init prefix `__objc3_runtime_image_local_init_state_`
  - platform startup participation
    `llvm.global_ctors-single-root-priority-65535`
  - init stub still stages the registration table before calling
    `objc3_runtime_register_image`
- non-goals:
  - no multi-image root fanout yet
  - no replay partitioning/late linker synthesis yet

## M263 registration-descriptor lowering and multi-image root emission (C002)

`M263-C002` materializes the `M263-A002` descriptor/image-root identities as
real lowering artifacts in the native IR/object path while preserving the
frozen `M263-C001` constructor-root/registration-table ABI.

- contract id
  `objc3c-runtime-registration-descriptor-and-image-root-lowering/m263-c002-v1`
- lowering model
  `frontend-identifiers-drive-emitted-registration-descriptor-and-image-root-globals`
- canonical logical sections:
  - `objc3.runtime.registration_descriptor`
  - `objc3.runtime.image_root`
- canonical symbol prefixes:
  - `__objc3_runtime_registration_descriptor_`
  - `__objc3_runtime_image_root_`
- payload models:
  - `registration-descriptor-record-points-at-image-root-image-descriptor-registration-table-linker-anchor-and-init-state`
  - `image-root-record-points-at-module-name-image-descriptor-registration-table-and-discovery-root`
- emitted proof minimums:
  - one `runtime_registration_descriptor_image_root_lowering` line in `module.ll`
  - one retained image-root global in `objc3.runtime.image_root`
  - one retained registration-descriptor global in `objc3.runtime.registration_descriptor`

## M263 archive/static-link bootstrap replay corpus (C003)

`M263-C003` turns the earlier retained archive/static-link discovery work into
one live bootstrap replay corpus over linked binaries.

- contract id
  `objc3c-runtime-bootstrap-archive-static-link-replay-corpus/m263-c003-v1`
- corpus model
  `merged-archive-static-link-discovery-artifacts-drive-live-bootstrap-replay-probes`
- binary proof model
  `plain-link-omits-bootstrap-images-retained-link-replays-them`
- upstream contracts:
  - `objc3c-runtime-metadata-archive-and-static-link-discovery/m253-d003-v1`
  - `objc3c-runtime-bootstrap-failure-restart-semantics/m263-b003-v1`
  - `objc3c-runtime-registration-descriptor-and-image-root-lowering/m263-c002-v1`
- emitted proof minimums:
  - one `runtime_bootstrap_archive_static_link_replay_corpus` line in `module.ll`
  - merged response/discovery artifacts retain archive-linked bootstrap objects
  - replay probes prove retained images register on startup and re-register after reset

## M263 runtime bootstrap table consumption freeze (D001)

`M263-D001` freezes the runtime-side bridge that consumes emitted registration
tables, rejects duplicate image identities before advancing bootstrap state,
and publishes one authoritative image-walk snapshot for runtime probes.

- contract id
  `objc3c-runtime-bootstrap-table-consumption-freeze/m263-d001-v1`
- upstream runtime/lowering contracts:
  - `objc3c-runtime-bootstrap-registrar-image-walk/m254-d002-v1`
  - `objc3c-runtime-registration-descriptor-and-image-root-lowering/m263-c002-v1`
- canonical models:
  - table consumption
    `next-public-register-call-consumes-staged-registration-table-once`
  - deduplication
    `translation-unit-identity-key-rejection-before-registration-state-advance`
  - image state publication
    `image-walk-snapshot-publishes-module-identity-root-counts-and-staged-table-usage`
- canonical runtime bridge:
  - `objc3_runtime_stage_registration_table_for_bootstrap`
  - `objc3_runtime_register_image`
  - `objc3_runtime_copy_image_walk_state_for_testing`
- frozen invariants:
  - staged tables must descriptor-match the runtime image before state is published
  - discovery root must close over every descriptor family before the image-walk
    snapshot is committed
  - duplicate registration must fail closed without incrementing registered-image
    counters or next-expected ordinal

## M263 live registration, discovery, and replay implementation (D002)

`M263-D002` freezes the already-live runtime tracking surface that sits above
`M263-D001`: successful startup registration, discovery-root accounting, reset,
and deterministic replay over retained bootstrap images.

- contract id
  `objc3c-runtime-live-registration-discovery-replay/m263-d002-v1`
- upstream runtime/lowering contracts:
  - `objc3c-runtime-bootstrap-table-consumption-freeze/m263-d001-v1`
  - `objc3c-runtime-bootstrap-reset-replay/m254-d003-v1`
- canonical models:
  - live registration
    `emitted-metadata-images-register-through-native-runtime-and-retained-bootstrap-catalog`
  - live discovery tracking
    `image-walk-snapshot-tracks-last-discovered-root-and-descriptor-families`
  - live replay tracking
    `reset-replay-state-snapshot-tracks-retained-images-reset-clears-and-last-replayed-identity`
- canonical runtime bridge:
  - `objc3_runtime_copy_image_walk_state_for_testing`
  - `objc3_runtime_copy_reset_replay_state_for_testing`
  - `objc3_runtime_replay_registered_images_for_testing`
- frozen invariants:
  - startup registration must retain exactly one bootstrap record per emitted image
  - reset must clear live counters while preserving the retained bootstrap catalog
  - replay must republish the same image-walk/discovery evidence through the
    staged-table path and record last-replayed identity/generation

## M263 live restart hardening (D003)

`M263-D003` freezes the runtime-owned idempotence/teardown/restart hardening
layer above `M263-D002`.

- contract id
  `objc3c-runtime-live-restart-hardening/m263-d003-v1`
- upstream runtime contracts:
  - `objc3c-runtime-live-registration-discovery-replay/m263-d002-v1`
  - `objc3c-runtime-bootstrap-failure-restart-semantics/m263-b003-v1`
  - `objc3c-runtime-bootstrap-reset-replay/m254-d003-v1`
- canonical models:
  - live idempotence
    `second-live-replay-without-reset-fails-closed-and-preserves-live-runtime-state`
  - live teardown
    `reset-clears-live-state-zeroes-image-local-init-cells-and-retains-bootstrap-catalog`
  - live restart evidence
    `repeated-reset-replay-cycles-publish-monotonic-reset-and-replay-generations`
- canonical runtime bridge:
  - `objc3_runtime_reset_for_testing`
  - `objc3_runtime_replay_registered_images_for_testing`
  - `objc3_runtime_copy_reset_replay_state_for_testing`
- frozen invariants:
  - replay without teardown must fail closed and preserve current live runtime state
  - repeated reset/replay cycles must keep the retained bootstrap catalog intact
  - reset generation and replay generation must advance monotonically across repeated restart cycles

## M263 bootstrap completion conformance gate (E001)

`M263-E001` freezes the integrated lane-E gate that decides whether the current
bootstrap tranche is actually complete for runnable native programs.

- contract id
  `objc3c-runtime-bootstrap-completion-gate/m263-e001-v1`
- upstream proof chain:
  - `objc3c-runtime-registration-descriptor-frontend-closure/m263-a002-v1`
  - `objc3c-runtime-bootstrap-failure-restart-semantics/m263-b003-v1`
  - `objc3c-runtime-bootstrap-archive-static-link-replay-corpus/m263-c003-v1`
  - `objc3c-runtime-live-restart-hardening/m263-d003-v1`
- canonical models:
  - evidence chain
    `a002-b003-c003-d003-summary-chain`
  - failure model
    `fail-closed-on-bootstrap-completion-evidence-drift`
- frozen invariants:
  - emitted registration-descriptor authority remains canonical for bootstrap-visible identities
  - single-image restart semantics stay deterministic and replay-safe
  - plain archive links omit bootstrap images while retained single/merged links replay them deterministically
  - repeated live reset/replay cycles remain idempotence-safe across restart generations

## M263 runnable multi-image bootstrap matrix closeout (E002)

`M263-E002` closes the milestone by binding the `M263-E001` gate to one
published operator runbook and one stable bootstrap matrix summary.

- contract id
  `objc3c-runtime-runnable-bootstrap-matrix-closeout/m263-e002-v1`
- closeout model
  `e001-gate-plus-published-bootstrap-matrix-runbook`
- required matrix cases:
  - `single-image-default`
  - `single-image-explicit`
  - `archive-backed-plain`
  - `archive-backed-single-retained`
  - `archive-backed-merged-retained`
- canonical operator script:
  - `scripts/check_objc3c_bootstrap_matrix.ps1`

## M257 synthesized accessor and property metadata lowering (C003)

`M257-C003` turns implementation-owned effective property accessors into real emitted method bodies while keeping true runtime instance allocation deferred to lane D.

- contract id
  `objc3c-executable-synthesized-accessor-property-lowering/m257-c003-v1`
- canonical source model
  `implementation-owned-properties-synthesize-missing-effective-instance-accessors-into-emitted-method-lists`
- storage model
  `one-private-i32-storage-global-per-synthesized-binding-symbol-pending-runtime-instance-layout`
- property descriptor model
  `property-descriptors-carry-effective-accessor-selectors-binding-symbols-layout-symbols-and-accessor-implementation-pointers`
- fail-closed model
  `no-missing-effective-accessor-bindings-no-duplicate-synthesized-owner-identities-no-runtime-layout-rederivation`

## M257 runtime property and layout consumption freeze (D001)

`M257-D001` freezes the truthful runtime boundary above `M257-C003`: runtime now consumes emitted accessor implementation pointers plus property/layout attachment identities, but alloc/new still return one canonical realized instance identity per class and synthesized accessor execution still uses the lane-C storage globals.

- contract id
  `objc3c-runtime-property-layout-consumption-freeze/m257-d001-v1`
- descriptor model
  `runtime-consumes-emitted-property-descriptor-accessor-pointers-binding-symbols-and-layout-identities-without-source-rediscovery`
- allocator model
  `alloc-new-return-one-canonical-realized-instance-identity-per-class-before-true-instance-slot-allocation`
- storage model
  `synthesized-accessor-execution-uses-lane-c-storage-globals-pending-runtime-instance-slots`
- fail-closed model
  `no-layout-rederivation-no-reflective-property-registration-no-per-instance-allocation-yet`

## M257 instance allocation, layout, and ivar-offset runtime support (D002)

`M257-D002` upgrades the runtime above `M257-D001` into true instance
allocation and per-instance synthesized accessor execution.

- contract id
  `objc3c-runtime-instance-allocation-layout-support/m257-d002-v1`
- descriptor model
  `runtime-consumes-emitted-property-descriptor-accessor-pointers-binding-symbols-and-layout-identities-without-source-rediscovery`
- allocator model
  `alloc-new-materialize-distinct-runtime-instance-identities-backed-by-realized-class-layout`
- storage model
  `synthesized-accessor-execution-reads-and-writes-per-instance-slot-storage-using-emitted-ivar-offset-layout-records`
- fail-closed model
  `no-layout-rederivation-no-shared-global-property-storage-no-reflective-property-registration-yet`

## M257 property metadata registration and reflective access helpers (D003)

`M257-D003` adds the private runtime reflection helper surface above `M257-D002`.

- contract id
  `objc3c-runtime-property-metadata-reflection/m257-d003-v1`
- registration model
  `runtime-registers-reflectable-property-accessor-and-layout-facts-from-emitted-metadata-without-source-rediscovery`
- query model
  `private-testing-helpers-query-realized-property-metadata-by-class-and-property-name-including-effective-accessors-and-layout-facts`
- fail-closed model
  `no-public-reflection-abi-no-reflective-source-recovery-no-property-query-success-without-realized-runtime-layout`

## M257 property/ivar execution gate (E001)

`M257-E001` freezes the first lane-E proof gate over the current executable
property/ivar surface without broadening the runnable sample matrix yet.

- contract id
  `objc3c-executable-property-ivar-execution-gate/m257-e001-v1`
- evidence model
  `a002-b003-c003-d003-summary-chain`
- execution gate model
  `runnable-property-ivar-evidence-consumes-source-sema-lowering-and-runtime-proofs`
- failure model
  `fail-closed-on-property-ivar-execution-evidence-drift`
- canonical upstream evidence:
  - `tmp/reports/m257/M257-A002/property_ivar_source_model_completion_summary.json`
  - `tmp/reports/m257/M257-B003/accessor_legality_attribute_interactions_summary.json`
  - `tmp/reports/m257/M257-C003/synthesized_accessor_property_lowering_summary.json`
  - `tmp/reports/m257/M257-D003/property_metadata_reflection_summary.json`
- the next implementation issue is `M257-E002`

## M257 runnable property/ivar/accessor execution matrix (E002)

`M257-E002` broadens the frozen `M257-E001` gate into one live runnable matrix
for executable properties and ivars.

- contract id
  `objc3c-runnable-property-ivar-accessor-execution-matrix/m257-e002-v1`
- evidence model
  `a002-b003-c003-d003-e001-summary-chain-plus-live-property-runtime-execution`
- execution matrix model
  `runnable-property-ivar-matrix-composes-upstream-summaries-with-live-storage-accessor-and-reflection-proof`
- failure model
  `fail-closed-on-runnable-property-ivar-matrix-drift-or-missing-live-runtime-proof`
- canonical live proof assets:
  - `tests/tooling/fixtures/native/m257_property_ivar_execution_matrix_positive.objc3`
  - `tests/tooling/runtime/m257_e002_property_ivar_execution_matrix_probe.cpp`
- the next implementation issue is `M258-A001`

## M258 runtime-aware import/module surface freeze (A001)

`M258-A001` freezes one frontend-published import/module boundary above the
existing local module-import graph lowering surface and below any real imported
runtime-owned declaration or foreign metadata-reference realization.

- contract id `objc3c-runtime-aware-import-module-surface/m258-a001-v1`
- semantic-surface path
  `frontend.pipeline.semantic_surface.objc_runtime_aware_import_module_surface_contract`
- source model
  `runtime-aware-import-module-surface-freezes-frontend-owned-runtime-declaration-and-metadata-reference-boundaries-before-cross-translation-unit-realization`
- non-goal model
  `no-imported-module-artifact-reader-no-imported-runtime-declaration-materialization-no-imported-runtime-metadata-reference-lowering`
- failure model
  `fail-closed-on-runtime-aware-import-module-surface-drift-or-premature-capability-claims`
- the frozen surface must publish:
  - module identity
  - protocol/interface/implementation/category/function declaration counts
  - current module-import-graph counts
  - landed=`false` flags for imported module artifacts, imported runtime-owned
    declarations, imported runtime metadata references, and public frontend API
    module handles
- emitted IR remains limited to the current translation unit; no imported
  runtime-owned declarations or foreign metadata references are lowered yet
- the public frontend C ABI remains fail closed for runtime-aware module import
  inputs until `M258-A002`
- `M258-A002` must preserve this exact surface while turning it into a real
  compiler/runtime capability

## M258 runtime-aware import/module frontend closure (A002)

`M258-A002` extends the frozen `M258-A001` surface into one emitted frontend
artifact while keeping IR lowering fail closed for foreign metadata.

- contract id `objc3c-runtime-aware-import-module-frontend-closure/m258-a002-v1`
- semantic-surface path
  `frontend.pipeline.semantic_surface.objc_runtime_aware_import_module_frontend_closure`
- emitted artifact
  `module.runtime-import-surface.json`
- payload model
  `runtime-aware-import-module-surface-json-v1`
- authority model
  `runtime-import-surface-artifact-derived-from-frozen-import-surface-and-runtime-metadata-source-records`
- payload ownership model
  `compiler-emits-runtime-import-surface-artifact-frontend-and-later-module-consumers-own-cross-translation-unit-handoff`
- the emitted artifact must publish:
  - the preserved `M258-A001` declaration/import-graph counts
  - canonical runtime-owned declaration inventories
  - canonical metadata-reference inventories for superclass/protocol/property
    accessor/property ivar-binding/method selector edges
  - landed=`true` flags for the frontend import/module surface and emitted
    artifact handoff
- the public frontend embedding ABI may advertise only the emitted filesystem
  artifact path contract for now; direct in-memory imported-module handles or
  foreign payload injection remain later work
- emitted IR still remains translation-unit local until later M258 lowering and
  runtime milestones consume this artifact

## M258 cross-module semantic preservation freeze (B001)

`M258-B001` freezes the semantic preservation boundary that later imported
runtime metadata handling must preserve.

- contract id
  `objc3c-cross-module-runtime-metadata-semantic-preservation/m258-b001-v1`
- semantic-surface path
  `frontend.pipeline.semantic_surface.objc_cross_module_runtime_metadata_semantic_preservation_contract`
- source frontend closure
  `objc3c-runtime-aware-import-module-frontend-closure/m258-a002-v1`
- source artifact
  `module.runtime-import-surface.json`
- authority model
  `semantic-preservation-freeze-derived-from-runtime-import-surface-and-runtime-metadata-source-records`
- conformance-shape model
  `superclass-protocol-and-category-attachment-shape`
- dispatch-trait model
  `selector-classness-accessor-ivar-binding-and-body-availability`
- effect-trait model
  `property-attribute-and-ownership-effect-profiles`
- the published freeze must account for:
  - superclass edges
  - protocol conformance / inheritance edges
  - category attachment shape
  - property accessor / ivar-binding traits
  - method selector / classness / body-availability traits
  - property attribute and ownership-effect profiles
- imported runtime metadata semantics remain fail closed:
  - imported conformance shape is not landed
  - imported dispatch traits are not landed
  - imported effect traits are not landed
  - imported runtime metadata semantics are not landed
  - ready-for-imported-metadata-semantic-rules remains `false`
  - ready-for-cross-module-dispatch-equivalence remains `false`
- imported runtime metadata semantics are not lowered into IR yet
- public embedding remains filesystem-artifact only; no in-memory imported
  module semantic handles or foreign payload injection ABI are exposed yet

## M258 imported metadata conformance, effect, and dispatch preservation rules (B002)

`M258-B002` converts the frozen `M258-B001` preservation boundary into a real
frontend capability that consumes emitted runtime import-surface artifacts.

- contract id
  `objc3c-imported-runtime-metadata-semantic-rules/m258-b002-v1`
- semantic-surface path
  `frontend.pipeline.semantic_surface.objc_imported_runtime_metadata_semantic_rules`
- source semantic-preservation contract
  `objc3c-cross-module-runtime-metadata-semantic-preservation/m258-b001-v1`
- input model
  `filesystem-runtime-import-surface-artifact-path-list`
- compiler behavior
  - repeated `--objc3-import-runtime-surface <path>` inputs are loaded before IR
    emission
  - duplicate input paths fail closed
  - duplicate imported module names fail closed
  - malformed or contract-invalid import-surface payloads fail closed
- landed semantic preservation
  - imported conformance shape is counted from the consumed artifacts
  - imported dispatch traits are counted from the consumed artifacts
  - imported effect traits are counted from the consumed artifacts
  - ready-for-imported-metadata-semantic-rules is `true` when the imported
    surfaces load successfully and the source `M258-B001` contract is ready
- imported runtime metadata payloads still are not lowered into IR in this lane

## M258 serialized metadata import and lowering freeze (C001)

`M258-C001` freezes the lane-C boundary directly above real imported-payload
lowering.

- contract id
  `objc3c-serialized-runtime-metadata-import-lowering/m258-c001-v1`
- semantic-surface path
  `frontend.pipeline.semantic_surface.objc_serialized_runtime_metadata_import_lowering_contract`
- source semantic-rule contract
  `objc3c-imported-runtime-metadata-semantic-rules/m258-b002-v1`
- input model
  `filesystem-runtime-import-surface-artifact-path-list`
- compiler behavior
  - the frontend publishes one deterministic fail-closed summary whenever
    imported runtime surface artifacts are consumed
  - the summary truthfully states that imported-surface ingest is landed
  - the summary truthfully states that serialized imported metadata
    rehydration, incremental reuse, and imported-payload IR lowering are not
    landed yet
- fail-closed non-goals
  - serialized imported metadata payloads are not rehydrated in this lane
  - incremental imported metadata reuse is not landed in this lane
  - imported metadata payloads are not lowered into IR in this lane
  - the public embedding ABI still does not expose serialized imported payload
    handles or incremental lowering hooks

## M258 module metadata serialization, deserialization, and artifact reuse (C002)

`M258-C002` lands the real frontend reuse path above direct imported-payload IR
lowering.

- contract id
  `objc3c-serialized-runtime-metadata-artifact-reuse/m258-c002-v1`
- semantic-surface path
  `frontend.pipeline.semantic_surface.objc_serialized_runtime_metadata_artifact_reuse`
- source contract
  `objc3c-serialized-runtime-metadata-import-lowering/m258-c001-v1`
- artifact member
  `serialized_runtime_metadata_reuse_payload`
- compiler behavior
  - emitted `module.runtime-import-surface.json` artifacts now carry a nested
    serialized runtime-metadata reuse payload
  - downstream imports prefer that payload when present and deserialize it back
    into runtime metadata source records
  - reused module names and transitive metadata counts are published
    deterministically
- current boundary
  - payload reuse is a frontend capability in this lane
  - runtime registration and cross-module realization remain lane-D work
  - imported payloads still are not lowered directly into LLVM IR in this lane

## M258 cross-module build and runtime orchestration freeze (D001)

`M258-D001` freezes the packaging/orchestration contract between the transitive
serialized import payload and the emitted local runtime registration manifest.

- contract id
  `objc3c-cross-module-build-runtime-orchestration/m258-d001-v1`
- semantic-surface path
  `frontend.pipeline.semantic_surface.objc_cross_module_build_runtime_orchestration_contract`
- source contracts
  - `objc3c-serialized-runtime-metadata-artifact-reuse/m258-c002-v1`
  - `objc3c-translation-unit-registration-manifest/m254-a002-v1`
- authoritative artifacts
  - `module.runtime-import-surface.json`
  - `module.runtime-registration-manifest.json`
- compiler behavior
  - the semantic surface now binds the direct imported runtime-surface inputs,
    the transitive reused module set, and the local registration-manifest
    descriptor inventory into one deterministic freeze packet
  - the boundary stays fail closed if either source contract disappears or
    drifts
- current boundary
  - cross-module link-plan artifacts are not landed
  - imported registration-manifest loading is not landed
  - runtime-archive aggregation is not landed
  - cross-module runtime-registration launch orchestration is not landed
  - no public cross-module orchestration ABI is exposed in this lane

## M258 cross-module runtime packaging, linking, and registration (D002)

`M258-D002` lands the first real lane-D packaging/runtime path that consumes
the frozen D001 orchestration inputs and emits deterministic cross-module link
artifacts.

- contract id
  `objc3c-cross-module-runtime-packaging-link-plan/m258-d002-v1`
- authoritative artifacts
  - `module.cross-module-runtime-link-plan.json`
  - `module.cross-module-runtime-linker-options.rsp`
- source contracts
  - `objc3c-cross-module-build-runtime-orchestration/m258-d001-v1`
  - `objc3c-runtime-aware-import-module-frontend-closure/m258-a002-v1`
  - `objc3c-translation-unit-registration-manifest/m254-a002-v1`
- compiler/runtime behavior
  - imported peer registration artifacts are validated fail closed
  - the emitted link plan orders object inputs by
    `ascending-registration-ordinal-then-translation-unit-identity-key`
  - the merged linker response file preserves the deterministic imported/local
  linker-anchor order
- the happy path now proves two-image runtime registration and replay through
  the emitted artifacts

## M258 cross-module object-model gate (E001)

`M258-E001` freezes the first lane-E proof gate over the runnable cross-module
object-model surface.

- contract id
  `objc3c-cross-module-object-model-gate/m258-e001-v1`
- evidence model
  `a002-b002-c002-d002-summary-chain`
- failure model
  `fail-closed-on-cross-module-object-model-evidence-drift`
- gate boundary
  - `M258-E001` consumes the emitted evidence from A002, B002, C002, and D002
    without landing any new parser, sema, lowering, or runtime behavior
  - `M258-E002` is the first issue allowed to broaden this freeze into a
    larger runnable import/module execution matrix

## M258 runnable import/module execution matrix (E002)

`M258-E002` broadens the frozen `M258-E001` gate into one live runnable import
and module execution matrix.

- contract id
  `objc3c-runnable-import-module-execution-matrix/m258-e002-v1`
- evidence model
  `a002-b002-c002-d002-e001-summary-chain-plus-live-cross-module-runtime-execution`
- execution matrix model
  `runnable-import-module-matrix-composes-upstream-summaries-with-live-two-image-startup-dispatch-selector-cache-and-replay-proof`
- truthful boundary
  - the IR/lowering surface remains the same object-local surface landed by
    `M258-D002`
  - lane E now proves the integrated multi-image runtime path above that
    surface, including startup registration, selector lookup, method-cache
    resolution, protocol conformance, reset, and replay
  - the matrix closes out onto `M259-A001`

## M259 runnable sample surface (A001)

`M259-A001` freezes the truthful runnable sample surface before `M259-A002`
widens the canonical sample set.

- contract id
  `objc3c-runnable-sample-surface/m259-a001-v1`
- evidence model
  `execution-smoke-replay-script-surface-plus-m256-d004-m257-e002-m258-e002-summary-chain`
- sample surface model
  `canonical-runnable-sample-surface-composes-scalar-smoke-object-property-and-import-module-proofs`
- truthful boundary
  - lane A treats execution smoke and replay proof as the scalar/core sample
    corpus boundary
  - runnable object, property, and import/module samples remain frozen through
    the already-landed `M256-D004`, `M257-E002`, and `M258-E002` proof chains
  - the freeze does not add blocks, ARC, async, throws, or actor sample claims
  - the next implementation issue is `M259-A002`

## M259 canonical runnable sample set (A002)

`M259-A002` composes the frozen `M259-A001` sample surface into one integrated
live runtime proof.

- contract id
  `objc3c-canonical-runnable-sample-set/m259-a002-v1`
- evidence model
  `a001-freeze-plus-live-integrated-runnable-object-property-category-protocol-sample`
- sample set model
  `integrated-runnable-sample-set-unifies-alloc-init-protocol-category-and-property-behavior`
- truthful boundary
  - the fixture and probe are single-module and runtime-backed
  - the live proof covers alloc/init, superclass dispatch, category dispatch,
    protocol conformance, and property access together
  - scalar execution smoke and replay remain separate corpus gates in this issue
  - no blocks, ARC, async, throws, actors, or import/module expansion land here
  - the next implementation issue is `M259-B001`

## M259 runnable core compatibility guard (B001)

`M259-B001` freezes the sema-owned compatibility/migration boundary around the
current runnable core.

- contract id
  `objc3c-runnable-core-compatibility-guard/m259-b001-v1`
- guard model
  `runnable-core-distinguishes-live-runtime-backed-core-from-source-only-or-fail-closed-advanced-surfaces`
- evidence model
  `a002-live-runnable-core-proof-plus-sema-compatibility-selection-and-unsupported-claim-boundary`
- truthful boundary
  - `M259-A002` remains the live runtime-backed proof floor
  - compatibility mode and migration assist remain live semantic selections
  - `O3S216` remains the live migration-assist fail-closed diagnostic
  - `@autoreleasepool`, block literals, `throws`, and ARC ownership qualifiers
    remain the currently landed unsupported-feature diagnostics
  - later advanced surfaces remain outside the runnable core and are not
    promoted by docs, smoke, replay, or package claims here
  - the next implementation issue is `M259-B002`

## M259 fail-closed unsupported advanced-feature diagnostics (B002)

`M259-B002` makes the runnable-core boundary executable as a hard fail-closed
semantic gate.

- contract id
  `objc3c-runnable-core-unsupported-advanced-feature-diagnostics/m259-b002-v1`
- guard model
  `runnable-core-crossing-into-unsupported-advanced-surfaces-fails-before-lowering-runtime-handoff`
- evidence model
  `a002-runnable-proof-plus-b001-guard-plus-live-o3s221-negative-source-probes`
- truthful boundary
  - `M259-A002` remains the positive runnable-core proof floor
  - `M259-B001` remains the frozen compatibility/migration boundary
  - the semantic packet
    `frontend.pipeline.semantic_surface.objc_compatibility_strictness_claim_semantics`
    must stay ready with zero live unsupported rejection sites on positive
    runnable probes
  - accepted unsupported advanced surfaces fail closed with `O3S221` before
    lowering/runtime handoff
  - this issue proves the live negative path for `throws`,
    `@autoreleasepool`, and ARC ownership qualifiers
  - block literals remain explicitly unsupported without over-claiming this
    issue as the canonical live proof path while that source surface is still
    gated earlier
  - the next implementation issue is `M259-C001`

## M259 end-to-end replay and inspection freeze (C001)

`M259-C001` freezes the current replay-proof and binary-inspection evidence
boundary for the runnable native Objective-C 3 slice before `M259-C002`
widens that evidence into live IR/object replay and deeper metadata inspection
proof.

- contract id
  `objc3c-runnable-replay-and-inspection-evidence-freeze/m259-c001-v1`
- freeze model
  `runnable-slice-replay-proof-and-single-sample-object-inspection-boundary`
- evidence model
  `execution-smoke-plus-replay-proof-plus-a002-object-section-anchor`
- failure model
  `fail-closed-on-runnable-replay-or-object-inspection-boundary-drift`
- truthful boundary
  - execution smoke remains the broad runnable replay corpus
  - replay proof remains the canonical deterministic replay artifact boundary
  - `M259-A002` remains the single integrated runnable sample allowed to drive
    live object-section inspection in this issue
  - archive/static-link, multi-module, multi-image, and broader binary
    inspection expansion are deferred
  - the next implementation issue is `M259-C002`

## M259 object and IR replay-proof plus metadata inspection evidence (C002)

`M259-C002` implements the live runnable replay-proof capability above the
`M259-C001` freeze.

- contract id
  `objc3c-runnable-object-ir-replay-and-metadata-inspection/m259-c002-v1`
- replay model
  `a002-canonical-runnable-sample-ir-object-and-readobj-section-replay`
- evidence model
  `execution-replay-proof-script-emits-live-ir-object-and-section-inspection-hashes-for-a002`
- failure model
  `fail-closed-on-ir-object-or-metadata-inspection-replay-drift`
- truthful boundary
  - `scripts/check_objc3c_execution_replay_proof.ps1` must now emit one live
    proof over the canonical A002 runnable sample rather than relying on a
    manifest-only summary layer
  - the proof compares replay-stable hashes for `module.ll`, `module.obj`, and
    `llvm-readobj --sections` output across two runs of the same canonical
    runnable sample
  - the proof remains rooted in the scalar/core smoke corpus plus the dedicated
    A002 object-model sample
  - broader archive/static-link, multi-module, and multi-image replay or
    inspection expansion is deferred
  - the next implementation issue is `M259-D001`

## M259 toolchain and runtime operations freeze (D001)

`M259-D001` freezes the supported local toolchain/runtime operations boundary
for the runnable core before `M259-D002` expands workflow and packaging
implementation.

- contract id
  `objc3c-runnable-toolchain-runtime-operations-freeze/m259-d001-v1`
- operations model
  `runnable-core-build-compile-smoke-replay-operations-boundary`
- evidence model
  `operations-freeze-docs-package-and-script-anchors-for-runnable-core`
- failure model
  `fail-closed-on-unsupported-packaging-or-runtime-operations-claim-drift`
- frozen operations
  - `npm run build:objc3c-native`
  - `pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/objc3c_native_compile.ps1 <input.objc3> --out-dir <out_dir> --emit-prefix module`
  - `npm run test:objc3c:execution-smoke`
  - `npm run test:objc3c:execution-replay-proof`
- truthful boundary
  - only the current Windows x64 + `pwsh` + `python` + `node`/`npm` + MSVC/CMake/Ninja + LLVM `llc`/`llvm-readobj` host baseline is claimed here
  - installer, system deployment, and cross-platform packaging claims remain deferred
  - the next implementation issue is `M259-D002`

## M259 staged runnable toolchain package workflow (D002)

`M259-D002` expands the frozen runnable-core operations boundary into a staged
package workflow that preserves the current script/artifact/test layout inside a
local package root.

- contract id
  `objc3c-runnable-build-install-run-package/m259-d002-v1`
- package model
  `staged-runnable-toolchain-bundle-with-repo-relative-layout`
- install model
  `local-package-root-not-system-install`
- staged package manifest
  `artifacts/package/objc3c-runnable-toolchain-package.json`
- required workflow continuity
  - build remains `npm run build:objc3c-native`
  - packaging is `npm run package:objc3c-native:runnable-toolchain`
  - packaged compile remains `scripts/objc3c_native_compile.ps1`
  - packaged smoke remains `scripts/check_objc3c_native_execution_smoke.ps1`
  - packaged replay remains `scripts/check_objc3c_execution_replay_proof.ps1`
- required package payload continuity
  - `artifacts/bin/objc3c-native.exe`
  - `artifacts/bin/objc3c-frontend-c-api-runner.exe`
  - `artifacts/lib/objc3_runtime.lib`
  - `tests/tooling/fixtures/native/execution`
  - `tests/tooling/runtime/objc3_msgsend_i32_shim.c`
  - the frontend readiness JSON payloads under `tmp/artifacts/objc3c-native/`
- truthful boundary
  - staged local package root only
  - no system install claim
  - no cross-platform packaging claim
  - no toolchain auto-provisioning claim
  - `M259-D003` documents platform bring-up on top of this workflow

## M259 platform prerequisites and runtime bring-up documentation (D003)

`M259-D003` does not widen the runnable boundary again. It documents the exact
supported Windows bring-up requirements and environment override surface above
`M259-D002` so repo-root and package-root workflows remain truthful.

- contract id
  `objc3c-runnable-platform-prerequisites-runtime-bringup/m259-d003-v1`
- bring-up model
  `supported-windows-host-prereqs-and-package-root-runtime-bringup`
- evidence model
  `docs-and-script-anchors-for-prereq-and-runtime-bringup-truthfulness`
- failure model
  `fail-closed-on-prerequisite-or-runtime-bringup-claim-drift`
- required documented prerequisites
  - Windows x64
  - `pwsh`
  - `python`
  - `node`/`npm`
  - LLVM `clang`, `clang++`, `llc`, `llvm-readobj`, `llvm-lib`
  - MSVC/Windows SDK linker tools reachable from `clang`
- required documented environment overrides
  - `OBJC3C_NATIVE_EXECUTABLE`
  - `OBJC3C_NATIVE_EXECUTION_CLANG_PATH`
  - `OBJC3C_NATIVE_EXECUTION_LLC_PATH`
  - `OBJC3C_NATIVE_EXECUTION_LLVM_READOBJ_PATH`
  - `OBJC3C_NATIVE_EXECUTION_RUN_ID`
- truthful boundary
  - package-root execution still assumes the repo-relative staged layout from `M259-D002`
  - no system install or auto-provisioning claim lands here
  - the next implementation issue is `M259-E001`

## M259 runnable object-model release gate (E001)

`M259-E001` freezes the final lane-E release-gate boundary for the current
runnable object-model slice without yet widening into the full conformance
matrix promised by `M259-E002`.

- contract id
  `objc3c-runnable-object-model-release-gate/m259-e001-v1`
- gate model
  `lane-e-release-gate-over-a002-b002-c002-d003`
- evidence model
  `freeze-the-release-gate-over-canonical-sample-compatibility-replay-and-bringup`
- failure model
  `fail-closed-on-release-gate-claim-drift`
- required preserved inputs
  - `M259-A002`
  - `M259-B002`
  - `M259-C002`
  - `M259-D003`
  - `scripts/check_objc3c_native_execution_smoke.ps1`
  - `scripts/check_objc3c_execution_replay_proof.ps1`
- explicit non-goals
  - no full runnable conformance matrix claim yet
  - no block/ARC release claim yet
  - no installer or cross-platform release claim yet
- truthful boundary
  - `M259-E001` freezes the gate only
  - the next implementation issue is `M259-E002`

## M259 full runnable object-model conformance matrix (E002)

`M259-E002` expands the frozen lane-E gate into a deterministic conformance
matrix for the current runnable object-model subset.

- contract id
  `objc3c-runnable-object-model-conformance-matrix/m259-e002-v1`
- matrix model
  `deterministic-row-per-runnable-claim-with-fixture-or-command-proof`
- evidence model
  `tracked-json-matrix-over-a002-b002-c002-d003-and-live-script-anchors`
- failure model
  `fail-closed-on-matrix-row-drift-or-unbacked-runnable-claim`
- matrix artifact
  `spec/planning/compiler/m259/m259_e002_full_runnable_object_model_conformance_matrix.json`
- row requirements
  - every row has a stable `row_id`
  - every row points at a concrete fixture or inspection command
  - every row names the summary/evidence artifact that proves the claim
- truthful boundary
  - only the current runnable object-model subset is covered
  - no block/ARC conformance claim yet
  - the next implementation issue is `M259-E003`

## M259 runnable object-model closeout and sign-off (E003)

`M259-E003` closes the tranche with synchronized docs, operator commands,
sample references, and sign-off evidence over every predecessor issue in
`M259`.

- contract id
  `objc3c-runnable-object-model-closeout-signoff/m259-e003-v1`
- closeout model
  `runbook-plus-signoff-summary-over-all-m259-predecessor-summaries`
- evidence model
  `tracked-runbook-and-signoff-summary-with-predecessor-green-chain`
- failure model
  `fail-closed-on-closeout-drift-or-missing-predecessor-signoff`
- required closeout assets
  - `docs/runbooks/m259_runnable_object_model_closeout.md`
  - `tmp/reports/m259/M259-E003/runnable_object_model_closeout_signoff_summary.json`
- truthful boundary
  - `M259` closes the runnable object-model slice only
  - block/ARC work remains deferred to `M260+`
  - the next implementation issue is `M260-A001`

## M260 runtime-backed object ownership surface freeze (A001)

`M260-A001` freezes the current ownership boundary above the newly runnable
object-model slice.

- contract id
  `objc3c-runtime-backed-object-ownership-surface-freeze/m260-a001-v1`
- surface model
  `runtime-backed-object-ownership-surface-freezes-property-accessor-and-legacy-lowering-ownership-profiles-before-live-arc-runtime-semantics`
- evidence model
  `canonical-runnable-sample-manifest-and-ir-ownership-profile-proof`
- failure model
  `fail-closed-on-ownership-surface-drift-or-premature-arc-runnable-claim`
- preserved truths
  - property/accessor ownership profiles emitted by the runnable object-model
    slice remain authoritative
  - legacy ownership lowering summaries remain the canonical replay surface for
    ownership qualifier, retain/release, weak/unowned, and ARC fix-it data
- non-goals
  - no live ARC retain/release/autorelease runtime semantics
  - no executable function/method ownership-qualifier support
  - no `@autoreleasepool` runnable support
- truthful boundary
  - `M260-A002` is the next implementation issue

## M260 runtime-backed object ownership attribute surface (A002)

`M260-A002` upgrades the frozen runtime-backed ownership boundary into an
emitted metadata capability for properties and members.

- contract id
  `objc3c-runtime-backed-object-ownership-attribute-surface/m260-a002-v1`
- source model
  `runtime-backed-property-source-surface-publishes-attribute-lifetime-hook-and-accessor-ownership-profiles`
- descriptor model
  `emitted-property-descriptor-records-carry-attribute-lifetime-hook-and-accessor-ownership-strings`
- runtime model
  `runtime-backed-property-metadata-consumes-emitted-ownership-strings-without-source-rediscovery`
- failure model
  `no-manifest-only-ownership-proof-no-source-recovery-no-live-arc-hook-emission-yet`
- truthful boundary
  - this issue is about emitted property/member ownership surface only
  - live ARC retain/release/autorelease runtime hook emission remains deferred
  - `M260-B001` is the next issue

## M260 retainable object semantic rules freeze (B001)

`M260-B001` freezes the truthful semantic boundary for retainable-object
behavior above the newly live runtime-backed property/member ownership surface.

- contract id
  `objc3c-retainable-object-semantic-rules-freeze/m260-b001-v1`
- semantic model
  `runtime-backed-object-semantic-rules-freeze-property-member-ownership-metadata-while-retain-release-and-storage-legality-remain-summary-driven`
- destruction model
  `destruction-order-autoreleasepool-and-live-arc-execution-stay-fail-closed-outside-runtime-backed-storage-legality`
- failure model
  `fail-closed-on-retainable-object-semantic-drift-or-premature-live-storage-legality-claim`
- preserved truths
  - runtime-backed property/member ownership metadata remains the truthful live
    ownership surface for the current slice
  - `retain_release_operation_lowering` remains the canonical replay surface
    for retain/release legality until `M260-B002`
  - `weak_unowned_semantics_lowering` remains the canonical replay surface for
    weak/unowned legality until later M260 semantic/runtime issues land
  - `autoreleasepool_scope_lowering` remains the canonical replay surface for
    non-runnable autoreleasepool behavior
- non-goals
  - no live ARC retain/release/autorelease execution semantics
  - no executable function/method ownership-qualifier support
  - no runnable destruction-order semantics
- truthful boundary
  - `M260-B002` is the next implementation issue

## M260 runtime-backed storage ownership legality (B002)

`M260-B002` turns the frozen retainable-object storage boundary into a live
semantic legality pass for runtime-backed Objective-C object properties.

- contract id
  `objc3c-runtime-backed-storage-ownership-legality/m260-b002-v1`
- owned storage model
  `explicit-strong-object-property-qualifiers-remain-legal-for-owned-runtime-backed-storage-while-conflicting-weak-or-unowned-modifiers-fail-closed`
- weak/unowned model
  `explicit-weak-and-unsafe-unretained-object-property-qualifiers-bind-runtime-backed-storage-legality-and-reject-conflicting-property-modifiers`
- failure model
  `fail-closed-on-runtime-backed-object-property-ownership-qualifier-modifier-drift`
- implemented legality
  - explicit `__weak` qualifiers are legal only with `weak` storage or no
    explicit ownership modifier
  - explicit `__unsafe_unretained` qualifiers are legal only with `assign`
    storage or no explicit ownership modifier
  - explicit `__strong` qualifiers are legal only with `strong`, `copy`, or no
    explicit ownership modifier
- fail-closed boundary
  - conflicting qualifier/modifier pairs now fail in semantic analysis before
    runtime metadata emission
  - `unowned` remains the safe runtime-backed storage profile and therefore is
    not treated as a synonym for explicit `__unsafe_unretained`
  - `M260-B003` is the next implementation issue

## M260 autoreleasepool and destruction-order semantics (B003)

`M260-B003` expands the fail-closed semantic model for runtime-backed object
ownership by distinguishing plain autoreleasepool rejection from the
ownership-sensitive case where owned runtime-backed object or synthesized
property storage would require deferred destruction-order support.

- contract id
  `objc3c-runtime-backed-autoreleasepool-destruction-order-semantics/m260-b003-v1`
- autoreleasepool model
  `autoreleasepool-scopes-remain-fail-closed-while-owned-runtime-backed-object-storage-publishes-destruction-order-edge-diagnostics`
- destruction model
  `owned-runtime-backed-object-or-synthesized-property-storage-inside-autoreleasepool-requires-deferred-destruction-order-runtime-support`
- failure model
  `fail-closed-on-autoreleasepool-destruction-order-semantic-drift-for-owned-runtime-backed-storage`
- implemented semantic expansion
  - plain `@autoreleasepool` still fails with the existing native-mode
    unsupported-feature diagnostic
  - `@autoreleasepool` combined with owned runtime-backed object or synthesized
    property storage now emits a second deterministic destruction-order
    diagnostic
- truthful boundary
  - no live autoreleasepool lowering or runtime support lands here
  - no destruction-order runtime remains lands here
  - `M260-C001` is the next implementation issue

## M260 ownership lowering baseline freeze (C001)

`M260-C001` freezes the current lane-C lowering boundary for runtime-backed
object ownership.

- contract id
  `objc3c-ownership-lowering-baseline-freeze/m260-c001-v1`
- ownership qualifier model
  `ownership-qualifier-lowering-remains-legacy-summary-driven-for-runtime-backed-object-metadata`
- runtime hook model
  `retain-release-autorelease-and-weak-lowering-stays-summary-only-without-live-runtime-hook-emission`
- autoreleasepool model
  `autoreleasepool-lowering-remains-summary-only-without-emitted-push-pop-hooks`
- failure model
  `no-live-ownership-runtime-hooks-no-arc-weak-side-table-entrypoints-no-destruction-lowering-yet`
- preserved lowering surfaces
  - `ownership_qualifier_lowering` remains the canonical replay surface for
    executable ownership qualifiers that still fail closed semantically
  - `retain_release_operation_lowering` remains the canonical replay surface
    for retain/release/autorelease counts until `M260-C002`
  - `autoreleasepool_scope_lowering` remains the canonical replay surface for
    non-runnable autoreleasepool lowering until `M260-C002`
  - `weak_unowned_semantics_lowering` remains the canonical replay surface for
    weak/unowned lowering counts until `M260-C002`
- truthful boundary
  - runtime-backed ownership metadata and storage legality are already live
  - no retain/release/autorelease function calls are emitted into LLVM IR yet
  - no autoreleasepool push/pop entrypoints are emitted yet
  - no weak side-table runtime entrypoints are emitted yet
  - `M260-C002` is the next implementation issue

## M260 ownership runtime hook emission (C002)

`M260-C002` upgrades synthesized runtime-backed property accessors into live
runtime hook emission.

- contract id
  `objc3c-ownership-runtime-hook-emission/m260-c002-v1`
- accessor model
  `synthesized-accessors-call-runtime-owned-current-property-and-ownership-hook-entrypoints`
- property-context model
  `runtime-dispatch-frame-selects-current-receiver-property-accessor-and-autorelease-queue`
- autorelease model
  `autorelease-values-drain-at-runtime-dispatch-return`
- failure model
  `owned-and-weak-runtime-backed-accessors-may-not-fall-back-to-summary-only-lowering`
- emitted lowering surface
  - LLVM IR now emits `ownership_runtime_hook_emission` summaries and
    `!objc3.objc_runtime_ownership_hook_emission`
  - strong synthesized getters call runtime-owned read, retain, and
    autorelease helpers
  - strong synthesized setters call runtime-owned retain, exchange, and
    release helpers
  - weak synthesized accessors call runtime-owned weak load/store helpers
  - legacy synthesized property storage globals remain emitted for historical
    `M257-C003` artifact compatibility only
- truthful boundary
  - live autoreleasepool push/pop lowering still remains outside this issue
  - `M260-D001` is the next implementation issue

## M260 runtime memory-management API freeze (D001)

`M260-D001` freezes the runtime memory-management API boundary after
`M260-C002`.

- contract id
  `objc3c-runtime-memory-management-api-freeze/m260-d001-v1`
- reference model
  `public-runtime-abi-stays-register-lookup-dispatch-while-reference-counting-helpers-remain-private-runtime-entrypoints`
- weak model
  `weak-storage-remains-served-through-private-runtime-helper-entrypoints-and-runtime-side-tables`
- autoreleasepool model
  `no-public-autoreleasepool-push-pop-api-yet-autorelease-helper-drains-only-on-dispatch-frame-return`
- failure model
  `no-public-memory-management-header-widening-no-user-facing-arc-entrypoints-yet`
- frozen runtime surface
  - LLVM IR now publishes `runtime_memory_management_api` summaries and
    `!objc3.objc_runtime_memory_management_api`
  - lowered ownership helpers remain private runtime entrypoints consumed by
    IR and runtime probes, not public runtime header declarations
  - live public autoreleasepool push/pop APIs still do not exist here
- truthful boundary
  - `M260-D002` is the next implementation issue

## M260 reference counting, weak table, and autoreleasepool implementation (D002)

`M260-D002` upgrades the frozen D001 helper boundary into runnable runtime
memory-management behavior.

- contract id
  `objc3c-runtime-memory-management-implementation/m260-d002-v1`
- refcount model
  `runtime-managed-instance-retain-counts-destroy-strong-owned-storage-on-final-release`
- weak model
  `weak-side-table-tracks-runtime-storage-observers-and-zeroes-them-on-final-release`
- autoreleasepool model
  `private-autoreleasepool-push-pop-scopes-retain-autoreleased-runtime-values-until-lifo-drain`
- failure model
  `memory-management-runtime-support-remains-private-lowered-and-runtime-probe-driven`
- live runtime surface
  - LLVM IR now publishes `runtime_memory_management_implementation` summaries
    and `!objc3.objc_runtime_memory_management_implementation`
  - native lowering now emits private autoreleasepool push/pop helper calls
  - runtime-backed getters, setters, retain/release, weak zeroing, and pool
    drain behavior now form one executable path
- truthful boundary
  - the helper/runtime surface still stays private to the runtime internals
  - `M260-E001` is the next issue

## M260 ownership runtime gate freeze (E001)

`M260-E001` freezes the evidence contract for the supported ownership runtime
baseline.

- contract id
  `objc3c-ownership-runtime-gate-freeze/m260-e001-v1`
- supported model
  `runtime-backed-object-baseline-proves-strong-weak-and-autoreleasepool-behavior-through-private-runtime-hooks`
- evidence model
  `gate-consumes-m260-c002-d001-d002-contract-summaries-and-runtime-probe-evidence`
- non-goal model
  `no-arc-automation-no-block-ownership-runtime-no-public-ownership-api-widening`
- failure model
  `integration-gate-must-not-claim-more-than-the-supported-runtime-backed-ownership-baseline`
- emitted/runtime anchors
  - LLVM IR now publishes `ownership_runtime_gate` summaries and
    `!objc3.objc_ownership_runtime_gate`
  - the gate consumes the already-live C002/D001/D002 boundaries rather than
    redefining the runtime surface
- truthful boundary
  - `M260-E002` must prove exactly this frozen runtime-backed ownership slice

## M260 runnable ownership smoke matrix and docs (E002)

`M260-E002` publishes the lane-E closeout matrix for the supported ownership
baseline.

- contract id
  `objc3c-runnable-ownership-smoke-matrix/m260-e002-v1`
- matrix model
  `closeout-matrix-consumes-a002-b003-c002-d002-and-e001-evidence-without-widening-the-runtime-backed-ownership-slice`
- runnable smoke model
  `real-integrated-compile-and-runtime-probe-prove-meaningful-strong-weak-and-autoreleasepool-object-programs`
- failure model
  `fail-closed-on-ownership-smoke-matrix-drift-or-closeout-doc-mismatch`
- canonical matrix rows
  - `A002` emitted ownership attribute surface
  - `B003` ownership-sensitive autoreleasepool destruction-order guardrail
  - `C002` ownership runtime hook lowering boundary
  - `D002` runtime memory-management execution proof
  - `E001` ownership runtime gate freeze
- truthful boundary
  - this issue closes the current ownership slice only
  - no ARC automation, block ownership runtime, or public ownership ABI
    widening lands here
  - the next issue is `M261-A001`

## M261 executable block source closure (A001)

`M261-A001` freezes the truthful block source surface before the runtime block
object model exists.

- contract id
  `objc3c-executable-block-source-closure/m261-a001-v1`
- source-surface model
  `parser-owned-block-literal-source-closure-freezes-capture-abi-storage-copy-dispose-and-baseline-profiles-before-runnable-block-realization`
- evidence model
  `hello-ir-boundary-plus-block-literal-o3s221-fail-closed-native-probe`
- fail-closed model
  `fail-closed-on-block-source-surface-drift-before-block-runtime-realization`
- truthful boundary
  - block literals must now parse through the shared block parser without
    parser-surface drift.
  - the parser/AST handoff keeps replay-stable capture, ABI, storage,
    copy/dispose, and determinism profiles only.
  - runnable block lowering, block pointer declarator spellings, and explicit
    `__block` byref storage spellings remain outside this issue.
  - `M261-A002` is the next issue.

## M261 block source model completion (A002)

`M261-A002` upgrades the frozen A001 block source surface into a deterministic
source-only frontend capability that later block sema/lowering/runtime work can
consume directly.

- contract id
  `objc3c-executable-block-source-model-completion/m261-a002-v1`
- source-only positive path:
  - `artifacts/bin/objc3c-frontend-c-api-runner.exe --no-emit-ir --no-emit-object`
    admits block literals through sema and emits the
    `objc_block_source_model_completion_surface` manifest object.
- source model content:
  - parameter-signature entries and explicit/implicit parameter counts
  - capture-inventory entries and truthful by-value readonly storage counts
  - invoke-surface entries for descriptor and invoke trampoline symbols
  - deterministic replay key
- emitted IR boundary:
  - `; executable_block_source_model_completion = ...`
- fail-closed rule:
  - runnable native emit paths still reject block literals with `O3S221`.
- non-goals:
  - explicit `__block` storage spelling
  - copy/dispose helper emission
  - runnable block object/runtime lowering
- architecture/spec/checker anchors for this issue are:
  - `docs/contracts/m261_block_literal_signature_capture_inventory_and_invoke_surface_source_modeling_core_feature_implementation_a002_expectations.md`
  - `spec/planning/compiler/m261/m261_a002_block_literal_signature_capture_inventory_and_invoke_surface_source_modeling_core_feature_implementation_packet.md`
  - `python scripts/check_m261_a002_block_literal_signature_capture_inventory_and_invoke_surface_source_modeling_core_feature_implementation.py`
  - `M261-B001` is the next issue.

## M261 block source storage annotations (A003)

`M261-A003` expands the deterministic lane-A block source model with truthful
byref-candidate, helper-intent, and escape-shape annotations that later lane-B
through lane-D runnable work must consume directly.

- contract id
  `objc3c-executable-block-source-storage-annotation/m261-a003-v1`
- source-only positive path:
  - `artifacts/bin/objc3c-frontend-c-api-runner.exe --no-emit-ir --no-emit-object`
    admits block literals and emits the
    `objc_block_source_storage_annotation_surface` manifest object.
- source annotation content:
  - mutated-capture inventory and derived byref-candidate counts
  - copy/dispose helper intent counts derived from byref-candidate sites
  - deterministic escape-shape categories for expression, initializer,
    assignment, return, call, and message positions
  - heap-candidate truth derived from the escape-shape class
  - deterministic replay key
- emitted IR boundary:
  - `; executable_block_source_storage_annotations = ...`
- fail-closed rule:
  - runnable native emit paths still reject block literals with `O3S221`.
- non-goals:
  - explicit `__block` spelling
  - runnable byref lowering
  - copy/dispose helper lowering
  - heap-promotion runtime support
- architecture/spec/checker anchors for this issue are:
  - `docs/contracts/m261_byref_storage_helper_intent_and_escape_shape_source_annotations_core_feature_expansion_a003_expectations.md`
  - `spec/planning/compiler/m261/m261_a003_byref_storage_helper_intent_and_escape_shape_source_annotations_core_feature_expansion_packet.md`
  - `python scripts/check_m261_a003_byref_storage_helper_intent_and_escape_shape_source_annotations_core_feature_expansion.py`
  - `M261-B001` is the next issue.

## M261 block runtime semantic rules (B001)

`M261-B001` freezes the truthful semantic-rule boundary that the current block
implementation exposes before lane-B begins implementing runnable capture,
byref, helper, escape, or invocation behavior.

- contract id
  `objc3c-executable-block-runtime-semantic-rules/m261-b001-v1`
- current semantic-rule content:
  - source-only block admission remains the only supported positive path.
  - block literals are currently validated as deterministic function-shaped
    source values only.
  - byref, helper-intent, and escape-shape truth remains source-owned and is
    not yet runnable runtime behavior.
  - native emit paths still fail closed on block literals with `O3S221`.
- emitted IR boundary:
  - `; executable_block_runtime_semantic_rules = ...`
- non-goals:
  - runnable capture legality beyond deterministic metadata checks
  - runnable byref lowering
  - helper lowering or helper emission
  - heap promotion or block-object execution
  - runnable block invocation semantics
- architecture/spec/checker anchors for this issue are:
  - `docs/contracts/m261_block_runtime_semantic_rules_contract_and_architecture_freeze_b001_expectations.md`
  - `spec/planning/compiler/m261/m261_b001_block_runtime_semantic_rules_contract_and_architecture_freeze_packet.md`
  - `python scripts/check_m261_b001_block_runtime_semantic_rules_contract_and_architecture_freeze.py`
  - `M261-B002` is the next issue.

## M261 capture legality, escape classification, and invocation typing (B002)

`M261-B002` turns the frozen block semantic boundary into a live source-only
sema capability without claiming runnable native block execution yet.

- contract id
  `objc3c-executable-block-capture-legality-escape-and-invocation/m261-b002-v1`
- current semantic implementation:
  - source-only frontend runs now reject undefined block captures with
    `O3S202`.
  - source-only frontend runs now type-check local block invocations against
    the block parameter signature and reject mismatches with `O3S206`.
  - truthful mutable-capture, byref, escape, and copy/dispose helper counts
    are preserved in the lowering handoff instead of being inflated to the
    full capture inventory.
  - native emit paths still fail closed on block literals with `O3S221`.
- current emitted/runtime boundary:
  - the native IR summary remains
    `; executable_block_runtime_semantic_rules = ...`
  - `lowering_block_storage_escape` and `lowering_block_copy_dispose` must now
    remain deterministic under truthful mutable/byref subset counts.
- non-goals:
  - runnable block-object execution
  - runnable heap promotion or helper emission
  - byref cell lowering beyond source-only legality/classification
- architecture/spec/checker anchors for this issue are:
  - `docs/contracts/m261_capture_legality_escape_classification_and_invocation_typing_core_feature_implementation_b002_expectations.md`
  - `spec/planning/compiler/m261/m261_b002_capture_legality_escape_classification_and_invocation_typing_core_feature_implementation_packet.md`
  - `python scripts/check_m261_b002_capture_legality_escape_classification_and_invocation_typing_core_feature_implementation.py`
  - `M261-B003` is the next issue.

## M261 byref mutation, copy-dispose eligibility, and object-capture ownership semantics (B003)

`M261-B003` extends the live source-only block semantic path so helper
eligibility and mutation legality track capture ownership instead of treating
every object capture the same.

- contract id
  `objc3c-executable-block-byref-copy-dispose-and-object-capture-ownership/m261-b003-v1`
- current semantic implementation:
  - mutating a captured `__weak` or `__unsafe_unretained` object now fails
    closed with `O3S206`.
  - owned object captures now promote copy/dispose helper eligibility even
    when no byref slots are present.
  - weak and unowned object captures remain non-owning and do not force helper
    generation by themselves.
  - native emit paths still fail closed on block literals with `O3S221`.
- current emitted/runtime boundary:
  - `lowering_block_copy_dispose` now reflects owned-object helper promotion
    without changing the source-owned A-lane annotation surface.
  - `lowering_block_storage_escape` remains truthful to byref-slot and
    escape-shape facts only.
- non-goals:
  - runnable block-object execution
  - live block helper emission
  - live weak/unowned byref runtime semantics
- architecture/spec/checker anchors for this issue are:
  - `docs/contracts/m261_byref_mutation_copy_dispose_eligibility_and_object_capture_ownership_core_feature_expansion_b003_expectations.md`
  - `spec/planning/compiler/m261/m261_b003_byref_mutation_copy_dispose_eligibility_and_object_capture_ownership_core_feature_expansion_packet.md`
  - `python scripts/check_m261_b003_byref_mutation_copy_dispose_eligibility_and_object_capture_ownership_core_feature_expansion.py`
  - `M261-C001` is the next issue.

## M261 block lowering ABI and artifact boundary (C001)

`M261-C001` freezes the truthful lane-C lowering boundary required for runnable
block objects while explicitly preserving the current native fail-closed rule.

- contract id
  `objc3c-executable-block-lowering-abi-artifact-boundary/m261-c001-v1`
- current boundary content:
  - source-only manifests already publish the capture, invoke, storage-escape,
    and copy/dispose lowering surfaces.
  - capture/invoke remain deterministic on the current owned-capture corpus.
  - storage-escape/copy-dispose remain truthful source-only helper and escape
    profiles and are not yet required to be deterministic.
  - native IR now republishes one dedicated
    `; executable_block_lowering_abi_artifact_boundary = ...` summary line.
  - helper symbol policy is still source-modeled only; invoke thunks, byref
    cells, and copy/dispose helpers are not emitted yet.
  - native emit paths still fail closed on block literals with `O3S221`.
- non-goals:
  - emitted block object records
  - emitted invoke-thunk bodies
  - emitted byref cell storage
  - emitted copy/dispose helper bodies
  - runnable block execution
- architecture/spec/checker anchors for this issue are:
  - `docs/contracts/m261_block_lowering_abi_and_artifact_boundary_contract_and_architecture_freeze_c001_expectations.md`
  - `spec/planning/compiler/m261/m261_c001_block_lowering_abi_and_artifact_boundary_contract_and_architecture_freeze_packet.md`
  - `python scripts/check_m261_c001_block_lowering_abi_and_artifact_boundary_contract_and_architecture_freeze.py`
  - `M261-C002` is the next issue.

## M261 executable block object and invoke-thunk lowering (C002)

`M261-C002` upgrades the frozen `M261-C001` lane-C boundary into one runnable
native lowering slice.

- contract id
  `objc3c-executable-block-object-and-invoke-thunk-lowering/m261-c002-v1`
- active supported slice:
  - native lowering emits one stack block object plus one internal invoke thunk
    for direct local invocation
  - captures must remain readonly scalar values
  - native compile/link/run proof exits `15` on the canonical positive fixture
- emitted proof details:
  - IR carries
    `; executable_block_object_invoke_thunk_lowering = ...`
  - block storage is materialized as `{ ptr, [N x i32] }`
  - the thunk pointer is stored into the block header and later invoked through
    one loaded function pointer
- deferred to `M261-C003`:
  - byref cells
  - copy/dispose helper bodies
  - owned object capture runtime lowering
  - heap-promotion semantics
  - those cases still fail closed with `O3S221`
- architecture/spec/checker anchors for this issue are:
  - `docs/contracts/m261_executable_block_object_and_invoke_thunk_lowering_core_feature_implementation_c002_expectations.md`
  - `spec/planning/compiler/m261/m261_c002_executable_block_object_and_invoke_thunk_lowering_core_feature_implementation_packet.md`
  - `python scripts/check_m261_c002_executable_block_object_and_invoke_thunk_lowering_core_feature_implementation.py`
  - `M261-C003` is the next issue.

## M261 byref-cell, copy-helper, and dispose-helper lowering (C003)

`M261-C003` upgrades the runnable `M261-C002` block lowering slice into the
first native path that emits stack byref cells plus copy/dispose helper bodies
for local nonescaping block captures.

- contract id
  `objc3c-executable-block-byref-helper-lowering/m261-c003-v1`
- active supported slice:
  - native lowering emits pointer-capture block storage when byref or object
    captures are present
  - native lowering emits stack byref layout records plus helper bodies for the
    nonescaping byref/object-capture slice
  - direct local block invocation remains runnable through the emitted block
    header and invoke thunk
  - compile/link/run proof exits `15`, `14`, `11`, and `9` on the canonical
    local-runtime fixtures
- emitted proof details:
  - IR carries
    `; executable_block_byref_helper_lowering = ...`
  - byref/runtime object captures now emit helper symbols such as
    `@__objc3_block_copy_helper_*` and `@__objc3_block_dispose_helper_*`
  - manifest lowering surfaces publish deterministic byref layout symbols for
    the runnable local byref slice
  - owned captures emit retain/release helper bodies while weak/unowned
    captures stay non-owning and helper-elided
- deferred to `M261-C004`:
  - escaping block heap-promotion/runtime hook lowering
  - first-class escaping block values
  - runtime-managed block allocation/copy semantics outside the local slice
- architecture/spec/checker anchors for this issue are:
  - `docs/contracts/m261_byref_cell_copy_helper_and_dispose_helper_lowering_core_feature_implementation_c003_expectations.md`
  - `spec/planning/compiler/m261/m261_c003_byref_cell_copy_helper_and_dispose_helper_lowering_core_feature_implementation_packet.md`
  - `python scripts/check_m261_c003_byref_cell_copy_helper_and_dispose_helper_lowering_core_feature_implementation.py`
  - `M261-C004` is the next issue.

## M261 heap-promotion and escaping-block runtime hook lowering (C004)

`M261-C004` widens the runnable `M261-C003` block lowering slice to admit
readonly scalar block values that escape through call arguments or return
values.

- contract id
  `objc3c-executable-block-escape-runtime-hook-lowering/m261-c004-v1`
- active supported slice:
  - native lowering emits private runtime heap-promotion and invoke hooks for
    readonly scalar escaping block values
  - local block bindings can be promoted by scalar use in call-argument or
    return-value positions
  - direct subsequent invocation resolves through the promoted runtime handle
  - compile/link/run proof exits `14` and `0` on the canonical escaping
    fixtures
- emitted proof details:
  - IR carries
    `; executable_block_escape_runtime_hook_lowering = ...`
  - IR declares and calls
    `@objc3_runtime_promote_block_i32` and
    `@objc3_runtime_invoke_block_i32`
  - object emission remains `llvm-direct`
- deferred to `M261-D001` and later:
  - block-object ABI freeze for the private runtime hook surface
  - runtime-managed escaping blocks with byref forwarding
  - runtime-managed escaping blocks with owned-object capture lifetimes
  - generalized block allocation/copy/dispose semantics outside the readonly
    scalar slice
- architecture/spec/checker anchors for this issue are:
  - `docs/contracts/m261_heap_promotion_and_escaping_block_runtime_hook_lowering_core_feature_expansion_c004_expectations.md`
  - `spec/planning/compiler/m261/m261_c004_heap_promotion_and_escaping_block_runtime_hook_lowering_core_feature_expansion_packet.md`
  - `python scripts/check_m261_c004_heap_promotion_and_escaping_block_runtime_hook_lowering_core_feature_expansion.py`
  - `M261-D001` is the next issue.

## M261 block runtime API and object layout (D001)

`M261-D001` freezes the truthful private runtime API and object-layout boundary
that the runnable `M261-C004` block lowering slice currently consumes.

- contract id
  `objc3c-runtime-block-api-object-layout-freeze/m261-d001-v1`
- frozen boundary:
  - public runtime headers still do not publish block helper entrypoints
  - private helper surface remains
    `objc3_runtime_promote_block_i32` and
    `objc3_runtime_invoke_block_i32`
  - the helper ABI is currently:
    - promotion: `ptr, i64, i32 -> i32`
    - invoke: `i32, i32, i32, i32, i32 -> i32`
  - private runtime block records remain opaque runtime-owned state rather than
    public object-layout ABI
  - emitted IR carries
    `; runtime_block_api_object_layout = ...`
- explicit non-goals:
  - no public block-object ABI
  - no generalized runtime-managed block allocation/copy/dispose surface
  - no byref-forwarded escaping block realization
  - no owned-object escaping block lifetime realization
- architecture/spec/checker anchors for this issue are:
  - `docs/contracts/m261_block_runtime_api_and_object_layout_contract_and_architecture_freeze_d001_expectations.md`
  - `spec/planning/compiler/m261/m261_d001_block_runtime_api_and_object_layout_contract_and_architecture_freeze_packet.md`
  - `python scripts/check_m261_d001_block_runtime_api_and_object_layout_contract_and_architecture_freeze.py`
  - `M261-D002` is the next issue.

## M261 block object allocation, copy-dispose, and invoke support (D002)

`M261-D002` upgrades the frozen D001 runtime helper boundary into a live
runtime capability for promoted block records.

- contract id
  `objc3c-runtime-block-allocation-copy-dispose-invoke-support/m261-d002-v1`
- current live behavior:
  - promoted runtime block records copy block storage into aligned runtime-owned
    word buffers
  - pointer-capture promotion preserves copy/dispose helper pointers
  - promotion runs the copy helper before publishing the runtime block handle
  - final runtime release runs the dispose helper before erasing the block
    record
  - `objc3_runtime_invoke_block_i32` now accepts promoted pointer-capture block
    records as well as the earlier readonly-scalar slice
  - emitted IR now carries
    `; runtime_block_allocation_copy_dispose_invoke_support = ...`
- still explicitly deferred:
  - byref-forwarded escaping block realization
  - runtime-reentrant ownership helper interop
  - public block-object ABI or public block runtime helper declarations
- architecture/spec/checker anchors for this issue are:
  - `docs/contracts/m261_block_object_allocation_copy_dispose_and_invoke_support_core_feature_implementation_d002_expectations.md`
  - `spec/planning/compiler/m261/m261_d002_block_object_allocation_copy_dispose_and_invoke_support_core_feature_implementation_packet.md`
  - `python scripts/check_m261_d002_block_object_allocation_copy_dispose_and_invoke_support_core_feature_implementation.py`
  - `M261-D003` is the next issue.

## M261 byref forwarding, heap promotion, and ownership interop for escaping blocks (D003)

`M261-D003` widens the live D002 block runtime so escaping pointer-capture
blocks no longer borrow stack-cell addresses after promotion.

- contract id
  `objc3c-runtime-block-byref-forwarding-heap-promotion-interop/m261-d003-v1`
- current live behavior:
  - promotion rewrites pointer-capture slots onto runtime-owned heap cells
    before helper execution
  - escaped byref mutation now persists across repeated block-handle invokes
    after the source frame returns
  - owned-capture copy/dispose helpers now run against runtime-owned capture
    cells instead of transient stack cells
  - emitted IR now carries
    `; runtime_block_byref_forwarding_heap_promotion_ownership_interop = ...`
- still explicitly deferred:
  - no public block-object ABI
  - no public stable runtime helper declarations
  - no bridge that forwards mutations back into a still-live caller stack frame
- architecture/spec/checker anchors for this issue are:
  - `docs/contracts/m261_byref_forwarding_cells_heap_promotion_and_ownership_interop_for_escaping_blocks_core_feature_expansion_d003_expectations.md`
  - `spec/planning/compiler/m261/m261_d003_byref_forwarding_cells_heap_promotion_and_ownership_interop_for_escaping_blocks_core_feature_expansion_packet.md`
  - `python scripts/check_m261_d003_byref_forwarding_cells_heap_promotion_and_ownership_interop_for_escaping_blocks_core_feature_expansion.py`
  - `M261-E001` is the next issue.

## M261 runnable block-runtime gate (E001)

`M261-E001` freezes the first lane-E proof gate for runnable blocks above the
retained source, sema, lowering, and runtime evidence chain.

- contract id
  `objc3c-runnable-block-runtime-gate/m261-e001-v1`
- evidence model
  `a003-b003-c004-d003-summary-chain`
- current gate claims:
  - runnable block support is validated against the retained A003/B003/C004/D003
    summaries rather than metadata-only summaries
  - the supported slice includes:
    - nonescaping byref and owned-capture helper-backed blocks
    - escaping readonly-scalar blocks through private runtime promotion/invoke
      hooks
    - escaping pointer-capture blocks through runtime-owned forwarding cells
      and helper interop after the source frame returns
  - emitted IR now carries:
    `; runnable_block_runtime_gate = ...`
    and `!objc3.objc_runnable_block_runtime_gate`
- still explicitly deferred:
  - no public block-object ABI
  - no public stable runtime helper declarations
  - no generalized foreign block ABI interop
  - no caller-frame forwarding bridge back into a still-live outer frame
- architecture/spec/checker anchors for this issue are:
  - `docs/contracts/m261_runnable_block_runtime_gate_contract_and_architecture_freeze_e001_expectations.md`
  - `spec/planning/compiler/m261/m261_e001_runnable_block_runtime_gate_contract_and_architecture_freeze_packet.md`
  - `python scripts/check_m261_e001_runnable_block_runtime_gate_contract_and_architecture_freeze.py`
  - `M261-E002` is the next issue.

## M261 runnable block execution matrix and docs (E002)

`M261-E002` closes the current M261 block-runtime tranche with one truthful
execution matrix above the retained `A003/B003/C004/D003/E001` chain.

- contract id
  `objc3c-runnable-block-execution-matrix/m261-e002-v1`
- evidence model
  `a003-b003-c004-d003-e001-summary-plus-integrated-native-block-smoke-matrix`
- closeout matrix claims:
  - the supported runnable block slice is proved through real native fixtures:
    - `m261_owned_object_capture_runtime_positive.objc3` exits `11`
    - `m261_nonowning_object_capture_runtime_positive.objc3` exits `9`
    - `m261_byref_cell_copy_dispose_runtime_positive.objc3` exits `14`
    - `m261_escaping_block_runtime_hook_argument_positive.objc3` exits `14`
    - `m261_escaping_block_runtime_hook_return_positive.objc3` exits `0`
  - `M261-D003` remains the authoritative escaping pointer-capture/byref
    runtime proof through its forwarding-cell runtime probe
  - emitted IR republishes:
    `; runnable_block_execution_matrix = ...`
    and `!objc3.objc_runnable_block_execution_matrix`
- still explicitly deferred:
  - no public block-object ABI
  - no public stable runtime helper declarations
  - no generalized foreign block ABI interop
  - no caller-frame forwarding bridge back into a still-live outer frame
- architecture/spec/checker anchors for this issue are:
  - `docs/contracts/m261_runnable_block_execution_matrix_for_captures_byref_helpers_and_escaping_blocks_cross_lane_integration_sync_e002_expectations.md`
  - `spec/planning/compiler/m261/m261_e002_runnable_block_execution_matrix_for_captures_byref_helpers_and_escaping_blocks_cross_lane_integration_sync_packet.md`
  - `python scripts/check_m261_e002_runnable_block_execution_matrix_for_captures_byref_helpers_and_escaping_blocks.py`
  - `M262-A001` is the next issue.

## M262 ARC source surface and mode boundary (A001)

`M262-A001` freezes the ARC-adjacent frontend and mode boundary that is already
truthfully present in the compiler.

- contract id
  `objc3c-arc-source-mode-boundary-freeze/m262-a001-v1`
- source model
  `ownership-qualifier-weak-unowned-autoreleasepool-and-arc-fixit-source-surfaces-remain-live-without-enabling-runnable-arc-mode`
- mode model
  `native-driver-rejects-fobjc-arc-while-executable-ownership-qualified-functions-and-methods-stay-fail-closed`
- current boundary:
  - ownership qualifier, weak/unowned, `@autoreleasepool`, and ARC fix-it
    surfaces remain replay-stable and visible to parser/sema/lowering summaries
  - runtime-backed ownership/property behavior from `M260` remains the only
    executable ownership baseline
  - the native driver still rejects `-fobjc-arc`
  - executable function/method ownership qualifiers still fail closed with
    `O3S221`
  - emitted IR now carries:
    `; arc_source_mode_boundary = ...`
    and `!objc3.objc_arc_source_mode_boundary`
- still explicitly deferred:
  - no user-visible `-fobjc-arc` or `-fno-objc-arc` mode split
  - no automatic ARC cleanup/retain-release insertion for general executable
    functions or methods
  - no claim that executable ownership-qualified functions or methods are
    runnable yet
- architecture/spec/checker anchors for this issue are:
  - `docs/contracts/m262_arc_source_surface_and_mode_boundary_contract_and_architecture_freeze_a001_expectations.md`
  - `spec/planning/compiler/m262/m262_a001_arc_source_surface_and_mode_boundary_contract_and_architecture_freeze_packet.md`
  - `python scripts/check_m262_a001_arc_source_surface_and_mode_boundary_contract_and_architecture_freeze.py`
  - `M262-A002` is the next issue.

## M262 ARC mode handling for methods, properties, returns, and block captures (A002)

`M262-A002` promotes the frozen ARC-adjacent boundary into a real explicit ARC
mode without claiming full ARC automation.

- contract id
  `objc3c-arc-mode-handling/m262-a002-v1`
- source model
  `ownership-qualified-method-property-return-and-block-capture-surfaces-are-runnable-under-explicit-arc-mode`
- mode model
  `driver-admits-fobjc-arc-and-fno-objc-arc-and-threads-arc-mode-through-frontend-sema-and-ir`
- current boundary:
  - `-fobjc-arc` admits ownership-qualified executable methods/functions
  - ownership-qualified property surfaces and block captures compile under
    explicit ARC mode
  - manifests and IR carry explicit ARC mode state
  - non-ARC mode still rejects executable ownership-qualified method/function
    signatures with `O3S221`
  - emitted IR now carries:
    `; arc_mode_handling = ...`
    and `!objc3.objc_arc_mode_handling`
- still explicitly deferred:
  - no generalized ARC cleanup/retain-release insertion
  - no claim of full ARC lifetime automation
  - no claim that forbidden ARC forms are complete yet
- architecture/spec/checker anchors for this issue are:
  - `docs/contracts/m262_arc_mode_handling_for_methods_properties_returns_and_block_captures_core_feature_implementation_a002_expectations.md`
  - `spec/planning/compiler/m262/m262_a002_arc_mode_handling_for_methods_properties_returns_and_block_captures_core_feature_implementation_packet.md`
  - `python scripts/check_m262_a002_arc_mode_handling_for_methods_properties_returns_and_block_captures_core_feature_implementation.py`
  - `M262-B001` is the next issue.

## M262 ARC semantic rules and forbidden forms (B001)

`M262-B001` freezes the semantic legality boundary that remains truthful once
explicit ARC mode exists.

- contract id
  `objc3c-arc-semantic-rules/m262-b001-v1`
- source model
  `explicit-arc-mode-admits-only-explicit-ownership-surfaces-while-forbidden-property-forms-and-broad-inference-remain-fail-closed`
- semantic model
  `conflicting-property-ownership-forms-and-atomic-ownership-aware-storage-still-fail-closed-while-general-arc-inference-remains-deferred`
- current boundary:
  - explicit ARC mode admits only explicitly spelled ownership-qualified
    executable surfaces
  - conflicting property ownership forms still fail closed
  - atomic ownership-aware properties still fail closed
  - broad ARC inference, lifetime extension, and method-family ARC semantics
    are still deferred
  - emitted IR now carries:
    `; arc_semantic_rules = ...`
    and `!objc3.objc_arc_semantic_rules`
- architecture/spec/checker anchors for this issue are:
  - `docs/contracts/m262_arc_semantic_rules_and_forbidden_forms_contract_and_architecture_freeze_b001_expectations.md`
  - `spec/planning/compiler/m262/m262_b001_arc_semantic_rules_and_forbidden_forms_contract_and_architecture_freeze_packet.md`
  - `python scripts/check_m262_b001_arc_semantic_rules_and_forbidden_forms_contract_and_architecture_freeze.py`
  - `M262-B002` is the next issue.

## M262 implicit retain-release inference and lifetime-extension semantics (B002)

`M262-B002` upgrades the supported explicit ARC slice from explicit-only
ownership spelling to one truthful inference boundary for unqualified object
signatures.

- contract id
  `objc3c-arc-inference-lifetime/m262-b002-v1`
- source model
  `explicit-arc-mode-now-infers-strong-owned-executable-object-signatures-for-the-supported-runnable-slice`
- semantic model
  `arc-enabled-unqualified-object-signatures-now-produce-canonical-retain-release-lifetime-accounting-while-nonarc-remains-zero-inference`
- current boundary:
  - under `-fobjc-arc`, unqualified object parameters and returns now infer
    strong-owned retain/release activity in sema and lowering
  - under `-fobjc-arc`, unqualified object property surfaces now infer a
    strong-owned lifetime profile in sema
  - the same source remains a zero-inference baseline without ARC mode
  - emitted IR now carries:
    `; arc_inference_lifetime = ...`
    and `!objc3.objc_arc_inference_lifetime`
  - the retain/release lowering replay profile is now the canonical live proof
    for this boundary
- architecture/spec/checker anchors for this issue are:
  - `docs/contracts/m262_implicit_retain_release_inference_and_lifetime_extension_semantics_core_feature_implementation_b002_expectations.md`
  - `spec/planning/compiler/m262/m262_b002_implicit_retain_release_inference_and_lifetime_extension_semantics_core_feature_implementation_packet.md`
  - `python scripts/check_m262_b002_implicit_retain_release_inference_and_lifetime_extension_semantics_core_feature_implementation.py`
  - `M262-B003` is the next issue.

## M262 weak, autorelease-return, property-synthesis, and block-interaction ARC semantics (B003)

`M262-B003` closes the next semantic interaction layer above the B002
inference baseline.

- contract id
  `objc3c-arc-interaction-semantics/m262-b003-v1`
- source model
  `explicit-arc-mode-now-covers-weak-autorelease-return-property-synthesis-and-block-ownership-interactions-for-the-supported-runnable-slice`
- semantic model
  `weak-properties-and-nonowning-captures-stay-nonretaining-autorelease-returns-stay-profiled-and-synthesized-property-accessors-publish-owned-lifetime-packets-under-arc`
- current boundary:
  - attribute-only strong properties now publish strong-owned lifetime and
    synthesized accessor ownership packets under ARC mode
  - attribute-only weak properties now publish weak lifetime and synthesized
    accessor ownership packets under ARC mode
  - explicit autorelease returns stay profiled through autorelease insertion
    accounting
  - owned block captures retain nonzero retain/release behavior under ARC mode
  - weak and unowned block captures remain non-owning under ARC mode
  - emitted IR now carries:
    `; arc_interaction_semantics = ...`
    and `!objc3.objc_arc_interaction_semantics`
- architecture/spec/checker anchors for this issue are:
  - `docs/contracts/m262_weak_autorelease_property_synthesis_and_block_interaction_arc_semantics_core_feature_expansion_b003_expectations.md`
  - `spec/planning/compiler/m262/m262_b003_weak_autorelease_property_synthesis_and_block_interaction_arc_semantics_core_feature_expansion_packet.md`
  - `python scripts/check_m262_b003_weak_autorelease_property_synthesis_and_block_interaction_arc_semantics_core_feature_expansion.py`
  - `M262-C001` is the next issue.

## M262 ARC lowering ABI and cleanup model (C001)

`M262-C001` freezes the current ARC lowering boundary that lane C already relies
on, without claiming that later ARC automation issues are complete.

- canonical contract id:
  - `objc3c-arc-lowering-abi-cleanup-model/m262-c001-v1`
- frozen boundary contents:
  - ARC semantic packets from `M262-A002`, `M262-B001`, `M262-B002`, and
    `M262-B003`
  - unwind-cleanup accounting already carried by the lowering handoff
  - the current private runtime helper entrypoints for retain/release,
    autorelease, weak property interaction, and autoreleasepool scope
- canonical ABI model:
  - `owned-value-lowering-targets-private-runtime-retain-release-autorelease-and-weak-helper-entrypoints-without-public-runtime-abi-expansion`
- canonical cleanup model:
  - `cleanup-scheduling-remains-explicit-summary-and-helper-boundary-only-until-m262-c002`
- emitted IR must now carry:
  - `; arc_lowering_abi_cleanup_model = ...`
- explicit non-goals:
  - no general ARC cleanup-scope insertion yet
  - no generalized weak load/store lowering yet
  - no automatic autorelease-return rewrite automation yet
- `M262-C002` is the next issue.

## M262 ARC automatic retain release autorelease insertion (C002)

`M262-C002` promotes the frozen ARC lowering boundary into a real helper-call
insertion path for the supported runnable ARC slice.

- canonical contract id:
  - `objc3c-arc-automatic-insertion/m262-c002-v1`
- supported source inputs now consumed by lane C:
  - ARC semantic insertion packets from `M262-A002`, `M262-B002`, and
    `M262-B003`
  - the helper ABI and cleanup boundary frozen in `M262-C001`
- canonical lowering model:
  - `owned-params-retain-on-entry-release-on-exit-and-autoreleasing-returns-lower-through-private-runtime-helpers`
- emitted IR must now carry:
  - `; arc_automatic_insertions = ...`
  - `!objc3.objc_arc_automatic_insertions = !{...}`
- supported automatic insertion now covers:
  - retain-on-entry for ARC-owned runnable parameters
  - release-on-exit for tracked ARC-owned storage
  - autorelease-return lowering for supported autoreleasing returns
- explicit non-goals:
  - no generalized local ARC cleanup stack
  - no exception-cleanup widening
  - no cross-module ARC optimization
- `M262-C003` is the next issue.

## M262 ARC cleanup emission weak load-store lowering and lifetime extension hooks (C003)

`M262-C003` widens the supported ARC lowering slice from helper insertion into
real cleanup emission and weak/runtime continuity for the supported runnable
subset.

- canonical contract id:
  - `objc3c-arc-cleanup-weak-lifetime-hooks/m262-c003-v1`
- supported source inputs now consumed by lane C:
  - the explicit ARC mode and interaction packets from `M262-A002` and
    `M262-B003`
  - the frozen helper ABI boundary from `M262-C001`
  - the live helper insertion path from `M262-C002`
- canonical lowering model:
  - `scope-exit-and-implicit-exit-cleanups-unwind-pending-block-dispose-and-arc-owned-storage-while-weak-current-property-access-stays-runtime-hooked`
- emitted IR must now carry:
  - `; arc_cleanup_weak_lifetime_hooks = ...`
  - `!objc3.objc_arc_cleanup_weak_lifetime_hooks = !{...}`
- supported cleanup/lifetime behavior now covers:
  - scope-exit dispose-helper emission for supported block captures
  - release-on-exit for tracked ARC-owned storage on supported implicit exits
  - deterministic weak current-property helper continuity under `-fobjc-arc`
- explicit non-goals:
  - no generalized weak local-storage lowering
  - no exception cleanup stack
  - no cross-module ARC optimization
- `M262-C004` is the next issue.

## M262 ARC and block-interaction lowering with autorelease-return conventions (C004)

`M262-C004` closes the supported escaping-block plus autoreleasing-return edge
inventory for the runnable ARC slice.

- canonical contract id:
  - `objc3c-arc-block-autorelease-return-lowering/m262-c004-v1`
- supported source inputs now consumed by lane C:
  - the `M262-C003` cleanup/weak/lifetime boundary
  - the retained escaping-block runtime-hook lowering from `M261-C004`
  - explicit return-autorelease packets already published by ARC sema
- canonical lowering model:
  - `escaping-block-promotion-and-terminal-branch-cleanup-compose-with-autoreleasing-returns-without-dropping-live-owned-storage-cleanup`
- emitted IR must now carry:
  - `; arc_block_autorelease_return_lowering = ...`
  - `!objc3.objc_arc_block_autorelease_return_lowering = !{...}`
- supported edge handling now covers:
  - escaping block promotion under explicit ARC mode
  - terminal branch cleanup that does not consume sibling-branch ARC cleanup
    state during code generation
  - autoreleasing returns that still execute required dispose/release cleanup
    after block interaction
- explicit non-goals:
  - no generalized method-family ARC automation
  - no public ARC runtime ABI
  - no cross-module ARC optimization
- `M262-D001` is the next issue.

## M262 runtime ARC helper API surface (D001)

`M262-D001` freezes the truthful private runtime helper ABI that the current
ARC lowering slice already consumes.

- canonical contract id:
  - `objc3c-runtime-arc-helper-api-surface-freeze/m262-d001-v1`
- canonical runtime/helper models:
  - `public-runtime-abi-stays-register-lookup-dispatch-while-arc-helper-entrypoints-remain-private-bootstrap-internal-runtime-abi`
  - `weak-storage-and-current-property-access-remain-served-through-private-runtime-helper-entrypoints-and-runtime-side-tables`
  - `autorelease-return-and-autoreleasepool-support-remain-private-runtime-helper-behavior-without-public-abi-widening`
- emitted IR must now carry:
  - `; runtime_arc_helper_api_surface = ...`
  - `!objc3.objc_runtime_arc_helper_api_surface`
- the frozen private helper ABI currently covers:
  - retain/release/autorelease helpers
  - current-property strong and weak access helpers
  - private autoreleasepool push/pop hooks
- explicit non-goals:
  - no public ARC runtime header widening
  - no user-facing ARC helper ABI
- `M262-D002` is the next issue.

## M262 runtime ARC helper runtime support (D002)

`M262-D002` proves the private ARC helper ABI frozen by `M262-D001` is a live
runtime-backed capability for the supported ARC property/weak and
autorelease-return slice.

- canonical contract id:
  - `objc3c-runtime-arc-helper-runtime-support/m262-d002-v1`
- canonical runtime-support models:
  - `m260-d002-runtime-baseline-plus-m262-c004-lowering-plus-m262-d001-private-helper-surface`
  - `arc-generated-weak-current-property-access-lowers-and-links-through-private-runtime-helper-entrypoints`
  - `arc-generated-autorelease-return-paths-link-and-execute-through-private-runtime-helper-entrypoints`
  - `runtime-library-backed-helper-entrypoints-remain-private-but-executable-through-linked-native-arc-programs`
- emitted IR must now carry:
  - `; runtime_arc_helper_runtime_support = ...`
  - `!objc3.objc_runtime_arc_helper_runtime_support`
- the supported runtime proof must now cover:
  - ARC property fixtures lowering through weak current-property helpers and
    emitting object artifacts successfully
  - ARC autorelease-return fixtures linking against the native runtime library
    and executing successfully
- explicit non-goals:
  - no public ARC helper ABI
  - no debug or ownership instrumentation hooks yet
- `M262-D003` is the next issue.

## M262 ARC ownership debug instrumentation and runtime validation hooks (D003)

`M262-D003` extends the live helper/runtime surface from `M262-D002` with a
private ARC debug snapshot surface for deterministic validation.

- canonical contract id:
  - `objc3c-runtime-arc-debug-instrumentation/m262-d003-v1`
- canonical debug/instrumentation models:
  - `m262-d002-live-helper-runtime-plus-private-bootstrap-internal-debug-snapshots`
  - `retain-release-autorelease-weak-current-property-and-autoreleasepool-helper-traffic-publishes-deterministic-debug-counters-and-last-value-context`
  - `runtime-probes-and-targeted-arc-fixtures-consume-private-debug-snapshots-without-widening-the-public-runtime-abi`
- emitted IR must now carry:
  - `; runtime_arc_debug_instrumentation = ...`
  - `!objc3.objc_runtime_arc_debug_instrumentation`
- the supported runtime proof must now cover:
  - deterministic counters for retain/release/autorelease helper traffic
  - deterministic counters for current-property and weak current-property helper traffic
  - deterministic counters for autoreleasepool push/pop helper traffic
  - last-value/property-context publication through private testing snapshots
- explicit non-goals:
  - no public ARC debug ABI
  - no user-facing ownership tracing hooks
  - no broader ARC runtime completeness claim beyond the supported runnable
    slice
- `M262-E001` is the next issue.

## M262 runnable ARC runtime gate (E001)

Contract id: `objc3c-runnable-arc-runtime-gate/m262-e001-v1`

`M262-E001` freezes the lane-E gate for the currently supported runnable ARC
slice. The gate consumes the `M262-A002`, `M262-B003`, `M262-C004`, and
`M262-D003` proof chain, publishes `!objc3.objc_runnable_arc_runtime_gate`,
and fails closed if the integrated ARC evidence drifts.

Evidence model:
`a002-b003-c004-d003-summary-chain`

## M262 runnable ARC closeout matrix and runbook (E002)

Contract id: `objc3c-runnable-arc-closeout/m262-e002-v1`

`M262-E002` closes the supported runnable ARC slice by consuming the
`M262-A002`, `M262-B003`, `M262-C004`, `M262-D003`, and `M262-E001` proof
chain plus the canonical ARC-positive execution-smoke rows under the dedicated
closeout run id.

Matrix model:
`closeout-matrix-consumes-a002-b003-c004-d003-and-e001-evidence-without-widening-the-supported-runnable-arc-slice`

Smoke model:
`integrated-arc-fixtures-and-private-property-runtime-probes-prove-supported-cleanup-block-and-property-behavior-through-native-toolchain-and-runtime`

Failure model:
`fail-closed-on-runnable-arc-closeout-drift-or-runbook-mismatch`
