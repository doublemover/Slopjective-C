# M314-C003 Expectations

Contract ID: `objc3c-cleanup-public-command-budget-and-cli-doc-sync/m314-c003-v1`

`M314-C003` enforces the compact public command budget against the live runner
surface and synchronizes the operator-facing command docs to that same source of
truth.

Expected outcomes:

- The live public script count stays within the `M314` budget.
- `package.json`, `README.md`, and the public-command runbook all describe the
  same public script surface.
- The public-command runbook is generated from the live runner metadata rather
  than maintained as a second handwritten list.
- The synchronized runbook records runner action IDs, pass-through rules, and
  backend surfaces for each public package script.

Boundary notes:

- Do not broaden the public command set.
- Do not rewrite maintainer workflow docs beyond the synchronized public-command
  surface; `M314-D001` owns the maintainer/operator docs contract.
