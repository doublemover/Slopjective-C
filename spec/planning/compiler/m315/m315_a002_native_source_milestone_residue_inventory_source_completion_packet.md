# M315-A002 Planning Packet

## Summary

Complete the native-source slice of the milestone-residue baseline so later lane
`B` cleanup work can target the worst product-code hotspots directly.

## Implementation shape

- Count milestone-coded residue within `native/objc3c/src/` only.
- Break the residue down by subdirectory, by file kind, and by hotspot file.
- Distinguish embedded-doc mass from code-file mass so later removal issues can
  prioritize the right files.

## Non-goals

- Do not remove native-source residue yet.
- Do not classify replay/proof artifacts yet.

## Evidence

- `spec/planning/compiler/m315/m315_a002_native_source_milestone_residue_inventory_source_completion_inventory.json`

Next issue: `M315-A003`.
