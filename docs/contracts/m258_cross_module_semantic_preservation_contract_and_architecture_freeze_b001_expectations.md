# M258 Cross-Module Semantic Preservation Contract And Architecture Freeze Expectations (B001)

Contract ID: `objc3c-cross-module-runtime-metadata-semantic-preservation/m258-b001-v1`
Issue: `#7160`

## Goal

Freeze the semantic facts that imported runtime metadata must preserve across
module boundaries before lane-B lands real imported metadata semantic
preservation rules.

## Published Surface

- Semantic surface:
  `frontend.pipeline.semantic_surface.objc_cross_module_runtime_metadata_semantic_preservation_contract`
- Source frontend closure:
  `objc3c-runtime-aware-import-module-frontend-closure/m258-a002-v1`
- Source artifact:
  `module.runtime-import-surface.json`

## Semantic Models

- Conformance shape:
  `superclass-protocol-and-category-attachment-shape`
- Dispatch traits:
  `selector-classness-accessor-ivar-binding-and-body-availability`
- Effect traits:
  `property-attribute-and-ownership-effect-profiles`

## Required Freeze Rules

1. The compiler publishes one deterministic semantic-surface packet derived from
   the live `M258-A002` frontend closure plus runtime metadata source records.
2. The surface must account for:
   - superclass edges
   - protocol conformance / inheritance edges
   - category attachment counts
   - property accessor traits
   - property ivar-binding traits
   - method selector / classness / body-availability traits
   - property-attribute and ownership-effect profile counts
   - executable binding trait counts
3. The surface must remain fail closed:
   - imported conformance shape is not landed
   - imported dispatch traits are not landed
   - imported effect traits are not landed
   - imported runtime metadata semantics are not landed
   - `ready_for_imported_metadata_semantic_rules` remains `false`
   - `ready_for_cross_module_dispatch_equivalence` remains `false`
4. `ir/objc3_ir_emitter.cpp` remains explicit that imported runtime metadata
   semantics are not lowered into IR yet.
5. `libobjc3c_frontend/api.h` remains explicit that imported runtime metadata
   semantics remain filesystem-artifact only and do not yet expose a live
   imported-module semantic ABI.

## Non-Goals

- `M258-B001` does not implement imported metadata semantic equivalence.
- `M258-B001` does not land imported conformance checking.
- `M258-B001` does not land imported dispatch equivalence.
- `M258-B001` does not lower imported runtime metadata semantics into IR.
- `M258-B001` does not widen the public frontend ABI with imported-module
  semantic handles.

## Validation

- `python scripts/check_m258_b001_cross_module_semantic_preservation_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m258_b001_cross_module_semantic_preservation_contract_and_architecture_freeze.py -q`
- `python scripts/run_m258_b001_lane_b_readiness.py`

## Evidence

- `tmp/reports/m258/M258-B001/cross_module_runtime_metadata_semantic_preservation_contract_summary.json`
