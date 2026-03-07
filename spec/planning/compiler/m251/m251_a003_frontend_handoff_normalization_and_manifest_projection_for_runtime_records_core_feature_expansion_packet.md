# M251-A003 Frontend Handoff Normalization and Manifest Projection for Runtime Records Packet

Packet: `M251-A003`
Milestone: `M251`
Lane: `A`
Implementation date: `2026-03-07`
Dependencies: `M251-A002`

## Purpose

Publish deterministic manifest and handoff payloads for runtime records so
later lowering/runtime work can consume declaration metadata without reparsing
frontend declaration trees.

## Scope Anchors

- Contract:
  `docs/contracts/m251_frontend_handoff_normalization_and_manifest_projection_for_runtime_records_core_feature_expansion_a003_expectations.md`
- Checker:
  `scripts/check_m251_a003_frontend_handoff_normalization_and_manifest_projection_for_runtime_records_contract.py`
- Tooling tests:
  `tests/tooling/test_check_m251_a003_frontend_handoff_normalization_and_manifest_projection_for_runtime_records_contract.py`
- Reference fixtures (reused from A002):
  - `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`
  - `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3`
- Build/readiness scripts (`package.json`):
  - `check:objc3c:m251-a003-frontend-handoff-normalization-and-manifest-projection-for-runtime-records-contract`
  - `test:tooling:m251-a003-frontend-handoff-normalization-and-manifest-projection-for-runtime-records-contract`
  - `check:objc3c:m251-a003-lane-a-readiness`
- Code anchors:
  - `native/objc3c/src/pipeline/objc3_frontend_types.h`
  - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
  - `native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp`
  - `native/objc3c/src/driver/objc3_objc3_path.cpp`
- Architecture/spec anchors:
  - `native/objc3c/src/ARCHITECTURE.md`
  - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
  - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
  - `docs/objc3c-native.md`

## Canonical Behavior

- Manifest projection is built before downstream emit/readiness failures are
  surfaced.
- Manifest-only frontend runs (`--no-emit-ir --no-emit-object`) succeed and
  emit the runtime-record handoff manifest.
- Full CLI/native runs remain fail-closed for later lowering/runtime gaps but
  preserve the manifest handoff artifact before returning a failing exit code.
- Emit-stage stage summaries report `attempted=false` and `skipped=true` for
  manifest-only frontend runs.

## Dynamic Probe Cases

The checker runs three deterministic probes:

1. `c_api_manifest_only_class`
   - `objc3c-frontend-c-api-runner.exe`
   - class/protocol/property/ivar fixture
   - expects success, manifest path populated, emit stage skipped.
2. `c_api_manifest_only_category`
   - `objc3c-frontend-c-api-runner.exe`
   - category/protocol/property fixture
   - expects success, manifest path populated, emit stage skipped.
3. `cli_fail_closed_manifest_preserved`
   - `objc3c-native.exe`
   - class/protocol/property/ivar fixture
   - expects non-zero exit while `module.manifest.json` still exists and keeps
     the runtime-record handoff projection.

## Gate Commands

- `python scripts/check_m251_a003_frontend_handoff_normalization_and_manifest_projection_for_runtime_records_contract.py`
- `python -m pytest tests/tooling/test_check_m251_a003_frontend_handoff_normalization_and_manifest_projection_for_runtime_records_contract.py -q`
- `npm run check:objc3c:m251-a003-lane-a-readiness`

## Evidence Output

- `tmp/reports/m251/M251-A003/runtime_record_manifest_handoff_contract_summary.json`
