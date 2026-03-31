# M315-A001 Residue Authenticity Inventory

- tracked_file_count: `4839`
- product_file_count: `435`
- archive_file_count: `80`
- generated_truth_file_count: `3`
- product_residue_hit_count: `59`
- generated_truth_residue_hit_count: `1`
- ll_file_count: `76`
- json_file_count: `2543`

## Residue bucket counts
- `docs`: `20`
- `native`: `9`
- `schemas`: `1`
- `scripts`: `24`
- `site`: `1`
- `stdlib`: `4`

## Generated truth residue hits
- `docs/objc3c-native.md` -> `M281`

## Authenticity classes
- `generated_truth`: `3`
  - checked-in generated outputs that are canonical user or operator surfaces
- `synthetic_or_replay_ll`: `76`
  - tracked llvm ir artifacts checked into test fixtures or replay contracts
- `tracked_test_and_conformance_json`: `2414`
  - tracked json fixtures, corpora, and validation contracts rooted under tests/
- `archive_or_planning`: `80`
  - tracked draft, planning, publish, or redirect material that is explicitly non-product

## Follow-on priorities
- remove milestone-coded residue from generated truth surfaces first
- replace milestone-coded identifiers in live source comments and contract strings with stable feature-surface identifiers
- separate genuine generated outputs from synthetic fixtures and archive-only material in machine-readable provenance

Next issue: `M315-B001`
