# M276 Historical Runner Compatibility Strategy During Build-Surface Migration Expectations (D003)

Contract ID: `objc3c-historical-runner-build-surface-compatibility/m276-d003-v1`

## Required outcomes

- Older readiness runners that still call `npm run build:objc3c-native` directly must remain truthful and deterministic during the migration.
- The public default build command must keep guaranteeing canonical native binaries at the published `artifacts/` paths for those historical callers.
- Historical raw-build callers must not imply that packet-generation or closeout work is bundled into the default build command.
- At least one helper-driven active runner and one representative historical raw-build runner must coexist correctly under the new build surface.

## Required implementation surface

- `scripts/run_m257_a001_lane_a_readiness.py`
- `scripts/run_m262_a001_lane_a_readiness.py`
- `scripts/build_objc3c_native.ps1`
- `package.json`
- `README.md`
- `docs/objc3c-native/src/50-artifacts.md`
- `native/objc3c/src/ARCHITECTURE.md`

## Compatibility model

- Active issue-work runners use `scripts/ensure_objc3c_native_build.py --mode fast`.
- Historical runners may continue calling `npm run build:objc3c-native` directly.
- The default build command remains a binary-only compatibility surface:
  - canonical native binaries are refreshed
  - packet generation is not implied
  - downstream runner checkers remain responsible for their own proof work

## Proof obligations

- One representative historical runner must still use the raw default build command.
- One representative active runner must use the shared helper.
- Both representative runners must complete successfully on the same repo state.
- The issue-local evidence must live under `tmp/reports/m276/M276-D003/`.
