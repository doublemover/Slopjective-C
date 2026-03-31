# M315-B001 Stable Identifier Policy Summary

- contract_id: `objc3c.source_hygiene.stable_identifier_authenticity_policy.v1`
- policy_path: `tests/tooling/fixtures/source_hygiene/stable_identifier_authenticity_policy.json`
- inventory_path: `tmp/reports/m315/M315-A001/residue_authenticity_inventory.json`
- durable_identifier_regex: `^objc3c\.[a-z][a-z0-9]*(\.[a-z][a-z0-9]*)+\.v[0-9]+$`
- generated_truth_file_count: `3`
- generated_truth_residue_hit_count: `1`
- product_residue_hit_count: `59`
- ok: `True`

## Authenticity classes
- `archive_reference`
- `generated_truth`
- `genuine_generated_output`
- `synthetic_fixture`

## Annotation families
- `surface_id`
- `artifact_family_id`
- `fixture_family_id`
- `report_family_id`
- `historical_ref`

## Checks
- `generated_truth_surface_listed`: `True`
- `archive_roots_listed`: `True`
- `authenticity_class_count`: `True`
- `replacement_examples_present`: `True`
- `inventory_baseline_loaded`: `True`

Next issue: `M315-B002`
