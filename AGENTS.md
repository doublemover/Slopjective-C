# Agent Rules

## Non-Destructive Artifact Policy (Hard Rule)
- Never run file deletion commands (for example: `rm`, `del`, `Remove-Item`, `git clean`).
- Never ask for approval to delete files.
- Never ask the user whether temporary files should be deleted.
- Keep all transient, scratch, and report artifacts in `tmp/`.
- If cleanup is needed, move artifacts into `tmp/` and leave them there.
