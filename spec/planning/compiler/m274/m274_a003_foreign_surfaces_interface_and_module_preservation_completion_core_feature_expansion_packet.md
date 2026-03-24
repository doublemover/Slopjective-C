# M274-A003 Packet: Part 11 Foreign Surfaces Interface And Module Preservation - Core Feature Expansion

Packet: `M274-A003`
Milestone: `M274`
Lane: `A`
Dependencies: `M274-A001`, `M274-A002`
Next issue: `M274-B001`

## Objective

Freeze the Part 11 foreign surface preservation packet across provider and consumer compilations so imported module inventories and foreign/C++/Swift-facing annotation facts stay deterministic across separate compilation.

## Required Outputs

- semantic surface `frontend.pipeline.semantic_surface.objc_part11_foreign_surface_interface_and_module_preservation`
- issue-local checker `scripts/check_m274_a003_foreign_surfaces_interface_and_module_preservation_completion_core_feature_expansion.py`
- issue-local readiness runner `scripts/run_m274_a003_lane_a_readiness.py`
- issue-local pytest `tests/tooling/test_check_m274_a003_foreign_surfaces_interface_and_module_preservation_completion_core_feature_expansion.py`

## Canonical Payload Expectations

The emitted manifest and runtime-import-surface artifacts must preserve a deterministic separate-compilation packet with the following field families:

- source replay continuity:
  - `foreign_import_source_replay_key`
  - `cpp_swift_source_replay_key`
- local provider facts:
  - `local_foreign_callable_count`
  - `local_import_module_annotation_count`
  - `local_imported_module_name_count`
  - `local_swift_name_annotation_count`
  - `local_swift_private_annotation_count`
  - `local_cpp_name_annotation_count`
  - `local_header_name_annotation_count`
  - `local_named_annotation_payload_count`
  - `local_import_module_names_lexicographic`
- imported provider facts:
  - `imported_provider_module_names_lexicographic`
  - `imported_module_count`
  - imported aggregate Part 11 local counts
- preservation state:
  - `runtime_import_artifact_ready`
  - `separate_compilation_preservation_ready`
  - `deterministic`

## Non-Goals

- new IR metadata nodes
- foreign ABI lowering
- runtime bridge generation
