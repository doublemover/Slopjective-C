# M253 COFF, ELF, and Mach-O Metadata Policy Surface Core Feature Expansion Expectations (B003)

Contract ID: `objc3c-runtime-metadata-object-format-policy/m253-b003-v1`
Status: Accepted
Issue: `#7089`
Scope: M253 lane-B object-format expansion of the runtime metadata layout policy so emitted IR/object paths carry an explicit host-format section and retention model for COFF, ELF, and Mach-O.

## Objective

Expand the normalized `M253-B002` layout policy into an explicit object-format surface. The frontend ABI remains logical and format-neutral; lowering is now responsible for mapping that logical metadata inventory into concrete host-format section spellings and retention-anchor behavior without relaxing fail-closed determinism.

## Required Invariants

1. `lower/objc3_lowering_contract.h` remains the canonical declaration point for:
   - `objc3c-runtime-metadata-object-format-policy/m253-b003-v1`
   - explicit object-format tokens `coff`, `elf`, and `mach-o`
   - explicit section-spelling models for COFF, ELF, and Mach-O
   - explicit retention-anchor models for COFF, ELF, and Mach-O
   - layout-policy fields that carry both logical and emitted section names.
2. `lower/objc3_lowering_contract.cpp` implements fail-closed host-format mapping that:
   - preserves the inherited B001/B002 ordering, relocation, linkage, and retention-root invariants
   - chooses one supported host object format
   - maps logical runtime metadata section names into emitted section spellings
   - keeps COFF and ELF logical names stable on their host paths
   - produces Mach-O `__DATA,__objc3_*` section spellings on Mach-O hosts.
3. `ir/objc3_ir_emitter.cpp` consumes emitted section spellings from the policy packet rather than reusing logical names directly. The emitted IR must publish:
   - the existing named metadata `!objc3.objc_runtime_metadata_layout_policy`
   - the existing `!55` metadata node
   - a replay comment `; runtime_metadata_layout_policy = ...`
   - explicit host-format tokens, section-spelling model, retention-anchor model, and emitted section names.
4. `io/objc3_process.cpp` makes post-write determinism format-aware:
   - detect produced object-file format
   - retain COFF timestamp normalization for COFF objects only
   - avoid silently assuming COFF for ELF or Mach-O objects.
5. Happy-path native emission over `hello.objc3` must still reach `module.obj` while the replay line and emitted IR prove the active host-format mapping.

## Dynamic Coverage

1. Native compile probe over `tests/tooling/fixtures/native/hello.objc3` proves emitted IR/object output carries:
   - `object_format_contract=objc3c-runtime-metadata-object-format-policy/m253-b003-v1`
   - host-format token `coff`, `elf`, or `mach-o`
   - host-format section-spelling model
   - host-format retention-anchor model
   - emitted image-info and family section spellings
   - successful non-empty `module.obj` emission.
2. The object file produced by the native probe must parse as the same host object format recorded in the replay policy.

## Non-Goals and Fail-Closed Rules

- `M253-B003` does not add new emitted metadata families such as methods, selectors, or string pools.
- `M253-B003` does not add runtime registration/bootstrap.
- `M253-B003` does not alter the logical metadata ABI frozen in `M251`/`M252`; it only expands the lowering-side emitted-format mapping surface.
- If the host-format mapping cannot be built, emission must fail closed rather than silently falling back to an implicit neutral model.

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `docs/objc3c-native.md`

## Build and Readiness Integration

- `package.json` includes `check:objc3c:m253-b003-coff-elf-and-mach-o-metadata-policy-surface-core-feature-expansion`.
- `package.json` includes `test:tooling:m253-b003-coff-elf-and-mach-o-metadata-policy-surface-core-feature-expansion`.
- `package.json` includes `check:objc3c:m253-b003-lane-b-readiness`.

## Validation

- `python scripts/check_m253_b003_coff_elf_and_mach_o_metadata_policy_surface_core_feature_expansion.py`
- `python -m pytest tests/tooling/test_check_m253_b003_coff_elf_and_mach_o_metadata_policy_surface_core_feature_expansion.py -q`
- `npm run check:objc3c:m253-b003-lane-b-readiness`

## Evidence Path

- `tmp/reports/m253/M253-B003/coff_elf_and_mach_o_metadata_policy_surface_core_feature_expansion_summary.json`
