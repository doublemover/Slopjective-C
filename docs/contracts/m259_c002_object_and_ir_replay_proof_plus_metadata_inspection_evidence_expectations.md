# M259 C002 Object And IR Replay-Proof Plus Metadata Inspection Evidence Expectations

Contract ID: `objc3c-runnable-object-ir-replay-and-metadata-inspection/m259-c002-v1`

Issue: `#7213`

## Objective

Implement the live replay-proof and metadata-inspection evidence path for the
canonical runnable object-model sample.

## Required implementation

1. Add the issue-local contract/checker/test/readiness assets:
   - `docs/contracts/m259_c002_object_and_ir_replay_proof_plus_metadata_inspection_evidence_expectations.md`
   - `spec/planning/compiler/m259/m259_c002_object_and_ir_replay_proof_plus_metadata_inspection_evidence_packet.md`
   - `scripts/check_m259_c002_object_and_ir_replay_proof_plus_metadata_inspection_evidence_core_feature_implementation.py`
   - `tests/tooling/test_check_m259_c002_object_and_ir_replay_proof_plus_metadata_inspection_evidence_core_feature_implementation.py`
   - `scripts/run_m259_c002_lane_c_readiness.py`
2. Publish explicit docs/spec/package/script anchors in:
   - `docs/objc3c-native.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `scripts/check_objc3c_native_execution_smoke.ps1`
   - `scripts/check_objc3c_execution_replay_proof.ps1`
   - `package.json`
3. Extend `scripts/check_objc3c_execution_replay_proof.ps1` into a real live
   capability that:
   - compiles `tests/tooling/fixtures/native/m259_a002_canonical_runnable_sample_set.objc3` twice
   - proves replay-stable hashes for `module.ll`
   - proves replay-stable hashes for `module.obj`
   - proves replay-stable hashes for `llvm-readobj --sections` output
   - verifies the required runtime metadata sections remain present
4. Add deterministic checker coverage over the live happy path.
5. Keep explicit truthful boundaries:
   - scalar/core smoke remains the broad replay corpus
   - C002 adds the dedicated canonical object-model IR/object/inspection proof
   - no broader archive/static-link, multi-module, or multi-image replay matrix lands here
6. The contract must explicitly hand off to `M259-D001`.

## Canonical models

- Replay model:
  `a002-canonical-runnable-sample-ir-object-and-readobj-section-replay`
- Evidence model:
  `execution-replay-proof-script-emits-live-ir-object-and-section-inspection-hashes-for-a002`
- Failure model:
  `fail-closed-on-ir-object-or-metadata-inspection-replay-drift`

## Evidence

- `tmp/reports/m259/M259-C002/object_and_ir_replay_proof_plus_metadata_inspection_summary.json`
