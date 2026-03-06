# M229 Class/Protocol/Category Metadata Generation Contract and Architecture Freeze Expectations (A001)

Contract ID: `objc3c-class-protocol-category-metadata-generation/m229-a001-v1`
Status: Accepted
Owner: Objective-C 3 native lane-A
Issue: `#5301`

## Objective

Freeze lane-A class/protocol/category metadata generation contract prerequisites for M229 so parser declaration forms, semantic linking summaries, typed handoff continuity, and IR metadata surfaces remain deterministic and fail-closed before lane-A expansion workpacks begin.

Issue `#5301` defines canonical lane-A contract-freeze scope.
This freeze treats code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/sema/objc3_sema_pass_manager.cpp`
- `native/objc3c/src/sema/objc3_sema_pass_manager_contract.h`
- `native/objc3c/src/pipeline/objc3_typed_sema_to_lowering_contract_surface.h`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Deterministic Invariants

1. Parser declaration surfaces keep explicit interface/implementation/protocol entrypoints and category semantic link generation.
2. Sema class/protocol/category linking summaries and protocol/category composition summaries remain deterministic and parity-aligned.
3. Typed sema-to-lowering surface preserves `class_protocol_category_linking_handoff_deterministic` fail-closed behavior.
4. Frontend artifact and IR emission metadata continue exporting class/protocol/category linking determinism for downstream ABI checks.
5. Architecture/spec anchors and readiness commands stay synchronized with lane-A contract-freeze intent.

## Required Commands

- `check:objc3c:m229-a001-class-protocol-category-metadata-generation-contract`
- `check:objc3c:m229-a001-lane-a-readiness`
- `python scripts/check_m229_a001_class_protocol_category_metadata_generation_contract.py`
- `python -m pytest tests/tooling/test_check_m229_a001_class_protocol_category_metadata_generation_contract.py -q`
- `npm run check:objc3c:m229-a001-lane-a-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-ast-extraction`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:execution-smoke`

## Evidence Output

- `tmp/reports/m229/M229-A001/class_protocol_category_metadata_generation_contract_summary.json`
