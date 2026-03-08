# M253-B003 COFF, ELF, and Mach-O Metadata Policy Surface Core Feature Expansion Packet

Packet: `M253-B003`
Milestone: `M253`
Wave: `W45`
Lane: `B`
Issue: `#7089`
Contract ID: `objc3c-runtime-metadata-object-format-policy/m253-b003-v1`
Dependencies: `M253-B002`

## Objective

Preserve the normalized `M253-B002` layout policy while expanding it into an explicit COFF/ELF/Mach-O object-format mapping surface for section spellings and post-write determinism.

## Canonical Object-Format Policy Surface

- object-format contract id `objc3c-runtime-metadata-object-format-policy/m253-b003-v1`
- explicit host-format tokens:
  - `coff`
  - `elf`
  - `mach-o`
- explicit section-spelling models:
  - `coff-logical-section-spellings`
  - `elf-logical-section-spellings`
  - `mach-o-data-segment-comma-section-spellings`
- explicit retention-anchor models:
  - `llvm.used-appending-global+coff-timestamp-normalization`
  - `llvm.used-appending-global+elf-stable-sections`
  - `llvm.used-appending-global+mach-o-data-segment-sections`
- logical section names remain the ABI/source-of-truth surface from `M251`/`M252`
- emitted section names are lowering-derived host-format spellings.

## Acceptance Criteria

- Extend `native/objc3c/src/lower/objc3_lowering_contract.h/.cpp` with explicit host-format tokens and emitted-section mapping fields.
- Keep `M253-B001` and `M253-B002` replay compatibility intact while adding explicit B003 host-format fields.
- Have `native/objc3c/src/ir/objc3_ir_emitter.cpp` emit image-info and family globals using policy-owned emitted section spellings.
- Have `native/objc3c/src/io/objc3_process.cpp` detect produced object format and apply post-write determinism only when the format requires it.
- Add deterministic docs/spec/package/checker/test evidence.
- Happy-path native emission over `tests/tooling/fixtures/native/hello.objc3`
  using the `hello.objc3` probe must still emit `module.obj`.

## Dynamic Probes

1. Native compile probe over `tests/tooling/fixtures/native/hello.objc3` proving emitted IR/object output carries:
   - `!objc3.objc_runtime_metadata_layout_policy`
   - `!55`
   - `; runtime_metadata_layout_policy = ...`
   - `object_format_contract=objc3c-runtime-metadata-object-format-policy/m253-b003-v1`
   - host-format token
   - emitted image-info/family section spellings
   - successful `module.obj` emission.
2. Object-file inspection over `module.obj` proving the actual object magic matches the host-format token carried in the replay line.

## Non-Goals

- `M253-B003` does not add new metadata families.
- `M253-B003` does not add runtime registration/bootstrap.
- `M253-B003` does not change the logical metadata ABI or matrix inventory.

## Validation Commands

- `python scripts/check_m253_b003_coff_elf_and_mach_o_metadata_policy_surface_core_feature_expansion.py`
- `python -m pytest tests/tooling/test_check_m253_b003_coff_elf_and_mach_o_metadata_policy_surface_core_feature_expansion.py -q`
- `npm run check:objc3c:m253-b003-lane-b-readiness`

## Evidence Path

- `tmp/reports/m253/M253-B003/coff_elf_and_mach_o_metadata_policy_surface_core_feature_expansion_summary.json`
