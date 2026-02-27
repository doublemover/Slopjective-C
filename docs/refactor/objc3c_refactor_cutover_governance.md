# ObjC3C Refactor Cutover Governance (`M132-E002`)

## 1. Purpose

Define cutover controls that make milestone transitions safe, reversible, and auditable for parallel-lane refactor delivery.

## 2. Governance scope

- Applies to every milestone that changes compiler entry points, pass ordering, or release packaging for ObjC3C refactor.
- Covers freeze windows, release criteria, rollback triggers, and recovery ownership.

## 3. milestone release criteria

| Milestone state | Required release criteria | Evidence owner |
| --- | --- | --- |
| `pre-cutover` | dependency issues closed, validation green, rollback path rehearsed | lane owner |
| `canary` | scoped rollout enabled, no Sev-1/Sev-2 regressions for one full CI cycle | integrator |
| `full-cutover` | canary stable, freeze checklist complete, sign-off recorded | release owner |
| `post-cutover` | monitoring window complete and no unresolved rollback triggers | release owner |

A milestone cannot advance until all required criteria for the current state are met.

## 4. freeze windows

| Freeze type | Duration | Entry condition | Exit condition |
| --- | --- | --- | --- |
| `soft-freeze` | 1 business day | cutover PR queued | only blocking fixes merge |
| `hard-freeze` | 1 business day | final cutover start | no functional merges; governance-only exceptions |

Freeze rules:

1. Publish start/end timestamps in UTC in the milestone issue.
2. Record approver and exception rationale for any freeze bypass.
3. Block non-emergency merges during `hard-freeze`.

## 5. Cutover checklist (`CG-E002`)

- [ ] Milestone issue lists owner, backup owner, and UTC freeze window.
- [ ] release criteria table rows are all satisfied and linked to evidence.
- [ ] Canary validation and regression scans are attached to cutover PR.
- [ ] Rollback command path and owner on-call are confirmed.
- [ ] Stakeholder notification is posted before and after cutover.

## 6. Rollback triggers and recovery playbook

Trigger categories:

- Functional regression above agreed baseline.
- Build or CI instability lasting more than one cycle.
- Data or ABI compatibility break on required fixtures.
- Missing governance evidence during freeze/cutover execution.

Recovery playbook (`RB-E002-01`..`RB-E002-06`):

1. Declare incident in milestone issue with severity and timestamp.
2. Freeze additional merges for affected surfaces.
3. Execute rollback action from the matrix.
4. Re-run validation gate commands and capture exit codes.
5. Publish root-cause note with remediation owner/date.
6. Resume cutover only after checklist re-verification.

## 7. rollback matrix

| Severity | rollback trigger | Detection signal | Immediate action | Owner | SLA |
| --- | --- | --- | --- | --- | --- |
| `Sev-1` | compiler crash or incorrect codegen in canary/full-cutover | failing fixture or production crash report | immediate rollback to previous stable refactor commit | release owner + lane owner | 30 minutes |
| `Sev-2` | deterministic CI failures across two consecutive pipelines | CI dashboard red on required jobs | suspend cutover and rollback latest refactor slice | integrator | 2 hours |
| `Sev-3` | non-blocking regression with bounded workaround | triaged bug with workaround confirmed | hold rollout progression; optional partial rollback | lane owner | next business day |
| `Gov-Block` | missing freeze/cutover evidence or unsigned checklist | governance checklist gap | stop promotion state until evidence complete; rollback if already promoted | release owner | same day |

Exit gate after rollback:

- Root cause linked to corrective issue.
- Fix validated in isolated slice PR.
- Cutover checklist re-run with fresh timestamps.

## 8. Validation command

```sh
rg -n "rollback|cutover|freeze|release criteria" docs/refactor/objc3c_refactor_cutover_governance.md
```
