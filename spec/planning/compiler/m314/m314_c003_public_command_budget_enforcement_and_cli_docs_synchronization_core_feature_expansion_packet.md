# M314-C003 Planning Packet

## Summary

Enforce the compact public command budget using the live runner action list and
sync the committed public command docs to that same live source.

## Implementation shape

- Add one renderer/check helper for the public command runbook.
- Commit one synchronized runbook under `docs/runbooks/`.
- Require `package.json`, `README.md`, and the live runner `--list-json` output
  to agree on the public command set.
- Freeze the budget/doc path machine-readably for later workflow-doc closeout.

## Non-goals

- Do not expand the public workflow API.
- Do not rewrite broader maintainer onboarding docs yet.
- Do not reintroduce legacy compatibility aliases beyond the retired-surface metadata.

## Evidence

- `docs/runbooks/objc3c_public_command_surface.md`
- `python scripts/render_objc3c_public_command_surface.py --check`
- `python scripts/objc3c_public_workflow_runner.py --list-json`

Next issue: `M314-D001`.
