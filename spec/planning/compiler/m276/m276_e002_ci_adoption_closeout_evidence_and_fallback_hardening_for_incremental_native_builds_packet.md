# M276-E002 CI Adoption, Closeout Evidence, And Fallback Hardening For Incremental Native Builds Packet

Issue: `#7391`
Milestone: `M276`
Lane: `E`

## Objective

Close the incremental native build modernization milestone by proving the new command taxonomy is truthful in both local and CI execution contexts.

## Dependency handoff

- Depends on `M276-D001`, `M276-D003`, and `M276-E001`.
- Consumes the fast/helper migration, historical compatibility boundary, and operator-facing command/runbook surface.
- Closes milestone `M276`.

## Implementation truths

- Local issue work uses a persistent build tree at `tmp/build-objc3c-native`.
- CI runners start from clean workspaces, so `fast`, `contracts`, and `full` define proof scope rather than warm-cache expectations.
- `.github/workflows/task-hygiene.yml` is the active fast-path workflow and must also prove the contracts-only packet path.
- `.github/workflows/compiler-closeout.yml` is the manual closeout workflow and must prove the full build path.
- Deterministic recovery for invalid persistent-build state is `build:objc3c-native:reconfigure`; no deletion workflow is part of the contract.

## Proof model

- Statistically verify the workflow/package/docs surface is synchronized.
- Move any existing persistent build tree aside into `tmp/` evidence storage and prove a cold fast build recreates it.
- Re-run the fast path without source edits and prove the native outputs are unchanged.
- Corrupt the fingerprint payload deterministically and prove the fast path refreshes configuration.
- Run the contracts-only path and prove native binary mtimes stay fixed while the source-derived plus binary-derived packet family refreshes.
- Run the full path and prove the closeout packet family refreshes.
- Emit stable evidence under `tmp/reports/m276/M276-E002/`.

## Exit condition

The repo has a truthful default local build path for ordinary issue work, a truthful contracts-only packet path, a truthful full closeout path, and documented deterministic fallback behavior without any destructive cleanup flow.
