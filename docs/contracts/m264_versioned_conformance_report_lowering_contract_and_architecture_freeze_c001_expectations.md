# M264-C001 Versioned Conformance Report Lowering Contract And Architecture Freeze Expectations

Contract ID: `objc3c-versioned-conformance-report-lowering/m264-c001-v1`

Scope: `M264` lane-C freezes the current lowering boundary that turns the live frontend truth surface into one machine-readable versioned conformance-report sidecar without overstating unsupported Objective-C 3 surfaces.

Requirements:

1. The lowering boundary publishes one canonical sidecar:
   - `module.objc3-conformance-report.json`
2. The lowered report must declare:
   - `schema_id = objc3c-versioned-conformance-report-v1`
   - `contract_id = objc3c-versioned-conformance-report-lowering/m264-c001-v1`
   - `semantic_contract_id = objc3c-compatibility-strictness-claim-semantics/m264-b001-v1`
   - `frontend_surface_path = frontend.pipeline.semantic_surface.objc_versioned_conformance_report_lowering_contract`
3. The lowered report must embed and preserve the already-truthful frontend packets for:
   - runnable feature claim inventory
   - feature-claim and strictness truth surface
   - compatibility/strictness claim semantics
4. The lowering boundary must keep the current separate-compilation truth explicit:
   - `canonical_interface_mode = no-standalone-interface-payload-yet`
5. The lowered report must remain fail-closed and truthful:
   - unsupported strictness/concurrency/macro surfaces stay suppressed or rejected
   - source-only recognized features stay downgraded rather than promoted to runnable
6. The manifest semantic surface publishes one canonical lowering packet at:
   - `frontend.pipeline.semantic_surface.objc_versioned_conformance_report_lowering_contract`
7. The native IR output must publish one textual boundary summary line containing:
   - `versioned_conformance_report_lowering =`
8. Dynamic validation proves both:
   - native executable lowering on `artifacts/bin/objc3c-native.exe`
   - frontend-runner manifest-only lowering on `artifacts/bin/objc3c-frontend-c-api-runner.exe --no-emit-ir --no-emit-object`
9. Explicit anchors remain present in:
   - `native/objc3c/src/lower/objc3_lowering_contract.h`
   - `native/objc3c/src/lower/objc3_lowering_contract.cpp`
   - `native/objc3c/src/pipeline/objc3_frontend_types.h`
   - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
   - `native/objc3c/src/io/objc3_manifest_artifacts.cpp`
   - `native/objc3c/src/driver/objc3_objc3_path.cpp`
   - `native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
   - `docs/objc3c-native.md`
   - `spec/CONFORMANCE_PROFILE_CHECKLIST.md`
   - `spec/DECISIONS_LOG.md`
   - `package.json`
10. Validation evidence lands at:
   - `tmp/reports/m264/M264-C001/versioned_conformance_report_lowering_summary.json`
11. Validation commands:
   - `python scripts/check_m264_c001_versioned_conformance_report_lowering_contract_and_architecture_freeze.py`
   - `python -m pytest tests/tooling/test_check_m264_c001_versioned_conformance_report_lowering_contract_and_architecture_freeze.py -q`
   - `python scripts/run_m264_c001_lane_c_readiness.py`
