# Experimental Extension Registry v1 (`C-11`)

_Canonical registry governance documentation - 2026-02-23._

This README is the canonical operator and consumer contract for the experimental
extension registry used by governance task `C-11`.

Primary artifacts:

- `registries/experimental_extensions/index.schema.json`
- `registries/experimental_extensions/README.md`
- `tests/governance/registry_compat/README.md`

## 1. Scope and Authority

This registry contract defines:

1. required JSON structure and field contracts,
2. lifecycle and compatibility metadata obligations,
3. deterministic validator command contract,
4. publication and escalation behavior for governance operations.

Policy overlays consumed by this registry:

- `C-02`: `spec/governance/capability_namespace_policy_v1.md`
- `C-05`: `spec/governance/extension_lifecycle_v1.md`
- `C-11`: registry format and publication obligations from
  `spec/planning/issue_176_extension_registry_format_package.md`

## 2. Top-Level Schema Contract

The top-level registry object requires these fields:

| Field | Type | Requirement |
| --- | --- | --- |
| `schema_version` | semver string | Required. |
| `registry_id` | string | Required and fixed to `experimental_extensions`. |
| `generated_at_utc` | RFC3339 timestamp | Required. |
| `governance_contract` | object | Required. |
| `extensions` | array | Required; at least one row. |

Top-level behavior:

1. additional top-level properties are forbidden by schema,
2. unknown or incompatible schema versions are fail-closed in governed mode,
3. every published snapshot must pass all required validators in Section 6.

## 3. Extension Row Contract

Each `extensions[*]` row requires:

| Field | Type | Requirement |
| --- | --- | --- |
| `extension_id` | string | Required. |
| `extension_version` | semver string | Required. |
| `lifecycle_state` | enum | Required; one of `experimental`, `provisional`, `stable`, `deprecated`, `retired`. |
| `compatibility` | object | Required. |

`compatibility` requires:

| Field | Type | Requirement |
| --- | --- | --- |
| `min_consumer_schema` | semver string | Required. |
| `max_consumer_schema` | semver string | Required. |
| `backward_policy` | enum | Required; `compatible`, `conditional`, or `incompatible`. |
| `forward_policy` | enum | Required; `compatible`, `conditional`, or `incompatible`. |
| `required_fields` | array[string] | Required; non-empty and unique. |

## 4. Policy Overlay Rules (`C-02`, `C-05`)

Schema acceptance is necessary but not sufficient. Governance policy validation
must also pass:

| Overlay rule ID | Rule |
| --- | --- |
| `OVL-01` | `extension_id` must satisfy `C-02` canonical namespace rules, even when schema regex is more permissive. |
| `OVL-02` | `lifecycle_state` must be coherent with namespace class and transition posture from `C-05`. |
| `OVL-03` | `stable` claims require portable namespace posture (`objc3.meta.*`) as defined by `C-02` and `C-05`. |
| `OVL-04` | `deprecated` and `retired` rows must preserve non-reuse identity behavior. |

## 5. Governance Contract Block Requirements

The required `governance_contract` object must contain:

| Field | Requirement |
| --- | --- |
| `contract_version` | Required semantic version. |
| `acceptance_gate_id` | Required and fixed to `AC-V013-GOV-02`. |
| `compatibility_matrix` | Required array with at least eight rows. |
| `required_field_policy` | Required object. |
| `validators` | Required array with at least six rows. |
| `waiver_policy` | Required object. |
| `acceptance_checklist` | Required array with at least five rows. |

### 5.1 Compatibility matrix IDs

Required matrix rows:

- `CM-RC-01` patch non-semantic change
- `CM-RC-02` minor optional addition
- `CM-RC-03` minor enum expansion
- `CM-RC-04` minor required addition
- `CM-RC-05` major required removal
- `CM-RC-06` major required rename without alias
- `CM-RC-07` unknown major input
- `CM-RC-08` required field type drift

### 5.2 Required-field policy invariants

`required_field_policy` enforces:

1. missing required fields are hard failures,
2. adding required fields is major-only,
3. removing required fields is major-only with migration,
4. incompatible required-field type changes are hard failures.

### 5.3 Validator IDs

Validator rows must use:

- `VAL-RC-01`
- `VAL-RC-02`
- `VAL-RC-03`
- `VAL-RC-04`
- `VAL-RC-05`
- `VAL-RC-06`

### 5.4 Waiver and escalation contract

Waiver policy requires:

1. waiver IDs that match `WVR-RC-[0-9]{2}`,
2. explicit owner and expiry metadata,
3. hard block on expired waiver.

Escalation levels in contract are fixed to `E1`, `E2`, `E3`, and `E4`.

## 6. Deterministic Validator Contract

Run from repository root:

| Validator ID | Command | Pass signal |
| --- | --- | --- |
| `VAL-RC-01` | `python scripts/spec_lint.py` | Output includes `spec-lint: OK`; exit `0`. |
| `VAL-RC-02` | `python scripts/check_issue_checkbox_drift.py` | Exit `0` and no blocking drift. |
| `VAL-RC-03` | `rg -n "compat\|version\|schema" spec/planning/v013_extension_registry_compat_validation_package.md` | Exit `0`. |
| `VAL-RC-04` | `python -c "import json,pathlib;json.loads(pathlib.Path('registries/experimental_extensions/index.schema.json').read_text(encoding='utf-8'));print('schema-json: OK')"` | Output includes `schema-json: OK`; exit `0`. |
| `VAL-RC-05` | `python -c 'import json,pathlib,sys;d=json.loads(pathlib.Path("registries/experimental_extensions/index.schema.json").read_text(encoding="utf-8"));p=d["$defs"]["governance_contract"]["properties"];need={"compatibility_matrix","required_field_policy","validators","waiver_policy","acceptance_checklist"};m=sorted(need-set(p));print("contract-keys: OK" if not m else "contract-keys: MISSING "+",".join(m));sys.exit(0 if not m else 1)'` | Output includes `contract-keys: OK`; exit `0`. |
| `VAL-RC-06` | `rg -n "AC-V013-GOV-02\|VAL-RC-\|ESC-RC-" tests/governance/registry_compat/README.md` | Exit `0`. |

Validator ordering is fixed (`VAL-RC-01` through `VAL-RC-06`).

## 7. Publication Workflow

| Stage ID | Stage | Required output |
| --- | --- | --- |
| `WF-RC-01` | Intake and row preparation | Candidate snapshot with complete top-level and extension-row fields. |
| `WF-RC-02` | Schema and compatibility validation | Full validator run with archived outputs and exit codes. |
| `WF-RC-03` | Governance review | Recorded disposition and any approved waiver records. |
| `WF-RC-04` | Publication | Published snapshot and immutable revision reference. |
| `WF-RC-05` | Post-publish verification | Replay validation and consumer notification record. |

Release-critical failures escalate via `E1` through `E4` with fail-closed posture.

## 8. Failure Taxonomy

| Failure code | Trigger | Required disposition |
| --- | --- | --- |
| `REGVAL-001` | Missing required structural field. | Reject publish. |
| `REGVAL-002` | Namespace violation or reserved-prefix misuse. | Reject row and require corrected identifier. |
| `REGVAL-003` | Lifecycle metadata inconsistency. | Reject update and require corrected lifecycle metadata. |
| `REGVAL-004` | Governance linkage gap. | Reject publish until decision linkage is complete. |
| `REGVAL-005` | Evidence reference gap. | Reject publish until evidence is complete. |
| `REGVAL-006` | Compatibility envelope error. | Reject row until corrected. |
| `REGVAL-007` | Determinism, digest, or signature mismatch. | Block publish and escalate for integrity triage. |
| `REGVAL-008` | Monotonicity violation in publication revisioning. | Reject publish and regenerate snapshot. |

## 9. Minimal Producer Example

```json
{
  "schema_version": "1.0.0",
  "registry_id": "experimental_extensions",
  "generated_at_utc": "2026-02-23T12:00:00Z",
  "governance_contract": {
    "contract_version": "1.0.0",
    "acceptance_gate_id": "AC-V013-GOV-02",
    "compatibility_matrix": [],
    "required_field_policy": {
      "required_fields": ["extension_id", "extension_version", "lifecycle_state", "compatibility", "governance_contract"],
      "missing_required_field_behavior": "hard_fail",
      "added_required_field_policy": "major_only",
      "removed_required_field_policy": "major_only_with_migration",
      "incompatible_type_change_behavior": "hard_fail"
    },
    "validators": [],
    "waiver_policy": {
      "waiver_classes": [],
      "non_waiverable_conditions": ["unknown_major_input"],
      "escalation_levels": [],
      "max_waiver_duration_days": 14,
      "requires_owner_and_expiry": true,
      "hard_block_on_expired_waiver": true
    },
    "acceptance_checklist": []
  },
  "extensions": [
    {
      "extension_id": "vendor.acme.meta.fast_hash",
      "extension_version": "0.4.2",
      "lifecycle_state": "provisional",
      "compatibility": {
        "min_consumer_schema": "1.0.0",
        "max_consumer_schema": "1.1.0",
        "backward_policy": "compatible",
        "forward_policy": "conditional",
        "required_fields": ["extension_id", "extension_version", "lifecycle_state", "compatibility"]
      }
    }
  ]
}
```

The example is illustrative only. Valid publication requires all schema minima
from `index.schema.json`, including minimum row counts for governance-contract
arrays, and policy overlays from Section 4.
