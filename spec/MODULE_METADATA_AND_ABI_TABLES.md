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
   - deterministic lane-E performance SLO dependency anchors for `M247-A001`, `M247-B001`,
     `M247-C001`, and `M247-D001`, including pending-lane tokens needed to keep
     compile/perf-budget governance evidence fail-closed before lane A-D contract
     assets are seeded.
   - deterministic lane-E performance SLO modular split dependency anchors for `M247-E001`,
     `M247-A002`, `M247-B002`, `M247-C002`, and `M247-D002`, including
     pending-lane tokens needed to keep modular split governance evidence
     fail-closed before lane A-D modular split assets are seeded.
   - deterministic lane-A suite partitioning metadata anchors for `M248-A001`
     with fixture ownership boundary evidence and parser replay-budget continuity
     so CI sharding partition drift fails closed.
   - deterministic lane-A suite partitioning modular split metadata anchors for
     `M248-A002` with explicit `M248-A001` dependency continuity so fixture
     scaffolding drift fails closed.
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
   - deterministic lane-B semantic/lowering modular split metadata anchors for
     `M248-B002` with explicit `M248-B001` dependency continuity so semantic
     scaffolding drift fails closed.
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
  - deterministic lane-D runner operations metadata anchors for `M248-D001`
    with compile-route evidence and perf-budget continuity so platform
    operation drift fails closed.
   - deterministic lane-D runner modular split metadata anchors for `M248-D002`
     with explicit `M248-D001` dependency continuity so platform scaffolding
     drift fails closed.

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
