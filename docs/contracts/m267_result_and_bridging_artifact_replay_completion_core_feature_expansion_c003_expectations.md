# M267 Result And Bridging Artifact Replay Completion Core Feature Expansion Expectations (C003)

Contract ID: `objc3c-part6-result-and-bridging-artifact-replay/m267-c003-v1`
Status: Accepted
Issue: `#7276`
Scope: M267 lane-C artifact-chain replay completion for Part 6 result and bridging lowering.

## Objective

Prove that the Part 6 lowering replay packet survives across a producer/consumer
module boundary through both emitted sidecar artifacts and the emitted
runtime-import-surface payload.

Canonical surface path:
`frontend.pipeline.semantic_surface.objc_part6_result_and_bridging_artifact_replay`

Canonical emitted sidecar artifact:
`module.part6-error-replay.json`

Canonical import artifact:
`module.runtime-import-surface.json`

## Required Invariants

1. The producer module must emit:
   - `module.manifest.json`
   - `module.ll`
   - `module.obj`
   - `module.part6-error-replay.json`
   - `module.runtime-import-surface.json`
2. The consumer module must be compiled with:
   - `--objc3-bootstrap-registration-order-ordinal 2`
   - `--objc3-import-runtime-surface <producer/module.runtime-import-surface.json>`
3. The producer and consumer manifests must both publish the
   `objc_part6_result_and_bridging_artifact_replay` semantic surface.
4. The emitted IR must publish both:
   - `; part6_result_and_bridging_artifact_replay = ...`
   - `!objc3.objc_part6_result_and_bridging_artifact_replay = !{!...}`
5. The runtime-import surface emitted by the consumer must preserve the imported
   Part 6 replay chain, including the producer module identity and the imported
   replay key inventories.
6. `module.object-backend.txt` remains `llvm-direct` for both producer and consumer.

## Happy-Path Coverage

The checker must prove:

1. A producer fixture that owns one trivial runtime class plus Part 6 throwing
   and bridging functions emits both the sidecar replay artifact and the runtime
   import artifact.
2. A consumer fixture compiled against the producer runtime-import surface
   preserves the imported replay chain, module-name inventory, and imported
   replay-key inventories.
3. The replay payload remains consistent between manifest output, runtime-import
   surface output, and IR anchors.

## Non-Goals And Fail-Closed Rules

- `M267-C003` does not introduce new runtime semantics.
- `M267-C003` does not claim generalized foreign error-object ABI support.
- `M267-C003` does not require linking or running a separate executable.
- Drift between the manifest surface, runtime-import surface, and IR anchors
  must fail closed.

## Validation

- `python scripts/check_m267_c003_result_and_bridging_artifact_replay_completion_core_feature_expansion.py`
- `python -m pytest tests/tooling/test_check_m267_c003_result_and_bridging_artifact_replay_completion_core_feature_expansion.py -q`
- `python scripts/run_m267_c003_lane_c_readiness.py`

## Evidence Path

- `tmp/reports/m267/M267-C003/result_and_bridging_artifact_replay_completion_summary.json`
