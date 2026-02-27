# Registry Compatibility Validation Suite (`V013-GOV-02`)

This directory documents the deterministic validation contract for extension
registry compatibility governance.

## 1. Scope and Artifacts

- Acceptance gate: `AC-V013-GOV-02`
- Source contract: `spec/planning/v013_extension_registry_compat_validation_package.md`
- Schema artifact: `registries/experimental_extensions/index.schema.json`

## 2. Backward/Forward Compatibility Matrix Contract

Compatibility outcomes use three deterministic values:

- `pass`: compatible and publishable.
- `conditional`: compatible only with explicit fallback/migration notes.
- `fail`: incompatible; publish is blocked.

Required matrix IDs:

- `CM-RC-01` patch nonsemantic change
- `CM-RC-02` minor optional addition
- `CM-RC-03` minor enum expansion
- `CM-RC-04` minor required addition (must fail)
- `CM-RC-05` major required removal
- `CM-RC-06` major required rename without alias (must fail)
- `CM-RC-07` unknown major input (must fail)
- `CM-RC-08` required field type drift (must fail)

## 3. Deterministic Validators

Run from repository root.

| Validator ID | Command | Expected deterministic signal |
| --- | --- | --- |
| `VAL-RC-01` | `python scripts/spec_lint.py` | `spec-lint: OK` |
| `VAL-RC-02` | `python scripts/check_issue_checkbox_drift.py` | exit `0` and no blocking drift |
| `VAL-RC-03` | `rg -n "compat\|version\|schema" spec/planning/v013_extension_registry_compat_validation_package.md` | exit `0` |
| `VAL-RC-04` | `python -c "import json,pathlib;json.loads(pathlib.Path('registries/experimental_extensions/index.schema.json').read_text(encoding='utf-8'));print('schema-json: OK')"` | `schema-json: OK` |
| `VAL-RC-05` | `python -c 'import json,pathlib,sys;d=json.loads(pathlib.Path("registries/experimental_extensions/index.schema.json").read_text(encoding="utf-8"));p=d["$defs"]["governance_contract"]["properties"];need={"compatibility_matrix","required_field_policy","validators","waiver_policy","acceptance_checklist"};m=sorted(need-set(p));print("contract-keys: OK" if not m else "contract-keys: MISSING "+",".join(m));sys.exit(0 if not m else 1)'` | `contract-keys: OK` |
| `VAL-RC-06` | `rg -n "AC-V013-GOV-02\|VAL-RC-\|ESC-RC-" tests/governance/registry_compat/README.md` | exit `0` |

Validator ordering is fixed (`VAL-RC-01`..`VAL-RC-06`). Any non-zero exit code is a blocking failure.

## 4. Waiver and Escalation Policy

Waivers are limited to `WVR-RC-01`..`WVR-RC-03` and require owner, rationale,
expiry, and approval evidence.

Non-waiverable classes:

- `CM-RC-07` unknown-major compatibility failure
- Required-field breaking change (`CM-RC-04`, `CM-RC-06`, `CM-RC-08`)
- Missing `AC-V013-GOV-02` acceptance mapping
- Validator nondeterminism

Escalation ladder:

- `ESC-RC-01` (`E1`): local validator failure, owner response `T+24h`
- `ESC-RC-02` (`E2`): repeated/major compatibility failure, response `T+48h`
- `ESC-RC-03` (`E3`): release-window blocker, response `T+72h`
- `ESC-RC-04` (`E4`): integrity or policy breach, immediate emergency hold

## 5. Acceptance Checklist (`AC-V013-GOV-02`)

- [x] `AC-V013-GOV-02-01` Compatibility matrix includes backward and forward outcomes.
- [x] `AC-V013-GOV-02-02` Required-field policy is explicit and deterministic.
- [x] `AC-V013-GOV-02-03` Validator command contract is deterministic.
- [x] `AC-V013-GOV-02-04` Waiver and escalation policy is explicit.
- [x] `AC-V013-GOV-02-05` `python scripts/spec_lint.py` transcript is recorded.

## 6. Validation Transcript (`VAL-RC-01`)

```sh
python scripts/spec_lint.py
```

Recorded output:

```text
spec-lint: OK
```
