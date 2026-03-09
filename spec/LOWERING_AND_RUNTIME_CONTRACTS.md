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
