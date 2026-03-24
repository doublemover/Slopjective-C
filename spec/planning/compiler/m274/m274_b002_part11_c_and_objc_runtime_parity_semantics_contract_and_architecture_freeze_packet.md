# M274-B002 Packet: Part 11 C And Objective-C Runtime Parity Semantics - Contract And Architecture Freeze

Packet: `M274-B002`
Milestone: `M274`
Lane: `B`
Issue: `#7364`
Dependencies: `M274-B001`, `M274-A001`, `M274-A002`, `M267-E002`, `M268-E002`, `M270-E002`
Next issue: downstream M274 runtime/lowering work

## Objective

Freeze the Part 11 runtime-parity semantic model for C and Objective-C callable surfaces while keeping ownership, error, concurrency, and metadata interactions deterministic and fail-closed.

## Required Outputs

- semantic surface `frontend.pipeline.semantic_surface.objc_part11_c_and_objc_runtime_parity_semantics`
- issue-local checker `scripts/check_m274_b002_part11_c_and_objc_runtime_parity_semantics_contract_and_architecture_freeze.py`
- issue-local readiness runner `scripts/run_m274_b002_lane_b_readiness.py`
- issue-local pytest `tests/tooling/test_check_m274_b002_part11_c_and_objc_runtime_parity_semantics_contract_and_architecture_freeze.py`

## Canonical payload expectations

The manifest packet is expected to expose these stable facts:

- callability counts:
  - `foreign_callable_sites`
  - `c_foreign_callable_sites`
  - `objc_method_foreign_callable_sites`
  - `import_module_annotation_sites`
  - `import_module_foreign_callable_sites`
  - `objc_runtime_parity_callable_sites`
  - `foreign_definition_rejection_sites`
  - `import_without_foreign_rejection_sites`
  - `implementation_annotation_rejection_sites`
- frozen / deferred state:
  - `dependency_required`
  - `declaration_only_foreign_c_enforced`
  - `import_module_requires_foreign_enforced`
  - `implementation_annotations_fail_closed`
  - `objc_runtime_parity_classified`
  - `ffi_abi_lowering_deferred`
  - `runtime_bridge_generation_deferred`
  - `deterministic`
  - `ready_for_lowering_and_runtime`

## Negative diagnostics

- `O3S331` foreign function must be declaration-only / extern surface
- `O3S332` `objc_import_module` requires `objc_foreign`
- `O3S333` implementation methods/categories cannot use Part 11 foreign/import annotations

## Fixture shape

The positive fixture should compile with the frontend runner using manifest emission only and should resemble:

- 2 foreign free functions
- 2 foreign Objective-C method declarations on interface / protocol surfaces
- 2 import-module annotations that also use `objc_foreign`
- 3 Objective-C runtime-parity callable sites
- no rejection sites in the positive corpus

The negative corpus should isolate one diagnosis per fixture so the checker can keep the fail-closed boundary deterministic.
