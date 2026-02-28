# M134 Cutover Fallback Drill Package (`M134-E002`)

## 1. Drill objective

Rehearse cutover rollback and fallback execution before final production promotion, and record deterministic evidence proving confidence in recovery paths.

## 2. ownership matrix

| Drill function | Primary owner | Backup owner | Approval required |
| --- | --- | --- | --- |
| Drill commander and timeline control | Release owner | Integrator | Release owner |
| Boundary integrity checks | Lane A owner | Lane B owner | Integrator |
| Semantic/pipeline validation checks | Lane B owner | Integrator | Integrator |
| Determinism/perf evidence capture | Lane D owner | Release owner | Release owner |
| Governance artifact publication | Lane E owner | Release owner | Release owner |

## 3. Drill scenarios (`DR-M134-01`..`DR-M134-04`)

| Drill ID | Scenario | Pass criteria | Evidence artifact |
| --- | --- | --- | --- |
| `DR-M134-01` | Pre-cutover baseline snapshot | baseline checks all pass with stable outputs | baseline command transcript + summary paths |
| `DR-M134-02` | Simulated cutover failure -> rollback | rollback initiated within SLA; stable baseline restored | rollback transcript + restored check outputs |
| `DR-M134-03` | Fallback mode execution | fallback path completes and core subsystem checks pass | fallback command transcript + subsystem summaries |
| `DR-M134-04` | Return-to-target cutover retry | target path can be re-attempted after remediation evidence | retry transcript + sign-off notes |

## 4. Drill workflow

1. Announce drill window and owners (UTC start/end).
2. Run baseline preflight checks and capture outputs.
3. Inject/trigger a controlled failure condition for rehearsal.
4. Execute rollback runbook and verify restoration.
5. Execute fallback path and validate subsystem behavior.
6. Re-run readiness checks and decide retry readiness.
7. Publish evidence bundle with explicit pass/fail and owner sign-off.

## 5. Command set for evidence capture

Required command evidence:

```sh
python scripts/check_objc3c_dependency_boundaries.py --strict
npm run dev:objc3c:lex
npm run dev:objc3c:parse
npm run dev:objc3c:sema
npm run dev:objc3c:lower
npm run dev:objc3c:ir
```

For each command capture:

- command text,
- exit code,
- UTC timestamp,
- operator,
- summary/log artifact path.

## 6. fallback and rollback rehearsal steps

### 6.1 rollback rehearsal (`RB-DRILL`)

1. Detect simulated cutover failure and declare incident.
2. Freeze merge activity for affected modules.
3. Roll back to last known stable cutover candidate.
4. Re-run boundary + subsystem checks.
5. Confirm baseline behavior restoration.

Success criteria:

- rollback completed within target SLA,
- all required checks return exit code `0`,
- incident timeline is fully recorded.

### 6.2 fallback rehearsal (`FB-DRILL`)

1. Activate fallback operating path (previous stable execution route).
2. Run subsystem checks for touched surfaces.
3. Compare outputs to baseline evidence for deterministic equivalence.
4. Record any residual risk requiring release-owner disposition.

Success criteria:

- fallback path is executable without manual guesswork,
- subsystem checks remain green,
- evidence package is complete and reviewable.

## 7. pending dependency evidence placeholders

`M134-D001` and `M134-D002` evidence is dependency-bound and may be pending at draft time.

| Pending ID | Dependency | Owner | Expected evidence | Unblock criteria | Status |
| --- | --- | --- | --- | --- | --- |
| `DR-PEND-D001` | `M134-D001` | Lane D owner | Deterministic replay proof on cutover candidate after rollback/fallback drill | attach signed replay summary and integrator acceptance | `PENDING` |
| `DR-PEND-D002` | `M134-D002` | Lane D owner | Perf/regression confidence record after fallback drill | publish perf guard output with no hard breach and release-owner acceptance | `PENDING` |

If either row remains `PENDING` at decision time, release owner must either:

- defer cutover, or
- publish explicit risk waiver with owner/date and follow-up issue.

## 8. Evidence log template

```text
Drill ID: DR-M134-02
Step: rollback rehearsal
Command: python scripts/check_objc3c_dependency_boundaries.py --strict
Timestamp (UTC): 2026-02-27T23:10:00Z
Owner: Lane A owner
Exit code: 0
Artifact: tmp/artifacts/objc3c-native/<run-id>/summary.json
Notes: baseline restored after rollback
```

## 9. Sign-off checklist

- [ ] All drill scenarios have pass/fail dispositions.
- [ ] Ownership and operator for each command are recorded.
- [ ] Pending evidence placeholders are either resolved or explicitly waived.
- [ ] Release owner and integrator sign-off fields are complete.
