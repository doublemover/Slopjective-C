# ObjC3C Dependency Violation Playbook (`M133-E002`)

## 1. Purpose

This playbook defines triage and remediation for architecture boundary violations detected by:

```sh
python scripts/check_objc3c_dependency_boundaries.py --strict
```

It is the single operational workflow for lane owners, reviewers, and integrators when the boundary CI gate fails.

## 2. Ownership

| Responsibility | Primary owner | Backup owner | SLA |
| --- | --- | --- | --- |
| Violation triage and owner assignment | Lane owner for touched module | Lane E backup | 4 business hours |
| Remediation implementation | Module owner lane | Integrator | 1 business day for Sev-2+, 2 business days for Sev-3 |
| Governance/escalation updates | Lane E governance owner | Release owner | Same business day |

## 3. Trigger and expected CI behavior

- Trigger: boundary gate step `00 Enforce ObjC3C dependency boundaries (strict fail-fast gate)` fails in task hygiene workflow.
- Expected behavior: workflow fails immediately before expensive Node/tooling steps.
- Actionable output sources:
  - checker output line(s): `forbidden include '<path>' (<from> -> <to>)`
  - CI error hint linking this playbook.

## 4. Triage workflow (`TRIAGE-E002-01`..`TRIAGE-E002-07`)

1. Capture failing command output and the violating file/line pair.
2. Classify severity using the matrix in section 6.
3. Confirm whether violation is intentional architecture change or accidental include drift.
4. Assign remediation owner based on violating module (`owner_module`).
5. Open/update issue with:
   - failing line(s),
   - severity,
   - expected allowed dependency path,
   - target PR/ETA.
6. If violation blocks multiple lanes, mark dependent issues as blocked and tag integrator.
7. Re-run strict checker locally after fix and record exit code in PR.

## 5. Remediation workflow (`FIX-E002-01`..`FIX-E002-06`)

1. Remove or replace forbidden include with an allowed dependency path.
1. If logic crosses layer boundaries, move shared contracts to an allowed upstream module.
1. Keep fix PR slice minimal and include only required dependency-surface changes.
1. Add or adjust tests/evidence proving no regressions from dependency inversion change.
1. Re-run:

```sh
python scripts/check_objc3c_dependency_boundaries.py --strict
```

1. Include checker output and rationale in PR under `Boundary remediation evidence`.

## 6. Severity and escalation matrix

| Severity | Detection criteria | Immediate action | Escalation path | Resolution target |
| --- | --- | --- | --- | --- |
| `Sev-1` | violation in release-cutover branch or multiple core modules (`pipeline`, `sema`, `lower`, `ir`) | freeze merges for impacted modules; hotfix PR | Integrator -> release owner immediately | same day |
| `Sev-2` | single-module forbidden include in active development branch | block merge of violating PR; assign module owner | Lane owner -> integrator within 4 business hours | 1 business day |
| `Sev-3` | non-critical or draft-branch violation without downstream impact | track in issue and remediate before review approval | Lane owner | 2 business days |
| `Gov-Block` | repeated violation without owner/ETA or missing evidence updates | stop promotion/merge until governance fields are complete | Lane E governance owner -> release owner | same day |

## 7. PR checklist for resolved violations

- [ ] Violation source file/line and module direction are documented.
- [ ] Fix preserves allowed dependency model from `native/objc3c/src/ARCHITECTURE.md`.
- [ ] `python scripts/check_objc3c_dependency_boundaries.py --strict` passes locally.
- [ ] No dependent lane is left with implicit breakage or hidden follow-up.
- [ ] Escalation notes are added when SLA/severity thresholds were exceeded.

## 8. Escalation protocol

1. Escalate immediately for `Sev-1` and `Gov-Block`.
2. Escalate `Sev-2` when no owner is assigned within SLA.
3. For repeated incidents in the same module, require root-cause + prevention note in the next PR.

## 9. Validation command

```sh
python scripts/check_objc3c_dependency_boundaries.py --strict
```
