# V013-GOV-02 Package: Extension Registry Compatibility Validation Suite {#v013-gov-02-extension-registry-compat-validation-package}

_Published v0.13 - 2026-02-23_

Status: published package for issue `#786` (`[W1][V013-GOV-02] Add extension registry compatibility validation suite`) in batch `BATCH-20260223-11R`. This package publishes the extension registry compatibility validation suite contract and closeout controls mapped to `AC-V013-GOV-02`.

## 1. Issue Contract and Scope

### 1.1 Canonical reseed contract (`#786`)

| Field | Value |
| --- | --- |
| `issue` | `#786` |
| `seed_id` | `V013-GOV-02` |
| `wave_id` | `W1` |
| `family_tag` | `FAM-GOV` |
| `worklane` | `WL-GOV` |
| `depends_on` | `V013-GOV-01` |
| `shard_class` | `medium` |
| `acceptance_gate_id` | `AC-V013-GOV-02` |
| `batch_id` | `BATCH-20260223-11R` |
| `milestone_id` | `#31` |
| `milestone_title` | `v0.13 Seed Wave W1 Reseed 1` |
| `artifact_targets` | `spec/planning/v013_extension_registry_compat_validation_package.md`, `spec/planning/evidence/lane_c/v013_seed_gov02_validation_20260223.md` |
| `required_validator` | `python scripts/spec_lint.py` |

### 1.2 Source references consumed

- `SRC-V013-05` `spec/planning/PARALLEL_LANE_OWNERSHIP_AND_HANDOFF.md`
- `SRC-V013-12` `spec/planning/v013_future_work_seed_matrix.md`
- `SRC-V013-13` `spec/planning/v012_wave16_candidate_shards_20260223.md`
- `SRC-V013-14` `spec/planning/v013_seed_wave_w1_reseed1_batch_manifest_20260223.md`

### 1.3 In-scope deliverables

1. Backward/forward compatibility matrix for the extension registry contract.
2. Deterministic validator command set with explicit pass/fail signals.
3. Waiver and escalation policy for compatibility validation outcomes.
4. Acceptance checklist mapping to `AC-V013-GOV-02`.
5. Validation transcript for `python scripts/spec_lint.py` plus lane-C evidence linkage.

### 1.4 Ownership lock for issue `#786`

1. Lane C write ownership is limited to the two paths listed in Section `1.1`.
2. `registries/experimental_extensions/index.schema.json` and `tests/governance/registry_compat/README.md` are consumed as read-only dependency artifacts in this issue.
3. Validation evidence for this issue is published in `spec/planning/evidence/lane_c/v013_seed_gov02_validation_20260223.md`.

## 2. Suite Contract Summary

The compatibility validation suite contract is published and validated against synchronized artifacts:

| Artifact | Contract role | Ownership posture for `#786` |
| --- | --- |
| `spec/planning/v013_extension_registry_compat_validation_package.md` | Governance contract and closeout ledger for `#786`. | Lane C write-owned artifact. |
| `spec/planning/evidence/lane_c/v013_seed_gov02_validation_20260223.md` | Deterministic issue evidence mapping acceptance rows to concrete verification checks. | Lane C write-owned artifact. |
| `registries/experimental_extensions/index.schema.json` | Machine-checkable schema contract including compatibility matrix, validators, waiver policy, and acceptance checklist structure. | Read-only dependency artifact for this issue. |
| `tests/governance/registry_compat/README.md` | Deterministic validator runbook and governance-response expectations for failures. | Read-only dependency artifact for this issue. |

## 3. Compatibility Matrix (Backward and Forward)

### 3.1 Required compatibility dimensions

1. Schema-versioning behavior is deterministic for patch, minor, and major updates.
2. Required-field behavior is explicit for add/remove/type-change scenarios.
3. Backward and forward outcomes are independently classified as `pass`, `conditional`, or `fail`.

### 3.2 Compatibility decision matrix

| Matrix ID | Change class | Schema versioning rule | Required-field rule | Backward result | Forward result | Deterministic action | Validator IDs |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `CM-RC-01` | `patch_nonsemantic` | Patch (`x.y.z`) only | Required fields unchanged | `pass` | `pass` | Accept publish if all validators pass. | `VAL-RC-01`, `VAL-RC-04` |
| `CM-RC-02` | `minor_optional_addition` | Minor (`x.y.0`) | New fields must remain optional | `conditional` | `pass` | Accept only when unknown optional fields are documented as ignorable. | `VAL-RC-03`, `VAL-RC-05` |
| `CM-RC-03` | `minor_enum_expansion` | Minor (`x.y.0`) | Required fields unchanged | `conditional` | `conditional` | Require explicit fallback handling note before publish. | `VAL-RC-03`, `VAL-RC-06` |
| `CM-RC-04` | `minor_required_addition` | Minor requested but invalid | New required fields are not allowed in minor updates | `fail` | `fail` | Reject change; reclassify as major with migration plan. | `VAL-RC-05` |
| `CM-RC-05` | `major_required_removal` | Major (`x.0.0`) required | Removed required field must include migration path | `fail` | `conditional` | Block until migration notes and transition window are attached. | `VAL-RC-05`, `VAL-RC-06` |
| `CM-RC-06` | `major_required_rename_without_alias` | Major (`x.0.0`) required | Rename requires compatibility alias window | `fail` | `fail` | Reject publish until alias or dual-field transition is defined. | `VAL-RC-05` |
| `CM-RC-07` | `unknown_major_input` | Unknown major from consumer perspective | Required-field interpretation is undefined | `fail` | `fail` | Hard fail closed and escalate under `E2`. | `VAL-RC-06` |
| `CM-RC-08` | `required_field_type_drift` | Any version | Required-field type changes are breaking | `fail` | `fail` | Reject publish and require schema correction. | `VAL-RC-04`, `VAL-RC-05` |

## 4. Deterministic Validators

### 4.1 Validator command contract

| Validator ID | Command | Pass signal | Failure disposition |
| --- | --- | --- | --- |
| `VAL-RC-01` | `python scripts/spec_lint.py` | Output contains `spec-lint: OK`; exit `0`. | Mark validation run failed and stop closeout. |
| `VAL-RC-02` | `python scripts/check_issue_checkbox_drift.py` | Exit `0`; no blocking drift findings. | Open governance follow-up and block publish. |
| `VAL-RC-03` | `rg -n "compat|version|schema" spec/planning/v013_extension_registry_compat_validation_package.md` | Exit `0`; at least one matching line. | Treat package as incomplete and block publish. |
| `VAL-RC-04` | `python -c "import json,pathlib;json.loads(pathlib.Path('registries/experimental_extensions/index.schema.json').read_text(encoding='utf-8'));print('schema-json: OK')"` | Output contains `schema-json: OK`; exit `0`. | Treat schema as invalid JSON and block publish. |
| `VAL-RC-05` | `python -c 'import json,pathlib,sys;d=json.loads(pathlib.Path("registries/experimental_extensions/index.schema.json").read_text(encoding="utf-8"));p=d["$defs"]["governance_contract"]["properties"];need={"compatibility_matrix","required_field_policy","validators","waiver_policy","acceptance_checklist"};m=sorted(need-set(p));print("contract-keys: OK" if not m else "contract-keys: MISSING "+",".join(m));sys.exit(0 if not m else 1)'` | Output contains `contract-keys: OK`; exit `0`. | Treat contract as incomplete and block publish. |
| `VAL-RC-06` | `rg -n "AC-V013-GOV-02|VAL-RC-|ESC-RC-" tests/governance/registry_compat/README.md` | Exit `0`; all required governance identifiers are present. | Treat test/readme contract as incomplete and block publish. |

### 4.2 Determinism rules

1. Validator commands are run from repository root with no environment-specific flags.
2. Any non-zero exit status is a hard failure for the suite.
3. Validator IDs are stable and cannot be renumbered without a major contract revision.

## 5. Waiver and Escalation Policy

### 5.1 Waiver classes

| Waiver ID | Allowed use | Max severity | Mandatory fields |
| --- | --- | --- | --- |
| `WVR-RC-01` | Temporary external dependency outage affecting non-critical validator execution evidence. | `medium` | owner, rationale, expiry date, mitigation plan, approval reference |
| `WVR-RC-02` | Toolchain environment defect with confirmed workaround and replay date. | `medium` | owner, rationale, expiry date, workaround, replay command |
| `WVR-RC-03` | Documentation mismatch that does not change schema behavior. | `low` | owner, rationale, expiry date, correction issue ID |

### 5.2 Non-waiverable conditions

1. Unknown-major compatibility (`CM-RC-07`) failures.
2. Required-field structural failures (`CM-RC-04`, `CM-RC-06`, `CM-RC-08`).
3. Deterministic validator nondeterminism (same input producing conflicting outcomes).
4. Missing acceptance gate mapping to `AC-V013-GOV-02`.

### 5.3 Escalation ladder

| Escalation ID | Trigger | Owner | Response SLA | Required action |
| --- | --- | --- | --- | --- |
| `ESC-RC-01` (`E1`) | Single validator failure with known local fix path. | Worklane owner (`WL-GOV`) | `T+24h` | Patch artifact, rerun failed validator, publish rerun output. |
| `ESC-RC-02` (`E2`) | Repeated validator failure or unknown-major compatibility failure. | Governance lane lead | `T+48h` | Freeze merge, open incident note, assign remediation owner. |
| `ESC-RC-03` (`E3`) | Blocking failure crossing release checkpoint windows. | Review board delegate | `T+72h` | Decide hold/override disposition; record rationale and deadlines. |
| `ESC-RC-04` (`E4`) | Integrity-risk or policy breach with no safe workaround. | Steering owner + security owner | Immediate | Apply emergency hold and require superseding corrective artifact set. |

## 6. Acceptance Checklist Mapping (`AC-V013-GOV-02`)

### 6.1 Deterministic acceptance rows

| Acceptance row | Criterion | Deterministic pass rule | Evidence |
| --- | --- | --- | --- |
| `AC-V013-GOV-02-01` | Compatibility matrix covers schema versioning and backward/forward policy. | Section `3.2` contains matrix rows `CM-RC-01`..`CM-RC-08` with backward and forward outcomes. | This package Section `3`; schema `governance_contract.compatibility_matrix`. |
| `AC-V013-GOV-02-02` | Required-field compatibility behavior is explicit. | Section `3.2` and Section `5.2` define blocking behavior for required-field changes. | This package Sections `3` and `5`; schema `required_field_policy`. |
| `AC-V013-GOV-02-03` | Validation suite command contract is deterministic. | Section `4.1` defines stable `VAL-RC-01`..`VAL-RC-06` commands and pass signals. | This package Section `4`; tests readme Section `3`. |
| `AC-V013-GOV-02-04` | Governance waiver/escalation path is documented. | Section `5` defines waiver classes, non-waiverable conditions, and `E1`..`E4` escalation ladder. | This package Section `5`; tests readme Section `4`. |
| `AC-V013-GOV-02-05` | Validation transcript for `python scripts/spec_lint.py` is recorded. | Section `7` includes command, output, and exit status. | This package Section `7`. |

### 6.2 Closeout checklist

| Checked | Checklist ID | Maps to | Pass rule |
| --- | --- | --- | --- |
| [x] | `CL-V013-GOV-02-01` | `AC-V013-GOV-02-01` | Compatibility matrix includes backward/forward outcomes and schema-versioning rules. |
| [x] | `CL-V013-GOV-02-02` | `AC-V013-GOV-02-02` | Required-field policy includes deterministic hard-fail conditions. |
| [x] | `CL-V013-GOV-02-03` | `AC-V013-GOV-02-03` | Validator contract defines stable commands and deterministic pass signals. |
| [x] | `CL-V013-GOV-02-04` | `AC-V013-GOV-02-04` | Waiver classes, non-waiverable conditions, and escalation ladder are complete. |
| [x] | `CL-V013-GOV-02-05` | `AC-V013-GOV-02-05` | `python scripts/spec_lint.py` transcript is present with successful result. |

Issue-body checklist closeout:

- [x] Compatibility matrix covers schema versioning, required fields, and forward/backward policy.
- [x] Validation suite command contract is deterministic.
- [x] Governance escalation and waiver path is documented.
- [x] Validation transcript included.

## 7. Validation Transcript (`python scripts/spec_lint.py`)

Command:

```sh
python scripts/spec_lint.py
```

Recorded output:

```text
spec-lint: OK
```

Exit status: `0` (`PASS`)

## 8. M15 Extension Registry Compatibility W1 Baseline Addendum (`ERC-DEP-M15-*`)

### 8.1 Deterministic dependency semantics

| Dependency ID | Gate class | Deterministic semantic rule | Fail criteria | Escalation owner | Unblock condition | Linked acceptance row |
| --- | --- | --- | --- | --- | --- | --- |
| `ERC-DEP-M15-01` | `Hard` | This package is canonical authority for M15 W1 compatibility semantics (`CM-RC-*`), validator contract (`VAL-RC-*`), and escalation policy (`ESC-RC-*`). | M15 artifacts omit this package as authority, remove required row IDs, or introduce contradictory compatibility/validator/escalation semantics. | `Lane A M15 owner (#894)` | Restore canonical references + deterministic IDs, rerun M15 command anchors, and republish clean evidence. | `AC-V014-M15-01` |
| `ERC-DEP-M15-02` | `Hard` | Schema contract in `registries/experimental_extensions/index.schema.json` preserves governance required keys and `acceptance_gate_id = AC-V013-GOV-02`. | Governance contract keys or acceptance gate constant drift, disappear, or become non-deterministic. | `Lane A M15 owner (#894)` | Repair schema keys/constants and rerun schema command anchors with exit `0`. | `AC-V014-M15-03`, `AC-V014-M15-05` |
| `ERC-DEP-M15-03` | `Hard` | Governance validator runbook in `tests/governance/registry_compat/README.md` retains deterministic ID set (`CM-RC-01`..`CM-RC-08`, `VAL-RC-01`..`VAL-RC-06`, `ESC-RC-01`..`ESC-RC-04`, `AC-V013-GOV-02`). | Required ID families are missing, renumbered, or ambiguous. | `Lane A M15 owner (#894)` | Reconcile README IDs to canonical set and rerun README command anchor with exit `0`. | `AC-V014-M15-04` |
| `ERC-DEP-M15-04` | `Hard` | Breaking compatibility classes (`CM-RC-04`, `CM-RC-06`, `CM-RC-07`, `CM-RC-08`) remain non-waiverable fail-closed controls. | Any artifact permits waiver, `HOLD`, or conditional-success bypass for these breaking classes. | `Lane A M15 owner (#894)` | Restore non-waiverable fail-closed language and rerun disposition command anchor with exit `0`. | `AC-V014-M15-02` |
| `ERC-DEP-M15-05` | `Soft` | Conditional classes (`CM-RC-02`, `CM-RC-03`, `CM-RC-05`) may enter `HOLD` only with explicit owner, ETA, and replay command; they cannot override hard gates. | Conditional drift lacks owner/ETA/replay command, or `HOLD` is used to bypass any hard gate failure. | `Lane A M15 owner (#894)` | Record owner + ETA + replay command, remediate drift, then rerun affected command anchors. | `AC-V014-M15-02` |
| `ERC-DEP-M15-06` | `Hard` | M15 lane-A acceptance artifacts must preserve stable `DEP/CMD/EVID/AC` IDs and deterministic failure-handling schema fields. | Missing stable IDs, missing dependency type/fail criteria/escalation owner/unblock condition fields, or broken evidence mapping. | `Lane A M15 owner (#894)` | Repair matrix/evidence schema and rerun matrix/evidence command anchors with exit `0`. | `AC-V014-M15-06`, `AC-V014-M15-07` |
| `ERC-DEP-M15-07` | `Hard` | Repository spec lint validator is the terminal release gate for lane-A-owned M15 artifacts. | `python scripts/spec_lint.py` exits non-zero or omits `spec-lint: OK`. | `Lane A M15 owner (#894)` | Resolve lint findings, rerun validator, and capture clean transcript evidence. | `AC-V014-M15-08` |

### 8.2 Fail-closed disposition rules

1. `PASS`: all hard gates (`ERC-DEP-M15-01`, `ERC-DEP-M15-02`, `ERC-DEP-M15-03`, `ERC-DEP-M15-04`, `ERC-DEP-M15-06`, `ERC-DEP-M15-07`) pass and linked evidence anchors are present.
2. `HOLD`: only soft gate `ERC-DEP-M15-05` remains open with explicit owner + ETA + replay command while every hard gate remains `PASS`.
3. `FAIL`: any hard gate fails, required evidence is missing, or soft-gate drift attempts to bypass hard controls.
4. Default fail-closed rule: if status classification is ambiguous or incomplete, the disposition is `FAIL` until deterministic evidence resolves ambiguity.

## 9. M16 Extension Registry Compatibility W2 Deterministic Hardening Addendum (`ERC-DEP-M16-*`)

### 9.1 W2 deterministic dependency semantics

| Dependency ID | Type | Deterministic semantic rule | Fail criteria | Escalation owner | Unblock condition | Linked acceptance row |
| --- | --- | --- | --- | --- | --- | --- |
| `ERC-DEP-M16-01` | `Hard` | This package is canonical W2 authority for strict validator parity, deterministic failure taxonomy, and publication-gate controls for extension registry compatibility. | W2 artifacts omit this package as authority, remove `ERC-DEP-M16-*` rows, or redefine W2 semantics with contradictory language. | `Lane A M16 owner (#899)` | Restore canonical references and W2 row set, rerun matrix command anchors, and republish clean evidence. | `AC-V014-M16-01` |
| `ERC-DEP-M16-02` | `Hard` | Validator ID set parity is strict: `VAL-RC-01`..`VAL-RC-06` must remain complete, unique, and stable across W2 authority artifacts. | Missing, duplicate, renumbered, or ad hoc validator IDs in W2-controlled artifacts. | `Lane A M16 owner (#899)` | Restore canonical validator ID set and rerun parity command anchors with exit `0`. | `AC-V014-M16-02` |
| `ERC-DEP-M16-03` | `Hard` | Validator command/pass-signal parity is strict: each `VAL-RC-*` command and pass signal in Section `4.1` remains semantically identical in W2 acceptance/evidence controls. | Command-string drift, pass-signal drift, or incompatible failure-disposition language for any `VAL-RC-*`. | `Lane A M16 owner (#899)` | Reconcile command/pass/failure semantics to Section `4.1` and rerun parity command anchors. | `AC-V014-M16-03` |
| `ERC-DEP-M16-04` | `Hard` | Failure taxonomy classes `FTX-RC-M16-01`..`FTX-RC-M16-07` are stable and map deterministically to disposition and escalation routes. | Missing taxonomy class IDs, ambiguous taxonomy mapping, or taxonomy-to-disposition drift. | `Lane A M16 owner (#899)` | Restore taxonomy ID set and deterministic mappings; rerun taxonomy command anchors with exit `0`. | `AC-V014-M16-04` |
| `ERC-DEP-M16-05` | `Hard` | Schema and runbook authority remain deterministic: governance schema contract keys/acceptance constant and runbook ID families stay intact. | Schema governance keys/`AC-V013-GOV-02` drift, or README loss/drift of `CM/VAL/ESC/AC` identifiers. | `Lane A M16 owner (#899)` | Repair schema/runbook drift and rerun schema + README command anchors with exit `0`. | `AC-V014-M16-05`, `AC-V014-M16-06` |
| `ERC-DEP-M16-06` | `Soft` | Conditional compatibility documentation drift (`CM-RC-02`, `CM-RC-03`, `CM-RC-05`) may remain only as `HOLD` with explicit owner + ETA + replay command and no hard-gate impact. | Missing owner/ETA/replay metadata for conditional drift, or any attempt to use conditional drift to bypass hard gates. | `Lane A M16 owner (#899)` | Record owner + ETA + replay command or resolve drift, then rerun impacted command anchors. | `AC-V014-M16-07` |
| `ERC-DEP-M16-07` | `Hard` | Publication gating is deterministic and fail-closed via `PUB-RC-M16-01`..`PUB-RC-M16-04`; ambiguous or incomplete state cannot publish. | Missing gate rules, contradictory gate logic, or ambiguous disposition outcomes in W2 artifacts. | `Lane A M16 owner (#899)` | Restore deterministic publication-gate rules and rerun gating command anchors with exit `0`. | `AC-V014-M16-08` |
| `ERC-DEP-M16-08` | `Hard` | Repository spec lint is the terminal release gate for lane-A-owned M16 artifacts. | `python scripts/spec_lint.py` exits non-zero or omits `spec-lint: OK`. | `Lane A M16 owner (#899)` | Resolve lint findings, rerun validator, and capture clean transcript evidence with exit code. | `AC-V014-M16-09` |

### 9.2 Strict validator parity profile (`VPAR-RC-M16-*`)

| Parity ID | Scope | Deterministic pass requirement | Failure taxonomy on breach | Linked acceptance row |
| --- | --- | --- | --- | --- |
| `VPAR-RC-M16-01` | Validator ID set parity | `VAL-RC-01`..`VAL-RC-06` appear as a complete, stable, non-duplicated set in W2 parity checkpoints. | `FTX-RC-M16-01` | `AC-V014-M16-02` |
| `VPAR-RC-M16-02` | Validator command parity | Canonical validator commands in Section `4.1` are preserved with no semantic drift in W2 command anchors and evidence transcript references. | `FTX-RC-M16-02` | `AC-V014-M16-03` |
| `VPAR-RC-M16-03` | Validator pass-signal parity | Pass criteria for each validator remain deterministic and unchanged (`exit 0` plus required output token where defined). | `FTX-RC-M16-03` | `AC-V014-M16-03` |
| `VPAR-RC-M16-04` | Validator failure-disposition parity | Failure outcomes remain fail-closed and preserve escalation route expectations from base governance policy. | `FTX-RC-M16-03`, `FTX-RC-M16-04` | `AC-V014-M16-03`, `AC-V014-M16-08` |

### 9.3 Deterministic failure taxonomy (`FTX-RC-M16-*`)

| Failure class ID | Type | Trigger condition | Required disposition | Escalation route | Waiver posture |
| --- | --- | --- | --- | --- | --- |
| `FTX-RC-M16-01` | `Hard` | Validator ID set drift (`VAL-RC-*` missing, renumbered, or duplicated). | `REJECT` | `ESC-RC-02` (`E2`) | Not waiverable |
| `FTX-RC-M16-02` | `Hard` | Validator command drift against canonical Section `4.1` command semantics. | `REJECT` | `ESC-RC-02` (`E2`) | Not waiverable |
| `FTX-RC-M16-03` | `Hard` | Pass-signal/failure-disposition drift for any validator. | `REJECT` | `ESC-RC-01` (`E1`) | Not waiverable |
| `FTX-RC-M16-04` | `Hard` | Any deterministic validator execution returns non-zero exit code. | `REJECT` | `ESC-RC-01` (`E1`) | Not waiverable |
| `FTX-RC-M16-05` | `Hard` | Schema governance key/constant drift or README deterministic ID family drift. | `REJECT` | `ESC-RC-02` (`E2`) | Not waiverable |
| `FTX-RC-M16-06` | `Soft` | Conditional compatibility documentation drift with hard gates still passing and explicit owner + ETA + replay command present. | `HOLD` | `ESC-RC-01` (`E1`) | Waiver not used; deterministic `HOLD` only |
| `FTX-RC-M16-07` | `Hard` | Missing evidence anchors, missing exit-code reporting, or publication-gate ambiguity. | `REJECT` | `ESC-RC-03` (`E3`) | Not waiverable |

### 9.4 Deterministic publication gating (`PUB-RC-M16-*`)

| Gate ID | Deterministic publication rule | Outcome when satisfied | Outcome when violated |
| --- | --- | --- | --- |
| `PUB-RC-M16-01` | All hard dependencies (`ERC-DEP-M16-01`, `ERC-DEP-M16-02`, `ERC-DEP-M16-03`, `ERC-DEP-M16-04`, `ERC-DEP-M16-05`, `ERC-DEP-M16-07`, `ERC-DEP-M16-08`) pass with linked evidence anchors. | `PUBLISH` candidate remains valid. | `REJECT`. |
| `PUB-RC-M16-02` | Soft dependency `ERC-DEP-M16-06` may produce `HOLD` only when explicit owner + ETA + replay command are present and all hard dependencies pass. | `HOLD` (deterministic, non-publishing). | `REJECT`. |
| `PUB-RC-M16-03` | Acceptance/evidence schema remains complete: stable `DEP/CMD/EVID/AC` IDs plus explicit exit codes for every replay command. | `PUBLISH` or valid `HOLD` (if only `PUB-RC-M16-02` applies). | `REJECT`. |
| `PUB-RC-M16-04` | Fail-closed default is always enforced when state is ambiguous, incomplete, or contradictory. | Disposition resolved deterministically by evidence. | `REJECT` until evidence ambiguity is cleared. |

Publication disposition rules:

1. `PUBLISH`: every hard dependency passes and no hard taxonomy class (`FTX-RC-M16-01`, `FTX-RC-M16-02`, `FTX-RC-M16-03`, `FTX-RC-M16-04`, `FTX-RC-M16-05`, `FTX-RC-M16-07`) is active.
2. `HOLD`: only soft class `FTX-RC-M16-06` is active with required owner + ETA + replay command, and all hard dependencies remain `PASS`.
3. `REJECT`: any hard failure taxonomy class is active, required evidence is missing, or publication state cannot be determined deterministically.
4. Default fail-closed rule: if there is any uncertainty, publish state is `REJECT` until deterministic evidence resolves the uncertainty.
