# M314-D001 Expectations

Contract ID: `objc3c-cleanup-maintainer-operator-workflow-docs/m314-d001-v1`

`M314-D001` freezes the maintainer/operator workflow documentation contract for
the cleaned public command surface.

Expected outcomes:

- One maintainer runbook exists at `docs/runbooks/objc3c_maintainer_workflows.md`.
- The maintainer runbook names the public command surface, the synchronized
  public-command runbook, and the retained incremental native build runbook as
  the canonical workflow docs.
- `package.json` carries machine-readable operator-doc metadata.
- `README.md` points maintainers at the maintainer runbook instead of treating
  the README itself as the only workflow reference.
- The contract explicitly states that `native/objc3c/` is the only supported
  compiler implementation path and that direct Python/PowerShell tooling remains
  an internal surface unless a public wrapper does not exist.

Boundary notes:

- Do not widen the public command surface.
- Do not rewrite CI closeout gates here; `M314-E001` and `M314-E002` own the
  gate/matrix proofs.
