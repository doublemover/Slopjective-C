# M315-D002 Planning Packet

Issue: `M315-D002`
Title: `Anti-noise enforcement implementation`
Contract ID: `objc3c.cleanup.anti-noise.enforcement/m315-d002-v1`

## Problem
`M315-D001` froze two targeted residue classes that still remained live in product code:
- `transitional_source_model`
- `legacy_m248_surface_identifier`

Those classes now need to reach zero in live product-code surfaces, and the repo needs a guard that prevents their reintroduction without broadening the enforcement scope to every older quarantined schema/path reference.

## Decision
- replace the `m248` final-readiness signoff surface with the stable `advanced_integration_closeout_signoff` family;
- replace milestone-coded lowering fail-closed and dependency literals with stable capability-oriented wording;
- keep `legacy_fixture_path_reference`, `dependency_issue_array`, `next_issue_schema_field`, and `issue_key_schema_field` quarantined outside the D002 zero-target set;
- add a live anti-noise enforcement checker and CI workflow that fail closed on:
  - tracked `compiler/objc3c/*.py` reintroduction;
  - synthetic parity-fixture authenticity drift;
  - milestone-coded residue in the four D002 target product-code files.

## Acceptance proof
- the B005 native-source sweep reports only the quarantined residual classes and `57` remaining milestone-token lines;
- the target files carry no `m2xx-*` tokens and no `m248_integration_closeout_signoff` residue;
- the stable `advanced_integration_closeout_signoff` identifiers and key surface are present in the final-readiness gate product code;
- the parity synthetic-fixture surface remains explicitly labeled and the D002 workflow runs the live guard.

Next issue: `M315-E001`.