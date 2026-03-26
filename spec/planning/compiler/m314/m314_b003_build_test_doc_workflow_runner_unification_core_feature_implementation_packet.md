# M314-B003 Planning Packet

## Summary

Implement one Python workflow runner for the compact public package subset defined in `M314-B002`.

## Implementation shape

- Add `scripts/objc3c_public_workflow_runner.py`.
- Route every `M314-B002` public package command through that runner.
- Keep PowerShell and direct Python tools as implementation details behind the runner.
- Leave non-public compatibility aliases untouched.

## Unified workflow coverage

- native build default and variants
- docs/spec build
- native compile wrapper
- spec lint
- public native test flows
- top-level test aliases in the public subset
- runnable toolchain packaging
- compile proof

## Non-goals

- Do not retire legacy aliases yet.
- Do not generalize the runner API beyond the public subset yet.
- Do not finalize documentation cleanup yet.

## Evidence

- `spec/planning/compiler/m314/m314_b003_build_test_doc_workflow_runner_unification_core_feature_implementation_plan.json`

Next issue: `M314-B004`.
