# M252 Inheritance Override Protocol Composition Validation Expectations (B002)

Contract ID: `objc3c-executable-metadata-semantic-validation/m252-b002-v1`

Scope: M252 lane-B core feature implementation for inheritance-chain, override,
protocol-composition, and metaclass-relationship validation on top of the
executable metadata graph.

## Required outcome

`M252-B002` adds one canonical native semantic-validation packet:
`Objc3ExecutableMetadataSemanticValidationSurface`.

The packet consumes the ready B001 semantic-consistency boundary plus the native
executable metadata graph and semantic override/composition handoffs. It must
remain fail-closed and not lowering-ready while still proving the happy path.

## Required anchors

1. `pipeline/objc3_frontend_types.h` defines
   `kObjc3ExecutableMetadataSemanticValidationContractId`,
   `Objc3ExecutableMetadataSemanticValidationSurface`, and
   `IsReadyObjc3ExecutableMetadataSemanticValidationSurface(...)`.
2. `pipeline/objc3_frontend_pipeline.cpp` materializes override edges with
   `method-to-overridden-method` and builds
   `BuildExecutableMetadataSemanticValidationSurface(...)`.
3. `pipeline/objc3_frontend_artifacts.cpp` publishes
   `frontend.pipeline.semantic_surface.objc_executable_metadata_semantic_validation_surface`.
4. The validation packet reports:
   - class inheritance edges,
   - protocol inheritance edges,
   - metaclass super edges,
   - override edges,
   - override lookup/conflict counts,
   - protocol composition counts,
   - fail-closed readiness booleans.
5. The happy-path fixture
   `tests/tooling/fixtures/native/m252_executable_metadata_semantic_validation.objc3`
   proves:
   - one class inheritance edge (`Widget -> Root`),
   - one protocol inheritance edge (`Worker -> BaseWorker`),
   - one metaclass super edge (`metaclass:Widget -> metaclass:Root`),
   - one class-method override edge,
   - one instance-method override edge,
   - zero override conflicts,
   - zero unresolved base interfaces,
   - zero invalid protocol composition sites.

## Non-goals

- `M252-B002` does not add category ambiguity diagnostics.
- `M252-B002` does not add property/ivar export blocking.
- `M252-B002` does not make metadata lowering-ready or runtime-ingest ready.

## Validation and evidence

- `python scripts/check_m252_b002_inheritance_override_protocol_composition_validation.py`
- `python -m pytest tests/tooling/test_check_m252_b002_inheritance_override_protocol_composition_validation.py -q`
- `npm run check:objc3c:m252-b002-lane-b-readiness`
- Evidence path:
  `tmp/reports/m252/M252-B002/executable_metadata_semantic_validation_summary.json`
