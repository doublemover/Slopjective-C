<!-- markdownlint-disable-file MD041 -->

## Result-like lowering artifact contract (M182-C001)

M182-C publishes deterministic lowering replay metadata for result-like control-flow handoff.

M182-C lowering contract anchors:

- `kObjc3ResultLikeLoweringLaneContract`
- `Objc3ResultLikeLoweringContract`
- `IsValidObjc3ResultLikeLoweringContract(...)`
- `Objc3ResultLikeLoweringReplayKey(...)`

Deterministic handoff checks:

- `normalized_sites + branch_merge_sites == result_like_sites`
- each of `result_success_sites`, `result_failure_sites`, `result_branch_sites`, `result_payload_sites`, and `contract_violation_sites` is bounded by `result_like_sites`
- `deterministic_result_like_lowering_handoff` requires zero contract violations

IR replay publication marker:

- `result_like_lowering = result_like_sites=<N>;result_success_sites=<N>;result_failure_sites=<N>;result_branch_sites=<N>;result_payload_sites=<N>;normalized_sites=<N>;branch_merge_sites=<N>;contract_violation_sites=<N>;deterministic=<bool>;lane_contract=m182-result-like-lowering-v1`

Lane-C validation command:

- `python -m pytest tests/tooling/test_objc3c_m182_lowering_result_like_contract.py -q`
