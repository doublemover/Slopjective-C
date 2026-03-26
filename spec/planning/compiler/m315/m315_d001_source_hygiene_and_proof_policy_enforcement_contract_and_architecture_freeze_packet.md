# M315-D001 Planning Packet

Issue: `M315-D001`
Title: `Source-hygiene and proof-policy enforcement contract`
Contract ID: `objc3c.cleanup.source-hygiene-and-proof-policy.enforcement/m315-d001-v1`

## Problem
After `M315-C004`, the repo has a truthful synthetic-fixture authenticity model, but product code still carries a narrow band of milestone-coded residue in two classes:
- transitional lowering-contract source-model strings;
- `m248` integration-closeout field and key names.

The enforcement lane should remove those classes and guard them from reappearing, without folding every older quarantined schema/path reference into the live guard.

## Decision
- freeze `M315-D002` against zero residuals for:
  - `transitional_source_model`
  - `legacy_m248_surface_identifier`
- keep `legacy_fixture_path_reference`, `dependency_issue_array`, `next_issue_schema_field`, and `issue_key_schema_field` explicitly quarantined outside the live D002 zero-residue target;
- require the live guard to enforce:
  - zero tracked prototype compiler Python sources under `compiler/objc3c`
  - explicit synthetic authenticity labeling for `library_cli_parity`
  - zero new milestone-coded product residue after the D002 sweep

## Acceptance proof
- the machine-readable contract matches the current B005 residual inventory;
- the contract records zero tracked prototype compiler Python sources;
- the contract records the committed synthetic parity-fixture surface exactly.

Next issue: `M315-D002`.
