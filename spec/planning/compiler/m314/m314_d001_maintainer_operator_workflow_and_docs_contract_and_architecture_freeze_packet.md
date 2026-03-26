# M314-D001 Planning Packet

## Summary

Freeze the canonical operator-doc map for the cleaned `M314` command surface so
maintainers, operators, and future gates all point at the same workflow docs.

## Implementation shape

- Add one maintainer-oriented runbook that links the public command runbook, the
  incremental native build runbook, and the README quickstart.
- Publish one machine-readable operator-doc contract with the canonical doc
  paths, primary entrypoint rules, and internal-surface policy.
- Mirror that contract into `package.json` and the README.

## Non-goals

- Do not add new public commands.
- Do not reintroduce package-level compatibility aliases here.
- Do not publish the milestone gate/matrix here.

## Evidence

- `docs/runbooks/objc3c_maintainer_workflows.md`
- `spec/planning/compiler/m314/m314_d001_maintainer_operator_workflow_and_docs_contract_and_architecture_freeze_contract.json`

Next issue: `M314-E001`.
