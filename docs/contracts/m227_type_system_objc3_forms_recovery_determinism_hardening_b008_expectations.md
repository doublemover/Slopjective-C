# M227 Type-System Completeness for ObjC3 Forms Recovery and Determinism Hardening Expectations (B008)

Contract ID: `objc3c-type-system-objc3-forms-recovery-determinism-hardening/m227-b008-v1`
Status: Accepted
Scope: lane-B type-system recovery/determinism hardening closure on top of B007 diagnostics hardening.

## Objective

Execute issue `#4849` by extending canonical ObjC3 type-form scaffolding with
explicit recovery/determinism consistency/readiness and deterministic key
continuity so sema/type metadata handoff remains deterministic and fails closed
when recovery continuity drifts.

## Dependency Scope

- Dependencies: `M227-B007`
- `M227-B007` remains a mandatory prerequisite:
  - `docs/contracts/m227_type_system_objc3_forms_diagnostics_hardening_b007_expectations.md`
  - `scripts/check_m227_b007_type_system_objc3_forms_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m227_b007_type_system_objc3_forms_diagnostics_hardening_contract.py`
  - `spec/planning/compiler/m227/m227_b007_type_system_objc3_forms_diagnostics_hardening_packet.md`

## Deterministic Invariants

1. `Objc3TypeFormScaffoldSummary` carries recovery/determinism hardening fields:
   - `recovery_determinism_consistent`
   - `recovery_determinism_ready`
   - `recovery_determinism_key`
2. Scaffold synthesis computes recovery/determinism continuity from B007
   diagnostics hardening consistency/readiness and diagnostics-key continuity.
3. `Objc3IdClassSelObjectPointerTypeCheckingSummary` carries:
   - `canonical_type_form_recovery_determinism_consistent`
   - `canonical_type_form_recovery_determinism_ready`
   - `canonical_type_form_recovery_determinism_key`
4. Integration and type-metadata handoff determinism requires recovery
   consistency/readiness and non-empty recovery-key continuity in addition to
   prior B007 invariants.
5. Readiness remains fail-closed when recovery consistency, readiness, or key
   continuity drifts.

## Build and Readiness Integration

- `package.json` includes:
  - `check:objc3c:m227-b008-type-system-objc3-forms-recovery-determinism-hardening-contract`
  - `test:tooling:m227-b008-type-system-objc3-forms-recovery-determinism-hardening-contract`
  - `check:objc3c:m227-b008-lane-b-readiness`
- lane-B readiness chaining preserves B007 continuity:
  - `check:objc3c:m227-b007-lane-b-readiness`
  - `check:objc3c:m227-b008-lane-b-readiness`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` includes explicit M227 lane-B B008
  recovery and determinism hardening anchor text.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` includes lane-B B008 fail-closed
  recovery/determinism wiring text.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` includes deterministic lane-B B008
  recovery/determinism metadata anchors.

## Validation

- `python scripts/check_m227_b007_type_system_objc3_forms_diagnostics_hardening_contract.py`
- `python scripts/check_m227_b008_type_system_objc3_forms_recovery_determinism_hardening_contract.py`
- `python -m pytest tests/tooling/test_check_m227_b008_type_system_objc3_forms_recovery_determinism_hardening_contract.py -q`
- `npm run check:objc3c:m227-b008-lane-b-readiness`

## Evidence Path

- `tmp/reports/m227/M227-B008/type_system_objc3_forms_recovery_determinism_hardening_contract_summary.json`
