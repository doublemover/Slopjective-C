# Legacy Validation Surface Map

- issue: `M313-B003`
- policy_id: `validation-consolidation-policy-v1`
- legacy_surface_count: `7`

## State counts
- `active`: `3`
- `migration-only`: `4`

## Classified surfaces
- `scripts/check_activation_triggers.py`
  - state: `migration-only`
  - namespace_bucket: `legacy/bootstrap-preflight`
  - successor_surface: `scripts/run_activation_preflight.py`
  - reference_count: `2`
  - inventory_gap: `false`
- `scripts/check_bootstrap_readiness.py`
  - state: `migration-only`
  - namespace_bucket: `legacy/bootstrap-preflight`
  - successor_surface: `scripts/run_bootstrap_readiness.py`
  - reference_count: `2`
  - inventory_gap: `false`
- `scripts/check_conformance_corpus_surface.py`
  - state: `active`
  - namespace_bucket: `retained-static/source-surface-contract`
  - successor_surface: `scripts/check_objc3c_conformance_corpus_integration.py + validate-conformance-corpus`
  - reference_count: `8`
  - inventory_gap: `true`
- `scripts/check_getting_started_surface.py`
  - state: `active`
  - namespace_bucket: `active/child-executable`
  - successor_surface: `scripts/check_getting_started_integration.py + validate-getting-started`
  - reference_count: `9`
  - inventory_gap: `true`
- `scripts/check_objc3c_end_to_end_determinism.py`
  - state: `migration-only`
  - namespace_bucket: `legacy/tooling-utility`
  - successor_surface: `future developer-tooling workflow integration under validate-developer-tooling`
  - reference_count: `2`
  - inventory_gap: `false`
- `scripts/check_objc3c_library_cli_parity.py`
  - state: `migration-only`
  - namespace_bucket: `legacy/tooling-utility`
  - successor_surface: `future developer-tooling workflow integration under validate-developer-tooling`
  - reference_count: `4`
  - inventory_gap: `false`
- `scripts/check_open_blocker_audit_contract.py`
  - state: `active`
  - namespace_bucket: `active/internal-contract`
  - successor_surface: `scripts/run_open_blocker_audit.py`
  - reference_count: `3`
  - inventory_gap: `true`

Next issues: `M313-C001`, `M313-C003`
