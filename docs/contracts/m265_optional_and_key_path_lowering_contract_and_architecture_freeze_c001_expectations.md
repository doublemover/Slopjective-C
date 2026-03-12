# M265-C001 Expectations

Contract ID: `objc3c-part3-optional-keypath-lowering/m265-c001-v1`

Scope: Freeze the first lowering-owned Part 3 packet so optional bindings,
optional sends, and nil-coalescing have one deterministic native lowering
boundary while validated typed key-path literals now lower into retained
descriptor handles and broader key-path runtime behavior remains later work.

Required anchors:
- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `docs/objc3c-native.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`
- `spec/PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md`
- `package.json`

Required packet:
- `frontend.pipeline.semantic_surface.objc_part3_optional_keypath_lowering_contract`

The packet must prove:
- optional binding, optional send, and nil-coalescing lowering sites are
  counted deterministically
- native lowering publishes one single-evaluation nil-short-circuit model for
  optional bindings, optional sends, and `??`
- validated typed key-path literals lower into retained native descriptor
  artifacts and stable nonzero handles
- the native optional-send happy path no longer evaluates arguments on the nil
  receiver arm
- generic-metadata replay evidence remains visible alongside the emitted
  key-path artifacts

Validation:
- `python scripts/check_m265_c001_optional_and_key_path_lowering_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m265_c001_optional_and_key_path_lowering_contract_and_architecture_freeze.py -q`
- `python scripts/run_m265_c001_lane_c_readiness.py`

Evidence:
- `tmp/reports/m265/M265-C001/optional_and_key_path_lowering_contract_summary.json`
