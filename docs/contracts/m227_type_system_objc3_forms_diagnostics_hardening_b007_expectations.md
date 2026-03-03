# M227 Type-System Completeness for ObjC3 Forms Diagnostics Hardening Expectations (B007)

Contract ID: `objc3c-type-system-objc3-forms-diagnostics-hardening/m227-b007-v1`
Status: Accepted
Scope: lane-B type-system diagnostics hardening closure on top of B006 edge robustness.

## Objective

Execute issue `#4848` by extending canonical ObjC3 type-form scaffolding with
explicit diagnostics-hardening consistency/readiness and deterministic key
continuity so sema/type metadata handoff fails closed on diagnostics drift.

## Dependency Scope

- Dependencies: `M227-B006`
- `M227-B006` remains a mandatory prerequisite:
  - `docs/contracts/m227_type_system_objc3_forms_edge_robustness_b006_expectations.md`
  - `scripts/check_m227_b006_type_system_objc3_forms_edge_robustness_contract.py`
  - `tests/tooling/test_check_m227_b006_type_system_objc3_forms_edge_robustness_contract.py`
  - `spec/planning/compiler/m227/m227_b006_type_system_objc3_forms_edge_robustness_packet.md`

## Deterministic Invariants

1. `Objc3TypeFormScaffoldSummary` carries diagnostics hardening fields:
   - `diagnostics_hardening_consistent`
   - `diagnostics_hardening_ready`
   - `diagnostics_hardening_key`
2. Scaffold synthesis computes diagnostics hardening from canonical
   reference/scalar/bridge-top invariants and emits deterministic key material.
3. `Objc3IdClassSelObjectPointerTypeCheckingSummary` carries:
   - `canonical_type_form_diagnostics_hardening_consistent`
   - `canonical_type_form_diagnostics_hardening_ready`
   - `canonical_type_form_diagnostics_hardening_key`
4. Integration and type-metadata handoff determinism requires diagnostics
   hardening consistency/readiness and non-empty key continuity in addition to
   prior B006 invariants.
5. Readiness remains fail-closed when diagnostics hardening consistency,
   readiness, or key continuity drifts.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m227-b007-type-system-objc3-forms-diagnostics-hardening-contract`
  - `test:tooling:m227-b007-type-system-objc3-forms-diagnostics-hardening-contract`
  - `check:objc3c:m227-b007-lane-b-readiness`
- lane-B readiness chaining preserves B006 continuity:
  - `check:objc3c:m227-b006-lane-b-readiness`
  - `check:objc3c:m227-b007-lane-b-readiness`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M227 lane-B B007
  diagnostics hardening anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-B B007 fail-closed
  diagnostics hardening wiring text.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-B B007
  diagnostics hardening metadata anchors.

## Validation

- `python scripts/check_m227_b007_type_system_objc3_forms_diagnostics_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m227_b007_type_system_objc3_forms_diagnostics_hardening_contract.py -q`
- `npm run check:objc3c:m227-b007-lane-b-readiness`

## Evidence Path

- `tmp/reports/m227/M227-B007/type_system_objc3_forms_diagnostics_hardening_contract_summary.json`
