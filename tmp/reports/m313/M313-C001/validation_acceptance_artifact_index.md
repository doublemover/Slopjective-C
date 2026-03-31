# M313-C001 Validation Acceptance Artifact Index

- contract_id: `objc3c.validation.acceptance.artifact.index.v1`
- schema_path: `schemas/objc3c-validation-acceptance-artifact-index-v1.schema.json`
- artifact_count: `4`

## Indexed artifacts
- `M313-A001` -> `validation-surface-inventory`
  - report_path: `tmp/reports/m313/M313-A001/validation_surface_inventory.json`
  - planning_source_paths: `tmp/planning/validation_consolidation/build_validation_surface_inventory.py`
- `M313-B001` -> `validation-policy`
  - report_path: `tmp/reports/m313/M313-B001/policy_summary.json`
  - planning_source_paths: `tmp/planning/validation_consolidation/validation_consolidation_policy.json`, `tmp/planning/validation_consolidation/build_validation_policy_summary.py`
- `M313-B002` -> `validation-harness-catalog`
  - report_path: `tmp/reports/m313/M313-B002/validation_harness_catalog.json`
  - planning_source_paths: `tmp/planning/validation_consolidation/build_validation_harness_catalog.py`, `tmp/planning/validation_consolidation/validation_harness_catalog.json`
- `M313-B003` -> `legacy-validation-surface-map`
  - report_path: `tmp/reports/m313/M313-B003/legacy_validation_surface_map.json`
  - planning_source_paths: `tmp/planning/validation_consolidation/build_legacy_validation_surface_map.py`, `tmp/planning/validation_consolidation/legacy_validation_surface_map.json`

Next issues: `M313-C002`, `M313-C003`, `M313-D001`
