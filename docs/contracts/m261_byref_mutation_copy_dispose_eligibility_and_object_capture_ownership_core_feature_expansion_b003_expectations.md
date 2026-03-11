# M261 Byref Mutation Copy-Dispose Eligibility And Object-Capture Ownership Core Feature Expansion Expectations (B003)

Contract ID: `objc3c-executable-block-byref-copy-dispose-and-object-capture-ownership/m261-b003-v1`

Scope: Lane-B source-only block semantics now distinguish owned vs weak/unowned object captures for mutation legality and helper eligibility while native block execution remains fail-closed.

## Required Outcomes

1. Mutating a captured `__weak` or `__unsafe_unretained` object fails closed with `O3S206`.
2. Owned object captures promote copy/dispose helper eligibility even when `byref_slot_count_total == 0`.
3. Weak and unowned object captures remain non-owning and do not force copy/dispose helpers by themselves.
4. Native emit paths still reject block literals with `O3S221`.
5. The next issue remains `M261-C001`.

## Validation

- `python scripts/check_m261_b003_byref_mutation_copy_dispose_eligibility_and_object_capture_ownership_core_feature_expansion.py`
- `python -m pytest tests/tooling/test_check_m261_b003_byref_mutation_copy_dispose_eligibility_and_object_capture_ownership_core_feature_expansion.py -q`
- `python scripts/run_m261_b003_lane_b_readiness.py`

## Evidence

- `tmp/reports/m261/M261-B003/byref_mutation_copy_dispose_eligibility_and_object_capture_ownership_summary.json`
