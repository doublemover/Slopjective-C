# M251-C003 Metadata Object Inspection Harness Conformance Matrix Implementation Packet

Packet: `M251-C003`
Milestone: `M251`
Lane: `C`
Issue: `#7064`

## Objective

Publish a deterministic inspection matrix for emitted runtime metadata scaffold objects so operator tooling and later runtime milestones can prove object layout with real `llvm-readobj`/`llvm-objdump` evidence instead of ad hoc inspection.

## Dependencies

- `M251-C002`

## Required implementation

1. Add a canonical `Objc3RuntimeMetadataObjectInspectionHarnessSummary` that depends on the `M251-C002` scaffold packet.
2. Project the inspection harness summary into manifest JSON and `Objc3IRFrontendMetadata`.
3. Publish the same contract into emitted LLVM IR as `!objc3.objc_runtime_metadata_object_inspection`.
4. Freeze the zero-descriptor inspection fixture at `tests/tooling/fixtures/native/m251_runtime_metadata_object_inspection_zero_descriptor.objc3`.
5. Freeze the emitted object-relative path at `module.obj` with emit-prefix `module`.
6. Publish matrix row `zero-descriptor-section-inventory` mapped to `llvm-readobj --sections module.obj`.
7. Publish matrix row `zero-descriptor-symbol-inventory` mapped to `llvm-objdump --syms module.obj`.
8. Add deterministic checker and tooling tests that compile the fixture, inspect the emitted object, and fail closed when the matrix drifts.

## Determinism and fail-closed rules

- `M251-C003` remains gated on the `M251-C002` scaffold packet and must not publish partial inspection rows when scaffold prerequisites are not ready.
- The inspection harness validates emitted object structure only; it does not claim runtime registration, live lookup, or executable object-model payload lowering.
- The canonical matrix rows are frozen operator surfaces rather than ad hoc shell guidance.

## Validation plan

The checker runs two deterministic probes:

1. Native compile probe on `tests/tooling/fixtures/native/m251_runtime_metadata_object_inspection_zero_descriptor.objc3` proving the manifest publishes the inspection matrix contract.
2. `llvm-readobj --sections` and `llvm-objdump --syms` probes on the emitted `module.obj` proving the metadata sections and retained scaffold symbols match the matrix.

## Evidence

- `tmp/reports/m251/M251-C003/runtime_metadata_object_inspection_harness_summary.json`
