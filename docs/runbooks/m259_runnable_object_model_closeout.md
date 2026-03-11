# M259 Runnable Object-Model Closeout Runbook

## Scope

This runbook closes the `M259` runnable object-model slice. It is limited to the current runnable object-model subset and does not claim block/ARC completion.

## Canonical commands

- Build native artifacts:
  - `npm run build:objc3c-native`
- Stage the runnable package:
  - `npm run package:objc3c-native:runnable-toolchain`
- Compile the canonical runnable sample:
  - `pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/objc3c_native_compile.ps1 tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3 --out-dir tmp/artifacts/compilation/objc3c-native/m259/e003/canonical --emit-prefix module`
- Run execution smoke:
  - `pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_native_execution_smoke.ps1`
- Run execution replay proof:
  - `pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_execution_replay_proof.ps1`
- Validate the runnable conformance matrix:
  - `python scripts/check_m259_e002_full_runnable_object_model_conformance_matrix_implementation.py`

## Canonical references

- Canonical runnable sample fixture:
  - `tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3`
- Runnable conformance matrix:
  - `spec/planning/compiler/m259/m259_e002_full_runnable_object_model_conformance_matrix.json`

## Required predecessor evidence

- `tmp/reports/m259/M259-A001/runnable_sample_surface_contract_summary.json`
- `tmp/reports/m259/M259-A002/canonical_runnable_sample_set_summary.json`
- `tmp/reports/m259/M259-B001/runnable_core_compatibility_guard_summary.json`
- `tmp/reports/m259/M259-B002/fail_closed_unsupported_advanced_feature_diagnostics_summary.json`
- `tmp/reports/m259/M259-C001/end_to_end_replay_and_inspection_summary.json`
- `tmp/reports/m259/M259-C002/object_and_ir_replay_proof_plus_metadata_inspection_summary.json`
- `tmp/reports/m259/M259-D001/toolchain_and_runtime_operations_contract_summary.json`
- `tmp/reports/m259/M259-D002/build_install_run_workflow_and_binary_packaging_summary.json`
- `tmp/reports/m259/M259-D003/platform_prerequisites_and_runtime_bring_up_documentation_summary.json`
- `tmp/reports/m259/M259-E001/runnable_object_model_release_gate_contract_summary.json`
- `tmp/reports/m259/M259-E002/full_runnable_object_model_conformance_matrix_summary.json`

## Sign-off statement

- `M259` is closed only when every predecessor summary above is green and the current runnable object-model slice remains limited to the documented non-goals.
- The next implementation issue is `M260-A001`.
