# Module Round-Trip Bucket

Minimum scope:

- emit interface -> import -> semantic equivalence,
- metadata version compatibility behavior,
- cross-module preservation of effects/isolation/dispatch attributes,
- OCI-1 (portable concurrency metadata) export/import checks.

## Wave 1 fixture set (E.3.2 modules/interfaces)

These fixtures cover the next E.3.2 implementation band:

- `MOD-53-01.json`, `MOD-53-02.json` for module-aware compilation and lookup.
- `IFC-54-01.json`, `IFC-54-02.json` for textual interface emission mode.
- `IFC-55-01.json`, `IFC-55-02.json` for canonical interface spellings.
- `META-56-01.json`, `META-56-02.json` for D Table A metadata preservation.
- `IFV-57-01.json`, `IFV-57-02.json` for strict interface verification mode.
- `META-63-01.json`, `IFC-63-01.json` for optional/nullability
  metadata+interface preservation.
- `BRG-78-MOD-01.json` for bridging-attribute metadata/interface preservation.
- `CORO-04.json`, `CORO-05.json` for async coroutine ABI metadata and interface
  round-trip stability.
- `EXE-05.json`, `EXEC-ATTR-03.json` for executor annotation/hop metadata
  preservation and canonical interface round-trips.
- `ACT-08.json`, `ACT-09.json` for actor isolation metadata and contextual
  keyword compatibility round-trip behavior.
- `SND-XM-01.json`, `SND-XM-02.json` for sendability metadata preservation and
  strict import-time mismatch diagnostics.
- `SYS-ATTR-05.json`..`SYS-ATTR-08.json` for Part 8 canonical spellings and
  borrowed/resource metadata preservation.
- `BRW-META-01.json`..`BRW-META-03.json` for borrowed/lifetime metadata
  preservation and interface-verification mismatch diagnostics.
- `PERF-ATTR-03.json`, `PERF-ATTR-04.json` for `objc_direct`/`objc_final`/
  `objc_sealed` metadata+interface preservation.
- `PERF-DIRMEM-04.json`, `PERF-LEG-03.json` for direct/final/sealed legality
  validation across module boundaries.
- `META-EXP-01.json`, `META-EXP-02.json` for deterministic derive-expansion
  round-trip preservation.
- `META-XM-01.json`..`META-XM-04.json` for macro/derive expansion preservation
  and metadata/interface mismatch detection.
- `INT-SWIFT-05.json`..`INT-SWIFT-08.json` for Swift interop
  metadata/interface preservation and mismatch diagnostics.

See `tests/conformance/module_roundtrip/manifest.json` for machine-readable
indexing.
