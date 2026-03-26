# M314-C002 Expectations

Contract ID: `objc3c-cleanup-parameterized-task-runner/m314-c002-v1`

`M314-C002` replaces the ad hoc action ladder in the public workflow runner with
an explicit parameterized action registry.

Expected outcomes:

- `scripts/objc3c_public_workflow_runner.py` exposes a stable action registry
  rather than a long open-coded `if action == ...` ladder.
- The runner publishes a machine-readable action listing via `--list-json`.
- The runner publishes per-action metadata via `--describe <action>`.
- Fixed-shape actions reject stray arguments; only `compile-objc3c` remains
  pass-through.
- `package.json` carries runner-mode and introspection metadata under
  `objc3cCommandSurface.workflowApi`.

Boundary notes:

- Keep the public action vocabulary identical to the `M314-C001` contract.
- Do not enforce the command budget or rewrite broader docs here; `M314-C003`
  owns that sync work.
