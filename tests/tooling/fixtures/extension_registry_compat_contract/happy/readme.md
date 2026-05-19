# Registry Transition/Rejection Validation Suite (`V013-GOV-02`)

This directory documents deterministic validation fixtures for extension
registry transition and rejection governance. Historical `compatibility_*`
field names are fixture keys only; they are not public support, retired route, or
alias contracts.

## 1. Scope and Artifacts

- Acceptance gate: `AC-V013-GOV-02`
- Source contract: `docs/reference/legacy_spec_anchor_index.md`
- Schema fixture: `schema.json`
- Canonical schema ID style: `https://objc3c.dev/schemas/<schema>.schema.json`

## 2. Backward/Forward Change-Rejection Matrix Contract

Change-rejection outcomes use three deterministic values:

- `pass`: accepted for this fixture and publishable.
- `conditional`: replay-only hold with explicit conversion notes; no alternate support.
- `fail`: rejected; publish is blocked.

Required matrix IDs:

- `CM-RC-01` patch nonsemantic change
- `CM-RC-02` minor optional addition
- `CM-RC-03` minor enum expansion
- `CM-RC-04` minor required addition (must fail)
- `CM-RC-05` major required removal
- `CM-RC-06` major required rename without transition mapping (must fail)
- `CM-RC-07` unknown major input (must fail)
- `CM-RC-08` required field type drift (must fail)

## 3. Deterministic Validators

Run from repository root.

| Validator ID | Command | Expected deterministic signal |
| --- | --- | --- |
| `VAL-RC-01` | `npm run objc3c -- lint-spec` | `spec-lint: OK` |
| `VAL-RC-02` | `python scripts/check_issue_checkbox_drift.py` | exit `0` and no blocking drift |
| `VAL-RC-03` | `rg -n "compat|version|schema" docs/reference/legacy_spec_anchor_index.md` | exit `0` |
| `VAL-RC-04` | Validate this fixture directory's `schema.json` as JSON. | `schema-json: OK` |
| `VAL-RC-05` | Check that `schema.json` still exposes the required governance contract keys. | `contract-keys: OK` |
| `VAL-RC-06` | Check this fixture directory's `readme.md` for required governance identifiers. | exit `0` |

Validator ordering is fixed (`VAL-RC-01`..`VAL-RC-06`). Any non-zero exit code is a blocking failure.

## 4. Waiver and Escalation Policy

Waivers are limited to `WVR-RC-01`..`WVR-RC-03` and require owner, rationale,
expiry, and approval evidence.

Non-waiverable classes:

- `CM-RC-07` unknown-major schema input failure
- Required-field breaking change (`CM-RC-04`, `CM-RC-06`, `CM-RC-08`)
- Missing `AC-V013-GOV-02` acceptance mapping
- Validator nondeterminism

Escalation ladder:

- `ESC-RC-01` (`E1`): local validator failure, owner response `T+24h`
- `ESC-RC-02` (`E2`): repeated/major schema-transition failure, response `T+48h`
- `ESC-RC-03` (`E3`): release-window blocker, response `T+72h`
- `ESC-RC-04` (`E4`): integrity or policy breach, immediate emergency hold

## 5. Acceptance Checklist (`AC-V013-GOV-02`)

- [x] `AC-V013-GOV-02-01` Compatibility matrix includes backward and forward outcomes.
- [x] `AC-V013-GOV-02-02` Required-field policy is explicit and deterministic.
- [x] `AC-V013-GOV-02-03` Validator command contract is deterministic.
- [x] `AC-V013-GOV-02-04` Waiver and escalation policy is explicit.
- [x] `AC-V013-GOV-02-05` `npm run objc3c -- lint-spec` transcript is recorded.

## 6. Validation Transcript (`VAL-RC-01`)

```sh
npm run objc3c -- lint-spec
```

Recorded output:

```text
spec-lint: OK
```
