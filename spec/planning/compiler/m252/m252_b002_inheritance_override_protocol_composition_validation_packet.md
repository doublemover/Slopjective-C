# M252-B002 Inheritance Override Protocol Composition Validation Packet

Packet: `M252-B002`
Milestone: `M252`
Lane: `B`

## Objective

Implement executable-metadata semantic validation for inheritance chains,
override legality, protocol composition, and metaclass relationships as a real
native pipeline capability rather than a manifest-only summary.

## Dependencies

- `M252-B001`
- `M252-A002`

## Required anchors

- `docs/contracts/m252_inheritance_override_protocol_composition_validation_b002_expectations.md`
- `tests/tooling/fixtures/native/m252_executable_metadata_semantic_validation.objc3`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/sema/objc3_sema_contract.h`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `package.json`

## Acceptance

- `Objc3ExecutableMetadataSemanticValidationSurface` is emitted by the native
  pipeline and manifest.
- The executable metadata graph carries `method-to-overridden-method` owner
  edges for legal class/interface overrides on the happy path.
- The semantic-validation packet is `ready=true`, fail-closed, and
  `lowering_admission_ready == false`.
- Deterministic checker, pytest coverage, and lane-B readiness exist.
- Evidence lands under `tmp/reports/m252/M252-B002/`.

## Commands

- `python scripts/check_m252_b002_inheritance_override_protocol_composition_validation.py`
- `python -m pytest tests/tooling/test_check_m252_b002_inheritance_override_protocol_composition_validation.py -q`
- `npm run check:objc3c:m252-b002-lane-b-readiness`
