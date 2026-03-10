# M257 Property Synthesis and Default Ivar Binding Full Semantics Core Feature Implementation Expectations (B002)

Contract ID: `objc3c-property-default-ivar-binding-semantics/m257-b002-v1`
Status: Accepted
Issue: `#7148`
Scope: `M257` lane-B implementation of interface-driven property synthesis and authoritative default ivar binding resolution.

## Objective

Implement live sema behavior so matched class implementations resolve default
ivar bindings from interface-declared properties even when the implementation
omits `@property` redeclarations entirely.

## Required implementation

1. Class-interface properties remain the authoritative synthesis inventory for matched class implementations.
2. Implementation-side `@property` redeclarations become optional overlays rather than required synthesis sites.
3. Optional implementation redeclarations must remain signature-compatible and default-ivar-binding-compatible with the interface-owned property.
4. Category implementations remain outside default ivar synthesis.
5. Canonical proof fixtures must include:
   - `tests/tooling/fixtures/native/m257_property_synthesis_default_ivar_binding_no_redeclaration.objc3`
   - `tests/tooling/fixtures/native/m257_property_ivar_source_model_completion_positive.objc3`
   - `tests/tooling/fixtures/native/m257_property_synthesis_default_ivar_binding_incompatible_redeclaration.objc3`
6. `package.json` must wire:
   - `check:objc3c:m257-b002-property-synthesis-and-default-ivar-binding-full-semantics`
   - `test:tooling:m257-b002-property-synthesis-and-default-ivar-binding-full-semantics`
   - `check:objc3c:m257-b002-lane-b-readiness`
7. Validation evidence lands at `tmp/reports/m257/M257-B002/property_synthesis_default_ivar_binding_full_semantics_summary.json`.

## Required diagnostics

- Incompatible implementation redeclaration => `O3S206`
- Missing authoritative default ivar binding => `O3S206`
- Binding drift between interface and implementation redeclaration => `O3S206`

## Non-goals

- No explicit `@synthesize` / `@dynamic` support yet.
- No synthesized accessor body emission yet.
- No runtime allocation or instance-layout realization yet.
- No category accessor execution semantics.

## Validation

- `python scripts/check_m257_b002_property_synthesis_and_default_ivar_binding_full_semantics_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m257_b002_property_synthesis_and_default_ivar_binding_full_semantics_core_feature_implementation.py -q`
- `npm run check:objc3c:m257-b002-lane-b-readiness`

## Evidence Path

- `tmp/reports/m257/M257-B002/property_synthesis_default_ivar_binding_full_semantics_summary.json`
