# M252 Property Ivar Export Legality Synthesis Preconditions Expectations (B004)

Contract ID: `objc3c-property-ivar-export-legality-synthesis-preconditions/m252-b004-v1`

Scope: M252 lane-B edge-case and compatibility completion for property and ivar
export legality, with a canonical sema-owned property-synthesis/ivar-binding
summary that later lowering and runtime metadata milestones can trust without
re-deriving source shape.

## Required outcome

`M252-B004` closes the remaining property/ivar export legality gap by making the
canonical sema property-synthesis/ivar-binding summary the source of truth for:

- manifest counters under `frontend.pipeline.sema_pass_manager`,
- the semantic surface
  `frontend.pipeline.semantic_surface.objc_property_synthesis_ivar_binding_surface`,
- and the lowering replay key published as
  `lowering_property_synthesis_ivar_binding_replay_key`.

## Required anchors

1. `parse/objc3_parser.cpp` preserves canonical property attribute spelling so
   later export legality checks do not need to reparse property attributes.
2. `sema/objc3_semantic_passes.cpp` documents that class implementation property
   synthesis plus ivar-binding preconditions remain the canonical sema summary
   consumed by later runtime metadata export milestones.
3. `sema/objc3_sema_contract.h` freezes the property-synthesis/ivar-binding
   summary as the canonical sema precondition surface instead of a generic
   property-declaration counter.
4. `pipeline/objc3_frontend_artifacts.cpp` builds the property-synthesis/ivar
   binding lowering contract from `Objc3SemaParityContractSurface`, not from the
   older property-attribute fallback.
5. The happy-path fixture
   `tests/tooling/fixtures/native/m252_b004_class_property_synthesis_ready.objc3`
   succeeds and publishes exactly one resolved default ivar binding with a
   replay key derived from the same sema summary.
6. The category-only fixture
   `tests/tooling/fixtures/native/m252_b004_category_property_export_only.objc3`
   succeeds and proves category property export does not spuriously increment
   class property-synthesis counters.
7. Negative fixtures prove:
   - missing implementation properties rejected by interface matching fail with
     `O3S206`,
   - incompatible implementation property signatures fail with `O3S206`.

## Non-goals

- `M252-B004` does not make metadata lowering-ready.
- `M252-B004` does not add runtime ingest or object-file metadata payloads.
- `M252-B004` does not implement executable property accessors or runtime
  registration.

## Validation and evidence

- `python scripts/check_m252_b004_property_ivar_export_legality_synthesis_preconditions.py`
- `python -m pytest tests/tooling/test_check_m252_b004_property_ivar_export_legality_synthesis_preconditions.py -q`
- `npm run check:objc3c:m252-b004-lane-b-readiness`
- Evidence path:
  `tmp/reports/m252/M252-B004/property_ivar_export_legality_synthesis_preconditions_summary.json`
