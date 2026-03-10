# M258-A002 Import Surface For Runtime-Owned Declarations And Metadata References Core Feature Implementation Packet

Packet: `M258-A002`
Issue: `#7159`
Milestone: `M258`
Lane: `A`
Dependencies: `M258-A001`
Next issue: `M258-B001`

## Objective

Implement import surface modeling for runtime-owned declarations and metadata references as one emitted frontend artifact so later cross-module work consumes a real compiler capability instead of only a manifest summary.

## Required Outputs

- semantic surface
  `frontend.pipeline.semantic_surface.objc_runtime_aware_import_module_frontend_closure`
- emitted artifact
  `module.runtime-import-surface.json`
- issue-local checker
  `scripts/check_m258_a002_import_surface_for_runtime_owned_declarations_and_metadata_references_core_feature_implementation.py`
- issue-local readiness runner
  `scripts/run_m258_a002_lane_a_readiness.py`
- issue-local pytest
  `tests/tooling/test_check_m258_a002_import_surface_for_runtime_owned_declarations_and_metadata_references_core_feature_implementation.py`

## Canonical Payload Expectations

The emitted artifact must preserve the frozen `M258-A001` declaration/import-graph counts and add:

- runtime-owned declarations:
  - classes
  - protocols
  - categories
  - properties
  - methods
  - ivars
- metadata references:
  - superclass edges
  - protocol conformance / inheritance edges
  - property accessor selector edges
  - property ivar-binding symbol edges
  - method selector edges

## Canonical Code Anchors

- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.h`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/io/objc3_manifest_artifacts.h`
- `native/objc3c/src/io/objc3_manifest_artifacts.cpp`
- `native/objc3c/src/driver/objc3_objc3_path.cpp`
- `native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp`
- `native/objc3c/src/libobjc3c_frontend/api.h`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`

## Canonical Proof Fixtures

- `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`
- `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3`

## Fail-Closed Rules

- Successful frontend compilation must not skip the emitted import-surface artifact.
- Driver and frontend-runner outputs must agree on artifact shape.
- IR must remain explicit that foreign metadata is not yet lowered.
- Public embedding ABI must stay filesystem-artifact based for this tranche.

## Evidence

- `tmp/reports/m258/M258-A002/runtime_aware_import_module_frontend_closure_summary.json`
