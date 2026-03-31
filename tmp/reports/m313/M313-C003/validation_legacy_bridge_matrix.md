# Validation Legacy Bridge Matrix

- issue: `M313-C003`
- bridge_count: `4`

## Migration-only bridges
- `scripts/check_activation_triggers.py`
  - namespace_bucket: `legacy/bootstrap-preflight`
  - successor_surface: `scripts/run_activation_preflight.py`
  - allowed_caller_count: `2`
- `scripts/check_bootstrap_readiness.py`
  - namespace_bucket: `legacy/bootstrap-preflight`
  - successor_surface: `scripts/run_bootstrap_readiness.py`
  - allowed_caller_count: `2`
- `scripts/check_objc3c_end_to_end_determinism.py`
  - namespace_bucket: `legacy/tooling-utility`
  - successor_surface: `future developer-tooling workflow integration under validate-developer-tooling`
  - allowed_caller_count: `2`
- `scripts/check_objc3c_library_cli_parity.py`
  - namespace_bucket: `legacy/tooling-utility`
  - successor_surface: `future developer-tooling workflow integration under validate-developer-tooling`
  - allowed_caller_count: `4`

Next issue: `M313-D002`
