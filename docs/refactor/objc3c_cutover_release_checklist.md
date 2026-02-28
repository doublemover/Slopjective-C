# ObjC3C Cutover Release Checklist (`M134-E002`)

## 1. Purpose

This checklist is the production-readiness gate for final ObjC3C modular cutover. It is fail-closed: cutover does not proceed until all required rows are `PASS` or explicitly `PENDING` with owner and unblock criteria.

## 2. ownership assignments

| Area | Primary ownership | Backup ownership | Decision authority |
| --- | --- | --- | --- |
| Cutover execution | Release owner | Integrator | Release owner |
| Boundary and architecture integrity | Lane A owner | Lane B owner | Integrator |
| Semantic/pipeline correctness | Lane B owner | Integrator | Integrator |
| Determinism/perf evidence | Lane D owner | Release owner | Release owner |
| Workflow/governance evidence | Lane E owner | Release owner | Release owner |

## 3. cutover readiness checklist (`CR-M134-01`..`CR-M134-12`)

- [ ] `CR-M134-01` Ownership roster published with on-call contact and UTC window.
- [ ] `CR-M134-02` Latest boundary gate passes: `python scripts/check_objc3c_dependency_boundaries.py --strict`.
- [ ] `CR-M134-03` Subsystem entrypoint checks (`dev:objc3c:lex|parse|sema|lower|ir`) are green for touched module surfaces.
- [ ] `CR-M134-04` No open `Sev-1`/`Sev-2` regressions in cutover scope.
- [ ] `CR-M134-05` Freeze window start/end UTC timestamps are published.
- [ ] `CR-M134-06` Rollback owner and rollback command path are confirmed.
- [ ] `CR-M134-07` Fallback drill run recorded in `spec/planning/compiler/m134/m134_cutover_drill_20260227.md`.
- [ ] `CR-M134-08` Evidence bundle includes command text, exit code, timestamp, and operator.
- [ ] `CR-M134-09` Stakeholder notification posted before cutover.
- [ ] `CR-M134-10` Stakeholder notification posted after cutover.
- [ ] `CR-M134-11` Post-cutover monitoring window completed without trigger breach.
- [ ] `CR-M134-12` Final release sign-off includes all pending evidence dispositions.

## 4. rollback and fallback confidence gates

Rollback triggers:

- compiler crash, wrong-code signal, or hard CI regression,
- deterministic replay drift in critical paths,
- missing mandatory governance evidence during freeze/cutover.

Fallback confidence requirements:

1. At least one successful rollback rehearsal in the drill package.
2. At least one successful fallback mode run restoring prior stable behavior.
3. Evidence artifacts retained under `tmp/artifacts/objc3c-native/*` with referenced summary paths.

## 5. pending evidence placeholders (D-lane dependencies)

These placeholders remain explicit until `M134-D001` and `M134-D002` evidence is delivered.

| Placeholder ID | Dependency | Owner | Required evidence | Current state | Unblock criteria |
| --- | --- | --- | --- | --- | --- |
| `PEND-D001` | `M134-D001` | Lane D owner | Determinism replay summary for cutover candidate across subsystem suites | `PENDING` | Attach replay summary + exit code `0` and integrator ack |
| `PEND-D002` | `M134-D002` | Lane D owner | Performance and regression budget confirmation for modularized path | `PENDING` | Publish perf guard report with no hard-threshold breaches and release-owner sign-off |

Cutover may proceed only with either:

- both rows marked `PASS`, or
- explicit release-owner waiver comment with risk rationale and follow-up issue owner/date.

## 6. evidence capture expectations

Every required checklist row must include:

- command run (or document artifact path),
- UTC timestamp,
- operator/owner,
- exit code or pass/fail disposition,
- artifact reference path.

Minimum evidence record template:

```text
ID: CR-M134-02
Command: python scripts/check_objc3c_dependency_boundaries.py --strict
Timestamp (UTC): 2026-02-27T22:15:00Z
Owner: Lane A owner
Exit code: 0
Artifact: tmp/artifacts/objc3c-native/<run-id>/summary.json
```

## 7. escalation rules

1. Escalate immediately to release owner for any rollback trigger.
2. Escalate to integrator when ownership boundaries conflict.
3. Escalate to lane E owner when evidence is missing or ambiguous.
4. Do not close cutover checklist while `PEND-D001` or `PEND-D002` is unresolved without explicit waiver.

## 8. validation command

```sh
rg -n "cutover|fallback|rollback|ownership" docs/refactor/objc3c_cutover_release_checklist.md
```
