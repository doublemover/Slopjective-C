# M227 Wave Execution Runbook

## Contract Anchors

- `objc3c-semantic-pass-performance-quality-guardrails/m227-a011-v1`
- `objc3c-type-system-objc3-forms-diagnostics-hardening/m227-b007-v1`
- `objc3c-typed-sema-to-lowering-modular-split-scaffold/m227-c002-v1`
- `objc3c-runtime-facing-type-metadata-semantics-contract/m227-d001-v1`
- `objc3c-lane-e-semantic-conformance-quality-gate-contract/m227-e001-v1`
- `objc3c-semantic-pass-cross-lane-integration-sync/m227-a012-v1`
- `objc3c-semantic-pass-docs-operator-runbook-sync/m227-a013-v1`

## Operator Command Sequence

1. `npm run build:objc3c-native`
2. `npm run test:objc3c:execution-smoke`
3. `npm run test:objc3c:execution-replay-proof`
4. `python scripts/check_m227_a012_semantic_pass_cross_lane_integration_sync_contract.py`
5. `python scripts/check_m227_a013_semantic_pass_docs_operator_runbook_sync_contract.py`
6. `python -m pytest tests/tooling/test_check_m227_a013_semantic_pass_docs_operator_runbook_sync_contract.py -q`
7. `npm run check:objc3c:m227-a013-lane-a-readiness`

## Evidence

- `tmp/reports/m227/`
