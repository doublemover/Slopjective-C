# M263 Runnable Multi-Image Bootstrap Matrix Cross-Lane Integration Sync Expectations (E002)

Contract ID: `objc3c-runtime-runnable-bootstrap-matrix-closeout/m263-e002-v1`

## Scope

`M263-E002` closes the bootstrap-hardening milestone with one published operator runbook and one stable bootstrap matrix summary that proves the current runnable startup surface across:

- single-image startup and restart
- archive-backed startup with no retained metadata discovery flags
- archive-backed startup with one retained image
- archive-backed startup with merged retained multi-image startup

## Required outputs

- `docs/runbooks/m263_bootstrap_matrix_operator_runbook.md`
- `scripts/check_objc3c_bootstrap_matrix.ps1`
- `scripts/check_m263_e002_runnable_multi_image_bootstrap_matrix_cross_lane_integration_sync.py`
- `tests/tooling/test_check_m263_e002_runnable_multi_image_bootstrap_matrix_cross_lane_integration_sync.py`
- `tmp/reports/m263/M263-E002/bootstrap_matrix_closeout_summary.json`

## Closeout rule

The closeout is invalid unless:

- `M263-E001` still passes and remains the authoritative lane-E gate.
- `M263-C003` still proves the archive-backed single-retained and merged retained multi-image replay paths.
- `M263-D003` still proves deterministic single-image restart hardening.
- the published operator runbook drives `scripts/check_objc3c_bootstrap_matrix.ps1` and that script emits a stable matrix summary under `tmp/artifacts/objc3c-native/bootstrap-matrix/`.
