# M276-D001 Readiness-Runner And Checker Migration To Fast-Vs-Full Validation Policy Packet

Issue: `#7389`
Milestone: `M276`
Lane: `D`

## Objective

Migrate the active issue-work readiness range off unconditional monolithic native rebuilds while preserving truthful dependency chaining and fail-closed validation semantics.

## Dependency handoff

- Depends on `M276-C001`, `M276-C002`, and `M276-D002`.
- Consumes the shared helper added in `M276-D002`.
- Leaves historical runner-compatibility expansion to `M276-D003`.

## Implementation truths

- Active `M262` and `M263` lane `A` through `D` readiness runners now acquire binaries through `scripts/ensure_objc3c_native_build.py --mode fast`.
- Lane `E` aggregators remain orchestration layers over the migrated lane runners instead of reacquiring native builds themselves.
- The helper summary path under `tmp/reports/<milestone>/<issue>/ensure_objc3c_native_build_summary.json` is now part of the active readiness contract.
- The issue does not change issue-local dependency checker chains; it changes only build acquisition and policy disclosure.

## Proof model

- Statistically verify the active runner range is migrated onto the helper and no longer hard-codes `build:objc3c-native`.
- Execute one migrated `M262` readiness chain and one migrated `M263` readiness chain.
- Verify the helper summaries emitted by those representative runs record `fast` mode.
- Verify the docs/package surface now explains the fast-vs-full boundary truthfully.

## Exit condition

The active readiness-runner range no longer pays avoidable native rebuild costs for every issue, the shared helper owns build acquisition across that range, and full builds are explicitly reserved for closeout/CI/cross-cutting proof paths.
