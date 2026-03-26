# M315-A001 Planning Packet

## Summary

Freeze the repo-wide milestone-residue inventory so the rest of `M315` works
from one explicit baseline instead of scattered anecdotes.

## Implementation shape

- Count milestone-coded residue matches repo-wide outside `.git/` and `tmp/`.
- Classify those matches by broad cleanup-relevant scope:
  package surface, operator docs, generated docs, planning/specs, tooling
  scripts, tests, native source, GitHub metadata, and other tracked surfaces.
- Record the top hotspot files and the top milestone families by raw match
  volume.
- Identify which downstream `M315` issues own the main cleanup slices.

## Non-goals

- Do not remove residue yet.
- Do not narrow the inventory to native source only.
- Do not classify artifact authenticity yet; `M315-A003` owns that.

## Evidence

- `spec/planning/compiler/m315/m315_a001_repo_wide_milestone_residue_inventory_contract_and_architecture_freeze_inventory.json`

Next issue: `M315-A002`.
