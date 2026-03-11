# M276-D003 Historical Runner Compatibility Strategy During Build-Surface Migration Packet

Issue: `#7395`
Milestone: `M276`
Lane: `D`

## Objective

Keep older readiness runners truthful while active issue work migrates onto the helper-driven fast build policy.

## Dependency handoff

- Depends on `M276-D002`.
- Consumes the helper-driven active range landed in `M276-D001`.
- Hands off closeout and operator-facing adoption work to `M276-E001`.

## Implementation truths

- The representative active helper path is `scripts/run_m262_a001_lane_a_readiness.py`.
- The representative historical raw-build path is `scripts/run_m257_a001_lane_a_readiness.py`.
- Historical raw-build callers remain supported through `npm run build:objc3c-native`, which is now a truthful binary-only compatibility surface.
- Historical compatibility does not imply packet regeneration or milestone-closeout semantics.

## Proof model

- Statistically verify the representative historical runner still calls the raw default build command.
- Statistically verify the representative active runner uses the helper fast path.
- Execute both representative runners successfully on the same repo state.
- Verify the helper summary from the active runner and the successful historical-runner log coexist under the D003 evidence root.

## Exit condition

The repo has an explicit, documented compatibility contract for older raw-build callers, and active helper-based runners can coexist with at least one representative historical runner without breaking the new incremental build surface.
