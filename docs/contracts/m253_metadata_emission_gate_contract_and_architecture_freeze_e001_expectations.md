# M253 Metadata Emission Gate Contract And Architecture Freeze Expectations (E001)

Contract ID: `objc3c-runtime-metadata-emission-gate/m253-e001-v1`

## Objective

Freeze one fail-closed evidence gate for proving the current metadata emission stack is live and synchronized before `M253-E002` cross-lane closeout.

## Required implementation

1. Add a canonical expectations document for the metadata emission gate.
2. Add this packet, a deterministic checker, and tooling tests:
   - `scripts/check_m253_e001_metadata_emission_gate.py`
   - `tests/tooling/test_check_m253_e001_metadata_emission_gate.py`
3. Add `M253-E001` anchor text to:
   - `docs/objc3c-native.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `native/objc3c/src/lower/objc3_lowering_contract.h`
   - `native/objc3c/src/lower/objc3_lowering_contract.cpp`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
   - `native/objc3c/src/io/objc3_process.cpp`
4. Keep the gate fail closed over the canonical upstream evidence:
   - `tmp/reports/m253/M253-A002/source_to_section_mapping_completeness_matrix_summary.json`
   - `tmp/reports/m253/M253-B003/coff_elf_and_mach_o_metadata_policy_surface_core_feature_expansion_summary.json`
   - `tmp/reports/m253/M253-C006/binary_inspection_harness_summary.json`
   - `tmp/reports/m253/M253-D003/archive_and_static_link_metadata_discovery_behavior_summary.json`
5. The checker must reject drift if any upstream summary disappears, stops reporting `ok: true`, or drops the case-level invariants that define the current object-emission boundary.
6. `package.json` must wire:
   - `check:objc3c:m253-e001-metadata-emission-gate`
   - `test:tooling:m253-e001-metadata-emission-gate`
   - `check:objc3c:m253-e001-lane-e-readiness`
7. The gate must explicitly hand off to `M253-E002`.

## Non-goals

- No new metadata emission feature implementation.
- No runtime registration/startup bootstrap.
- No new object-inspection corpus beyond the already landed upstream proofs.

## Evidence

- `tmp/reports/m253/M253-E001/metadata_emission_gate_summary.json`
