<!-- markdownlint-disable-file MD041 -->

## Live Validation Commands

From repo root:

```powershell
npm run test:objc3c
npm run test:objc3c:execution-smoke
npm run test:objc3c:execution-replay-proof
npm run test:objc3c:full
npm run test:ci
npm run proof:objc3c
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
