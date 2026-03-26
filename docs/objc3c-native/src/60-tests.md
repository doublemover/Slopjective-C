<!-- markdownlint-disable-file MD041 -->

## Live Validation Commands

From repo root:

```powershell
python scripts/objc3c_public_workflow_runner.py test-fast
python scripts/objc3c_public_workflow_runner.py test-recovery
python scripts/objc3c_public_workflow_runner.py test-execution-smoke
python scripts/objc3c_public_workflow_runner.py test-execution-replay
python scripts/objc3c_public_workflow_runner.py test-runtime-acceptance
python scripts/objc3c_public_workflow_runner.py test-full
python scripts/objc3c_public_workflow_runner.py test-nightly
python scripts/objc3c_public_workflow_runner.py test-ci
python scripts/objc3c_public_workflow_runner.py validate-runtime-architecture
python scripts/objc3c_public_workflow_runner.py proof-objc3c
python scripts/ci/check_task_hygiene.py
python scripts/check_objc3c_dependency_boundaries.py --strict
```

Targeted entrypoints accept bounded selectors when you need signal without the full corpus:

```powershell
python scripts/objc3c_public_workflow_runner.py test-execution-smoke -Limit 12
python scripts/objc3c_public_workflow_runner.py test-recovery -Limit 24
python scripts/objc3c_public_workflow_runner.py test-fixture-matrix -ShardIndex 0 -ShardCount 4
python scripts/objc3c_public_workflow_runner.py test-negative-expectations -FixtureGlob "tests/tooling/fixtures/native/recovery/negative/negative_assignment_*"
python scripts/objc3c_public_workflow_runner.py test-execution-replay -CaseId synthesized-accessor
```

Composite runner entrypoints also write one integrated report to `tmp/reports/objc3c-public-workflow/<action>.json`, with the exact child-suite summary paths captured from the live smoke, runtime-acceptance, replay, recovery, and matrix scripts.

## What The Live Test Surface Covers

- `test-fast`: bounded execution-smoke slice, runtime acceptance, and canonical replay/native-truth proof
- `test-smoke`: full runnable execution smoke corpus
- `test-recovery`: recovery compile success and deterministic diagnostics replay as a non-default heavy path
- `test-full`: smoke, runtime acceptance, and replay/native-truth proof without the recovery fan-out
- `test-nightly`: full validation plus recovery, positive fixture-matrix, and static negative-expectation sweeps
- `test:objc3c:runtime-architecture`: full public workflow plus runtime architecture proof-packet alignment
- dependency-boundary enforcement
- compact task-hygiene enforcement
- runtime dispatch over realized classes/categories/protocols
- synthesized property accessor execution over realized instance storage
- runtime-backed storage ownership reflection over emitted property descriptors
- native-output provenance through real compile and probe paths

## Current Corrective Gaps Under Test

- unresolved dispatch still has one deterministic fallback path after slow-path miss
- synthesized accessor IR still carries transitional lowering residue even though live getter/setter execution is already runtime-backed
- native-output truth requires the emitted object and linked probe to stay coupled end to end
