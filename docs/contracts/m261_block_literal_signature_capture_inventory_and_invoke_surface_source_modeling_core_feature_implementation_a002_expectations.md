# M261 Block Literal Signature Capture Inventory And Invoke Surface Source Modeling Core Feature Implementation Expectations (A002)

Contract ID: `objc3c-executable-block-source-model-completion/m261-a002-v1`

## Intent

`M261-A002` upgrades the frozen `M261-A001` block source closure into a
truthful source-only frontend capability that models:

- block parameter signatures,
- capture inventory and storage class,
- invoke-surface symbols and replay keys.

Native emit paths must still fail closed on runnable block lowering with
`O3S221`.

## Required artifacts

- `docs/contracts/m261_block_literal_signature_capture_inventory_and_invoke_surface_source_modeling_core_feature_implementation_a002_expectations.md`
- `spec/planning/compiler/m261/m261_a002_block_literal_signature_capture_inventory_and_invoke_surface_source_modeling_core_feature_implementation_packet.md`
- `scripts/check_m261_a002_block_literal_signature_capture_inventory_and_invoke_surface_source_modeling_core_feature_implementation.py`
- `tests/tooling/test_check_m261_a002_block_literal_signature_capture_inventory_and_invoke_surface_source_modeling_core_feature_implementation.py`
- `tests/tooling/fixtures/native/m261_block_source_model_completion_positive.objc3`

## Static requirements

1. Parser/AST code anchors must publish the completed source model on `Expr`.
2. Frontend pipeline/sema must admit block literals only for source-only runs.
3. Frontend artifacts must publish the
   `objc_block_source_model_completion_surface` manifest object.
4. LLVM IR must emit the
   `; executable_block_source_model_completion = ...` boundary comment.
5. Docs/spec/architecture/package wiring must remain explicit.

## Dynamic requirements

1. `artifacts/bin/objc3c-frontend-c-api-runner.exe` with `--no-emit-ir --no-emit-object`
   must accept the positive fixture and emit a manifest.
2. That manifest must publish a deterministic completed block source model with:
   - one block literal site,
   - three signature entries,
   - two explicit typed parameters,
   - one implicit parameter,
   - two capture inventory entries,
   - two by-value readonly captures,
   - two invoke-surface entries,
   - zero contract violations.
3. The same fixture through `artifacts/bin/objc3c-native.exe` must still fail
   with `O3S221`.
4. Validation evidence must land under:
   `tmp/reports/m261/M261-A002/`

## Handoff

`M261-B001` is the next issue.
