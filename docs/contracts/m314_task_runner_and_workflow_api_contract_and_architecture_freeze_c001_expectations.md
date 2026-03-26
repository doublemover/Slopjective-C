# M314-C001 Expectations

Contract ID: `objc3c-cleanup-task-runner-workflow-api/m314-c001-v1`

`M314-C001` freezes the operator-facing API contract for the unified public
workflow runner introduced earlier in `M314`.

Expected outcomes:

- `scripts/objc3c_public_workflow_runner.py` is named as the single public
  task-runner entrypoint behind the compact `package.json` command surface.
- The public action set is published machine-readably in one workflow API
  contract JSON document under `spec/planning/compiler/m314/`.
- `package.json` carries workflow API metadata under
  `objc3cCommandSurface.workflowApi`.
- `README.md` states that the documented public package commands are thin
  wrappers over the unified workflow runner.
- The contract freezes pass-through behavior only for `compile-objc3c`; all
  other public actions remain fixed-shape operator entrypoints.

Boundary notes:

- Do not rewrite the runner implementation shape yet; `M314-C002` owns the
  parameterized registry implementation.
- Do not expand the public command budget; `M314-C003` owns enforcement and CLI
  synchronization.
