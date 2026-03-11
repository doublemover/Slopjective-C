# M276 Readiness-Runner And Checker Migration To Fast-Vs-Full Validation Policy Expectations (D001)

Contract ID: `objc3c-readiness-runner-fast-vs-full-migration/m276-d001-v1`

## Required outcomes

- The active `M262` and `M263` lane `A` through `D` readiness runners must acquire native binaries through `scripts/ensure_objc3c_native_build.py`.
- Those active readiness runners must default to helper `fast` mode instead of hard-coding `npm run build:objc3c-native`.
- Active lane `E` aggregators may remain simple readiness aggregators; they must not reacquire native builds directly.
- Full native builds remain reserved for milestone closeout, CI, and explicitly cross-cutting validation paths.
- The migration must preserve deterministic fail-closed behavior by keeping issue-local dependency checker chains intact after build acquisition changes.

## Required implementation surface

- `scripts/run_m262_*_lane_*_readiness.py`
- `scripts/run_m263_*_lane_*_readiness.py`
- `scripts/ensure_objc3c_native_build.py`
- `README.md`
- `docs/objc3c-native/src/50-artifacts.md`
- `native/objc3c/src/ARCHITECTURE.md`
- `package.json`

## Active runner scope

- `M262-A001`
- `M262-A002`
- `M262-B001`
- `M262-B002`
- `M263-A001`
- `M263-A002`
- `M263-B001`
- `M263-B002`
- `M263-B003`
- `M263-C001`
- `M263-C002`
- `M263-C003`
- `M263-D001`
- `M263-D002`
- `M263-D003`

## Proof obligations

- No active runner in scope may still contain a raw `build:objc3c-native` build-acquisition call.
- Every active runner in scope must request helper `fast` mode and emit a deterministic summary under `tmp/reports/`.
- At least one migrated `M262` readiness chain and one migrated `M263` readiness chain must complete successfully after the migration.
- The issue-local evidence must live under `tmp/reports/m276/M276-D001/`.
