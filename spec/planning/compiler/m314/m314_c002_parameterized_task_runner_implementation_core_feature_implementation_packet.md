# M314-C002 Planning Packet

## Summary

Implement the `M314-C001` workflow API contract as a parameterized action
registry with stable introspection output.

## Implementation shape

- Replace the runner action ladder with one action-spec registry plus one action
  handler map.
- Add `--list-json` and `--describe <action>` so later docs and budget checks
  can consume the live runner surface instead of scraping source.
- Enforce the `M314-C001` pass-through rule by rejecting extra arguments on
  fixed-shape actions.

## Non-goals

- Do not change the public action vocabulary.
- Do not reintroduce package-level alias families here.
- Do not finish CLI docs synchronization; `M314-C003` owns that.

## Evidence

- `spec/planning/compiler/m314/m314_c002_parameterized_task_runner_implementation_core_feature_implementation_registry.json`
- `python scripts/objc3c_public_workflow_runner.py --list-json`
- `python scripts/objc3c_public_workflow_runner.py --describe compile-objc3c`

Next issue: `M314-C003`.
