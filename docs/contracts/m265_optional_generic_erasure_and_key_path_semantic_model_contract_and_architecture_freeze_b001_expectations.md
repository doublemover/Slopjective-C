# M265-B001 Expectations

Contract ID: `objc3c-part3-type-semantic-model/m265-b001-v1`

Scope: Freeze the fail-closed semantic model for Part 3 optional bindings, optional sends, pragmatic generic-erasure metadata, nullability semantic counts, and typed key-path legality before later lowering/runtime work.

Required anchors:
- `native/objc3c/src/sema/objc3_sema_contract.h`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `docs/objc3c-native.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`
- `spec/PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md`
- `package.json`

Required packet:
- `frontend.pipeline.semantic_surface.objc_part3_type_semantic_model`

The packet must prove:
- optional binding sites and clause counts are published deterministically
- guard bindings are distinguished from plain `if let` bindings
- optional sends fail closed for non-ObjC-reference receivers
- typed key paths fail closed when the root does not resolve to `self` or an in-scope identifier
- pragmatic generic-erasure and nullability semantic site counts remain visible through the semantic surface
- the semantic packet is deterministic and source-only truthful even before runnable lowering is complete

Validation:
- `python scripts/check_m265_b001_optional_generic_erasure_and_key_path_semantic_model_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m265_b001_optional_generic_erasure_and_key_path_semantic_model_contract_and_architecture_freeze.py -q`
- `python scripts/run_m265_b001_lane_b_readiness.py`

Evidence:
- `tmp/reports/m265/M265-B001/optional_generic_erasure_keypath_semantic_model_summary.json`
