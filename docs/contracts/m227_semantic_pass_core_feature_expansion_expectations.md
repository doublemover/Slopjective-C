# M227 Semantic Pass Core Feature Expansion Expectations (A004)

Contract ID: `objc3c-sema-pass-core-feature-expansion/m227-a004-v1`
Status: Accepted
Scope: expanded pass-flow diagnostics emission accounting, transition graph counts, replay-key determinism, and artifact projection.

## Objective

Expand core semantic pass-flow features to include deterministic diagnostics-emission accounting and replay-key readiness signals for downstream conformance/replay gates.

## Required Invariants

1. Pass-flow summary includes expansion fields:
   - `diagnostics_emitted_by_pass`
   - `transition_edge_count`
   - `diagnostics_emission_totals_consistent`
   - `replay_key_deterministic`
2. Sema manager populates expansion fields before scaffold finalization:
   - `result.sema_pass_flow_summary.diagnostics_emitted_by_pass = result.diagnostics_emitted_by_pass;`
3. Scaffold finalization computes expansion invariants:
   - Emission totals consistency equals `sum(diagnostics_emitted_by_pass) == diagnostics_total`.
   - Transition edge count derived from executed pass count.
   - Replay key deterministic flag enforced from handoff-key prefix and non-placeholder fingerprint.
4. Frontend artifact projection includes expansion fields under `sema_pass_manager`:
   - `pass_flow_diagnostics_emitted_by_build`
   - `pass_flow_diagnostics_emitted_by_validate_bodies`
   - `pass_flow_diagnostics_emitted_by_validate_pure_contract`
   - `pass_flow_transition_edge_count`
   - `pass_flow_diagnostics_emission_totals_consistent`
   - `pass_flow_replay_key_deterministic`

## Validation

- `python scripts/check_m227_a004_semantic_pass_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m227_a004_semantic_pass_core_feature_expansion_contract.py -q`

## Evidence Path

- `tmp/reports/m227/M227-A004/semantic_pass_core_feature_expansion_contract_summary.json`
