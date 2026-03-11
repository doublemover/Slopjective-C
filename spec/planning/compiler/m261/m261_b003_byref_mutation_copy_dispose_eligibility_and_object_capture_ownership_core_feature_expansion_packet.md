# M261-B003 Byref Mutation Copy-Dispose Eligibility And Object-Capture Ownership Core Feature Expansion Packet

Issue: `#7184`
Packet: `M261-B003`
Milestone: `M261`
Lane: `B`

## Summary

Close the remaining semantic gap around byref mutation, helper eligibility, and object-capture ownership so source-only block semantics stay aligned with the planned runnable block runtime.

## Required Work

- reject mutation of captured `__weak` and `__unsafe_unretained` objects
- promote copy/dispose helper eligibility for owned object captures even without byref cells
- keep weak/unowned object captures non-owning for helper-eligibility purposes
- preserve the native fail-closed block boundary
- land deterministic docs/checker/readiness/test coverage

## Anchors

- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/ast/objc3_ast.h`
- `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `docs/objc3c-native.md`
- `spec/PART_0_BASELINE_AND_NORMATIVE_REFERENCES.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`

## Validation

- `python scripts/check_m261_b003_byref_mutation_copy_dispose_eligibility_and_object_capture_ownership_core_feature_expansion.py`
- `python -m pytest tests/tooling/test_check_m261_b003_byref_mutation_copy_dispose_eligibility_and_object_capture_ownership_core_feature_expansion.py -q`
- `python scripts/run_m261_b003_lane_b_readiness.py`

`M261-C001` is the explicit next issue after this implementation lands.
