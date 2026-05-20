# ObjC 3.0 Conformance Suite Skeleton

This directory defines the minimum conformance-suite layout required by `P0-15`.

## Required buckets

- `parser/`: grammar and ambiguity coverage.
- `semantic/`: typing/effects/isolation/nullability behavior.
- `lowering_abi/`: runtime/lowering and ABI boundary checks.
- `module_roundtrip/`: interface emit/import and metadata preservation.
- `diagnostics/`: required diagnostics and fix-it behavior.

## Profile coverage minima

- Core:
  - At least 15 parser tests.
  - At least 25 semantic tests.
  - At least 10 lowering/ABI tests.
  - At least 12 module round-trip tests.
  - At least 20 diagnostics tests.
- Strict:
  - At least 10 additional diagnostics/fix-it tests over Core.
- Strict Concurrency:
  - At least 12 additional isolation/Sendable tests.
- Strict System:
  - At least 12 additional borrowed/resource/capture tests.

## Wave progress snapshot

Current fixture inventory added for E.3.1-E.3.12 waves:

- Parser fixtures: 21 (`TUV-01`..`TUV-05`, `LMV-48-NEG-01`, `FTM-50-01`..`FTM-50-03`, `ASY-01`..`ASY-02`, `AWT-01`..`AWT-02`, `SYS-ATTR-01`..`SYS-ATTR-04`, `CAP-01`..`CAP-02`, `PERF-ATTR-01`..`PERF-ATTR-02`)
- Diagnostics fixtures: 78 (`SCM-01`..`SCM-06`, `CRPT-01`..`CRPT-06`, `NUL-60-NEG-01`, `NUL-60-FIX-01`, `LFT-69-NEG-01`, `LFT-69-FIX-01`, `NLE-74-NEG-01`..`NLE-74-NEG-02`, `ERR-79-NEG-01`, `ERR-79-FIX-01`, `ASY-05`..`ASY-06`, `CONC-DIAG-01`..`CONC-DIAG-06`, `SND-07`..`SND-08`, `AWT-05`..`AWT-06`, `RES-06`, `AGR-NEG-01`, `BRW-NEG-03`..`BRW-NEG-05`, `CAP-06`..`CAP-08`, `SYS-DIAG-01`..`SYS-DIAG-08`, `PERF-DYN-01`..`PERF-DYN-04`, `PERF-DIAG-01`..`PERF-DIAG-04`, `META-MAC-01`..`META-MAC-04`, `DIAG-GRP-01`..`DIAG-GRP-10`, `MIG-01`..`MIG-08`)
- Semantic fixtures: 90 (`TYP-58-*`, `NNB-59-*`, `OPT-61-*`, `OPT-62-*`, `GEN-64-*`, `GEN-65-*`, `KPATH-66-*`, `DEF-70-*`, `GRD-72-*`, `MTC-73-*`, `THR-75-*`, `TRY-77-*`, `BRG-78-*`, `ASY-03`..`ASY-04`, `CAN-01`..`CAN-04`, `EXE-01`..`EXE-03`, `EXEC-ATTR-01`..`EXEC-ATTR-02`, `ACT-01`..`ACT-04`, `SND-01`..`SND-06`, `AWT-03`..`AWT-04`, `RES-01`..`RES-03`, `LIFE-01`..`LIFE-03`, `BRW-NEG-01`..`BRW-NEG-02`, `BRW-POS-01`..`BRW-POS-04`, `CAP-03`..`CAP-05`, `PERF-DIRMEM-01`..`PERF-DIRMEM-03`, `PERF-LEG-01`..`PERF-LEG-02`, `META-DRV-01`..`META-DRV-03`, `META-PKG-01`..`META-PKG-03`, `INT-C-01`..`INT-C-06`, `INT-CXX-01`..`INT-CXX-04`, `INT-SWIFT-01`..`INT-SWIFT-04`)
- Lowering/ABI fixtures: 35 (`ARC-67-*`, `ARP-68-RT-*`, `DEF-71-RT-*`, `THR-76-ABI-*`, `CORO-01`..`CORO-03`, `CAN-05`..`CAN-07`, `EXE-04`, `ACT-05`..`ACT-07`, `RES-04`..`RES-05`, `AGR-RT-01`..`AGR-RT-03`, `LIFE-04`..`LIFE-05`, `INT-C-07`..`INT-C-12`, `INT-CXX-05`..`INT-CXX-08`)
- Module round-trip fixtures: 42 (`MOD-53-*`, `IFC-54-*`, `IFC-55-*`, `META-56-*`, `IFV-57-*`, `META-63-*`, `IFC-63-*`, `BRG-78-MOD-*`, `CORO-04`..`CORO-05`, `EXE-05`, `EXEC-ATTR-03`, `ACT-08`..`ACT-09`, `SND-XM-01`..`SND-XM-02`, `INT-SWIFT-05`..`INT-SWIFT-08`, `SYS-ATTR-05`..`SYS-ATTR-08`, `BRW-META-01`..`BRW-META-03`, `PERF-ATTR-03`..`PERF-ATTR-04`, `PERF-DIRMEM-04`, `PERF-LEG-03`, `META-EXP-01`..`META-EXP-02`, `META-XM-01`..`META-XM-04`)

Machine-readable indexes:

- `tests/conformance/parser/manifest.json`
- `tests/conformance/diagnostics/manifest.json`
- `tests/conformance/semantic/manifest.json`
- `tests/conformance/lowering_abi/manifest.json`
- `tests/conformance/module_roundtrip/manifest.json`
- `tests/conformance/hard_cutover_catalog.json` (behavior boundary and
  no-compatibility policy for parser, semantic, lowering/ABI, IR/module, and
  runtime/e2e fixture groups)
- `tests/conformance/hard_cutover_issue_index.json` (local issue evidence map
  for `#8132`-`#8150`, keyed to behavior fixture boundaries and local commits)
- `tests/conformance/hard_cutover_acceptance_area_owners.json` (acceptance-area
  ownership index tying compiler/runtime/workflow/docs areas to code paths,
  behavior fixtures, current runtime storage/reflection/public ABI probe anchors,
  and hard-cutover issues)
- `tests/conformance/hard_cutover_behavior_evidence_topology.json` (behavior-first
  phase/family topology connecting native fixtures, strict-error coverage,
  generated-boundary status, and issue closeout artifacts)
- `tests/conformance/hard_cutover_fixture_family_owner_index.json` (fixture-family
  ownership index separating canonical positives, retired-surface rejection,
  generated provenance, tooling metadata, and issue closeout artifacts)
- `tests/conformance/hard_cutover_fixture_boundary_contracts.json` (boundary
  contract index tying canonical behavior, generated provenance, retired
  surfaces, reference anchors, and lexical residue dispositions to their owner
  indexes)
- `tests/conformance/hard_cutover_behavior_outcome_owner_index.json` (behavior
  outcome ownership index separating canonical support, rejection, strict-error,
  generated provenance, residue-audit, and closeout-only evidence)
- `tests/conformance/hard_cutover_diagnostic_outcome_code_index.json`
  (diagnostic/strict-error code owner index for retired and unsupported
  behavior outcomes)
- `tests/conformance/hard_cutover_retired_surface_absence.json` (retired
  modes, retired adapters, alternate acceptance paths, and retired-source lanes mapped
  to rejection, strict-error, or absent support)
- `tests/conformance/hard_cutover_positive_residue_audit.json` (read-only
  positive-fixture residue audit documenting compatibility-looking lexical hits
  that are not positive compatibility expectations)
- `tests/conformance/longitudinal_suites.json` (retained regression and adoption basis)
- `tests/conformance/corpus_surface.json` (taxonomy, audit surface, and gap model)
- `tests/conformance/support_claim_runnable_evidence_catalog.json` (checked-in
  traceability from implemented support claims to runnable conformance evidence,
  with positive and negative/strict-error rows for the `#8058`/`#8059` object
  foundation slice)
- `tests/conformance/behavior_owner_splits/index.json` (behavior-first owner
  split for broad conformance buckets before parser, semantic, lowering, IR,
  runtime, e2e, or rejection evidence can be cited)
- `tests/conformance/COVERAGE_MAP.md` (issue/family traceability map)

## Hard-Cutover Fixture Policy

Conformance metadata is canonical-first. Fixtures must not preserve retired
modes, retired adapters, alternate acceptance paths, retired-source lanes,
unsupported-feature claims, or runtime-dispatch paths as positive behavior. When a fixture documents an
unavailable feature configuration, the expected result is a strict error with
stable diagnostic metadata, not retired route acceptance.

Behavior ownership is phase-first:

| Boundary | Positive authority | Rejection / strict-error authority |
| --- | --- | --- |
| parser | `tests/native/parser/positive` | `tests/native/parser/negative` for old-mode literals and removed mode/retired-route flags |
| semantic | `tests/native/sema/{types,ownership,objc,control_flow,concurrency}` | `tests/native/sema/{negative,errors,concurrency}` for retired adapter gates and unsupported feature claims |
| lowering ABI | `tests/native/lowering` and canonical `tests/conformance/lowering_abi` ABI fixtures | `tests/native/lowering/errors` and strict lowering/link diagnostics for removed runtime retired route paths |
| IR/module | `tests/native/ir/{module,function,metadata,runtime_calls}` | `tests/native/ir/runtime_calls` strict linkage evidence for unsupported runtime helpers |
| runtime | `tests/native/runtime/{object_model,storage,arc,blocks,concurrency}` and runtime probe ownership metadata | `tests/native/runtime/{dispatch,errors}` for strict dispatch and unresolved-symbol outcomes |
| e2e | `tests/native/e2e/{smoke,feature_matrix}` | `tests/native/e2e/negative_execution` for execution-boundary rejection and strict-error outcomes |
| generated fixtures | none | none; generated artifacts are replay/schema provenance only |

`tests/conformance/hard_cutover_fixture_boundary_contracts.json` is the
machine-readable owner matrix for these boundaries. `tests/conformance/hard_cutover_retired_surface_fixture_contracts.json`
keeps stable negative detector pattern ids such as `O3C002`,
`OBJC3-E-REMOVED-RETIRED_ROUTE-FLAG`, `OBJC3-E-REMOVED-COMPATIBILITY-GATE`,
`OBJC3-E-REMOVED-RUNTIME-RETIRED_ROUTE`, `link.unresolved_symbol`, and `O3RT002`
attached to rejection or strict-error fixture families. These ids are
intentional negative residues, not positive compatibility support.

Runtime probe metadata under `tests/tooling/runtime/` is treated as fixture
evidence for canonical runtime ownership only. Storage, reflection, registration,
object-model, and public ABI probes may anchor live owner paths; strict dispatch
probes remain rejection/strict-error evidence and must not become retired
adapter, alternate acceptance path, or retired-source lane positive claims.

Live validation entrypoints:

- `npm run objc3c -- validate-conformance-corpus`
- `npm run objc3c -- validate-runnable-conformance-corpus`
- `npm run objc3c -- test-nightly`

## Cross-module preservation requirements

Each profile run must include tests that validate preservation of:

- `throws` and `async` effects,
- executor and actor-isolation metadata,
- dispatch legality attributes (`objc_direct`, `objc_final`, `objc_sealed`),
- profile-gated borrowed/lifetime metadata (Strict System).

## Portable diagnostic assertion format

Conformance tests should assert diagnostics using stable metadata, not exact prose.

Preferred assertion shape per expected diagnostic:

- `code`: stable diagnostic identifier.
- `severity`: `error` / `warning` / `note`.
- `span`: line/column start and end.
- `fixits`: list of machine-applicable edits (when required).

Implementations may keep native test harness syntax, but they shall be able to emit this canonical structure for conformance reporting.
