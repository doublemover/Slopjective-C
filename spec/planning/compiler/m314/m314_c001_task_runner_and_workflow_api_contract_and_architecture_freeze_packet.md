# M314-C001 Planning Packet

## Summary

Freeze the API contract for the unified public workflow runner so later runner
implementation and command-budget work has one stable action vocabulary.

## Implementation shape

- Publish one machine-readable workflow API contract describing the public
  action identifiers, package-script wrappers, pass-through rules, and backend
  surfaces.
- Mirror the contract ID and runner path into `package.json`.
- Document that the public package commands delegate to the shared runner rather
  than acting as unrelated direct script entrypoints.

## Non-goals

- Do not refactor the runner implementation into a registry yet.
- Do not reintroduce package-level alias families here.
- Do not broaden operator docs beyond the public-surface note.

## Evidence

- `spec/planning/compiler/m314/m314_c001_task_runner_and_workflow_api_contract_and_architecture_freeze_contract.json`

Next issue: `M314-C002`.
