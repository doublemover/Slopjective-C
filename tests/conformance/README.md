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
- `tests/conformance/COVERAGE_MAP.md` (issue/family traceability map)

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
