# Objective‑C 3.0 — Module Metadata and ABI Surface Tables {#d}

_Working draft v0.11 — last updated 2026-02-27_

## D.0 Purpose {#d-0}

Objective‑C 3.0 adds source-level semantics that **must survive module boundaries**:
effects (`async`, `throws`), nullability defaults, executor/actor isolation, and dispatch controls (`objc_direct`, `objc_sealed`, etc.).

This document is a **normative checklist** for what a conforming implementation must preserve in:

- module metadata (binary or AST-based), and
- any emitted textual interface ([B.7](#b-7)).

It also summarizes which features are **ABI-affecting** and what ABI stability constraints apply.

> This document does not mandate a specific on-disk format. It defines _required information_.

## D.1 Terminology {#d-1}

- **Module metadata**: any compiled representation that an importer uses instead of re-parsing raw headers (e.g., Clang module files, serialized AST, interface stubs).
- **Textual interface**: a tool-emitted, importable text representation intended to preserve semantics for distribution ([B.7](#b-7)).
- **ABI-affecting**: changes the calling convention, symbol shape, layout, or runtime dispatch surface such that mismatches across translation units could cause miscompilation or runtime faults.

## D.2 Module metadata requirements (normative) {#d-2}

A conforming implementation shall preserve, for all exported declarations:

1. **Full type information** including:
   - parameter/return types,
   - nullability qualifiers and defaults ([Part 3](#part-3)),
   - ownership qualifiers and ARC-related attributes ([Part 4](#part-4)),
   - pragmatic generics information ([Part 3](#part-3)) sufficient for type checking (even if erased at ABI).

2. **Effect information**:
   - whether the callable is `throws`,
   - whether the callable is `async`,
   - and whether the callable is _potentially suspending at call sites_ due to isolation ([Part 7](#part-7)).

3. **Isolation and scheduling metadata**:
   - executor affinity (`objc_executor(...)`) ([Part 7](#part-7)),
   - actor isolation (actor type, isolated vs nonisolated members) ([Part 7](#part-7)),
   - Sendable-like constraints for cross-task/actor boundaries ([Part 7](#part-7)).

4. **Dispatch and dynamism controls**:
   - `objc_direct`, `objc_final`, `objc_sealed` ([Part 9](#part-9)),
   - any attributes that change call legality or override/subclass legality.

5. **Interop bridging markers**:
   - `objc_nserror` and `objc_status_code(...)` ([Part 6](#part-6)),
   - any language-defined overlays/bridges that affect call lowering.

6. **Provenance**:
   - the owning module of each exported declaration ([Part 2](#part-2)),
   - API partition membership (public/SPI/private) if the toolchain supports it ([Part 2](#part-2)).

7. **Lowering pass-graph replay anchors**:
   - deterministic replay keys for lowering-boundary normalization and runtime dispatch declaration synthesis used by direct IR emission,
   - explicit pass-graph core-feature readiness flag and replay key suitable for IR metadata emission,
   - explicit pass-graph core-feature expansion readiness flag and expansion key suitable for IR metadata emission and replay-proof closeout,
   - deterministic ownership-aware lowering replay keys for ownership qualifier,
     retain/release, autoreleasepool, and ARC diagnostics/fixit contracts,
   - ownership-aware lowering modular split scaffold readiness keys proving
     lane-contract replay-key determinism before lowering/emit handoff,
    - ownership-aware lowering core-feature implementation readiness markers
      proving fail-closed ownership-lowering replay-key determinism and
      milestone optimization improvements before direct IR emission hardening,
    - ownership-aware lowering core-feature expansion readiness/key markers
      proving weak/unowned expansion-accounting consistency and deterministic
      replay-proof key transport before direct IR emission,
    - direct IR-emission completeness metadata keys for pass-graph core/expansion
      readiness suitable for replay-proof closeout evidence,
   - deterministic IR-emission completeness modular split scaffold key/ready
     anchors linking pass-graph core, expansion, and edge compatibility replay
     evidence,
   - deterministic IR-emission core-feature implementation readiness/key anchors
     proving modular split transport and direct-IR boundary handoff stability,
   - deterministic object-emission backend route keys and output markers for
     clang/llvm-direct compile routing reliability evidence,
    - deterministic toolchain/runtime modular split scaffold keys for backend
      selection, backend capability availability, and IR/object compile-route
      readiness evidence,
    - deterministic toolchain/runtime core-feature implementation keys for
      backend dispatch consistency and backend-output marker path/payload
      readiness evidence,
    - deterministic edge-case compatibility readiness/key anchors for lowering
      compatibility handoff and pragma-order gating evidence,
    - deterministic edge-case expansion/robustness readiness/key anchors for
      lowering pass-graph robustness evidence,
    - deterministic diagnostics hardening readiness/key anchors for lowering
      pass-graph diagnostics-hardening evidence,
    - deterministic recovery and determinism readiness/key anchors for lowering
      pass-graph recovery-determinism evidence,
    - deterministic conformance-matrix readiness/key anchors for lowering
      pass-graph conformance-matrix evidence,
   - deterministic conformance-corpus readiness/key anchors for lowering
     pass-graph conformance-corpus evidence,
    - deterministic performance-quality guardrails readiness/key anchors for
      lowering pass-graph performance-quality evidence,
    - deterministic cross-lane integration sync anchors for `A011`, `B007`,
      `C005`, `D006`, and `E006` dependency continuity evidence,
   - deterministic docs/operator runbook synchronization anchors for `A011`,
     `B007`, `C005`, `D006`, `E006`, and `A012` documentation continuity
     evidence,
   - deterministic lane-A release-candidate replay dry-run anchors for
     `A013`, wrapper replay artifact continuity, and lowering replay-proof
     evidence continuity,
   - deterministic lane-A advanced core workpack (shard 1) anchors for
     `A014`, toolchain/runtime GA advanced-core consistency/readiness, and
     advanced-core-key continuity evidence,
   - deterministic lane-A integration closeout and gate sign-off anchors for
     `A015`, toolchain/runtime GA integration-closeout sign-off
     consistency/readiness, and integration-closeout sign-off key continuity
     evidence,
   - deterministic ownership-aware lowering edge-case compatibility
     consistency/readiness and compatibility-key anchors for lane-B closeout
     evidence,
   - deterministic ownership-aware lowering edge-case robustness
     expansion/readiness and robustness-key anchors for lane-B expansion
     evidence,
   - deterministic ownership-aware lowering diagnostics hardening
     consistency/readiness and diagnostics-key anchors for lane-B B007
     diagnostics hardening evidence,
   - deterministic ownership-aware lowering recovery and determinism
     consistency/readiness and recovery-key anchors for lane-B B008 recovery and determinism
     hardening evidence,
   - deterministic ownership-aware lowering conformance matrix
     consistency/readiness and conformance-matrix-key anchors for lane-B B009
     conformance matrix implementation evidence,
   - deterministic ownership-aware lowering conformance corpus
     consistency/readiness and conformance-corpus-key anchors for lane-B B010
     conformance corpus expansion evidence,
   - deterministic ownership-aware lowering performance and quality guardrails
     consistency/readiness, performance-quality-key, and parse-lowering
     performance/quality accounting anchors for lane-B B011 evidence,
   - deterministic ownership-aware lowering cross-lane integration
     consistency/readiness and cross-lane-integration-key anchors for lane-B
     B012 evidence linking ownership-aware guardrails with lane-A pass-graph
     conformance/performance continuity,
   - deterministic lane-B ownership-aware lowering docs/operator runbook
     synchronization metadata anchors for `M228-B013` plus explicit
     `M228-B012` dependency continuity so docs/runbook synchronization drift
     fails closed,
   - deterministic lane-B ownership-aware lowering release-candidate/replay
     metadata anchors for `M228-B014` plus explicit `M228-B013` dependency
     continuity so release/replay drift fails closed,
   - deterministic lane-B ownership-aware lowering advanced-core-shard1
     metadata anchors for `M228-B015` plus explicit `M228-B014` dependency
     continuity so advanced-core-shard1 drift fails closed,
   - deterministic lane-B ownership-aware lowering
     advanced-edge-compatibility-shard1 metadata anchors for `M228-B016` plus
     explicit `M228-B015` dependency continuity so
     advanced-edge-compatibility-shard1 drift fails closed,
   - deterministic lane-B ownership-aware lowering advanced-diagnostics-shard1
     metadata anchors for `M228-B017` plus explicit `M228-B016` dependency
     continuity so advanced-diagnostics-shard1 drift fails closed,
   - deterministic lane-B ownership-aware lowering advanced-conformance-shard1
     metadata anchors for `M228-B018` plus explicit `M228-B017` dependency
     continuity so advanced-conformance-shard1 drift fails closed,
   - deterministic lane-B ownership-aware lowering advanced-integration-shard1
     metadata anchors for `M228-B019` plus explicit `M228-B018` dependency
     continuity so advanced-integration-shard1 drift fails closed,
   - deterministic lane-B ownership-aware lowering advanced-performance-shard1
     metadata anchors for `M228-B020` plus explicit `M228-B019` dependency
     continuity so advanced-performance-shard1 drift fails closed,
   - deterministic lane-B ownership-aware lowering advanced-core-shard2
     metadata anchors for `M228-B021` plus explicit `M228-B020` dependency
     continuity so advanced-core-shard2 drift fails closed,
   - deterministic lane-B ownership-aware lowering integration-closeout-signoff
     metadata anchors for `M228-B022` plus explicit `M228-B021` dependency
     continuity so integration-closeout-signoff drift fails closed,
   - deterministic IR-emission core-feature expansion readiness/key anchors for
     lane-C expansion evidence continuity,
   - deterministic IR-emission edge-case compatibility completion
     consistency/readiness and compatibility-key anchors for lane-C closeout
     evidence continuity,
    - deterministic IR-emission recovery and determinism hardening
      consistency/readiness and recovery-determinism-key anchors for lane-C
      fail-closed evidence continuity,
    - deterministic IR-emission conformance matrix implementation
      consistency/readiness and conformance-matrix-key anchors for lane-C
      fail-closed evidence continuity,
    - deterministic IR-emission conformance corpus expansion
      consistency/readiness and conformance-corpus-key anchors for lane-C
      fail-closed evidence continuity,
    - deterministic IR-emission performance-quality guardrails
      consistency/readiness and performance-quality-key anchors for lane-C
      fail-closed evidence continuity,
    - deterministic IR-emission cross-lane integration sync
      consistency/readiness and cross-lane-integration-key anchors for lane-C
      fail-closed evidence continuity,
    - deterministic lane-C IR-emission docs/operator runbook
      synchronization metadata anchors for `M228-C013` plus explicit
      `M228-C012` dependency continuity so docs/runbook synchronization drift
      fails closed before release-candidate dry-run closure,
    - deterministic lane-C IR-emission release-candidate/replay
      metadata anchors for `M228-C014` plus explicit `M228-C013` dependency
      continuity so release/replay drift fails closed,
    - deterministic lane-C IR-emission advanced-core-shard1 metadata anchors
      for `M228-C015` plus explicit `M228-C014` dependency continuity so
      advanced-core-shard1 drift fails closed,
    - deterministic lane-C IR-emission advanced-edge-compatibility-shard1 metadata anchors
      for `M228-C016` plus explicit `M228-C015` dependency continuity so
      advanced-edge-compatibility-shard1 drift fails closed,
    - deterministic lane-C IR-emission advanced-diagnostics-shard1 metadata anchors
      for `M228-C017` plus explicit `M228-C016` dependency continuity so
      advanced-diagnostics-shard1 drift fails closed,
    - deterministic lane-C IR-emission advanced-conformance-shard1 metadata anchors
      for `M228-C018` plus explicit `M228-C017` dependency continuity so
      advanced-conformance-shard1 drift fails closed,
    - deterministic lane-C IR-emission advanced-integration-shard1 metadata anchors
      for `M228-C019` plus explicit `M228-C018` dependency continuity so
      advanced-integration-shard1 drift fails closed,
    - deterministic lane-D runtime-facing type metadata metadata anchors for `M227-D001`
      with canonical reference type-form order, runtime dispatch default symbol
      continuity (`objc3_msgsend_i32`), and fail-closed sema/pipeline/artifact
      handoff evidence continuity,
    - deterministic lane-D runtime-facing type metadata modular split/scaffolding metadata anchors for `M227-D002`
      with explicit `M227-D001` dependency continuity so sema scaffold/runtime metadata handoff drift fails closed,
    - deterministic lane-D runtime-facing type metadata edge-case expansion and robustness metadata anchors for `M227-D006`
      with explicit `M227-D005` dependency continuity and fail-closed evidence continuity so edge-case expansion/robustness evidence drift fails closed,
    - deterministic lane-D runtime-facing type metadata diagnostics hardening metadata anchors for `M227-D007`
      with explicit `M227-D006` dependency continuity and fail-closed diagnostics-hardening evidence continuity so diagnostics-hardening evidence drift fails closed,
    - deterministic lane-D runtime-facing type metadata recovery/determinism hardening metadata anchors for `M227-D008`
      with explicit `M227-D007` dependency continuity and fail-closed recovery-determinism evidence continuity so recovery/determinism evidence drift fails closed,
    - deterministic lane-D runtime-facing type metadata conformance matrix implementation metadata anchors for `M227-D009`
      with explicit `M227-D008` dependency continuity and fail-closed conformance-matrix evidence continuity so conformance-matrix evidence drift fails closed,
    - deterministic lane-D runtime-facing type metadata conformance corpus expansion metadata anchors for `M227-D010`
      with explicit `M227-D009` dependency continuity and fail-closed conformance-corpus evidence continuity so conformance-corpus evidence drift fails closed,
    - deterministic lane-D runtime-facing type metadata performance and quality guardrails metadata anchors for `M227-D011`
      with explicit `M227-D010` dependency continuity and fail-closed performance-quality evidence continuity so performance-quality evidence drift fails closed,
    - deterministic lane-D runtime-facing type metadata integration closeout and gate sign-off metadata anchors for `M227-D012`
      with explicit `M227-D011` dependency continuity and fail-closed integration-closeout-signoff evidence continuity so integration-closeout/sign-off evidence drift fails closed,
    - deterministic lane-D toolchain/runtime edge-case compatibility
      consistency/readiness and compatibility-key anchors for closeout evidence,
   - deterministic lane-D toolchain/runtime edge-case robustness
     consistency/readiness and robustness-key anchors for expansion evidence
     continuity,
   - deterministic lane-D toolchain/runtime recovery and determinism
     consistency/readiness and recovery-determinism-key anchors for fail-closed
     evidence continuity,
   - deterministic lane-D toolchain/runtime conformance matrix
     consistency/readiness and conformance-matrix-key anchors for fail-closed
     evidence continuity,
   - deterministic lane-D toolchain/runtime conformance corpus
     consistency/readiness and conformance-corpus-key anchors for fail-closed
     evidence continuity,
   - deterministic lane-D toolchain/runtime performance and quality guardrails
     consistency/readiness and performance-quality-key anchors for fail-closed
     evidence continuity,
   - enough stage-handoff state (`lex -> parse -> sema -> lower -> emit`) to
     fail closed when lowering/emit routing is inconsistent,
   - deterministic lane-E closeout dependency anchors for `M228-A001`, `M228-B001`,
     `M228-C002`, and `M228-D001`, including pending-lane tokens needed to keep
     replay-proof/performance gate evidence fail-closed before C002 asset seeding.
   - deterministic lane-E modular split/scaffolding closeout dependency anchors for `M228-E001`, `M228-A002`,
     `M228-B002`, `M228-C004`, and `M228-D002`, including pending-lane tokens needed to keep replay-proof/performance
     modular split/scaffolding gate evidence fail-closed before C004 asset seeding.
   - deterministic lane-E core-feature closeout dependency anchors for `M228-E002`, `M228-A003`,
     `M228-B003`, `M228-C003`, and `M228-D003`, including cross-lane core-feature readiness tokens
     needed to keep replay-proof/performance core-feature gate evidence fail-closed.
   - deterministic lane-E core-feature expansion dependency anchors for `M228-E003`, `M228-A003`,
     `M228-B004`, `M228-C003`, `M228-D003`, and pending token `M228-C008`,
     including cross-lane expansion-readiness continuity needed to keep
     replay-proof/performance core-feature expansion gate evidence fail-closed.
   - deterministic lane-E edge-case compatibility completion dependency anchors for
     `M228-E004`, `M228-A004`, `M228-B006`, `M228-C004`, `M228-D005`, and
     pending token `M228-C010`, including pending-token continuity needed to
     keep replay-proof/performance closeout evidence fail-closed.
   - deterministic lane-E E006 edge-case expansion/robustness dependency
     anchors for `M228-E005`, `M228-A006`, `M228-B006`, `M228-C006`, and
     `M228-D006`, including cross-lane robustness-readiness continuity needed
     to keep replay-proof/performance closeout evidence fail-closed.
   - deterministic lane-E diagnostics hardening dependency anchors for
     `M228-E006`, `M228-A007`, `M228-B007`, `M228-D007`, and pending token
     `M228-C007`, including pending-token continuity needed to keep
     replay-proof/performance diagnostics-hardening closeout evidence
     fail-closed while lane-C diagnostics assets are pending.
    - deterministic lane-E recovery and determinism hardening dependency anchors
      for `M228-E007`, `M228-A008`, `M228-B008`, `M228-D008`, and pending token
      `M228-C008`, including pending-token continuity needed to keep
      replay-proof/performance recovery/determinism closeout evidence fail-closed
      while lane-C recovery/determinism assets are pending.
    - deterministic lane-E conformance matrix implementation dependency anchors
      for `M228-E008`, `M228-A009`, `M228-B009`, `M228-C008`, `M228-D009`, and
      pending tokens `M228-A007`, `M228-B010`, `M228-C017`, `M228-D007`,
      including pending-token continuity needed to keep replay-proof/performance
      conformance-matrix closeout evidence fail-closed while issue dependency
      continuity tokens remain pending.
    - deterministic toolchain/runtime core-feature expansion readiness/key markers for
      backend marker-path and marker payload-to-route consistency evidence.
   - deterministic lane-C lowering/codegen cost profiling and controls metadata anchors for `M247-C001`
     with explicit dependency token (`none`) and fail-closed cost-profile evidence continuity so
     lane-C contract-freeze governance remains deterministic before modular split stages advance.
   - deterministic lane-C lowering/codegen modular split/scaffolding metadata anchors for `M247-C002`
     with explicit `M247-C001` dependency continuity and fail-closed modular split evidence continuity so
     lane-C modular split/scaffolding governance remains deterministic before core-feature implementation stages advance.
   - deterministic lane-C lowering/codegen core feature implementation metadata anchors for `M247-C003`
     with explicit `M247-C002` dependency continuity and fail-closed core feature evidence continuity so
     lane-C core feature implementation governance remains deterministic before core-feature expansion stages advance.
   - deterministic lane-C lowering/codegen core feature expansion metadata anchors for `M247-C004`
     with explicit `M247-C003` dependency continuity and fail-closed core feature expansion evidence continuity so
     lane-C core feature expansion governance remains deterministic before edge-case compatibility stages advance.
    - deterministic lane-C lowering/codegen edge-case and compatibility completion metadata anchors for `M247-C005`
      with explicit `M247-C004` dependency continuity and fail-closed edge-case compatibility evidence continuity so
      lane-C edge-case compatibility governance remains deterministic before robustness expansion stages advance.
    - deterministic lane-C lowering/codegen edge-case expansion and robustness metadata anchors for `M247-C006`
      with explicit `M247-C005` dependency continuity and fail-closed edge-case compatibility evidence continuity so
      lane-C edge-case robustness governance remains deterministic before diagnostics-hardening stages advance.
    - deterministic lane-C lowering/codegen diagnostics hardening metadata anchors for `M247-C007`
      with explicit `M247-C006` dependency continuity and fail-closed edge-case robustness evidence continuity so
      lane-C diagnostics-hardening governance remains deterministic before recovery/determinism stages advance.
     - deterministic lane-C lowering/codegen recovery and determinism hardening metadata anchors for `M247-C008`
       with explicit `M247-C007` dependency continuity and fail-closed diagnostics hardening evidence continuity so
       lane-C recovery/determinism governance remains deterministic before conformance matrix stages advance.
     - deterministic lane-C lowering/codegen conformance matrix implementation metadata anchors for `M247-C009`
       with explicit `M247-C008` dependency continuity and fail-closed recovery/determinism evidence continuity so
       lane-C conformance matrix governance remains deterministic before conformance corpus stages advance.
    - deterministic lane-E performance SLO dependency anchors for `M247-A001`, `M247-B001`,
      `M247-C001`, and `M247-D001`, including pending-lane tokens needed to keep
     compile/perf-budget governance evidence fail-closed before lane A-D contract
     assets are seeded.
     - deterministic lane-E performance SLO modular split dependency anchors for `M247-E001`,
       `M247-A002`, `M247-B002`, `M247-C002`, and `M247-D002`, including
       pending-lane tokens needed to keep modular split governance evidence
       fail-closed before lane A-D modular split assets are seeded.
     - deterministic lane-E performance SLO core feature implementation dependency anchors for `M247-E002`,
       `M247-A003`, `M247-B003`, `M247-C003`, and `M247-D002`, including
       pending-lane tokens needed to keep core feature implementation governance
       evidence fail-closed while lane A/B/C/D seeds remain pending.
     - deterministic lane-E performance SLO edge-case expansion/robustness metadata anchors for `M247-E006`
       with explicit `M247-E005`, `M247-A006`, `M247-B007`, `M247-C006`, and
       `M247-D005` dependency continuity so lane-E robustness contract-gating
       evidence remains fail-closed before diagnostics-hardening assets are seeded.
     - deterministic lane-E performance SLO diagnostics-hardening metadata anchors for `M247-E007`
       with explicit `M247-E006`, `M247-A007`, `M247-B007`, `M247-C007`, and
       `M247-D007` dependency continuity so lane-E diagnostics-hardening contract-gating
       evidence remains fail-closed before recovery/determinism assets are seeded.
     - deterministic lane-B semantic hot-path analysis/budgeting metadata anchors for `M247-B008`
       with explicit pending token `M247-B007` continuity so lane-B
       recovery/determinism contract-gating evidence remains fail-closed before
      diagnostics hardening assets are seeded.
    - deterministic lane-B semantic hot-path analysis/budgeting conformance matrix metadata anchors for `M247-B009`
      with explicit `M247-B008` dependency continuity so lane-B
      conformance matrix contract-gating evidence remains fail-closed before
      conformance corpus assets are seeded.
    - deterministic lane-B semantic hot-path analysis/budgeting conformance corpus metadata anchors for `M247-B010`
      with explicit `M247-B009` dependency continuity so lane-B
      conformance corpus contract-gating evidence remains fail-closed before
      performance-quality guardrail assets are seeded.
    - deterministic lane-B semantic hot-path analysis/budgeting cross-lane integration metadata anchors for `M247-B012`
      with explicit `M247-B011` dependency continuity so lane-B
      cross-lane integration contract-gating evidence remains fail-closed before
      docs/runbook synchronization assets are seeded.
    - deterministic lane-B semantic hot-path analysis/budgeting docs/operator runbook synchronization metadata anchors for `M247-B013`
      with explicit `M247-B012` dependency continuity so lane-B docs/runbook synchronization contract-gating evidence remains fail-closed.
    - deterministic lane-B semantic hot-path analysis/budgeting release-candidate/replay dry-run metadata anchors for `M247-B014`
      with explicit `M247-B013` dependency continuity so lane-B release-candidate/replay dry-run contract-gating evidence remains fail-closed.
    - deterministic lane-B semantic hot-path analysis/budgeting advanced core workpack (shard 1) metadata anchors for `M247-B015`
      with explicit `M247-B014` dependency continuity so lane-B advanced core workpack (shard 1) contract-gating evidence remains fail-closed.
    - deterministic lane-A frontend profiling and hot-path decomposition contract-freeze metadata anchors for `M247-A001`
      with explicit dependency token (`none`) so parser/AST profiling and hot-path decomposition evidence and compile-time budget continuity remain fail-closed before lane-A modular split stages advance.
    - deterministic lane-A frontend profiling and hot-path decomposition modular split metadata anchors for `M247-A002`
      with explicit `M247-A001` dependency continuity so profiling/hot-path modular split drift fails closed before lane-A core-feature implementation stages advance.
    - deterministic lane-A frontend profiling and hot-path decomposition
      edge-case expansion/robustness metadata anchors for `M247-A006` with
      explicit `M247-A005` dependency continuity so parser-boundary profiling
      contract-gating evidence remains fail-closed before lane-A A005 assets
      are seeded.
    - deterministic lane-A frontend profiling and hot-path decomposition
      diagnostics-hardening metadata anchors for `M247-A007` with explicit
      `M247-A006` dependency continuity so profiling diagnostics and
      compile-time budget contract-gating evidence remains fail-closed before
      lane-A A006 assets are seeded.
    - deterministic lane-A frontend profiling and hot-path decomposition
      recovery/determinism metadata anchors for `M247-A008` with explicit
      `M247-A007` dependency continuity so recovery replay determinism and
      compile-time budget contract-gating evidence remains fail-closed before
      lane-A A007 assets are seeded.
    - deterministic lane-A frontend profiling and hot-path decomposition
      conformance matrix metadata anchors for `M247-A009` with explicit
      `M247-A008` dependency continuity so conformance matrix
      contract-gating evidence remains fail-closed before conformance corpus
      assets are seeded.
    - deterministic lane-A frontend profiling and hot-path decomposition
      conformance corpus metadata anchors for `M247-A010` with explicit
      `M247-A009` dependency continuity so conformance corpus
      contract-gating evidence remains fail-closed before performance and
      quality guardrail assets are seeded.
    - deterministic lane-A frontend profiling and hot-path decomposition
      performance and quality guardrails metadata anchors for `M247-A011` with explicit
      `M247-A010` dependency continuity so performance and quality guardrails
      contract-gating evidence remains fail-closed before cross-lane integration assets are seeded.
    - deterministic lane-A frontend profiling and hot-path decomposition
      cross-lane integration sync metadata anchors for `M247-A012` with explicit
      `M247-A011` dependency continuity so cross-lane synchronization
      contract-gating evidence remains fail-closed while pending cross-lane
      dependency continuity (`M247-B012`, `M247-C012`, `M247-D012`, `M247-E012`)
      is tracked explicitly.
    - deterministic lane-A frontend profiling and hot-path decomposition
      docs and operator runbook synchronization metadata anchors for `M247-A013`
      with explicit `M247-A012` dependency continuity so docs/runbook
      synchronization contract-gating evidence remains fail-closed while pending
      cross-lane dependency continuity (`M247-B013`, `M247-C013`, `M247-D013`,
      `M247-E013`) is tracked explicitly.
    - deterministic lane-A frontend profiling and hot-path decomposition
      release-candidate/replay dry-run metadata anchors for `M247-A014`
      with explicit `M247-A013` dependency continuity so lane-A release-candidate/replay dry-run contract-gating evidence remains fail-closed.
    - deterministic lane-A frontend profiling and hot-path decomposition
      advanced edge compatibility workpack (shard 1) metadata anchors for `M247-A016`
      with explicit `M247-A015` dependency continuity so lane-A advanced edge compatibility workpack (shard 1) contract-gating evidence remains fail-closed.
    - deterministic lane-D runtime/link/build throughput optimization core
      feature expansion metadata anchors for `M247-D004` with explicit pending
      dependency token continuity for `M247-D003` so throughput
      contract-gating evidence remains fail-closed before lane-D core-feature
      implementation assets are seeded.
    - deterministic lane-D runtime/link/build throughput optimization edge-case and compatibility completion metadata anchors for `M247-D005`
      with explicit `M247-D004` dependency continuity and fail-closed compatibility evidence continuity so
      throughput edge-case governance remains fail-closed before lane-D compatibility closure is promoted.
    - deterministic lane-D runtime/link/build throughput optimization conformance matrix implementation metadata anchors for `M247-D009`
      with explicit `M247-D008` dependency continuity and fail-closed conformance matrix evidence continuity so
      throughput conformance matrix governance remains fail-closed before lane-D conformance-corpus assets are promoted.
   - deterministic lane-A interop surface syntax/declaration-form metadata anchors for `M244-A001`
      with explicit dependency tokens (`none`) and fail-closed evidence continuity
      so interop declaration-form metadata drift fails closed.
    - deterministic lane-A interop surface modular split metadata anchors for `M244-A002`
      with explicit `M244-A001` dependency continuity and fail-closed evidence
      so interop modular split scaffolding drift fails closed.
    - deterministic lane-A interop surface core-feature implementation metadata anchors for `M244-A003`
      with explicit `M244-A002` dependency continuity and fail-closed evidence
      so interop core-feature implementation drift fails closed.
    - deterministic lane-A interop surface core-feature expansion metadata anchors for `M244-A004`
      with explicit `M244-A003` dependency continuity and fail-closed evidence
      so interop core-feature expansion drift fails closed.
    - deterministic lane-A interop surface edge-case and compatibility completion metadata anchors for `M244-A005`
      with explicit `M244-A004` dependency continuity and fail-closed evidence
      so interop edge-case compatibility drift fails closed.
    - deterministic lane-A interop surface edge-case expansion and robustness metadata anchors for `M244-A006`
      with explicit `M244-A005` dependency continuity and fail-closed evidence
      so interop edge-case expansion drift fails closed.
    - deterministic lane-A interop surface diagnostics hardening metadata anchors for `M244-A007`
      with explicit `M244-A006` dependency continuity and fail-closed evidence
      so interop diagnostics hardening drift fails closed.
    - deterministic lane-A interop surface recovery and determinism hardening metadata anchors for `M244-A008`
      with explicit `M244-A007` dependency continuity and fail-closed evidence
      so interop recovery and determinism hardening drift fails closed.
    - deterministic lane-A interop surface conformance matrix implementation metadata anchors for `M244-A009`
      with explicit `M244-A008` dependency continuity and fail-closed evidence
      so interop conformance matrix drift fails closed.
    - deterministic lane-A interop surface conformance corpus expansion metadata anchors for `M244-A010`
      with explicit `M244-A009` dependency continuity and fail-closed evidence
      so interop conformance corpus drift fails closed.
    - deterministic lane-A interop surface performance and quality guardrails metadata anchors for `M244-A011`
      with explicit `M244-A010` dependency continuity and fail-closed evidence
      so interop performance/quality drift fails closed.
    - deterministic lane-A interop surface cross-lane integration sync metadata anchors for `M244-A012`
      with explicit `M244-A011`/`M244-B007`/`M244-C007`/`M244-D004`/`M244-E006` dependency continuity and fail-closed evidence
      so interop integration drift fails closed.
    - deterministic lane-A interop surface integration closeout and gate sign-off metadata anchors for `M244-A013`
      with explicit `M244-A012` dependency continuity and fail-closed evidence
      so interop integration closeout drift fails closed.
   - deterministic lane-C interop lowering/ABI conformance metadata anchors for `M244-C001`
     with explicit dependency tokens (`none`) and fail-closed evidence continuity
     so lowering/ABI conformance metadata drift fails closed.
    - deterministic lane-C interop lowering/ABI conformance modular split metadata anchors for `M244-C002`
      with explicit `M244-C001` dependency continuity and fail-closed evidence
      so lowering/ABI modular split scaffolding drift fails closed.
    - deterministic lane-C interop lowering/ABI conformance core-feature metadata anchors for `M244-C003`
      with explicit `M244-C002` dependency continuity and fail-closed evidence
      so lowering/ABI core-feature implementation drift fails closed.
    - deterministic lane-C interop lowering/ABI conformance core-feature expansion metadata anchors for `M244-C004`
      with explicit `M244-C003` dependency continuity and fail-closed evidence
      so lowering/ABI core-feature expansion drift fails closed.
    - deterministic lane-C interop lowering/ABI conformance edge-case and compatibility completion metadata anchors for `M244-C005`
      with explicit `M244-C004` dependency continuity and fail-closed evidence
      so lowering/ABI edge-case compatibility drift fails closed.
    - deterministic lane-C interop lowering/ABI conformance edge-case expansion and robustness metadata anchors for `M244-C006`
      with explicit `M244-C005` dependency continuity and fail-closed evidence
      so lowering/ABI edge-case expansion drift fails closed.
    - deterministic lane-C interop lowering/ABI conformance diagnostics hardening metadata anchors for `M244-C007`
      with explicit `M244-C006` dependency continuity and fail-closed evidence
      so lowering/ABI diagnostics hardening drift fails closed.
    - deterministic lane-C interop lowering/ABI conformance recovery and determinism hardening metadata anchors for `M244-C008`
      with explicit `M244-C007` dependency continuity and fail-closed evidence
      so lowering/ABI recovery and determinism hardening drift fails closed.
    - deterministic lane-C interop lowering/ABI conformance conformance matrix implementation metadata anchors for `M244-C009`
      with explicit `M244-C008` dependency continuity and fail-closed evidence
      so lowering/ABI conformance matrix implementation drift fails closed.
    - deterministic lane-C interop lowering/ABI conformance conformance corpus expansion metadata anchors for `M244-C010`
      with explicit `M244-C009` dependency continuity and fail-closed evidence
      so lowering/ABI conformance corpus expansion drift fails closed.
    - deterministic lane-C interop lowering/ABI conformance performance and quality guardrails metadata anchors for `M244-C011`
      with explicit `M244-C010` dependency continuity and fail-closed evidence
      so lowering/ABI performance and quality guardrail drift fails closed.
    - deterministic lane-C interop lowering/ABI conformance cross-lane integration sync metadata anchors for `M244-C012`
      with explicit `M244-C011` dependency continuity and fail-closed evidence
      so lowering/ABI cross-lane integration sync drift fails closed.
    - deterministic lane-C interop lowering/ABI conformance docs/operator runbook synchronization metadata anchors for `M244-C013`
      with explicit `M244-C012` dependency continuity and fail-closed evidence
      so lowering/ABI docs/runbook synchronization drift fails closed.
    - deterministic lane-C interop lowering/ABI conformance release-candidate/replay dry-run metadata anchors for `M244-C014`
      with explicit `M244-C013` dependency continuity and fail-closed evidence
      so lowering/ABI release-candidate/replay dry-run drift fails closed.
    - deterministic lane-D runtime/link bridge-path metadata anchors for `M244-D001`
      with explicit dependency tokens (`M244-A001`) and fail-closed evidence continuity
      so runtime/link bridge-path metadata drift fails closed.
    - deterministic lane-D runtime/link bridge-path modular split metadata anchors for `M244-D002`
      with explicit `M244-D001` dependency continuity and fail-closed evidence continuity
      so runtime/link bridge-path modular split metadata drift fails closed.
    - deterministic lane-D runtime/link bridge-path core feature implementation metadata anchors for `M244-D003`
      with explicit `M244-D002` dependency continuity and fail-closed evidence continuity
      so runtime/link bridge-path core feature implementation metadata drift fails closed.
    - deterministic lane-D runtime/link bridge-path core feature expansion metadata anchors for `M244-D004`
      with explicit `M244-D003` dependency continuity and fail-closed evidence continuity
      so runtime/link bridge-path core feature expansion metadata drift fails closed.
    - deterministic lane-D runtime/link bridge-path edge-case and compatibility completion metadata anchors for `M244-D005`
      with explicit `M244-D004` dependency continuity and fail-closed evidence continuity
      so runtime/link bridge-path edge-case and compatibility completion metadata drift fails closed.
    - deterministic lane-D runtime/link bridge-path edge-case expansion and robustness metadata anchors for `M244-D006`
      with explicit `M244-D005` dependency continuity and fail-closed evidence continuity
      so runtime/link bridge-path edge-case expansion and robustness metadata drift fails closed.
    - deterministic lane-D runtime/link bridge-path diagnostics hardening metadata anchors for `M244-D007`
      with explicit `M244-D006` dependency continuity and fail-closed evidence continuity
      so runtime/link bridge-path diagnostics hardening metadata drift fails closed.
    - deterministic lane-D runtime/link bridge-path recovery and determinism hardening metadata anchors for `M244-D008`
      with explicit `M244-D007` dependency continuity and fail-closed evidence continuity
      so runtime/link bridge-path recovery and determinism hardening metadata drift fails closed.
   - deterministic lane-B interop semantic/type mediation metadata anchors for `M244-B001`
     with semantic integration + typed handoff determinism evidence, explicit dependency tokens (`none`),
     and fail-closed evidence continuity so interop semantic/type mediation drift fails closed.
   - deterministic lane-B interop semantic/type mediation modular split metadata anchors for `M244-B002`
     with explicit `M244-B001` dependency continuity and fail-closed evidence
     so interop semantic/type mediation modular split scaffolding drift fails closed.
   - deterministic lane-B interop semantic/type mediation core-feature metadata anchors for `M244-B003`
     with explicit `M244-B002` dependency continuity and fail-closed evidence
     so interop semantic/type mediation core-feature implementation drift fails closed.
   - deterministic lane-B interop semantic/type mediation core-feature expansion metadata anchors for `M244-B004`
     with explicit `M244-B003` dependency continuity and fail-closed evidence
     so interop semantic/type mediation core-feature expansion drift fails closed.
   - deterministic lane-B interop semantic/type mediation edge-case and compatibility completion metadata anchors for `M244-B005`
     with explicit `M244-B004` dependency continuity and fail-closed evidence
     so interop semantic/type mediation edge-case and compatibility completion drift fails closed.
    - deterministic lane-B interop semantic/type mediation edge-case expansion and robustness metadata anchors for `M244-B006`
      with explicit `M244-B005` dependency continuity and fail-closed evidence
      so interop semantic/type mediation edge-case expansion and robustness drift fails closed.
    - deterministic lane-B interop semantic/type mediation cross-lane integration sync metadata anchors for `M244-B012`
      with explicit `M244-B011` dependency continuity and fail-closed evidence
      so interop semantic/type mediation cross-lane integration sync drift fails closed.
    - deterministic lane-B interop semantic/type mediation docs/operator runbook synchronization metadata anchors for `M244-B013`
      with explicit `M244-B012` dependency continuity and fail-closed evidence
      so interop semantic/type mediation docs/runbook synchronization drift fails closed.
    - deterministic lane-B interop semantic/type mediation advanced core workpack (shard 1) metadata anchors for `M244-B015`
      with explicit `M244-B014` dependency continuity and fail-closed evidence
      so interop semantic/type mediation advanced core workpack (shard 1) drift fails closed.
    - deterministic lane-E interop conformance gate and operations dependency anchors for
       `M244-A001`, `M244-B001`, `M244-C001`, and `M244-D001`, including dependency-reference tokens
      wired through `npm run --if-present` readiness hooks so governance evidence stays fail-closed
      on token/reference drift without requiring pending lane-B/C/D artifacts before they land.
    - deterministic lane-E interop conformance gate and operations modular split/scaffolding dependency anchors for
      `M244-E001`, `M244-A002`, `M244-B002`, `M244-C002`, and `M244-D002`, including dependency-reference tokens
      wired through `npm run --if-present` readiness hooks so governance evidence stays fail-closed
      on token/reference drift while staged lane-B/C/D modular split assets remain pending GH seed.
    - deterministic lane-E interop conformance gate and operations core-feature implementation dependency anchors for
      `M244-E002`, `M244-A002`, `M244-B003`, `M244-C004`, and `M244-D004`, including dependency-reference tokens
      wired through `npm run --if-present` readiness hooks so governance evidence stays fail-closed
      on token/reference drift while staged lane-B/C/D core-feature assets remain pending GH seed.
    - deterministic lane-E interop conformance gate and operations core-feature expansion dependency anchors for
      `M244-E003`, `M244-A003`, `M244-B004`, `M244-C005`, and `M244-D005`, including dependency-reference tokens
      wired through `npm run --if-present` readiness hooks so governance evidence stays fail-closed
      on token/reference drift while staged lane-B/C/D core-feature expansion assets remain pending GH seed.
    - deterministic lane-E interop conformance gate and operations edge-case and compatibility completion dependency anchors for
      `M244-E004`, `M244-A004`, `M244-B006`, `M244-C007`, and `M244-D006`, including dependency-reference tokens
      wired through `npm run --if-present` readiness hooks so governance evidence stays fail-closed
      on token/reference drift while staged lane-B/C/D edge-case and compatibility completion assets remain pending GH seed.
    - deterministic lane-E interop conformance gate and operations edge-case expansion and robustness dependency anchors for
      `M244-E005`, `M244-A005`, `M244-B007`, `M244-C008`, and `M244-D008`, including dependency-reference tokens
      wired through `npm run --if-present` readiness hooks so governance evidence stays fail-closed
      on token/reference drift while staged lane-B/C/D edge-case expansion and robustness assets remain pending GH seed.
    - deterministic lane-E interop conformance gate and operations diagnostics hardening dependency anchors for
      `M244-E006`, `M244-A005`, `M244-B008`, `M244-C009`, and `M244-D009`, including dependency-reference tokens
      wired through `npm run --if-present` readiness hooks so governance evidence stays fail-closed
      on token/reference drift while staged lane-B/C/D diagnostics hardening assets remain pending GH seed.
    - deterministic lane-E interop conformance gate and operations recovery and determinism hardening dependency anchors for
      `M244-E007`, `M244-A006`, `M244-B009`, `M244-C011`, and `M244-D010`, including dependency-reference tokens
      wired through `npm run --if-present` readiness hooks so governance evidence stays fail-closed
      on token/reference drift while staged lane-B/C/D recovery and determinism hardening assets remain pending GH seed.
    - deterministic lane-E interop conformance gate and operations conformance matrix implementation dependency anchors for
      `M244-E008`, `M244-A007`, `M244-B010`, `M244-C012`, and `M244-D012`, including dependency-reference tokens
      wired through `npm run --if-present` readiness hooks so governance evidence stays fail-closed
      on token/reference drift while staged lane-B/C/D conformance matrix implementation assets remain pending GH seed.
    - deterministic lane-A suite partitioning metadata anchors for `M248-A001`
      with fixture ownership boundary evidence and parser replay-budget continuity
      so CI sharding partition drift fails closed.
    - deterministic lane-A suite partitioning modular split metadata anchors for
      `M248-A002` with explicit `M248-A001` dependency continuity so fixture
      scaffolding drift fails closed.
    - deterministic lane-A suite partitioning and fixture ownership recovery and determinism hardening metadata anchors for `M248-A008`
      with explicit `M248-A007` dependency continuity and fail-closed recovery/determinism evidence continuity.
    - deterministic lane-A frontend behavior parity metadata anchors for `M245-A001`
      with toolchain portability evidence and parser replay-budget continuity
      so frontend portability drift fails closed.
    - deterministic lane-A property and ivar syntax surface completion metadata anchors for `M234-A001`
      with property/ivar semantics evidence and parser replay-budget continuity
      so synthesized accessor surface drift fails closed.
    - deterministic lane-A qualifier/generic grammar normalization metadata anchors for `M235-A001`
      with nullability/generics/qualifier semantics evidence and parser replay-budget continuity
      so qualifier and generic grammar surface drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference metadata anchors for `M235-B001`
      with nullability/generics/qualifier semantic-inference evidence and parser replay-budget continuity
      so qualifier and generic semantic-inference mediation drift fails closed.
    - deterministic lane-C qualified type lowering and ABI representation metadata anchors for `M235-C001`
      with qualified-type lowering and ABI representation evidence and lowering replay-budget continuity
      so qualified-type lowering and ABI representation drift fails closed.
    - deterministic lane-C qualified type lowering and ABI representation modular split/scaffolding metadata anchors for `M235-C002`
      with explicit `M235-C001` dependency continuity so modular split/scaffolding lowering and ABI drift fails closed.
    - deterministic lane-C qualified type lowering and ABI representation core feature implementation metadata anchors for `M235-C003`
      with explicit `M235-C002` dependency continuity so core feature implementation lowering and ABI drift fails closed.
    - deterministic lane-C qualified type lowering and ABI representation core feature expansion metadata anchors for `M235-C004`
      with explicit `M235-C003` dependency continuity so core feature expansion lowering and ABI drift fails closed.
    - deterministic lane-C qualified type lowering and ABI representation edge-case and compatibility completion metadata anchors for `M235-C005`
      with explicit `M235-C004` dependency continuity so edge-case and compatibility completion lowering and ABI drift fails closed.
    - deterministic lane-D interop behavior for qualified generic APIs metadata anchors for `M235-D001`
      with explicit `M235-C001` dependency continuity so interop contract and architecture drift fails closed.
    - deterministic lane-D interop behavior for qualified generic APIs modular split/scaffolding metadata anchors for `M235-D002`
      with explicit `M235-D001` dependency continuity so modular split/scaffolding interop drift fails closed.
    - deterministic lane-D interop behavior for qualified generic APIs core feature implementation metadata anchors for `M235-D003`
      with explicit `M235-D002` dependency continuity so core feature implementation interop drift fails closed.
    - deterministic lane-D interop behavior for qualified generic APIs core feature expansion metadata anchors for `M235-D004`
      with explicit `M235-D003` dependency continuity so core feature expansion interop drift fails closed.
    - deterministic lane-E qualifier/generic conformance gate metadata anchors for `M235-E001`
      with explicit `M235-A001`/`M235-B001`/`M235-C001` dependency continuity so lane-E freeze drift fails closed.
    - deterministic lane-E qualifier/generic conformance gate modular split/scaffolding metadata anchors for `M235-E002`
      with explicit `M235-E001`/`M235-A002`/`M235-B004`/`M235-C003`/`M235-D001` dependency continuity so lane-E modular split/scaffolding drift fails closed.
    - deterministic lane-E qualifier/generic conformance gate core feature implementation metadata anchors for `M235-E003`
      with explicit `M235-E002`/`M235-A003`/`M235-B006`/`M235-C004`/`M235-D002` dependency continuity so lane-E core feature implementation drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference modular split metadata anchors for `M235-B002`
      with explicit `M235-B001` dependency continuity so nullability/generics/qualifier semantic-inference scaffolding drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference core feature metadata anchors for `M235-B003`
      with explicit `M235-B002` dependency continuity so core feature implementation drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference core feature expansion metadata anchors for `M235-B004`
      with explicit `M235-B003` dependency continuity so core feature expansion drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference edge-case and compatibility completion metadata anchors for `M235-B005`
      with explicit `M235-B004` dependency continuity so edge-case and compatibility completion drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference edge-case expansion and robustness metadata anchors for `M235-B006`
      with explicit `M235-B005` dependency continuity so edge-case expansion and robustness drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference diagnostics hardening metadata anchors for `M235-B007`
      with explicit `M235-B006` dependency continuity so diagnostics hardening drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference recovery and determinism hardening metadata anchors for `M235-B008`
      with explicit `M235-B007` dependency continuity so recovery and determinism hardening drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference conformance matrix implementation metadata anchors for `M235-B009`
      with explicit `M235-B008` dependency continuity so conformance matrix implementation drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference conformance corpus expansion metadata anchors for `M235-B010`
      with explicit `M235-B009` dependency continuity so conformance corpus expansion drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference performance and quality guardrails metadata anchors for `M235-B011`
      with explicit `M235-B010` dependency continuity so performance and quality guardrails drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference cross-lane integration sync metadata anchors for `M235-B012`
      with explicit `M235-B011` dependency continuity so cross-lane integration sync drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference docs and operator runbook synchronization metadata anchors for `M235-B013`
      with explicit `M235-B012` dependency continuity so docs and operator runbook synchronization drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference release-candidate and replay dry-run metadata anchors for `M235-B014`
      with explicit `M235-B013` dependency continuity so release-candidate and replay dry-run drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference advanced core workpack (shard 1) metadata anchors for `M235-B015`
      with explicit `M235-B014` dependency continuity so advanced core workpack (shard 1) drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference advanced edge compatibility workpack (shard 1) metadata anchors for `M235-B016`
      with explicit `M235-B015` dependency continuity so advanced edge compatibility workpack (shard 1) drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference advanced diagnostics workpack (shard 1) metadata anchors for `M235-B017`
      with explicit `M235-B016` dependency continuity so advanced diagnostics workpack (shard 1) drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference advanced conformance workpack (shard 1) metadata anchors for `M235-B018`
      with explicit `M235-B017` dependency continuity so advanced conformance workpack (shard 1) drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference advanced integration workpack (shard 1) metadata anchors for `M235-B019`
      with explicit `M235-B018` dependency continuity so advanced integration workpack (shard 1) drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference advanced performance workpack (shard 1) metadata anchors for `M235-B020`
      with explicit `M235-B019` dependency continuity so advanced performance workpack (shard 1) drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference advanced core workpack (shard 2) metadata anchors for `M235-B021`
      with explicit `M235-B020` dependency continuity so advanced core workpack (shard 2) drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference advanced edge compatibility workpack (shard 2) metadata anchors for `M235-B022`
      with explicit `M235-B021` dependency continuity so advanced edge compatibility workpack (shard 2) drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference advanced diagnostics workpack (shard 2) metadata anchors for `M235-B023`
      with explicit `M235-B022` dependency continuity so advanced diagnostics workpack (shard 2) drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference advanced conformance workpack (shard 2) metadata anchors for `M235-B024`
      with explicit `M235-B023` dependency continuity so advanced conformance workpack (shard 2) drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference advanced integration workpack (shard 2) metadata anchors for `M235-B025`
      with explicit `M235-B024` dependency continuity so advanced integration workpack (shard 2) drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference advanced performance workpack (shard 2) metadata anchors for `M235-B026`
      with explicit `M235-B025` dependency continuity so advanced performance workpack (shard 2) drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference advanced core workpack (shard 3) metadata anchors for `M235-B027`
      with explicit `M235-B026` dependency continuity so advanced core workpack (shard 3) drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference advanced edge compatibility workpack (shard 3) metadata anchors for `M235-B028`
      with explicit `M235-B027` dependency continuity so advanced edge compatibility workpack (shard 3) drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference advanced diagnostics workpack (shard 3) metadata anchors for `M235-B029`
      with explicit `M235-B028` dependency continuity so advanced diagnostics workpack (shard 3) drift fails closed.
    - deterministic lane-B qualifier/generic semantic inference integration closeout and gate sign-off metadata anchors for `M235-B030`
      with explicit `M235-B029` dependency continuity so integration closeout and gate sign-off drift fails closed.
    - deterministic lane-A qualifier/generic grammar normalization modular split metadata anchors for `M235-A002`
      with explicit `M235-A001` dependency continuity so nullability/generics/qualifier scaffolding drift fails closed.
    - deterministic lane-A qualifier/generic grammar normalization core feature metadata anchors for `M235-A003`
      with explicit `M235-A002` dependency continuity so core feature implementation drift fails closed.
    - deterministic lane-A qualifier/generic grammar normalization core feature expansion metadata anchors for `M235-A004`
      with explicit `M235-A003` dependency continuity so core feature expansion drift fails closed.
    - deterministic lane-A qualifier/generic grammar normalization edge-case and compatibility completion metadata anchors for `M235-A005`
      with explicit `M235-A004` dependency continuity so edge-case and compatibility completion drift fails closed.
    - deterministic lane-A qualifier/generic grammar normalization edge-case expansion and robustness metadata anchors for `M235-A006`
      with explicit `M235-A005` dependency continuity so edge-case expansion and robustness drift fails closed.
    - deterministic lane-A qualifier/generic grammar normalization diagnostics hardening metadata anchors for `M235-A007`
      with explicit `M235-A006` dependency continuity so diagnostics hardening drift fails closed.
    - deterministic lane-A qualifier/generic grammar normalization recovery and determinism hardening metadata anchors for `M235-A008`
      with explicit `M235-A007` dependency continuity so recovery and determinism hardening drift fails closed.
    - deterministic lane-A qualifier/generic grammar normalization conformance matrix implementation metadata anchors for `M235-A009`
      with explicit `M235-A008` dependency continuity so conformance matrix implementation drift fails closed.
    - deterministic lane-A qualifier/generic grammar normalization conformance corpus expansion metadata anchors for `M235-A010`
      with explicit `M235-A009` dependency continuity so conformance corpus expansion drift fails closed.
    - deterministic lane-A qualifier/generic grammar normalization performance and quality guardrails metadata anchors for `M235-A011`
      with explicit `M235-A010` dependency continuity so performance and quality guardrails drift fails closed.
    - deterministic lane-A qualifier/generic grammar normalization cross-lane integration sync metadata anchors for `M235-A012`
      with explicit `M235-A011` dependency continuity so cross-lane integration sync drift fails closed.
    - deterministic lane-A qualifier/generic grammar normalization docs and operator runbook synchronization metadata anchors for `M235-A013`
      with explicit `M235-A012` dependency continuity so docs/runbook synchronization drift fails closed.
    - deterministic lane-A qualifier/generic grammar normalization release-candidate/replay dry-run metadata anchors for `M235-A014`
      with explicit `M235-A013` dependency continuity so lane-A release-candidate/replay dry-run contract-gating evidence remains fail-closed.
    - deterministic lane-A qualifier/generic grammar normalization advanced edge compatibility workpack (shard 1) metadata anchors for `M235-A016`
      with explicit `M235-A015` dependency continuity so lane-A advanced edge compatibility workpack (shard 1) contract-gating evidence remains fail-closed.
    - deterministic lane-A qualifier/generic grammar normalization integration closeout and gate sign-off metadata anchors for `M235-A017`
      with explicit `M235-A016` dependency continuity so lane-A integration closeout and gate sign-off contract-gating evidence remains fail-closed.
    - deterministic lane-C accessor and ivar lowering metadata anchors for `M234-C001`
      with property/ivar lowering evidence and lowering replay-budget continuity
      so accessor/ivar lowering surface drift fails closed.
    - deterministic lane-C accessor and ivar lowering modular split metadata anchors for `M234-C002`
      with explicit `M234-C001` dependency continuity so accessor/ivar lowering scaffolding drift fails closed.
    - deterministic lane-C accessor and ivar lowering core-feature metadata anchors for `M234-C003`
      with explicit `M234-C001` and `M234-C002` dependency continuity so accessor/ivar lowering
      core-feature implementation drift fails closed.
    - deterministic lane-C accessor and ivar lowering core-feature expansion metadata anchors for `M234-C004`
      with explicit `M234-C003` dependency continuity and fail-closed core-feature expansion evidence continuity
      so accessor/ivar lowering core-feature expansion drift fails closed.
    - deterministic lane-C accessor and ivar lowering edge-case and compatibility completion metadata anchors for `M234-C005`
      with explicit `M234-C004` dependency continuity so edge-case and compatibility completion drift fails closed.
    - deterministic lane-C accessor and ivar lowering edge-case expansion and robustness metadata anchors for `M234-C006`
      with explicit `M234-C005` dependency continuity so edge-case expansion and robustness drift fails closed.
    - deterministic lane-C accessor and ivar lowering diagnostics hardening metadata anchors for `M234-C007`
      with explicit `M234-C006` dependency continuity so diagnostics hardening drift fails closed.
    - deterministic lane-C accessor and ivar lowering recovery and determinism hardening metadata anchors for `M234-C008`
      with explicit `M234-C007` dependency continuity so recovery and determinism hardening drift fails closed.
    - deterministic lane-C accessor and ivar lowering conformance matrix implementation metadata anchors for `M234-C009`
      with explicit `M234-C008` dependency continuity so conformance matrix implementation drift fails closed.
    - deterministic lane-C accessor and ivar lowering conformance corpus expansion metadata anchors for `M234-C010`
      with explicit `M234-C009` dependency continuity so conformance corpus expansion drift fails closed.
    - deterministic lane-C accessor and ivar lowering performance and quality guardrails metadata anchors for `M234-C011`
      with explicit `M234-C010` dependency continuity so performance and quality guardrails drift fails closed.
    - deterministic lane-C accessor and ivar lowering cross-lane integration sync metadata anchors for `M234-C012`
      with explicit `M234-C011` dependency continuity so cross-lane integration sync drift fails closed.
    - deterministic lane-C accessor and ivar lowering docs and operator runbook synchronization metadata anchors for `M234-C013`
      with explicit `M234-C012` dependency continuity so docs and operator runbook synchronization drift fails closed.
    - deterministic lane-C accessor and ivar lowering release-candidate and replay dry-run metadata anchors for `M234-C014`
      with explicit `M234-C013` dependency continuity so release-candidate and replay dry-run drift fails closed.
    - deterministic lane-C accessor and ivar lowering advanced core workpack (shard 1) metadata anchors for `M234-C015`
      with explicit `M234-C014` dependency continuity so advanced core workpack (shard 1) drift fails closed.
    - deterministic lane-C accessor and ivar lowering advanced edge compatibility workpack (shard 1) metadata anchors for `M234-C016`
      with explicit `M234-C015` dependency continuity so advanced edge compatibility workpack (shard 1) drift fails closed.
    - deterministic lane-C accessor and ivar lowering integration closeout and gate sign-off metadata anchors for `M234-C017`
      with explicit `M234-C016` dependency continuity so integration closeout and gate sign-off drift fails closed.
    - deterministic lane-D runtime property metadata integration metadata anchors for `M234-D001`
      with runtime property metadata evidence and metadata replay-budget continuity
      so runtime property metadata integration drift fails closed.
    - deterministic lane-D runtime property metadata integration modular split metadata anchors for
      `M234-D002` with explicit `M234-D001` dependency continuity so runtime property metadata
      modular split/scaffolding drift fails closed.
    - deterministic lane-D runtime property metadata integration core feature metadata anchors for `M234-D003`
      with explicit `M234-D002` dependency continuity so core feature implementation drift fails closed.
    - deterministic lane-D runtime property metadata integration core feature expansion metadata anchors for `M234-D004`
      with explicit `M234-D003` dependency continuity so core feature expansion drift fails closed.
    - deterministic lane-A property and ivar syntax surface completion modular split metadata anchors for `M234-A002`
      with explicit `M234-A001` dependency continuity so property/ivar scaffolding drift fails closed.
    - deterministic lane-A property and ivar syntax surface completion core feature metadata anchors for `M234-A003`
      with explicit `M234-A002` dependency continuity so core feature implementation drift fails closed.
    - deterministic lane-A property and ivar syntax surface completion core feature metadata anchors for `M234-A004`
      with explicit `M234-A003` dependency continuity so core feature expansion drift fails closed.
    - deterministic lane-A property and ivar syntax surface completion edge-case and compatibility completion metadata anchors for `M234-A005`
      with explicit `M234-A004` dependency continuity so edge-case and compatibility completion drift fails closed.
    - deterministic lane-A property and ivar syntax surface completion edge-case expansion and robustness metadata anchors for `M234-A006`
      with explicit `M234-A005` dependency continuity so edge-case expansion and robustness drift fails closed.
    - deterministic lane-A property and ivar syntax surface completion diagnostics hardening metadata anchors for `M234-A007`
      with explicit `M234-A006` dependency continuity so diagnostics hardening drift fails closed.
    - deterministic lane-A property and ivar syntax surface completion recovery and determinism hardening metadata anchors for `M234-A008`
      with explicit `M234-A007` dependency continuity so recovery and determinism hardening drift fails closed.
    - deterministic lane-A property and ivar syntax surface completion conformance matrix implementation metadata anchors for `M234-A009`
      with explicit `M234-A008` dependency continuity so conformance matrix implementation drift fails closed.
    - deterministic lane-A property and ivar syntax surface completion conformance corpus expansion metadata anchors for `M234-A010`
      with explicit `M234-A009` dependency continuity so conformance corpus expansion drift fails closed.
    - deterministic lane-A property and ivar syntax surface completion performance and quality guardrails metadata anchors for `M234-A011`
      with explicit `M234-A010` dependency continuity so guardrail drift fails closed.
    - deterministic lane-A property and ivar syntax surface completion cross-lane integration sync metadata anchors for `M234-A012`
      with explicit `M234-A011` dependency continuity so cross-lane drift fails closed.
    - deterministic lane-A property and ivar syntax surface completion docs and operator runbook synchronization metadata anchors for `M234-A013`
      with explicit `M234-A012` dependency continuity so docs/runbook drift fails closed.
    - deterministic lane-A property and ivar syntax surface completion release-candidate/replay dry-run metadata anchors for `M234-A014`
      with explicit `M234-A013` dependency continuity so lane-A release-candidate/replay dry-run contract-gating evidence remains fail-closed.
    - deterministic lane-A property and ivar syntax surface completion advanced core workpack (shard 1) metadata anchors for `M234-A015`
      with explicit `M234-A014` dependency continuity so advanced-core-shard1 evidence remains fail-closed.
    - deterministic lane-A property and ivar syntax surface completion integration closeout and gate sign-off metadata anchors for `M234-A016`
      with explicit `M234-A015` dependency continuity so lane-A integration closeout and gate sign-off contract-gating evidence remains fail-closed.
    - deterministic lane-A frontend behavior parity modular split metadata anchors for `M245-A002`
      with explicit `M245-A001` dependency continuity so compatibility scaffolding drift fails closed.
    - deterministic lane-A frontend behavior parity core feature metadata anchors for `M245-A003`
      with explicit `M245-A002` dependency continuity so core feature implementation drift fails closed.
    - deterministic lane-A frontend behavior parity core feature metadata anchors for `M245-A004`
      with explicit `M245-A003` dependency continuity so core feature expansion drift fails closed.
    - deterministic lane-A frontend behavior parity edge-case and compatibility completion metadata anchors for `M245-A005`
      with explicit `M245-A004` dependency continuity so edge-case and compatibility completion drift fails closed.
    - deterministic lane-A frontend behavior parity edge-case expansion and robustness metadata anchors for `M245-A006`
      with explicit `M245-A005` dependency continuity so edge-case expansion and robustness drift fails closed.
    - deterministic lane-A frontend behavior parity diagnostics hardening metadata anchors for `M245-A007`
      with explicit `M245-A006` dependency continuity so diagnostics hardening drift fails closed.
    - deterministic lane-A frontend behavior parity recovery and determinism hardening metadata anchors for `M245-A008`
      with explicit `M245-A007` dependency continuity so recovery and determinism hardening drift fails closed.
    - deterministic lane-A frontend behavior parity conformance matrix implementation metadata anchors for `M245-A009`
      with explicit `M245-A008` dependency continuity so conformance matrix implementation drift fails closed.
    - deterministic lane-A frontend behavior parity conformance corpus expansion metadata anchors for `M245-A010`
      with explicit `M245-A009` dependency continuity so conformance corpus expansion drift fails closed.
    - deterministic lane-A frontend behavior parity integration closeout and gate sign-off metadata anchors for `M245-A011`
      with explicit `M245-A010` dependency continuity so integration closeout and gate sign-off drift fails closed.
    - deterministic lane-A feature packaging metadata anchors for `M249-A001`
      with release packaging compatibility evidence and parser replay-budget continuity
      so distribution surface drift fails closed.
   - deterministic lane-A feature packaging modular split metadata anchors for `M249-A002`
     with explicit `M249-A001` dependency continuity so compatibility scaffolding drift fails closed.
   - deterministic lane-A feature packaging core feature metadata anchors for `M249-A003`
     with explicit `M249-A002` dependency continuity so core feature implementation drift fails closed.
    - deterministic lane-B semantic/lowering metadata anchors for `M248-B001`
      with semantic fixture ownership evidence and lowering replay continuity so
      CI sharding semantic drift fails closed.
    - deterministic lane-B type-system completeness for ObjC3 forms metadata anchors for `M227-B001`
      with canonical reference/message/bridge-top form evidence and semantic-pass fail-closed continuity
      so canonical ObjC type-form drift fails closed.
    - deterministic lane-B type-system diagnostics hardening metadata anchors for `M227-B007`
      with canonical type-form diagnostics consistency/readiness and diagnostics-key continuity evidence,
      plus explicit `M227-B006` dependency continuity so diagnostics drift fails closed.
    - deterministic lane-B type-system recovery/determinism hardening metadata anchors for `M227-B008`
      with canonical type-form recovery consistency/readiness and recovery-key continuity evidence,
      plus explicit `M227-B007` dependency continuity so recovery drift fails closed.
    - deterministic lane-B type-system conformance matrix metadata anchors for `M227-B009`
      with canonical type-form conformance matrix consistency/readiness and conformance-matrix-key continuity evidence,
      plus explicit `M227-B008` dependency continuity so conformance matrix drift fails closed.
    - deterministic lane-B type-system conformance corpus metadata anchors for `M227-B010`
      with canonical type-form conformance corpus consistency/readiness, case-accounting continuity,
      and conformance-corpus-key continuity evidence, plus explicit `M227-B009` dependency continuity
      so conformance corpus drift fails closed.
    - deterministic lane-B type-system performance and quality guardrails metadata anchors for `M227-B011`
      with canonical type-form performance/quality guardrail accounting, consistency/readiness, and
      performance-quality-key continuity evidence, plus explicit `M227-B010` dependency continuity so
      guardrail drift fails closed.
    - deterministic lane-B type-system docs/operator runbook synchronization metadata anchors for `M227-B013`
      with lane-B operator command/evidence continuity in `docs/runbooks/m227_wave_execution_runbook.md`,
      plus explicit `M227-B012` dependency continuity so docs/runbook synchronization drift fails closed.
    - deterministic lane-B type-system release-candidate replay dry-run metadata anchors for `M227-B014`
      with lane-B release/replay command/evidence continuity in `docs/runbooks/m227_wave_execution_runbook.md`,
      plus explicit `M227-B013` dependency continuity so release-candidate/replay dry-run drift fails closed.
    - deterministic lane-B type-system advanced core workpack (shard 1) metadata anchors for `M227-B015`
      with lane-B advanced-core command/evidence continuity in `docs/runbooks/m227_wave_execution_runbook.md`,
      plus explicit `M227-B014` dependency continuity so advanced core workpack (shard 1) drift fails closed.
    - deterministic lane-B type-system advanced edge compatibility workpack (shard 1) metadata anchors for `M227-B016`
      with lane-B advanced-edge-compatibility command/evidence continuity in `docs/runbooks/m227_wave_execution_runbook.md`,
      plus explicit `M227-B015` dependency continuity so advanced edge compatibility workpack (shard 1) drift fails closed.
    - deterministic lane-B type-system advanced diagnostics workpack (shard 1) metadata anchors for `M227-B017`
      with lane-B advanced-diagnostics command/evidence continuity in `docs/runbooks/m227_wave_execution_runbook.md`,
      plus explicit `M227-B016` dependency continuity so advanced diagnostics workpack (shard 1) drift fails closed.
    - deterministic lane-B type-system advanced conformance workpack (shard 1) metadata anchors for `M227-B018`
      with lane-B advanced-conformance command/evidence continuity in `docs/runbooks/m227_wave_execution_runbook.md`,
      plus explicit `M227-B017` dependency continuity so advanced conformance workpack (shard 1) drift fails closed.
    - deterministic lane-B type-system advanced integration workpack (shard 1) metadata anchors for `M227-B019`
      with lane-B advanced-integration command/evidence continuity in `docs/runbooks/m227_wave_execution_runbook.md`,
      plus explicit `M227-B018` dependency continuity so advanced integration workpack (shard 1) drift fails closed.
    - deterministic lane-B type-system advanced performance workpack (shard 1) metadata anchors for `M227-B020`
      with lane-B advanced-performance command/evidence continuity in `docs/runbooks/m227_wave_execution_runbook.md`,
      plus explicit `M227-B019` dependency continuity so advanced performance workpack (shard 1) drift fails closed.
    - deterministic lane-B type-system advanced core workpack (shard 2) metadata anchors for `M227-B021`
      with lane-B advanced-core-shard2 command/evidence continuity in `docs/runbooks/m227_wave_execution_runbook.md`,
      plus explicit `M227-B020` dependency continuity so advanced core workpack (shard 2) drift fails closed.
    - deterministic lane-B type-system advanced edge compatibility workpack (shard 2) metadata anchors for `M227-B022`
      with lane-B advanced-edge-compatibility-shard2 command/evidence continuity in `docs/runbooks/m227_wave_execution_runbook.md`,
      plus explicit `M227-B021` dependency continuity so advanced edge compatibility workpack (shard 2) drift fails closed.
    - deterministic lane-B type-system advanced diagnostics workpack (shard 2) metadata anchors for `M227-B023`
      with lane-B advanced-diagnostics-shard2 command/evidence continuity in `docs/runbooks/m227_wave_execution_runbook.md`,
      plus explicit `M227-B022` dependency continuity so advanced diagnostics workpack (shard 2) drift fails closed.
    - deterministic lane-B type-system advanced conformance workpack (shard 2) metadata anchors for `M227-B024`
      with lane-B advanced-conformance-shard2 command/evidence continuity in `docs/runbooks/m227_wave_execution_runbook.md`,
      plus explicit `M227-B023` dependency continuity so advanced conformance workpack (shard 2) drift fails closed.
    - deterministic lane-B type-system advanced integration workpack (shard 2) metadata anchors for `M227-B025`
      with lane-B advanced-integration-shard2 command/evidence continuity in `docs/runbooks/m227_wave_execution_runbook.md`,
      plus explicit `M227-B024` dependency continuity so advanced integration workpack (shard 2) drift fails closed.
    - deterministic lane-B type-system advanced performance workpack (shard 2) metadata anchors for `M227-B026`
      with lane-B advanced-performance-shard2 command/evidence continuity in `docs/runbooks/m227_wave_execution_runbook.md`,
      plus explicit `M227-B025` dependency continuity so advanced performance workpack (shard 2) drift fails closed.
    - deterministic lane-B type-system advanced core workpack (shard 3) metadata anchors for `M227-B027`
      with lane-B advanced-core-shard3 command/evidence continuity in `docs/runbooks/m227_wave_execution_runbook.md`,
      plus explicit `M227-B026` dependency continuity so advanced core workpack (shard 3) drift fails closed.
    - deterministic lane-B type-system advanced edge compatibility workpack (shard 3) metadata anchors for `M227-B028`
      with lane-B advanced-edge-compatibility-shard3 command/evidence continuity in `docs/runbooks/m227_wave_execution_runbook.md`,
      plus explicit `M227-B027` dependency continuity so advanced edge compatibility workpack (shard 3) drift fails closed.
    - deterministic lane-B type-system advanced diagnostics workpack (shard 3) metadata anchors for `M227-B029`
      with lane-B advanced-diagnostics-shard3 command/evidence continuity in `docs/runbooks/m227_wave_execution_runbook.md`,
      plus explicit `M227-B028` dependency continuity so advanced diagnostics workpack (shard 3) drift fails closed.
    - deterministic lane-B type-system advanced conformance workpack (shard 3) metadata anchors for `M227-B030`
      with lane-B advanced-conformance-shard3 command/evidence continuity in `docs/runbooks/m227_wave_execution_runbook.md`,
      plus explicit `M227-B029` dependency continuity so advanced conformance workpack (shard 3) drift fails closed.
    - deterministic lane-B type-system advanced integration workpack (shard 3) metadata anchors for `M227-B031`
      with lane-B advanced-integration-shard3 command/evidence continuity in `docs/runbooks/m227_wave_execution_runbook.md`,
      plus explicit `M227-B030` dependency continuity so advanced integration workpack (shard 3) drift fails closed.
    - deterministic lane-B type-system advanced performance workpack (shard 3) metadata anchors for `M227-B032`
      with lane-B advanced-performance-shard3 command/evidence continuity in `docs/runbooks/m227_wave_execution_runbook.md`,
      plus explicit `M227-B031` dependency continuity so advanced performance workpack (shard 3) drift fails closed.
    - deterministic lane-B type-system advanced core workpack (shard 4) metadata anchors for `M227-B033`
      with lane-B advanced-core-shard4 command/evidence continuity in `docs/runbooks/m227_wave_execution_runbook.md`,
      plus explicit `M227-B032` dependency continuity so advanced core workpack (shard 4) drift fails closed.
    - deterministic lane-B type-system advanced edge compatibility workpack (shard 4) metadata anchors for `M227-B034`
      with lane-B advanced-edge-compatibility-shard4 command/evidence continuity in `docs/runbooks/m227_wave_execution_runbook.md`,
      plus explicit `M227-B033` dependency continuity so advanced edge compatibility workpack (shard 4) drift fails closed.
    - deterministic lane-B type-system advanced diagnostics workpack (shard 4) metadata anchors for `M227-B035`
      with lane-B advanced-diagnostics-shard4 command/evidence continuity in `docs/runbooks/m227_wave_execution_runbook.md`,
      plus explicit `M227-B034` dependency continuity so advanced diagnostics workpack (shard 4) drift fails closed.
    - deterministic lane-B type-system advanced conformance workpack (shard 4) metadata anchors for `M227-B036`
      with lane-B advanced-conformance-shard4 command/evidence continuity in `docs/runbooks/m227_wave_execution_runbook.md`,
      plus explicit `M227-B035` dependency continuity so advanced conformance workpack (shard 4) drift fails closed.
    - deterministic lane-B type-system advanced integration workpack (shard 4) metadata anchors for `M227-B037`
      with lane-B advanced-integration-shard4 command/evidence continuity in `docs/runbooks/m227_wave_execution_runbook.md`,
      plus explicit `M227-B036` dependency continuity so advanced integration workpack (shard 4) drift fails closed.
    - deterministic lane-B type-system advanced performance workpack (shard 4) metadata anchors for `M227-B038`
      with lane-B advanced-performance-shard4 command/evidence continuity in `docs/runbooks/m227_wave_execution_runbook.md`,
      plus explicit `M227-B037` dependency continuity so advanced performance workpack (shard 4) drift fails closed.
    - deterministic lane-B type-system integration closeout and gate sign-off metadata anchors for `M227-B039`
      with lane-B integration-closeout-and-gate-signoff command/evidence continuity in `docs/runbooks/m227_wave_execution_runbook.md`,
      plus explicit `M227-B038` dependency continuity so integration closeout and gate sign-off drift fails closed.
    - deterministic lane-C typed sema-to-lowering metadata anchors for `M227-C001`
      with typed sema handoff evidence and lowering metadata continuity so
      sema-to-lowering contract drift fails closed.
    - deterministic lane-C message-send lowering and call-emission metadata anchors for `M232-C001`
      with operator runbook/packet/checker continuity and fail-closed semantic-to-lowering
      contract evidence so message-send lowering contract drift fails closed.
    - deterministic lane-C message-send lowering and call-emission modular split/scaffolding metadata anchors for `M232-C002`
      with explicit `M232-C001` dependency continuity and fail-closed modular split/scaffolding
      evidence so message-send lowering modular split/scaffolding drift fails closed.
    - deterministic lane-C message-send lowering and call-emission core-feature metadata anchors for `M232-C003`
      with explicit `M232-C002` dependency continuity and fail-closed core-feature
      evidence so message-send lowering core-feature drift fails closed.
    - deterministic lane-C message-send lowering and call-emission core-feature expansion metadata anchors for `M232-C004`
      with explicit `M232-C003` dependency continuity and fail-closed core-feature expansion
      evidence so message-send lowering core-feature-expansion drift fails closed.
    - deterministic lane-C message-send lowering and call-emission edge-case/compatibility metadata anchors for `M232-C005`
      with explicit `M232-C004` dependency continuity and fail-closed edge-case/compatibility
      evidence so message-send lowering edge-case/compatibility drift fails closed.
    - deterministic lane-C message-send lowering and call-emission edge-case expansion/robustness metadata anchors for `M232-C006`
      with explicit `M232-C005` dependency continuity and fail-closed edge-case expansion/robustness
      evidence so message-send lowering edge-case expansion/robustness drift fails closed.
    - deterministic lane-C message-send lowering and call-emission diagnostics hardening metadata anchors for `M232-C007`
      with explicit `M232-C006` dependency continuity and fail-closed diagnostics-hardening
      evidence so message-send lowering diagnostics-hardening drift fails closed.
    - deterministic lane-C message-send lowering and call-emission recovery and determinism hardening metadata anchors for `M232-C008`
      with explicit `M232-C007` dependency continuity and fail-closed recovery and determinism hardening
      evidence so message-send lowering recovery and determinism hardening drift fails closed.
    - deterministic lane-C message-send lowering and call-emission conformance matrix implementation metadata anchors for `M232-C009`
      with explicit `M232-C008` dependency continuity and fail-closed conformance-matrix implementation
      evidence so message-send lowering conformance-matrix implementation drift fails closed.
    - deterministic lane-C message-send lowering and call-emission conformance corpus expansion metadata anchors for `M232-C010`
      with explicit `M232-C009` dependency continuity and fail-closed conformance-corpus expansion
      evidence so message-send lowering conformance-corpus expansion drift fails closed.
    - deterministic lane-C message-send lowering and call-emission performance and quality guardrails metadata anchors for `M232-C011`
      with explicit `M232-C010` dependency continuity and fail-closed performance and quality guardrails
      evidence so message-send lowering performance and quality guardrails drift fails closed.
    - deterministic lane-C message-send lowering and call-emission cross-lane integration sync metadata anchors for `M232-C012`
      with explicit `M232-C011` dependency continuity and fail-closed cross-lane integration sync
      evidence so message-send lowering cross-lane integration sync drift fails closed.
    - deterministic lane-C message-send lowering and call-emission docs and operator runbook synchronization metadata anchors for `M232-C013`
      with explicit `M232-C012` dependency continuity and fail-closed docs and operator runbook synchronization
      evidence so message-send lowering docs and operator runbook synchronization drift fails closed.
    - deterministic lane-C message-send lowering and call-emission release-candidate and replay dry-run metadata anchors for `M232-C014`
      with explicit `M232-C013` dependency continuity and fail-closed release-candidate and replay dry-run
      evidence so message-send lowering release-candidate and replay dry-run drift fails closed.
    - deterministic lane-C message-send lowering and call-emission advanced core workpack (shard 1) metadata anchors for `M232-C015`
      with explicit `M232-C014` dependency continuity and fail-closed advanced core workpack (shard 1)
      evidence so message-send lowering advanced core workpack (shard 1) drift fails closed.
    - deterministic lane-C message-send lowering and call-emission advanced edge compatibility workpack (shard 1) metadata anchors for `M232-C016`
      with explicit `M232-C015` dependency continuity and fail-closed advanced edge compatibility workpack (shard 1)
      evidence so message-send lowering advanced edge compatibility workpack (shard 1) drift fails closed.
    - deterministic lane-C message-send lowering and call-emission advanced diagnostics workpack (shard 1) metadata anchors for `M232-C017`
      with explicit `M232-C016` dependency continuity and fail-closed advanced diagnostics workpack (shard 1)
      evidence so message-send lowering advanced diagnostics workpack (shard 1) drift fails closed.
    - deterministic lane-C message-send lowering and call-emission advanced conformance workpack (shard 1) metadata anchors for `M232-C018`
      with explicit `M232-C017` dependency continuity and fail-closed advanced conformance workpack (shard 1)
      evidence so message-send lowering advanced conformance workpack (shard 1) drift fails closed.
    - deterministic lane-C typed sema-to-lowering modular split metadata anchors for `M227-C002`
      with explicit `M227-C001` dependency continuity so modular split handoff drift fails closed.
    - deterministic lane-C typed sema-to-lowering core feature metadata anchors for `M227-C003`
      with explicit `M227-C002` dependency continuity so typed core-feature handoff drift fails closed.
    - deterministic lane-A semantic-pass conformance matrix metadata anchors for `M227-A009`
      with parser/sema conformance-matrix evidence and corpus replay continuity
      so parser/sema conformance-matrix drift fails closed.
    - deterministic lane-A semantic-pass conformance corpus metadata anchors for `M227-A010`
      with parser/sema conformance-corpus replay evidence and fail-closed continuity
      so parser/sema conformance-corpus drift fails closed.
    - deterministic lane-A semantic-pass performance and quality guardrails metadata anchors for `M227-A011`
      with parser/sema performance-quality guardrails evidence and fail-closed continuity
      so parser/sema performance-quality drift fails closed.
    - deterministic lane-E semantic conformance quality-gate dependency anchors for `M227-A001`, `M227-B002`, `M227-C001`, and `M227-D001`
      with fail-closed readiness continuity (`check:objc3c:m227-a001-lane-a-readiness`, `check:objc3c:m227-b002-lane-b-readiness`, `check:objc3c:m227-c001-lane-c-readiness`, `check:objc3c:m227-d001-lane-d-readiness`)
      so semantic conformance lane-E quality-gate metadata governance drift fails closed.
    - deterministic lane-E semantic conformance modular split/scaffolding dependency anchors for `M227-E001`, `M227-A002`, `M227-B004`, `M227-C003`, and `M227-D002`
      with fail-closed readiness continuity (`check:objc3c:m227-e001-lane-e-quality-gate-readiness`, `check:objc3c:m227-a002-lane-a-readiness`, `check:objc3c:m227-b004-lane-b-readiness`, `check:objc3c:m227-c003-lane-c-readiness`, `check:objc3c:m227-d002-lane-d-readiness`)
      so semantic conformance lane-E modular split/scaffolding metadata governance drift fails closed.
    - deterministic lane-E semantic conformance edge-case expansion and robustness dependency anchors for `M227-E005`, `M227-A006`, `M227-B006`, `M227-C006`, and `M227-D006`
      with fail-closed readiness continuity (`check:objc3c:m227-e006-lane-e-readiness`)
      so semantic conformance lane-E edge-case expansion/robustness metadata governance drift fails closed.
    - deterministic lane-E semantic conformance diagnostics hardening dependency anchors for `M227-E006`, `M227-A007`, `M227-B007`, `M227-C007`, and `M227-D007`
      with fail-closed readiness continuity (`check:objc3c:m227-e007-lane-e-readiness`)
      so semantic conformance lane-E diagnostics hardening metadata governance drift fails closed.
    - deterministic lane-E semantic conformance recovery and determinism hardening dependency anchors for `M227-E007`, `M227-A008`, `M227-B008`, `M227-C008`, and `M227-D008`
      with fail-closed readiness continuity (`check:objc3c:m227-e008-lane-e-readiness`)
      so semantic conformance lane-E recovery and determinism hardening metadata governance drift fails closed.
    - deterministic lane-E semantic conformance matrix implementation dependency anchors for `M227-E008`, `M227-A009`, `M227-B018`, `M227-C012`, and `M227-D005`
      with fail-closed readiness continuity (`check:objc3c:m227-e009-lane-e-readiness`)
      so semantic conformance lane-E conformance matrix implementation metadata governance drift fails closed.
    - deterministic lane-E semantic conformance corpus expansion dependency anchors for `M227-E009`, `M227-A011`, `M227-B020`, `M227-C013`, and `M227-D006`
      with fail-closed readiness continuity (`check:objc3c:m227-e010-lane-e-readiness`)
      so semantic conformance lane-E conformance corpus expansion metadata governance drift fails closed.
    - deterministic lane-E semantic conformance performance and quality guardrails dependency anchors for `M227-E010`, `M227-A012`, `M227-B021`, `M227-C014`, and `M227-D007`
      with fail-closed readiness continuity (`check:objc3c:m227-e011-lane-e-readiness`)
      so semantic conformance lane-E performance and quality guardrails metadata governance drift fails closed.
    - deterministic lane-E semantic conformance cross-lane integration sync dependency anchors for `M227-E011`, `M227-A013`, `M227-B023`, `M227-C016`, and `M227-D007`
      with fail-closed readiness continuity (`check:objc3c:m227-e012-lane-e-readiness`)
      so semantic conformance lane-E cross-lane integration sync metadata governance drift fails closed.
    - deterministic lane-E semantic conformance docs and operator runbook synchronization dependency anchors for `M227-E012`, `M227-A014`, `M227-B025`, `M227-C017`, and `M227-D008`
      with fail-closed readiness continuity (`check:objc3c:m227-e013-lane-e-readiness`)
      so semantic conformance lane-E docs and operator runbook synchronization metadata governance drift fails closed.
    - deterministic lane-E semantic conformance release-candidate and replay dry-run dependency anchors for `M227-E013`, `M227-A015`, `M227-B027`, `M227-C018`, and `M227-D008`
      with fail-closed readiness continuity (`check:objc3c:m227-e014-lane-e-readiness`)
      so semantic conformance lane-E release-candidate and replay dry-run metadata governance drift fails closed.
    - deterministic lane-E semantic conformance advanced core workpack (shard 1) dependency anchors for `M227-E014`, `M227-A016`, `M227-B029`, `M227-C020`, and `M227-D009`
      with fail-closed readiness continuity (`check:objc3c:m227-e015-lane-e-readiness`)
      so semantic conformance lane-E advanced core workpack (shard 1) metadata governance drift fails closed.
    - deterministic lane-E semantic conformance advanced edge compatibility workpack (shard 1) dependency anchors for `M227-E015`, `M227-A017`, `M227-B031`, `M227-C021`, and `M227-D010`
      with fail-closed readiness continuity (`check:objc3c:m227-e016-lane-e-readiness`)
      so semantic conformance lane-E advanced edge compatibility workpack (shard 1) metadata governance drift fails closed.
    - deterministic lane-E semantic conformance advanced diagnostics workpack (shard 1) dependency anchors for `M227-E016`, `M227-A018`, `M227-B033`, `M227-C022`, and `M227-D010`
      with fail-closed readiness continuity (`check:objc3c:m227-e017-lane-e-readiness`)
      so semantic conformance lane-E advanced diagnostics workpack (shard 1) metadata governance drift fails closed.
    - deterministic lane-E semantic conformance advanced conformance workpack (shard 1) dependency anchors for `M227-E017`, `M227-A019`, `M227-B035`, `M227-C023`, and `M227-D011`
      with fail-closed readiness continuity (`check:objc3c:m227-e018-lane-e-readiness`)
      so semantic conformance lane-E advanced conformance workpack (shard 1) metadata governance drift fails closed.
    - deterministic lane-E semantic conformance advanced integration workpack (shard 1) dependency anchors for `M227-E018`, `M227-A020`, `M227-B037`, `M227-C025`, and `M227-D011`
      with fail-closed readiness continuity (`check:objc3c:m227-e019-lane-e-readiness`)
      so semantic conformance lane-E advanced integration workpack (shard 1) metadata governance drift fails closed.
    - deterministic lane-E semantic conformance integration closeout and gate sign-off dependency anchors for `M227-E019`, `M227-A021`, `M227-B039`, `M227-C026`, and `M227-D012`
      with fail-closed readiness continuity (`check:objc3c:m227-e020-lane-e-readiness`)
      so semantic conformance lane-E integration closeout and gate sign-off metadata governance drift fails closed.
    - deterministic lane-A semantic-pass cross-lane integration sync metadata anchors for `M227-A012`
      with lane dependency contract evidence (`M227-A011`, `M227-B007`, `M227-C002`, `M227-D001`, `M227-E001`)
      so semantic-pass cross-lane dependency drift fails closed.
    - deterministic lane-A semantic-pass docs/operator runbook synchronization metadata anchors for `M227-A013`
      with operator command-sequencing and dependency-anchor continuity evidence
      so semantic-pass runbook drift fails closed.
    - deterministic lane-A semantic-pass release-candidate replay dry-run metadata anchors for `M227-A014`
      with replay artifact evidence (`module.manifest.json`, `module.diagnostics.json`, `module.ll`, `module.object-backend.txt`)
      and fail-closed readiness continuity so semantic-pass release replay drift fails closed.
    - deterministic lane-A semantic-pass advanced core workpack (shard 1) metadata anchors for `M227-A015`
      with advanced-core consistency/readiness/key evidence
      (`toolchain_runtime_ga_operations_advanced_core_consistent`,
      `toolchain_runtime_ga_operations_advanced_core_ready`,
      `toolchain_runtime_ga_operations_advanced_core_key`) so advanced-core
      shard1 drift fails closed.
    - deterministic lane-A semantic-pass advanced edge compatibility workpack (shard 1) metadata anchors for `M227-A016` with edge-compatibility
      consistency/readiness/key evidence
      (`toolchain_runtime_ga_operations_advanced_edge_compatibility_consistent`,
      `toolchain_runtime_ga_operations_advanced_edge_compatibility_ready`,
      `toolchain_runtime_ga_operations_advanced_edge_compatibility_key`) so
      advanced edge-compatibility shard1 drift fails closed.
    - deterministic lane-A semantic-pass advanced diagnostics workpack (shard 1) metadata anchors for `M227-A017`
      with diagnostics consistency/readiness/key evidence
      (`toolchain_runtime_ga_operations_advanced_diagnostics_consistent`,
      `toolchain_runtime_ga_operations_advanced_diagnostics_ready`,
      `toolchain_runtime_ga_operations_advanced_diagnostics_key`) so advanced
      diagnostics shard1 drift fails closed.
    - deterministic lane-A semantic-pass advanced conformance workpack (shard 1) metadata anchors for `M227-A018`
      with conformance consistency/readiness/key evidence
      (`toolchain_runtime_ga_operations_advanced_conformance_consistent`,
      `toolchain_runtime_ga_operations_advanced_conformance_ready`,
      `toolchain_runtime_ga_operations_advanced_conformance_key`) so advanced
      conformance shard1 drift fails closed.
    - deterministic lane-A semantic-pass advanced integration workpack (shard 1) metadata anchors for `M227-A019`
      with integration consistency/readiness/key evidence
      (`toolchain_runtime_ga_operations_advanced_integration_consistent`,
      `toolchain_runtime_ga_operations_advanced_integration_ready`,
      `toolchain_runtime_ga_operations_advanced_integration_key`) so advanced
      integration shard1 drift fails closed.
    - deterministic lane-A semantic-pass advanced performance workpack (shard 1) metadata anchors for `M227-A020`
      with performance consistency/readiness/key evidence
      (`toolchain_runtime_ga_operations_advanced_performance_consistent`,
      `toolchain_runtime_ga_operations_advanced_performance_ready`,
      `toolchain_runtime_ga_operations_advanced_performance_key`) so advanced
      performance shard1 drift fails closed.
    - deterministic lane-A semantic-pass integration closeout and gate sign-off metadata anchors for `M227-A021`
      with closeout/sign-off consistency/readiness/key evidence
      (`toolchain_runtime_ga_operations_integration_closeout_signoff_consistent`,
      `toolchain_runtime_ga_operations_integration_closeout_signoff_ready`,
      `toolchain_runtime_ga_operations_integration_closeout_signoff_key`) so
      integration closeout/sign-off drift fails closed.
    - deterministic lane-B semantic/lowering modular split metadata anchors for
      `M248-B002` with explicit `M248-B001` dependency continuity so semantic
      scaffolding drift fails closed.
   - deterministic lane-B semantic parity/platform constraints core feature metadata anchors for
     `M245-B003` with explicit `M245-B002` dependency continuity so core feature implementation drift fails closed.
   - deterministic lane-B semantic parity/platform constraints core feature expansion metadata anchors for
     `M245-B004` with explicit `M245-B003` dependency continuity so core feature expansion drift fails closed.
   - deterministic lane-B semantic parity/platform constraints edge-case and compatibility completion metadata anchors for
     `M245-B005` with explicit `M245-B004` dependency continuity so edge-case and compatibility completion drift fails closed.
   - deterministic lane-B semantic parity/platform constraints edge-case expansion and robustness metadata anchors for
     `M245-B006` with explicit `M245-B005` dependency continuity so edge-case expansion and robustness drift fails closed.
   - deterministic lane-B semantic parity/platform constraints diagnostics hardening metadata anchors for
     `M245-B007` with explicit `M245-B006` dependency continuity so diagnostics hardening drift fails closed.
   - deterministic lane-B semantic parity/platform constraints recovery and determinism hardening metadata anchors for
     `M245-B008` with explicit `M245-B007` dependency continuity so recovery and determinism hardening drift fails closed.
   - deterministic lane-B semantic parity/platform constraints conformance matrix implementation metadata anchors for
     `M245-B009` with explicit `M245-B008` dependency continuity so conformance matrix drift fails closed.
   - deterministic lane-B semantic parity/platform constraints conformance corpus expansion metadata anchors for
     `M245-B010` with explicit `M245-B009` dependency continuity so conformance corpus drift fails closed.
    - deterministic lane-B semantic parity/platform constraints performance and quality guardrails metadata anchors for
      `M245-B011` with explicit `M245-B010` dependency continuity so performance/quality drift fails closed.
     - deterministic lane-B semantic parity/platform constraints cross-lane integration sync metadata anchors for
       `M245-B012` with explicit `M245-B011` dependency continuity so cross-lane integration sync drift fails closed.
     - deterministic lane-B semantic parity/platform constraints integration closeout and gate sign-off metadata anchors for
       `M245-B013` with explicit `M245-B012` dependency continuity so integration closeout and gate sign-off drift fails closed.
   - deterministic lane-B semantic compatibility/migration metadata anchors for `M249-B001`
     with sema pass-flow compatibility evidence and parse/lowering compatibility handoff continuity
     so migration drift fails closed.
   - deterministic lane-B semantic compatibility/migration modular split metadata anchors for
     `M249-B002` with explicit `M249-B001` dependency continuity so migration
     scaffolding drift fails closed.
    - deterministic lane-B semantic compatibility/migration core feature metadata anchors for `M249-B003` with explicit `M249-B002` dependency continuity so core feature implementation drift fails closed.
    - deterministic lane-C replay metadata anchors for `M248-C001` with artifact
      contract evidence and execution replay continuity so CI replay drift fails
      closed.
    - deterministic lane-C replay modular split metadata anchors for `M248-C002`
      with explicit `M248-C001` dependency continuity so modular split replay
      drift fails closed.
    - deterministic lane-C lowering/IR portability metadata anchors for `M245-C001`
      with lowering portability evidence and IR emission continuity so runtime
      portability drift fails closed.
    - deterministic lane-C lowering/IR portability modular split metadata anchors for `M245-C002`
      with explicit `M245-C001` dependency continuity so portability scaffolding drift fails closed.
    - deterministic lane-C lowering/IR portability core-feature metadata anchors for `M245-C003`
      with explicit `M245-C001` and `M245-C002` dependency continuity so portability
      core-feature drift fails closed.
    - deterministic lane-C lowering/IR portability core-feature expansion metadata anchors for `M245-C004`
      with explicit `M245-C003` dependency continuity and fail-closed core-feature expansion evidence continuity so
      portability expansion drift fails closed.
    - deterministic lane-C lowering/IR portability edge-case and compatibility completion metadata anchors for `M245-C005`
      with explicit `M245-C004` dependency continuity and fail-closed edge-case compatibility continuity so
      portability edge-case and compatibility completion drift fails closed.
    - deterministic lane-C lowering/IR portability edge-case expansion and robustness metadata anchors for `M245-C006`
      with explicit `M245-C005` dependency continuity and fail-closed edge-case robustness continuity so
      portability edge-case expansion and robustness drift fails closed.
    - deterministic lane-C lowering/IR portability diagnostics hardening metadata anchors for `M245-C007`
      with explicit `M245-C006` dependency continuity and fail-closed diagnostics hardening continuity so
      portability diagnostics hardening drift fails closed.
    - deterministic lane-C lowering/IR portability recovery and determinism hardening metadata anchors for `M245-C008`
      with explicit `M245-C007` dependency continuity and fail-closed recovery/determinism continuity so
      portability recovery and determinism hardening drift fails closed.
    - deterministic lane-C lowering/IR portability conformance matrix implementation metadata anchors for `M245-C009`
      with explicit `M245-C008` dependency continuity and fail-closed conformance-matrix continuity so
      portability conformance matrix implementation drift fails closed.
    - deterministic lane-C lowering/IR portability conformance corpus expansion metadata anchors for `M245-C010`
      with explicit `M245-C009` dependency continuity and fail-closed conformance-corpus continuity so
      portability conformance corpus expansion drift fails closed.
    - deterministic lane-C lowering/IR portability performance and quality guardrails metadata anchors for `M245-C011`
      with explicit `M245-C010` dependency continuity and fail-closed performance/quality continuity so
      portability performance and quality guardrail drift fails closed.
     - deterministic lane-C lowering/IR portability cross-lane integration sync metadata anchors for `M245-C012`
       with explicit `M245-C011` dependency continuity and fail-closed cross-lane continuity so
       portability cross-lane integration sync drift fails closed.
     - deterministic lane-C lowering/IR portability docs and operator runbook synchronization metadata anchors for `M245-C013`
       with explicit `M245-C012` dependency continuity and fail-closed docs/runbook continuity so
       portability docs and operator runbook synchronization drift fails closed.
     - deterministic lane-C lowering/IR portability release-candidate and replay dry-run metadata anchors for `M245-C014`
       with explicit `M245-C013` dependency continuity and fail-closed release/replay continuity so
       portability release-candidate and replay dry-run drift fails closed.
     - deterministic lane-C lowering/IR portability advanced core workpack (shard 1) metadata anchors for `M245-C015`
       with explicit `M245-C014` dependency continuity and fail-closed advanced-core continuity so
       portability advanced core workpack drift fails closed.
     - deterministic lane-C lowering/IR portability integration closeout and gate sign-off metadata anchors for `M245-C016`
       with explicit `M245-C015` dependency continuity and fail-closed integration-closeout continuity so
       portability integration closeout and gate sign-off drift fails closed.
    - deterministic lane-D build/link/runtime reproducibility modular split metadata anchors for `M245-D002`
      with explicit `M245-D001` dependency continuity so reproducibility scaffolding drift fails closed.
    - deterministic lane-D build/link/runtime reproducibility core feature metadata anchors for `M245-D003`
      with explicit `M245-D002` dependency continuity so core feature implementation drift fails closed.
    - deterministic lane-D build/link/runtime reproducibility core feature expansion metadata anchors for `M245-D004`
      with explicit `M245-D003` dependency continuity so core feature expansion drift fails closed.
    - deterministic lane-D build/link/runtime reproducibility edge-case and compatibility completion metadata anchors for `M245-D005`
      with explicit `M245-D004` dependency continuity so edge-case and compatibility completion drift fails closed.
    - deterministic lane-D build/link/runtime reproducibility edge-case expansion and robustness metadata anchors for `M245-D006`
      with explicit `M245-D005` dependency continuity so edge-case expansion and robustness drift fails closed.
    - deterministic lane-D build/link/runtime reproducibility diagnostics hardening metadata anchors for `M245-D007`
      with explicit `M245-D006` dependency continuity so diagnostics hardening drift fails closed.
    - deterministic lane-D build/link/runtime reproducibility recovery and determinism hardening metadata anchors for `M245-D008`
      with explicit `M245-D007` dependency continuity so recovery and determinism hardening drift fails closed.
    - deterministic lane-D build/link/runtime reproducibility conformance matrix implementation metadata anchors for `M245-D009`
      with explicit `M245-D008` dependency continuity so conformance matrix drift fails closed.
    - deterministic lane-D build/link/runtime reproducibility conformance corpus expansion metadata anchors for `M245-D010`
      with explicit `M245-D009` dependency continuity so conformance corpus drift fails closed.
    - deterministic lane-D build/link/runtime reproducibility performance and quality guardrails metadata anchors for `M245-D011`
      with explicit `M245-D010` dependency continuity so performance/quality drift fails closed.
     - deterministic lane-D build/link/runtime reproducibility cross-lane integration sync metadata anchors for `M245-D012`
       with explicit `M245-D011` dependency continuity so cross-lane integration sync drift fails closed.
     - deterministic lane-D build/link/runtime reproducibility docs and operator runbook synchronization metadata anchors for `M245-D013`
       with explicit `M245-D012` dependency continuity so docs/runbook synchronization drift fails closed.
     - deterministic lane-D build/link/runtime reproducibility release-candidate and replay dry-run metadata anchors for `M245-D014`
       with explicit `M245-D013` dependency continuity so release/replay drift fails closed.
     - deterministic lane-D build/link/runtime reproducibility advanced core workpack (shard 1) metadata anchors for `M245-D015`
       with explicit `M245-D014` dependency continuity so advanced-core drift fails closed.
     - deterministic lane-D build/link/runtime reproducibility advanced edge compatibility workpack (shard 1) metadata anchors for `M245-D016`
       with explicit `M245-D015` dependency continuity so advanced-edge drift fails closed.
     - deterministic lane-D build/link/runtime reproducibility advanced diagnostics workpack (shard 1) metadata anchors for `M245-D017`
       with explicit `M245-D016` dependency continuity so advanced-diagnostics drift fails closed.
     - deterministic lane-D build/link/runtime reproducibility advanced conformance workpack (shard 1) metadata anchors for `M245-D018`
       with explicit `M245-D017` dependency continuity so advanced-conformance drift fails closed.
     - deterministic lane-D build/link/runtime reproducibility advanced integration workpack (shard 1) metadata anchors for `M245-D019`
       with explicit `M245-D018` dependency continuity so advanced-integration drift fails closed.
     - deterministic lane-D build/link/runtime reproducibility advanced performance workpack (shard 1) metadata anchors for `M245-D020`
       with explicit `M245-D019` dependency continuity so advanced-performance drift fails closed.
    - deterministic lane-E portability gate/release checklist dependency anchors for
      `M245-A001`, `M245-B001`, `M245-C001`, and `M245-D001` so lane
      integration freeze evidence remains deterministic and fail-closed.
    - deterministic lane-E portability gate/release checklist modular split/scaffolding dependency anchors for
      `M245-E001`, `M245-A002`, `M245-B002`, `M245-C002`, and `M245-D002` so lane
      split continuity evidence remains deterministic and fail-closed.
    - deterministic lane-E portability gate/release checklist core feature implementation dependency anchors for
      `M245-E002`, `M245-A001`, `M245-B001`, `M245-C002`, and `M245-D002` so lane
      implementation continuity evidence remains deterministic and fail-closed.
    - deterministic lane-E portability gate/release checklist core feature expansion dependency anchors for
      `M245-E003`, `M245-A002`, `M245-B002`, `M245-C002`, and `M245-D003` so lane
      expansion continuity evidence remains deterministic and fail-closed.
    - deterministic lane-E portability gate/release checklist edge-case and compatibility completion dependency anchors for
      `M245-E004`, `M245-A002`, `M245-B002`, `M245-C003`, and `M245-D004` so lane
      edge-case and compatibility completion continuity evidence remains deterministic and fail-closed.
    - deterministic lane-E portability gate/release checklist edge-case expansion and robustness dependency anchors for
      `M245-E005`, `M245-A002`, `M245-B003`, `M245-C003`, and `M245-D004` so lane
      edge-case expansion and robustness continuity evidence remains deterministic and fail-closed.
    - deterministic lane-E portability gate/release checklist diagnostics hardening dependency anchors for
      `M245-E006`, `M245-A003`, `M245-B003`, `M245-C004`, and `M245-D005` so lane
      diagnostics hardening continuity evidence remains deterministic and fail-closed.
    - deterministic lane-E portability gate/release checklist recovery and determinism hardening dependency anchors for
      `M245-E007`, `M245-A003`, `M245-B004`, `M245-C004`, and `M245-D006` so lane
      recovery and determinism hardening continuity evidence remains deterministic and fail-closed.
    - deterministic lane-E portability gate/release checklist conformance matrix implementation dependency anchors for
      `M245-E008`, `M245-A003`, `M245-B004`, `M245-C005`, and `M245-D007` so lane
      conformance matrix continuity evidence remains deterministic and fail-closed.
    - deterministic lane-E portability gate/release checklist conformance corpus expansion dependency anchors for
      `M245-E009`, `M245-A004`, `M245-B004`, `M245-C006`, and `M245-D007` so lane
      conformance corpus continuity evidence remains deterministic and fail-closed.
    - deterministic lane-E portability gate/release checklist performance and quality guardrails dependency anchors for
      `M245-E010`, `M245-A004`, `M245-B005`, `M245-C006`, and `M245-D008` so lane
      performance/quality continuity evidence remains deterministic and fail-closed.
     - deterministic lane-E portability gate/release checklist cross-lane integration sync dependency anchors for
       `M245-E011`, `M245-A005`, `M245-B005`, `M245-C007`, and `M245-D009` so lane
       cross-lane integration sync continuity evidence remains deterministic and fail-closed.
     - deterministic lane-E portability gate/release checklist docs and operator runbook synchronization dependency anchors for
       `M245-E012`, `M245-A005`, `M245-B006`, `M245-C007`, and `M245-D009` so lane
       docs/runbook synchronization continuity evidence remains deterministic and fail-closed.
     - deterministic lane-E portability gate/release checklist release-candidate and replay dry-run dependency anchors for
       `M245-E013`, `M245-A005`, `M245-B006`, `M245-C008`, and `M245-D010` so lane
       release/replay continuity evidence remains deterministic and fail-closed.
     - deterministic lane-E portability gate/release checklist advanced core workpack (shard 1) dependency anchors for
       `M245-E014`, `M245-A006`, `M245-B007`, `M245-C008`, and `M245-D011` so lane
       advanced-core continuity evidence remains deterministic and fail-closed.
     - deterministic lane-E portability gate/release checklist advanced edge compatibility workpack (shard 1) dependency anchors for
       `M245-E015`, `M245-A006`, `M245-B007`, `M245-C009`, and `M245-D012` so lane
       advanced-edge continuity evidence remains deterministic and fail-closed.
     - deterministic lane-E portability gate/release checklist advanced diagnostics workpack (shard 1) dependency anchors for
       `M245-E016`, `M245-A006`, `M245-B008`, `M245-C009`, and `M245-D012` so lane
       advanced-diagnostics continuity evidence remains deterministic and fail-closed.
     - deterministic lane-E portability gate/release checklist advanced conformance workpack (shard 1) dependency anchors for
       `M245-E017`, `M245-A007`, `M245-B008`, `M245-C010`, and `M245-D013` so lane
       advanced-conformance continuity evidence remains deterministic and fail-closed.
     - deterministic lane-E portability gate/release checklist advanced integration workpack (shard 1) dependency anchors for
       `M245-E018`, `M245-A007`, `M245-B009`, `M245-C010`, and `M245-D014` so lane
       advanced-integration continuity evidence remains deterministic and fail-closed.
     - deterministic lane-E portability gate/release checklist advanced performance workpack (shard 1) dependency anchors for
       `M245-E019`, `M245-A008`, `M245-B009`, `M245-C011`, and `M245-D014` so lane
       advanced-performance continuity evidence remains deterministic and fail-closed.
    - deterministic lane-A frontend optimization hint metadata anchors for `M246-A001`
      with parser/AST hint-capture evidence and optimizer budget continuity so optimization
      hint-capture drift fails closed.
    - deterministic lane-A frontend optimization hint modular split metadata anchors for `M246-A002`
      with explicit `M246-A001` dependency continuity so hint-capture scaffolding drift fails closed.
    - deterministic lane-B semantic invariants for optimization legality metadata anchors for `M246-B001`
      with semantic legality evidence and optimizer replay-budget continuity so legality
      drift fails closed.
    - deterministic lane-B semantic invariants for optimization legality modular split metadata anchors for `M246-B002`
      with explicit `M246-B001` dependency continuity so modular split drift fails closed.
    - deterministic lane-C IR optimization pass wiring metadata anchors for `M246-C001`
      with IR pass-wiring evidence and optimizer replay-budget continuity so IR validation
      drift fails closed.
    - deterministic lane-C IR optimization pass wiring modular split metadata anchors for `M246-C002`
      with explicit `M246-C001` dependency continuity so modular split drift fails closed.
    - deterministic lane-D toolchain integration and optimization controls metadata anchors for `M246-D001`
      with toolchain-control evidence and optimizer replay-budget continuity so control
      drift fails closed.
    - deterministic lane-E optimization gate and perf evidence contract-freeze dependency anchors for
      `M246-A001`, `M246-B001`, `M246-C002`, and `M246-D001` so gate
      continuity evidence remains deterministic and fail-closed.
    - deterministic lane-E optimization gate and perf evidence modular split/scaffolding dependency anchors for
      `M246-E001`, `M246-A002`, `M246-B002`, `M246-C004`, and `M246-D002` so gate
      split continuity evidence remains deterministic and fail-closed.
    - deterministic lane-E optimization gate and perf evidence core feature implementation dependency anchors for
      `M246-E002`, `M246-A002`, `M246-B003`, `M246-C005`, and `M246-D002` so gate
      core-feature continuity evidence remains deterministic and fail-closed.
    - deterministic lane-E optimization gate and perf evidence core feature expansion dependency anchors for
      `M246-E003`, `M246-A003`, `M246-B004`, `M246-C007`, and `M246-D003` so gate
      expansion continuity evidence remains deterministic and fail-closed.
    - deterministic lane-E optimization gate and perf evidence edge-case and compatibility completion dependency anchors for
      `M246-E004`, `M246-A004`, `M246-B005`, `M246-C009`, and `M246-D004` so gate
      compatibility continuity evidence remains deterministic and fail-closed.
    - deterministic lane-E optimization gate and perf evidence edge-case expansion and robustness dependency anchors for
      `M246-E005`, `M246-A005`, `M246-B006`, `M246-C011`, and `M246-D005` so gate
      robustness continuity evidence remains deterministic and fail-closed.
    - deterministic lane-C IR/object packaging metadata anchors for `M249-C001`
      with symbol policy evidence and object package continuity so CI artifact
     packaging drift fails closed.
   - deterministic lane-C IR/object packaging modular split metadata anchors for `M249-C002`
     with explicit `M249-C001` dependency continuity so symbol-policy scaffolding drift fails closed.
   - deterministic lane-C IR/object packaging core-feature metadata anchors for `M249-C003`
     with explicit `M249-C001` and `M249-C002` dependency continuity so symbol-policy
     core-feature drift fails closed.
   - deterministic lane-D installer/runtime operations metadata anchors for `M249-D001`
     with runtime-route evidence and support-tooling continuity so installer/runtime
     drift fails closed.
   - deterministic lane-D installer/runtime operations modular split metadata anchors for
     `M249-D002` with explicit `M249-D001` dependency continuity so support-tooling
     scaffolding drift fails closed.
   - deterministic lane-D installer/runtime operations core feature metadata anchors for `M249-D003`
     with explicit `M249-D002` dependency continuity so core feature implementation drift fails closed.
   - deterministic lane-D installer/runtime operations core feature expansion metadata anchors for `M249-D004`
     with explicit `M249-D003` dependency continuity so core feature expansion drift fails closed.
- deterministic lane-D runtime metadata and lookup plumbing metadata anchors for `M233-D001`
     with runtime-route evidence and lookup-plumbing continuity so installer/runtime
     drift fails closed.
   - deterministic lane-D runtime metadata and lookup plumbing modular split metadata anchors for
     `M233-D002` with explicit `M233-D001` dependency continuity so lookup-plumbing
     scaffolding drift fails closed.
   - deterministic lane-D runtime metadata and lookup plumbing core feature metadata anchors for `M233-D003`
     with explicit `M233-D002` dependency continuity so core feature implementation drift fails closed.
   - deterministic lane-D runtime metadata and lookup plumbing core feature expansion metadata anchors for `M233-D004`
     with explicit `M233-D003` dependency continuity so core feature expansion drift fails closed.
   - deterministic lane-D runtime metadata and lookup plumbing release-candidate replay dry-run metadata anchors for `M233-D014`
     with explicit `M233-D013` dependency continuity and fail-closed replay dry-run evidence continuity.
   - deterministic lane-D runtime metadata and lookup plumbing advanced core workpack (shard 1) metadata anchors for `M233-D015`
     with explicit `M233-D014` dependency continuity and fail-closed advanced core evidence continuity.
   - deterministic lane-D runtime metadata and lookup plumbing advanced edge compatibility workpack (shard 1) metadata anchors for `M233-D016`
     with explicit `M233-D015` dependency continuity and fail-closed advanced edge compatibility evidence continuity.
   - deterministic lane-D runtime metadata and lookup plumbing advanced diagnostics workpack (shard 1) metadata anchors for `M233-D017`
     with explicit `M233-D016` dependency continuity and fail-closed advanced diagnostics evidence continuity.
   - deterministic lane-D runtime metadata and lookup plumbing advanced conformance workpack (shard 1) metadata anchors for `M233-D018`
     with explicit `M233-D017` dependency continuity and fail-closed advanced conformance evidence continuity.
   - deterministic lane-D runtime metadata and lookup plumbing advanced integration workpack (shard 1) metadata anchors for `M233-D019`
     with explicit `M233-D018` dependency continuity and fail-closed advanced integration evidence continuity.
   - deterministic lane-D runtime metadata and lookup plumbing advanced performance workpack (shard 1) metadata anchors for `M233-D020`
     with explicit `M233-D019` dependency continuity and fail-closed advanced performance workpack (shard 1)/sign-off evidence continuity.
   - deterministic lane-D runtime metadata and lookup plumbing advanced edge compatibility workpack (shard 2) metadata anchors for `M233-D022`
     with explicit `M233-D021` dependency continuity and fail-closed advanced edge compatibility evidence continuity.
   - deterministic lane-D runtime metadata and lookup plumbing advanced diagnostics workpack (shard 2) metadata anchors for `M233-D023`
     with explicit `M233-D022` dependency continuity and fail-closed advanced diagnostics evidence continuity.
   - deterministic lane-D runtime metadata and lookup plumbing advanced conformance workpack (shard 2) metadata anchors for `M233-D024`
     with explicit `M233-D023` dependency continuity and fail-closed advanced conformance evidence continuity.
   - deterministic lane-D runtime metadata and lookup plumbing advanced integration workpack (shard 2) metadata anchors for `M233-D025`
     with explicit `M233-D024` dependency continuity and fail-closed advanced integration evidence continuity.
   - deterministic lane-D runtime metadata and lookup plumbing advanced performance workpack (shard 2) metadata anchors for `M233-D026`
     with explicit `M233-D025` dependency continuity and fail-closed advanced performance evidence continuity.
   - deterministic lane-D runtime metadata and lookup plumbing advanced core workpack (shard 3) metadata anchors for `M233-D027`
     with explicit `M233-D026` dependency continuity and fail-closed advanced core evidence continuity.
   - deterministic lane-D runtime metadata and lookup plumbing integration closeout and gate sign-off metadata anchors for `M233-D028`
     with explicit `M233-D027` dependency continuity and fail-closed integration closeout and gate sign-off evidence continuity.
   - deterministic lane-E conformance corpus and gate closeout dependency anchors for
     `M233-A001`, `M233-B001`, `M233-C001`, and `M233-D002` so lane-E metadata governance drift fails closed.
   - deterministic lane-E conformance corpus and gate closeout modular split/scaffolding dependency anchors for
     `M233-E001`, `M233-A001`, `M233-B002`, `M233-C003`, and `M233-D003` so lane-E modular split/scaffolding metadata governance drift fails closed.
   - deterministic lane-E conformance corpus and gate closeout core feature implementation dependency anchors for
     `M233-E002`, `M233-A002`, `M233-B003`, `M233-C004`, and `M233-D005` so lane-E core-feature metadata governance drift fails closed.
   - deterministic lane-E conformance corpus and gate closeout core feature expansion dependency anchors for
     `M233-E003`, `M233-A003`, `M233-B004`, `M233-C005`, and `M233-D007` so lane-E core-feature expansion metadata governance drift fails closed.
   - deterministic lane-D installer/runtime operations and support tooling release-candidate replay dry-run metadata anchors for `M249-D014`
     with explicit `M249-D013` dependency continuity and fail-closed replay dry-run evidence continuity.
   - deterministic lane-D installer/runtime operations and support tooling advanced core workpack (shard 1) metadata anchors for `M249-D015`
     with explicit `M249-D014` dependency continuity and fail-closed advanced core evidence continuity.
   - deterministic lane-D installer/runtime operations and support tooling advanced edge compatibility workpack (shard 1) metadata anchors for `M249-D016`
     with explicit `M249-D015` dependency continuity and fail-closed advanced edge compatibility evidence continuity.
   - deterministic lane-D installer/runtime operations and support tooling advanced diagnostics workpack (shard 1) metadata anchors for `M249-D017`
     with explicit `M249-D016` dependency continuity and fail-closed advanced diagnostics evidence continuity.
   - deterministic lane-D installer/runtime operations and support tooling advanced conformance workpack (shard 1) metadata anchors for `M249-D018`
     with explicit `M249-D017` dependency continuity and fail-closed advanced conformance evidence continuity.
   - deterministic lane-D installer/runtime operations and support tooling advanced integration workpack (shard 1) metadata anchors for `M249-D019`
     with explicit `M249-D018` dependency continuity and fail-closed advanced integration evidence continuity.
   - deterministic lane-D installer/runtime operations and support tooling integration closeout and gate sign-off metadata anchors for `M249-D020`
     with explicit `M249-D019` dependency continuity and fail-closed integration closeout/sign-off evidence continuity.
   - deterministic lane-D CLI/reporting output metadata anchors for `M243-D001`
     with diagnostics artifact and summary payload continuity so diagnostics UX
     and fix-it engine output contract drift fails closed.
   - deterministic lane-D CLI/reporting output modular split scaffold metadata anchors for `M243-D002`
     with explicit `M243-D001` dependency continuity so diagnostics UX and
     fix-it engine output contract scaffolding drift fails closed.
   - deterministic lane-D CLI/reporting output core feature metadata anchors for `M243-D003`
     with explicit `M243-D002` dependency continuity so core feature implementation drift fails closed.
  - deterministic lane-D CLI/reporting output core feature expansion metadata anchors for `M243-D004`
    with explicit `M243-D003` dependency continuity so core feature expansion drift fails closed.
  - deterministic lane-D CLI/reporting output edge-case compatibility completion metadata anchors for `M243-D005`
    with explicit `M243-D004` dependency continuity so edge-case compatibility drift fails closed.
  - deterministic lane-D CLI/reporting output edge-case expansion and robustness metadata anchors for `M243-D006`
    with explicit `M243-D005` dependency continuity so edge-case expansion drift fails closed.
  - deterministic lane-D CLI/reporting output recovery and determinism hardening metadata anchors for `M243-D008`
    with explicit `M243-D007` dependency continuity so recovery and determinism drift fails closed.
  - deterministic lane-D CLI/reporting output conformance matrix implementation metadata anchors for `M243-D009`
    with explicit `M243-D008` dependency continuity so conformance matrix drift fails closed.
  - deterministic lane-D CLI/reporting output conformance corpus expansion metadata anchors for `M243-D010`
    with explicit `M243-D009` dependency continuity so conformance corpus drift fails closed.
  - deterministic lane-D CLI/reporting output performance and quality guardrails metadata anchors for `M243-D011`
    with explicit `M243-D010` dependency continuity so performance/quality guardrail drift fails closed.
  - deterministic lane-D CLI/reporting output cross-lane integration sync metadata anchors for `M243-D012`
    with explicit `M243-D011` dependency continuity so cross-lane integration sync drift fails closed.
  - deterministic lane-D CLI/reporting output docs/operator runbook synchronization metadata anchors for `M243-D013`
    with explicit `M243-D012` dependency continuity so docs/runbook synchronization drift fails closed.
  - deterministic lane-C lowering/runtime diagnostics surfacing modular split
    metadata anchors for `M243-C002` with explicit `M243-C001` dependency
    continuity so diagnostics surfacing scaffold drift fails closed.
    - deterministic lane-C lowering/runtime diagnostics surfacing core feature
      metadata anchors for `M243-C003` with explicit `M243-C002` dependency
      continuity so core feature implementation drift fails closed.
    - deterministic lane-C lowering/runtime diagnostics surfacing core feature
      expansion metadata anchors for `M243-C004` with explicit `M243-C003`
      dependency continuity so core feature expansion drift fails closed.
    - deterministic lane-C lowering/runtime diagnostics surfacing edge-case
      compatibility completion metadata anchors for `M243-C005` with explicit
      `M243-C004` dependency continuity so edge-case compatibility drift fails
      closed.
    - deterministic lane-C lowering/runtime diagnostics surfacing edge-case
      expansion and robustness metadata anchors for `M243-C006` with explicit
      `M243-C005` dependency continuity so edge-case expansion drift fails
      closed.
    - deterministic lane-C lowering/runtime diagnostics surfacing diagnostics hardening metadata anchors for `M243-C007` with explicit `M243-C006`
      dependency continuity so diagnostics hardening drift fails closed.
    - deterministic lane-C lowering/runtime diagnostics surfacing recovery and determinism hardening metadata anchors for `M243-C008` with explicit
      `M243-C007` dependency continuity so recovery and determinism drift fails
      closed.
    - deterministic lane-C lowering/runtime diagnostics surfacing conformance matrix implementation metadata anchors for `M243-C009` with explicit
      `M243-C008` dependency continuity so conformance matrix drift fails
      closed.
    - deterministic lane-C lowering/runtime diagnostics surfacing conformance corpus expansion metadata anchors for `M243-C010` with explicit
      `M243-C009` dependency continuity so conformance corpus drift fails
      closed.
    - deterministic lane-C lowering/runtime diagnostics surfacing performance and quality guardrails metadata anchors for `M243-C011` with explicit
      `M243-C010` dependency continuity so performance/quality guardrail drift
      fails closed.
    - deterministic lane-C lowering/runtime diagnostics surfacing cross-lane integration sync metadata anchors for `M243-C012` with explicit
      `M243-C011` dependency continuity so cross-lane integration sync drift
      fails closed.
   - deterministic lane-A diagnostic grammar hooks/source precision recovery
     and determinism hardening metadata anchors for `M243-A008` with explicit
     `M243-A007` dependency continuity so parser diagnostic replay hardening
     drift fails closed.
   - deterministic lane-A diagnostic grammar hooks/source precision performance and quality guardrails metadata anchors for `M243-A011` with explicit `M243-A010` dependency continuity
     so parser diagnostic grammar-hook readiness-chain and evidence metadata
     drift fail closed.
   - deterministic lane-A diagnostic grammar hooks/source precision integration closeout and gate sign-off metadata anchors for `M243-A012` with explicit `M243-A011` dependency continuity
     so parser diagnostic grammar-hook closeout-sign-off metadata drift fails
     closed.
    - deterministic lane-B semantic diagnostic taxonomy/fix-it synthesis core
      feature metadata anchors for `M243-B003` with explicit `M243-B002`
      dependency continuity so core feature implementation drift fails closed.
   - deterministic lane-B semantic diagnostic taxonomy/fix-it synthesis core
     feature expansion metadata anchors for `M243-B004` with explicit `M243-B003`
     dependency continuity so core feature expansion drift fails closed.
   - deterministic lane-B semantic diagnostic taxonomy/fix-it synthesis
     edge-case compatibility completion metadata anchors for `M243-B005` with
     explicit `M243-B004` dependency continuity so edge-case compatibility
     completion drift fails closed.
   - deterministic lane-B semantic diagnostic taxonomy/fix-it synthesis
     edge-case expansion and robustness metadata anchors for `M243-B006` with
     explicit `M243-B005` dependency continuity so edge-case expansion
     robustness drift fails closed.
   - deterministic lane-B semantic diagnostic taxonomy/fix-it synthesis
     diagnostics hardening metadata anchors for `M243-B007` with
     explicit `M243-B006` dependency continuity so diagnostics hardening
     drift fails closed.
   - deterministic lane-B semantic diagnostic taxonomy/fix-it synthesis
     recovery and determinism hardening metadata anchors for `M243-B008` with
     explicit `M243-B007` dependency continuity so recovery determinism
     drift fails closed.
   - deterministic lane-B semantic diagnostic taxonomy/fix-it synthesis
     conformance corpus expansion metadata anchors for `M243-B010` with
     explicit `M243-B009` dependency continuity so conformance corpus
     drift fails closed.
   - deterministic lane-B semantic diagnostic taxonomy/fix-it synthesis
     performance and quality guardrails metadata anchors for `M243-B011` with
     explicit `M243-B010` dependency continuity so performance/quality guardrails
     drift fails closed.
   - deterministic lane-B semantic diagnostic taxonomy/fix-it synthesis
     cross-lane integration sync metadata anchors for `M243-B012` with
     explicit `M243-B011` dependency continuity so cross-lane integration sync
     drift fails closed.
   - deterministic lane-B semantic diagnostic taxonomy/fix-it synthesis
     docs/operator runbook synchronization metadata anchors for `M243-B013` with
     explicit `M243-B012` dependency continuity so docs/runbook synchronization
     drift fails closed.
   - deterministic lane-B semantic diagnostic taxonomy/fix-it synthesis integration closeout and gate sign-off metadata anchors for `M243-B014` with explicit `M243-B013` dependency continuity
     so integration closeout/gate-sign-off readiness metadata drift fails closed.
   - deterministic lane-E release gate/docs/runbooks dependency anchors for
     `M249-A001`, `M249-B001`, `M249-C001`, and `M249-D001`, including
      pending-lane tokens needed to keep release-gate documentation/runbook
      governance evidence fail-closed before lane A-D contract assets are seeded.
   - deterministic lane-E release gate/docs/runbooks modular split/scaffolding dependency anchors for
     `M249-E001`, `M249-A002`, `M249-B002`, `M249-C002`, and `M249-D002`,
     including pending-lane tokens needed to keep release-gate documentation/runbook modular split/scaffolding
     governance evidence fail-closed before lane A-D modular split/scaffolding assets are seeded.
   - deterministic lane-E release gate/docs/runbooks core feature implementation dependency anchors for
     `M249-E002`, `M249-A003`, `M249-B003`, `M249-C003`, and `M249-D003`,
     including pending-lane tokens needed to keep release-gate documentation/runbook core feature implementation
     governance evidence fail-closed before lane A-D core feature assets are seeded.
   - deterministic lane-E release gate/docs/runbooks advanced core workpack (shard 1) dependency anchors for
     `M249-E014`, `M249-A006`, `M249-B007`, `M249-C008`, and `M249-D015`,
     including advanced-core continuity tokens needed to keep release-gate
     documentation/runbook advanced-core governance evidence fail-closed while
     shard-1 dependency readiness remains staged.
   - deterministic lane-E release gate/docs/runbooks advanced edge compatibility workpack (shard 1) metadata anchors for `M249-E016`
     with explicit `M249-E015` dependency continuity and fail-closed advanced edge compatibility evidence continuity.
  - deterministic lane-E release gate/docs/runbooks advanced diagnostics workpack (shard 1) metadata anchors for `M249-E017`
    with explicit `M249-E016`, `M249-A007`, `M249-B008`, `M249-C009`, and `M249-D017` dependency continuity
    and fail-closed advanced diagnostics evidence continuity.
  - deterministic lane-E release gate/docs/runbooks advanced conformance workpack (shard 1) metadata anchors for `M249-E018`
    with explicit `M249-E017`, `M249-A007`, `M249-B008`, `M249-C009`, and `M249-D015` dependency continuity
    and fail-closed advanced conformance evidence continuity.
  - deterministic lane-E release gate/docs/runbooks advanced integration workpack (shard 1) metadata anchors for `M249-E019`
    with explicit `M249-E018`, `M249-A007`, `M249-B009`, `M249-C010`, and `M249-D016` dependency continuity
    and fail-closed advanced integration evidence continuity.
  - deterministic lane-E release gate/docs/runbooks advanced performance workpack (shard 1) metadata anchors for `M249-E020`
    with explicit `M249-E019`, `M249-A008`, `M249-B009`, `M249-C010`, and `M249-D017` dependency continuity
    and fail-closed advanced performance evidence continuity.
  - deterministic lane-E release gate/docs/runbooks advanced core workpack (shard 2) metadata anchors for `M249-E021`
    with explicit `M249-E020`, `M249-A008`, `M249-B010`, `M249-C011`, and `M249-D018` dependency continuity
    and fail-closed advanced core (shard 2) evidence continuity.
  - deterministic lane-E release gate/docs/runbooks advanced edge compatibility workpack (shard 2) metadata anchors for `M249-E022`
    with explicit `M249-E021`, `M249-A008`, `M249-B010`, `M249-C011`, and `M249-D018` dependency continuity
    and fail-closed advanced edge compatibility (shard 2) evidence continuity.
  - deterministic lane-E release gate/docs/runbooks advanced diagnostics workpack (shard 2) metadata anchors for `M249-E023`
    with explicit `M249-E022`, `M249-A009`, `M249-B011`, `M249-C012`, and `M249-D019` dependency continuity
    and fail-closed advanced diagnostics (shard 2) evidence continuity.
  - deterministic lane-E release gate/docs/runbooks integration closeout and gate signoff metadata anchors for `M249-E024`
    with explicit `M249-E023`, `M249-A009`, `M249-B011`, `M249-C012`, and `M249-D020` dependency continuity
    and fail-closed integration closeout/gate-signoff evidence continuity.
  - deterministic lane-E diagnostics quality gate and replay policy dependency anchors for
    `M243-A001`, `M243-B001`, `M243-C001`, and `M243-D001`, including
    pending-lane tokens needed to keep diagnostics quality gate/replay-policy
    governance evidence fail-closed before lane C-D contract-freeze assets are seeded.
   - deterministic lane-E diagnostics quality gate and replay policy modular split/scaffolding dependency anchors for
     `M243-E001`, `M243-A001`, `M243-B001`, `M243-C001`, and `M243-D001`, including
     pending-lane tokens needed to keep diagnostics quality gate/replay-policy modular split/scaffolding
     governance evidence fail-closed while dependency tokens remain pending GH seed.
   - deterministic lane-E diagnostics quality gate and replay policy core feature implementation dependency anchors for
     `M243-E002`, `M243-A003`, `M243-B003`, `M243-C002`, and `M243-D002`, including
     mixed-lane maturity tokens needed to keep diagnostics quality gate/replay-policy core feature
     implementation governance evidence fail-closed while lane readiness remains staged.
   - deterministic lane-E diagnostics quality gate and replay policy core feature expansion dependency anchors for
     `M243-E003`, `M243-A004`, `M243-B004`, `M243-C003`, and `M243-D003`, including
     cross-lane expansion maturity tokens needed to keep diagnostics quality gate/replay-policy core feature
     expansion governance evidence fail-closed while lane readiness remains staged.
   - deterministic lane-E diagnostics quality gate and replay policy edge-case and compatibility completion dependency anchors for
     `M243-E004`, `M243-A005`, `M243-B005`, `M243-C005`, and `M243-D005`, including
     cross-lane compatibility maturity tokens needed to keep diagnostics quality gate/replay-policy edge-case and
     compatibility completion governance evidence fail-closed while lane readiness remains staged.
   - deterministic lane-E diagnostics quality gate and replay policy edge-case expansion and robustness dependency anchors for
     `M243-E005`, `M243-A002`, `M243-B003`, `M243-C003`, and `M243-D004`, including
     mixed-lane dependency maturity tokens needed to keep diagnostics quality gate/replay-policy edge-case expansion and
     robustness governance evidence fail-closed while lane readiness remains staged.
   - deterministic lane-E diagnostics quality gate and replay policy diagnostics hardening dependency anchors for
     `M243-E006`, `M243-A003`, `M243-B003`, `M243-C004`, and `M243-D005`, including
     mixed-lane dependency maturity tokens needed to keep diagnostics quality gate/replay-policy diagnostics hardening
     governance evidence fail-closed while lane readiness remains staged.
   - deterministic lane-E diagnostics quality gate and replay policy recovery and determinism hardening dependency anchors for
     `M243-E007`, `M243-A003`, `M243-B004`, `M243-C004`, and `M243-D006`, including
     mixed-lane dependency maturity tokens needed to keep diagnostics quality gate/replay-policy recovery and determinism
     hardening governance evidence fail-closed while lane readiness remains staged.
  - deterministic lane-E diagnostics quality gate and replay policy conformance matrix implementation dependency anchors for
    `M243-E008`, `M243-A003`, `M243-B004`, `M243-C005`, and `M243-D006`, including
    mixed-lane dependency maturity tokens needed to keep diagnostics quality gate/replay-policy conformance matrix
    implementation governance evidence fail-closed while lane readiness remains staged.
  - deterministic lane-E diagnostics quality gate and replay policy conformance corpus expansion dependency anchors for
    `M243-E009`, `M243-A004`, `M243-B005`, `M243-C005`, and `M243-D007`, including
    mixed-lane dependency maturity tokens needed to keep diagnostics quality gate/replay-policy conformance corpus
    expansion governance evidence fail-closed while lane readiness remains staged.
  - deterministic lane-E diagnostics quality gate and replay policy performance and quality guardrails dependency anchors for
    `M243-E010`, `M243-A004`, `M243-B005`, `M243-C006`, and `M243-D008`, including
    mixed-lane dependency maturity tokens needed to keep diagnostics quality gate/replay-policy performance and quality
    guardrails governance evidence fail-closed while lane readiness remains staged.
  - deterministic lane-E diagnostics quality gate and replay policy cross-lane integration sync dependency anchors for
    `M243-E011`, `M243-A012`, `M243-B012`, `M243-C011`, and `M243-D012`, including
    mixed-lane dependency maturity tokens needed to keep diagnostics quality gate/replay-policy cross-lane integration
    sync governance evidence fail-closed while lane readiness remains staged.
  - deterministic lane-D runner operations metadata anchors for `M248-D001`
    with compile-route evidence and perf-budget continuity so platform
    operation drift fails closed.
  - deterministic lane-D runner modular split metadata anchors for `M248-D002`
     with explicit `M248-D001` dependency continuity so platform scaffolding
     drift fails closed.
  - deterministic lane-D runner core feature metadata anchors for `M248-D003`
    with explicit `M248-D002` dependency continuity so nullable-tool-path core feature drift fails closed.
  - deterministic lane-D runner/platform operations core feature expansion metadata anchors for `M248-D004`
    with explicit `M248-D003` dependency continuity and fail-closed expansion evidence continuity.
  - deterministic lane-D runner/platform operations edge-case and compatibility completion metadata anchors for `M248-D005`
    with explicit `M248-D004` dependency continuity and fail-closed compatibility evidence continuity.
  - deterministic lane-D runner/platform operations edge-case expansion and robustness metadata anchors for `M248-D006`
    with explicit `M248-D005` dependency continuity and fail-closed robustness evidence continuity.
  - deterministic lane-D runner/platform operations diagnostics hardening metadata anchors for `M248-D007`
    with explicit `M248-D006` dependency continuity and fail-closed diagnostics evidence continuity.
  - deterministic lane-D runner/platform operations recovery and determinism hardening metadata anchors for `M248-D008`
    with explicit `M248-D007` dependency continuity and fail-closed recovery/determinism evidence continuity.
  - deterministic lane-D runner/platform operations conformance matrix implementation metadata anchors for `M248-D009`
    with explicit `M248-D008` dependency continuity and fail-closed conformance matrix evidence continuity.
  - deterministic lane-D runner/platform operations conformance corpus expansion metadata anchors for `M248-D010`
    with explicit `M248-D009` dependency continuity and fail-closed conformance corpus evidence continuity.
  - deterministic lane-D runner/platform operations performance and quality guardrails metadata anchors for `M248-D011`
    with explicit `M248-D010` dependency continuity and fail-closed performance and quality evidence continuity.
  - deterministic lane-D runner/platform operations cross-lane integration sync metadata anchors for `M248-D012`
    with explicit `M248-D011` dependency continuity and fail-closed cross-lane integration evidence continuity.
  - deterministic lane-D runner/platform operations docs/operator runbook synchronization metadata anchors for `M248-D013`
    with explicit `M248-D012` dependency continuity and fail-closed docs/runbook synchronization evidence continuity.
  - deterministic lane-D runner/platform operations release-candidate replay dry-run metadata anchors for `M248-D014`
    with explicit `M248-D013` dependency continuity and fail-closed replay dry-run evidence continuity.
  - deterministic lane-D runner/platform operations advanced core workpack (shard 1) metadata anchors for `M248-D015`
    with explicit `M248-D014` dependency continuity and fail-closed advanced core evidence continuity.
  - deterministic lane-D runner/platform operations advanced edge compatibility workpack (shard 1) metadata anchors for `M248-D016`
    with explicit `M248-D015` dependency continuity and fail-closed advanced edge compatibility evidence continuity.
  - deterministic lane-D runner/platform operations advanced diagnostics workpack (shard 1) metadata anchors for `M248-D017`
    with explicit `M248-D016` dependency continuity and fail-closed advanced diagnostics evidence continuity.

### D.2.1 Metadata encoding/version header (normative) {#d-2-1}

Each module metadata payload shall carry a version header with:

- `schema_major` (non-negative integer),
- `schema_minor` (non-negative integer),
- a `required_capabilities` set naming non-ignorable semantic extensions used by the payload.

In addition, each encoded metadata field shall be classified as one of:

- **required semantic field**: importer must understand and validate it to preserve [Table A](#d-3-1) semantics,
- **ignorable extension field**: importer may ignore it without changing required language semantics.

Unknown required semantic fields (or unknown `required_capabilities`) shall be treated as incompatibilities.
Unknown ignorable extension fields are forward-compatible and may be skipped.

### D.2.2 Versioning rules (normative) {#d-2-2}

`schema_major` shall be incremented for any incompatible encoding or semantic interpretation change, including:

- changing the meaning of an existing required field,
- deleting a required field without an equivalent replacement,
- reinterpreting a required field such that an older importer would miscompile accepted code.

`schema_minor` shall be incremented for compatible additive changes, including:

- adding new ignorable extension fields,
- adding new required capabilities that can be detected and rejected by older importers.

Within one `schema_major` line, a change is compatible only if an importer that does not implement the new minor revision will either:

- ignore only ignorable extension fields, or
- reject import deterministically before semantic use because a required field/capability is unknown.

Within one `schema_major` line:

- changing a required semantic field to ignorable, or changing semantic defaults for omitted required metadata, is incompatible and requires `schema_major` increment,
- changing an ignorable extension field to required semantic is allowed only with a new required capability and `schema_minor` increment so older importers hard-error instead of miscompiling,
- editorial-only/spec-clarification updates with no encoding or semantic change do not require a schema version change.

### D.2.3 Importer validation outcomes (normative) {#d-2-3}

Importer validation distinguishes:

- **hard error**: import is rejected for that module/interface unit,
- **recoverable diagnostic**: import may continue with conservative assumptions, as allowed by profile rules in [Table E](#d-3-5).

A conforming importer shall not silently drop missing, unknown, or mismatched required semantic metadata.

Importer validation shall, at minimum:

1. validate `schema_major` support (mismatch is a hard error),
2. validate `required_capabilities` and required-field recognition (unknown required data is a hard error),
3. classify absent/invalid known-required semantic metadata as a missing-metadata condition and diagnose per [Table E](#d-3-5),
4. accept unknown ignorable extension fields without changing required [Table A](#d-3-1) semantics.

### D.2.4 Portable concurrency interface metadata (OCI-1) {#d-2-4}

To support cross-toolchain interchange beyond native module files, implementations shall support OCI-1 metadata export/import for concurrency-relevant declarations.

OCI-1 requirements:

- schema header: `oci_schema_major`, `oci_schema_minor`,
- declaration identity key stable across emit/import within a release line,
- required concurrency fields listed in [Table F](#d-3-6).

OCI-1 versioning and compatibility follow the same rules as [D.2.2](#d-2-2) and [D.3.4](#d-3-4).
Unknown required OCI-1 fields/capabilities are hard errors.

## D.3 Required metadata tables {#d-3}

### D.3.1 Table A — “must preserve across module boundaries” {#d-3-1}

| Feature                                                         | Applies to                         | Must be recorded in module metadata | Must be emitted in textual interface | Notes                                                                                                  |
| --------------------------------------------------------------- | ---------------------------------- | ----------------------------------: | -----------------------------------: | ------------------------------------------------------------------------------------------------------ |
| Nullability qualifiers (`_Nullable`, `_Nonnull`)                | types, params, returns, properties |                                  ✅ |                                   ✅ | Includes inferred defaults where part of the public contract.                                          |
| Nonnull-by-default regions                                      | headers / interface units          |                                  ✅ |                                   ✅ | Canonical pragmas from [B.2](#b-2).                                                                    |
| Pragmatic generics parameters                                   | class/protocol types, collections  |                                  ✅ |                                   ✅ | ABI may erase; type checking must not.                                                                 |
| `T?`, `T!` (optional spellings)                                 | type surface                       |                                  ✅ |                                   ✅ | Emitted form may choose canonical spellings but semantics must match.                                  |
| `throws` effect                                                 | functions/methods/blocks           |                                  ✅ |                                   ✅ | ABI-affecting (see [Table B](#d-3-2)).                                                                 |
| `async` effect                                                  | functions/methods/blocks           |                                  ✅ |                                   ✅ | ABI-affecting (see [Table B](#d-3-2)).                                                                 |
| Executor affinity `objc_executor(...)`                          | funcs/methods/types                |                                  ✅ |                                   ✅ | May imply call-site `await` when crossing executors ([Part 7](#part-7)).                               |
| Actor type / actor isolation                                    | actor classes + members            |                                  ✅ |                                   ✅ | Includes nonisolated markings.                                                                         |
| Sendable-like constraints                                       | types, captures, params/returns    |                                  ✅ |                                   ✅ | Importers must be able to enforce strict checks.                                                       |
| Borrowed pointer qualifier (`borrowed T *`)                     | types (params/returns)             |                                  ✅ |                                   ✅ | [Part 8](#part-8); enables escape diagnostics in strict-system. Importers must preserve the qualifier. |
| Borrowed-return marker (`objc_returns_borrowed(owner_index=N)`) | functions/methods                  |                                  ✅ |                                   ✅ | [Part 8](#part-8); identifies the owner parameter for lifetime checking of interior pointers.          |
| Task-spawn recognition (`objc_task_spawn` etc.)                 | stdlib entry points                |                                  ✅ |                                   ✅ | Needed for compiler enforcement at call sites.                                                         |
| Direct method `objc_direct`                                     | methods                            |                                  ✅ |                                   ✅ | Changes call legality and category interactions.                                                       |
| Final `objc_final`                                              | classes/methods                    |                                  ✅ |                                   ✅ | Optimization + legality constraints.                                                                   |
| Sealed `objc_sealed`                                            | classes                            |                                  ✅ |                                   ✅ | Cross-module subclass legality.                                                                        |
| Derive/macro synthesized declarations                           | types/members                      |                                  ✅ |                                   ✅ | Synthesized members must be visible to importers before layout/ABI decisions.                          |
| Error bridging markers (`objc_nserror`, `objc_status_code`)     | funcs/methods                      |                                  ✅ |                                   ✅ | Affects lowering of `try` and wrappers.                                                                |
| Availability / platform gates                                   | all exported decls                 |                                  ✅ |                                   ✅ | Not a new ObjC 3.0 feature, but required for correctness.                                              |

### D.3.2 Table B — ABI boundary summary (v1) {#d-3-2}

| Feature                  | ABI impact                            | Required stability property                                       | Canonical/recommended ABI shape                                                      |
| ------------------------ | ------------------------------------- | ----------------------------------------------------------------- | ------------------------------------------------------------------------------------ |
| `throws`                 | **Yes**                               | Caller and callee must agree on calling convention across modules | Recommended: trailing `id<Error> * _Nullable outError` ([C.4](#c-4)).                |
| `async`                  | **Yes**                               | Importers must call using the same coroutine/continuation ABI     | Recommended: LLVM coroutine lowering ([C.5](#c-5)), with a task/executor context.    |
| Executor/actor isolation | Usually **no** (call-site scheduling) | Metadata must exist so importer inserts hops and requires `await` | No ABI change required for sync bodies; hop is an `await`ed scheduling operation.    |
| Optional chaining/sends  | No                                    | Semantics preserved in caller IR                                  | Lower to conditional receiver check; args not evaluated on nil ([C.3.1](#c-3-1)).    |
| Direct methods           | Sometimes (dispatch surface)          | Importers must know legality + dispatch mode                      | Recommended: direct symbol call + omit dynamic lookup where permitted ([C.8](#c-8)). |
| Generics (pragmatic)     | No (erased)                           | Importers must type-check consistently                            | Erased in ABI; preserved in metadata/interface.                                      |
| Key paths                | Library ABI                           | Standard library must define stable representation                | ABI governed by stdlib; metadata must preserve `KeyPath<Root,Value>` types.          |

### D.3.3 Table C — Runtime hooks (minimum expectations) {#d-3-3}

This table names **conceptual hooks**. Implementations may use different symbol names.

| Area                 | Minimum required capability                                          | Notes                                                                  |
| -------------------- | -------------------------------------------------------------------- | ---------------------------------------------------------------------- |
| Executors            | Enqueue continuation onto an executor; query “current executor”      | Needed for `objc_executor(...)` and for `await` resumption.            |
| Tasks                | Create child/detached tasks; join; cancellation state                | Standard library provides API; compiler enforces via attributes.       |
| Actors               | Each actor instance maps to a serial executor; enqueue isolated work | Representation is implementation-defined (inline field or side table). |
| Autorelease pools    | Push/pop implicit pools around execution slices                      | Normative on ObjC runtimes ([C.7](#c-7)).                              |
| Diagnostics metadata | Preserve enough source mapping for async/macro debugging             | [Part 12](#part-12) requires debuggability.                            |

### D.3.4 Table D — Metadata version compatibility matrix (normative) {#d-3-4}

| Producer metadata vs importer support / payload condition             | Compatibility direction                      | Required importer behavior                                                                                    |
| --------------------------------------------------------------------- | -------------------------------------------- | ------------------------------------------------------------------------------------------------------------- |
| `schema_major` equal; producer `schema_minor` <= importer max minor   | backward (new importer reads older payload)  | Accept; treat absent newer fields as unavailable.                                                             |
| `schema_major` equal; producer `schema_minor` > importer max minor    | forward (older importer reads newer payload) | Accept only if all unknown elements are ignorable extension fields and all `required_capabilities` are known. |
| `schema_major` equal; producer minor newer with unknown required data | forward                                      | Hard error: reject import; report unknown required field/capability.                                          |
| `schema_major` equal; known-required field missing/invalid in payload | both                                         | Diagnose per [Table E](#d-3-5); ABI-significant/effect-lowering omissions remain hard errors in all profiles. |
| `schema_major` differs                                                | both                                         | Hard error: reject import; report producer and importer major versions.                                       |

For forward compatibility, ignorable extension fields are explicitly non-semantic for [Table A](#d-3-1) conformance and may be skipped.

### D.3.5 Table E — Importer validation by conformance profile (normative) {#d-3-5}

Profiles are defined in [Part 1](#part-1) and the checklist in [E.2](#e-2).

| Validation condition                                                                                                                          | Core                                                                   | Strict                  | Strict Concurrency      | Strict System           |
| --------------------------------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------- | ----------------------- | ----------------------- | ----------------------- |
| Missing or mismatched ABI-significant/effect-lowering metadata (`throws`, `async`, `objc_nserror`, `objc_status_code(...)`, dispatch markers) | Hard error                                                             | Hard error              | Hard error              | Hard error              |
| Missing or mismatched isolation metadata (`objc_executor(...)`, actor isolation/nonisolated markers)                                          | Recoverable diagnostic; conservative import                            | Hard error              | Hard error              | Hard error              |
| Missing or mismatched Sendable/task-spawn metadata                                                                                            | Recoverable diagnostic                                                 | Recoverable diagnostic  | Hard error              | Hard error              |
| Missing or mismatched borrowed/lifetime system metadata                                                                                       | Recoverable diagnostic                                                 | Recoverable diagnostic  | Recoverable diagnostic  | Hard error              |
| Unknown ignorable extension field                                                                                                             | Ignored (may emit note)                                                | Ignored (may emit note) | Ignored (may emit note) | Ignored (may emit note) |
| Unknown required field/capability                                                                                                             | Hard error                                                             | Hard error              | Hard error              | Hard error              |
| Textual interface vs module metadata mismatch for any [Table A](#d-3-1) item                                                                  | Hard error for ABI-significant items; otherwise recoverable diagnostic | Hard error              | Hard error              | Hard error              |

Missing-metadata severity by profile shall follow [Table E](#d-3-5) exactly:

- **Core**: non-ABI missing metadata may be recoverable, but diagnostics are mandatory and import must remain conservative.
- **Strict**: missing isolation metadata is a hard error; sendability/task-spawn and borrowed/lifetime metadata remain recoverable unless another row upgrades severity.
- **Strict Concurrency**: missing concurrency metadata required for isolation/sendability/task-spawn is a hard error.
- **Strict System**: missing concurrency metadata and borrowed/lifetime metadata is a hard error.

When OCI-1 is the active interchange path, missing OCI-1 fields map into these same rows by category (`effects.*`, `isolation.*`, `sendable.*`).

### D.3.6 Table F — OCI-1 required concurrency metadata fields (normative) {#d-3-6}

| OCI-1 field              | Meaning                                                                  | Required for profile                                                      |
| ------------------------ | ------------------------------------------------------------------------ | ------------------------------------------------------------------------- |
| `decl_id`                | Stable declaration identity key for merge/join across interface payloads | Core and above                                                            |
| `effects.async`          | Whether declaration is async                                             | Core and above                                                            |
| `effects.throws`         | Whether declaration is throwing                                          | Core and above                                                            |
| `isolation.executor`     | Executor affinity (`objc_executor(...)` equivalent)                      | Core and above                                                            |
| `isolation.actor`        | Actor isolation binding for declarations/members                         | Core and above                                                            |
| `isolation.nonisolated`  | Explicit nonisolated marker                                              | Core and above                                                            |
| `sendable.boundary`      | Whether Sendable-like checking applies at this boundary                  | Strict Concurrency and Strict System (diagnostic optional in Core/Strict) |
| `sendable.unsafe_marker` | Unsafe-sendable escape marker metadata                                   | Strict Concurrency and Strict System                                      |

## D.4 Conformance tests (minimum) {#d-4}

A conforming implementation’s test suite shall include the following module/interface tests.

### D.4.1 Normative round-trip interface test flow {#d-4-1}

For each representative API set, tests shall perform all of:

1. Build module `M` from source declarations.
2. Emit an interface representation `I` for `M` (textual or equivalent).
3. Import `I` in a clean translation unit and separately import `M`.
4. Verify that imported declarations from step 3 are semantically equivalent for [Table A](#d-3-1) items.
5. Compile call sites that exercise the imported declarations and verify diagnostics/typing behavior are unchanged.

Minimum required declaration coverage for round-trip tests:

- `throws` declarations (function + method), including bridging markers (`objc_nserror`, `objc_status_code(...)`) and unchanged `try` obligations.
- `async` declarations with `await` call sites.
- combined `async throws` declarations with `try await` call sites.
- Executor-isolated declarations (`objc_executor(...)`) requiring a hop.
- Actor-isolated declarations, including at least one `nonisolated` member.
- Dispatch legality attributes (`objc_direct`, `objc_final`, `objc_sealed`).
- Profile-gated metadata: Sendable/task-spawn metadata and borrowed/lifetime metadata where claimed.

Round-trip verification shall compare, for each covered declaration, the effect/attribute tuple seen by importers:

- effects (`throws`, `async`) and error-bridging markers,
- isolation/executor/nonisolated metadata,
- task-spawn/sendability metadata where profile-claimed,
- dispatch and borrowed/lifetime markers where applicable.

### D.4.2 Compatibility and diagnostics tests {#d-4-2}

A conforming test suite shall include positive and negative import tests for [Table D](#d-3-4) and [Table E](#d-3-5), including:

- importing older minor metadata with a newer importer (accepted),
- importing newer minor metadata containing only ignorable extension fields (accepted),
- importing newer metadata with unknown required fields/capabilities (hard error),
- importing metadata with different `schema_major` (hard error),
- intentionally removing or altering metadata for `throws`/`async`/error-bridging/isolation and asserting profile-appropriate diagnostics,
- profile-matrix checks for missing metadata severity (Core recoverable for non-ABI metadata; Strict isolation hard error; Strict Concurrency hard error for concurrency metadata; Strict System hard error for concurrency plus borrowed/lifetime metadata).

### D.4.3 OCI-1 export/import tests (normative minimum) {#d-4-3}

A conforming test suite shall include OCI-1-specific tests that validate:

- exporting OCI-1 from an interface and importing it in a clean build preserves concurrency behavior (`await`/hop/isolation diagnostics),
- missing required OCI-1 fields are diagnosed per [D.3.5](#d-3-5),
- unknown required OCI-1 fields/capabilities trigger hard errors,
- additive OCI-1 minor-version fields marked ignorable are accepted,
- OCI-1 round-trip preserves `effects.async`, `effects.throws`, and required isolation/sendability fields from [Table F](#d-3-6).

## D.5 Lane-C Typed Edge-Case Metadata Anchors (implementation anchor) {#d-5}

deterministic lane-C typed sema-to-lowering edge-case compatibility metadata anchors for `M227-C005` must remain synchronized across typed handoff keys, parse-artifact replay keys, and readiness gating metadata surfaces before lane-C closeout can pass.

deterministic lane-C typed sema-to-lowering edge-case expansion and robustness metadata anchors for `M227-C006` must remain synchronized across typed robustness keys, parse-artifact robustness keys, and readiness alignment metadata surfaces before lane-C robustness closure can pass.

deterministic lane-C typed sema-to-lowering diagnostics-hardening metadata anchors for `M227-C007` must remain synchronized across typed diagnostics-hardening keys, parse diagnostics-hardening keys, and readiness alignment metadata surfaces before lane-C diagnostics closure can pass.

deterministic lane-C typed sema-to-lowering recovery/determinism metadata anchors for `M227-C008` must remain synchronized across typed recovery keys, parse recovery keys, and readiness alignment metadata surfaces before lane-C recovery closure can pass.

deterministic lane-C typed sema-to-lowering conformance matrix metadata anchors for `M227-C009` must remain synchronized across typed matrix keys, parse matrix keys, and readiness alignment metadata surfaces before lane-C matrix closure can pass.

deterministic lane-C typed sema-to-lowering conformance corpus metadata anchors for `M227-C010` must remain synchronized across typed corpus keys, parse corpus keys, and readiness alignment metadata surfaces before lane-C corpus closure can pass.

deterministic lane-C typed sema-to-lowering performance/quality guardrails metadata anchors for `M227-C011` must remain synchronized across typed guardrail keys, parse guardrail keys, and readiness alignment metadata surfaces before lane-C guardrail closure can pass.

deterministic lane-C typed sema-to-lowering cross-lane integration metadata anchors for `M227-C012` must remain synchronized across typed integration-sync keys, parse integration-sync keys, and readiness alignment metadata surfaces before lane-C integration-sync closure can pass.

deterministic lane-C typed sema-to-lowering docs/runbook synchronization metadata anchors for `M227-C013` must remain synchronized across typed docs/runbook keys, parse docs/runbook keys, and readiness alignment metadata surfaces before lane-C docs/runbook closure can pass.

deterministic lane-C typed sema-to-lowering release-candidate/replay metadata anchors for `M227-C014` must remain synchronized across typed dry-run keys, parse dry-run keys, and readiness alignment metadata surfaces before lane-C dry-run closure can pass.

deterministic lane-C typed sema-to-lowering advanced-core-shard1 metadata anchors for `M227-C015` must remain synchronized across typed shard-1 keys, parse shard-1 keys, and readiness alignment metadata surfaces before lane-C shard-1 closure can pass.

deterministic lane-C typed sema-to-lowering advanced-edge-compatibility-shard1 metadata anchors for `M227-C016` must remain synchronized across typed edge-compatibility shard-1 keys, parse edge-compatibility shard-1 keys, and readiness alignment metadata surfaces before lane-C shard-1 edge compatibility closure can pass.

deterministic lane-C typed sema-to-lowering advanced-diagnostics-shard1 metadata anchors for `M227-C017` must remain synchronized across typed diagnostics shard-1 keys, parse diagnostics shard-1 keys, and readiness alignment metadata surfaces before lane-C shard-1 diagnostics closure can pass.

deterministic lane-C typed sema-to-lowering advanced-conformance-shard1 metadata anchors for `M227-C018` must remain synchronized across typed conformance shard-1 keys, parse conformance shard-1 keys, and readiness alignment metadata surfaces before lane-C shard-1 conformance closure can pass.

deterministic lane-C typed sema-to-lowering advanced-integration-shard1 metadata anchors for `M227-C019` must remain synchronized across typed integration shard-1 keys, parse integration shard-1 keys, and readiness alignment metadata surfaces before lane-C shard-1 integration closure can pass.

deterministic lane-C typed sema-to-lowering advanced-performance-shard1 metadata anchors for `M227-C020` must remain synchronized across typed performance shard-1 keys, parse performance shard-1 keys, and readiness alignment metadata surfaces before lane-C shard-1 performance closure can pass.

deterministic lane-C typed sema-to-lowering advanced-core-shard2 metadata anchors for `M227-C021` must remain synchronized across typed core-shard2 keys, parse core-shard2 keys, and readiness alignment metadata surfaces before lane-C shard-2 core closure can pass.

deterministic lane-C typed sema-to-lowering advanced-edge-compatibility-shard2 metadata anchors for `M227-C022` must remain synchronized across typed edge-compatibility-shard2 keys, parse edge-compatibility-shard2 keys, and readiness alignment metadata surfaces before lane-C shard-2 edge compatibility closure can pass.

deterministic lane-C typed sema-to-lowering advanced-diagnostics-shard2 metadata anchors for `M227-C023` must remain synchronized across typed diagnostics-shard2 keys, parse diagnostics-shard2 keys, and readiness alignment metadata surfaces before lane-C shard-2 diagnostics closure can pass.

deterministic lane-C typed sema-to-lowering advanced-conformance-shard2 metadata anchors for `M227-C024` must remain synchronized across typed conformance-shard2 keys, parse conformance-shard2 keys, and readiness alignment metadata surfaces before lane-C shard-2 conformance closure can pass.

deterministic lane-C typed sema-to-lowering advanced-integration-shard2 metadata anchors for `M227-C025` must remain synchronized across typed integration-shard2 keys, parse integration-shard2 keys, and readiness alignment metadata surfaces before lane-C shard-2 integration closure can pass.

deterministic lane-C typed sema-to-lowering integration-closeout-signoff metadata anchors for `M227-C026` must remain synchronized across typed integration-closeout keys, parse integration-closeout keys, and readiness alignment metadata surfaces before lane-C sign-off closure can pass.

deterministic property conformance gate and docs dependency anchors for `M234-E001` must remain synchronized across lane-E governance metadata and dependency continuity surfaces, including `M234-A001`, `M234-B001`, `M234-C001`, and `M234-D001`.

deterministic property conformance gate and docs modular split/scaffolding dependency anchors for `M234-E002` must remain synchronized across lane-E governance metadata and dependency continuity surfaces, including `M234-E001`, `M234-A002`, `M234-B002`, `M234-C002`, and `M234-D002`.

deterministic property conformance gate and docs core feature implementation dependency anchors for `M234-E003` must remain synchronized across lane-E governance metadata and dependency continuity surfaces, including `M234-E002`, `M234-A003`, `M234-B003`, `M234-C003`, and `M234-D002`.

deterministic property conformance gate and docs core feature expansion dependency anchors for `M234-E004` must remain synchronized across lane-E governance metadata and dependency continuity surfaces, including `M234-E003`, `M234-A004`, `M234-B004`, `M234-C004`, and `M234-D003`.

deterministic lane-A class/protocol/category metadata generation anchors for `M229-A001` must remain synchronized across parser declaration metadata, semantic linking summaries, typed handoff surfaces, and IR frontend metadata profiles.

deterministic lane-A class/protocol/category metadata generation metadata anchors for `M229-A001` must preserve class/protocol/category metadata evidence and parser replay-budget continuity so runtime ABI metadata linkage drift fails closed.

deterministic lane-A class/protocol/category metadata generation modular split anchors for `M229-A002` must preserve explicit `M229-A001` dependency continuity so class/protocol/category metadata scaffolding drift fails closed.

deterministic lane-A class/protocol/category metadata generation core feature anchors for `M229-A003` must preserve explicit `M229-A002` dependency continuity so class/protocol/category metadata core-feature drift fails closed.

deterministic lane-A class/protocol/category metadata generation core feature expansion anchors for `M229-A004` must preserve explicit `M229-A003` dependency continuity so class/protocol/category metadata core-feature-expansion drift fails closed.

deterministic lane-A conformance corpus governance and sharding contract-freeze anchors for `M230-A001` must preserve explicit lane-A contract-freeze metadata continuity so conformance corpus governance/sharding drift fails closed.

deterministic lane-A conformance corpus governance and sharding modular split anchors for `M230-A002` must preserve explicit `M230-A001` dependency continuity so conformance corpus governance/sharding scaffolding drift fails closed.

deterministic lane-A conformance corpus governance and sharding core feature anchors for `M230-A003` must preserve explicit `M230-A002` dependency continuity so conformance corpus governance/sharding core-feature drift fails closed.

deterministic lane-A conformance corpus governance and sharding core feature expansion anchors for `M230-A004` must preserve explicit `M230-A003` dependency continuity so conformance corpus governance/sharding core-feature-expansion drift fails closed.

deterministic lane-A conformance corpus governance and sharding edge-case and compatibility completion anchors for `M230-A005` must preserve explicit `M230-A004` dependency continuity so conformance corpus governance/sharding edge-case-and-compatibility-completion drift fails closed.

deterministic lane-A conformance corpus governance and sharding edge-case expansion and robustness anchors for `M230-A006`
explicit `M230-A005` dependency continuity so conformance corpus governance/sharding edge-case-expansion-and-robustness drift fails closed


deterministic lane-A conformance corpus governance and sharding diagnostics hardening anchors for `M230-A007`
explicit `M230-A006` dependency continuity so conformance corpus governance/sharding diagnostics-hardening drift fails closed


deterministic lane-A conformance corpus governance and sharding recovery and determinism hardening anchors for `M230-A008`
explicit `M230-A007` dependency continuity so conformance corpus governance/sharding recovery-and-determinism-hardening drift fails closed


deterministic lane-A conformance corpus governance and sharding conformance matrix implementation anchors for `M230-A009`
explicit `M230-A008` dependency continuity so conformance corpus governance/sharding conformance-matrix-implementation drift fails closed


deterministic lane-A conformance corpus governance and sharding conformance corpus expansion anchors for `M230-A010`
explicit `M230-A009` dependency continuity so conformance corpus governance/sharding conformance-corpus-expansion drift fails closed


deterministic lane-A conformance corpus governance and sharding performance and quality guardrails anchors for `M230-A011`
explicit `M230-A010` dependency continuity so conformance corpus governance/sharding performance-and-quality-guardrails drift fails closed


deterministic lane-A conformance corpus governance and sharding integration closeout and gate sign-off anchors for `M230-A012`
explicit `M230-A011` dependency continuity so conformance corpus governance/sharding integration-closeout-and-gate-sign-off drift fails closed


deterministic lane-A declaration grammar expansion and normalization contract-freeze anchors for `M231-A001`
explicit lane-A contract-freeze metadata continuity so declaration grammar expansion/normalization drift fails closed


deterministic lane-A declaration grammar expansion and normalization modular split anchors for `M231-A002`
explicit `M231-A001` dependency continuity so declaration grammar expansion/normalization scaffolding drift fails closed


deterministic lane-A declaration grammar expansion and normalization core feature anchors for `M231-A003`
explicit `M231-A002` dependency continuity so declaration grammar expansion/normalization core-feature drift fails closed


deterministic lane-A declaration grammar expansion and normalization core feature anchors for `M231-A004`
explicit `M231-A003` dependency continuity so declaration grammar expansion/normalization core-feature drift fails closed


deterministic lane-A declaration grammar expansion and normalization core feature anchors for `M231-A005`
explicit `M231-A004` dependency continuity so declaration grammar expansion/normalization core-feature drift fails closed


deterministic lane-A declaration grammar expansion and normalization core feature anchors for `M231-A006`
explicit `M231-A005` dependency continuity so declaration grammar expansion/normalization core-feature drift fails closed

