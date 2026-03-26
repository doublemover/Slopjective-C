<!-- markdownlint-disable-file MD041 -->

## Live Validation Commands

From repo root:

```powershell
python scripts/objc3c_public_workflow_runner.py test-recovery
python scripts/objc3c_public_workflow_runner.py test-execution-smoke
python scripts/objc3c_public_workflow_runner.py test-execution-replay
python scripts/objc3c_public_workflow_runner.py test-full
python scripts/objc3c_public_workflow_runner.py test-ci
python scripts/objc3c_public_workflow_runner.py proof-objc3c
python scripts/ci/check_task_hygiene.py
python scripts/check_objc3c_dependency_boundaries.py --strict
```

## What The Live Test Surface Covers

- recovery and deterministic compile behavior
- runnable execution smoke coverage
- replay proof coverage
- fixture-matrix execution
- negative fixture expectations
- dependency-boundary enforcement
- compact task-hygiene enforcement
- runtime dispatch over realized classes/categories/protocols
- synthesized property accessor execution over realized instance storage
- native-output provenance through real compile and probe paths

## Current Corrective Gaps Under Test

- unresolved dispatch still has one deterministic fallback path after slow-path miss
- synthesized accessor IR still carries transitional lowering residue even though live getter/setter execution is already runtime-backed
- native-output truth requires the emitted object and linked probe to stay coupled end to end
