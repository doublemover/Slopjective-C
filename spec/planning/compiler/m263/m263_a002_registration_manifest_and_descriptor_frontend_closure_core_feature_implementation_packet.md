# M263-A002 Registration Manifest and Descriptor Frontend Closure Core Feature Implementation Packet

Packet: `M263-A002`
Milestone: `M263`
Lane: `A`
Wave: `W56`
Implementation date: `2026-03-09`
Dependencies: `M263-A001`
Next issue: `M263-B001`

## Purpose

Close the remaining frontend/source-model gap above `M254` by emitting one deterministic registration-descriptor artifact derived from the frozen `M263-A001` source surface and the already-emitted runtime registration manifest.

## Scope Anchors

- Contract:
  `docs/contracts/m263_registration_manifest_and_descriptor_frontend_closure_core_feature_implementation_a002_expectations.md`
- Checker:
  `scripts/check_m263_a002_registration_manifest_and_descriptor_frontend_closure_core_feature_implementation.py`
- Tooling tests:
  `tests/tooling/test_check_m263_a002_registration_manifest_and_descriptor_frontend_closure_core_feature_implementation.py`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m263-a002-registration-manifest-and-descriptor-frontend-closure`
  - `test:tooling:m263-a002-registration-manifest-and-descriptor-frontend-closure`
  - `check:objc3c:m263-a002-lane-a-readiness`
- Code anchors:
  - `native/objc3c/src/token/objc3_token_contract.h`
  - `native/objc3c/src/lex/objc3_lexer.cpp`
  - `native/objc3c/src/parse/objc3_parser.cpp`
  - `native/objc3c/src/pipeline/objc3_frontend_types.h`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
  - `native/objc3c/src/io/objc3_manifest_artifacts.h`
  - `native/objc3c/src/io/objc3_manifest_artifacts.cpp`
  - `native/objc3c/src/io/objc3_process.h`
  - `native/objc3c/src/io/objc3_process.cpp`
  - `native/objc3c/src/driver/objc3_objc3_path.cpp`
  - `native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `docs/objc3c-native.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`

## Fail-Closed Boundary

- Contract id:
  `objc3c-runtime-registration-descriptor-frontend-closure/m263-a002-v1`
- Upstream registration-manifest contract id:
  `objc3c-translation-unit-registration-manifest/m254-a002-v1`
- Upstream source-surface contract id:
  `objc3c-bootstrap-registration-descriptor-image-root-source-surface/m263-a001-v1`
- Semantic surface path:
  `frontend.pipeline.semantic_surface.objc_runtime_registration_descriptor_frontend_closure`
- Payload model:
  `runtime-registration-descriptor-json-v1`
- Artifact relative path:
  `module.runtime-registration-descriptor.json`
- Authority model:
  `registration-descriptor-artifact-derived-from-source-surface-and-registration-manifest`
- Payload ownership model:
  `compiler-emits-registration-descriptor-artifact-runtime-consumes-bootstrap-identity`
- Required happy-path proof:
  - explicit pragma-driven descriptor artifact emission
  - module-derived default descriptor artifact emission
- Required carry-through:
  - resolved registration descriptor identifier
  - resolved image-root identifier
  - identity-source classification
  - bootstrap-visible ownership model
  - registration entrypoint symbol
  - runtime-support archive path
  - constructor-root / init-stub / registration-table / image-local-init symbols

## Non-Goals

- no bootstrap-table lowering yet
- no multi-image replay behavior yet
- no runtime bootstrap execution yet
- no legality/failure freeze yet

## Gate Commands

- `python scripts/check_m263_a002_registration_manifest_and_descriptor_frontend_closure_core_feature_implementation.py`
- `python -m pytest tests/tooling/test_check_m263_a002_registration_manifest_and_descriptor_frontend_closure_core_feature_implementation.py -q`
- `python scripts/run_m263_a002_lane_a_readiness.py`

## Evidence Output

- `tmp/reports/m263/M263-A002/registration_manifest_and_descriptor_frontend_closure_summary.json`
