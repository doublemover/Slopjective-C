# M264-B001 Expectations

Contract ID: `objc3c-compatibility-strictness-claim-semantics/m264-b001-v1`

Scope: Freeze the sema-owned legality packet that classifies the current Objective-C 3 frontend claim surface into valid selections, downgraded source-only claims, and rejected unsupported strictness/macro surfaces.

Required anchors:
- `native/objc3c/src/sema/objc3_sema_contract.h`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `docs/objc3c-native.md`
- `spec/CONFORMANCE_PROFILE_CHECKLIST.md`
- `spec/DECISIONS_LOG.md`
- `package.json`

Required packet:
- `frontend.pipeline.semantic_surface.objc_compatibility_strictness_claim_semantics`

The packet must prove:
- compatibility mode and migration assist remain live semantic selections
- source-only recognized claims remain downgraded and never promote to runnable
- strictness / strict-concurrency selection remain rejected
- feature-macro publication remains suppressed
- the semantic packet is fail-closed and deterministic

Validation:
- `python scripts/check_m264_b001_compatibility_strictness_and_claim_semantics_contract_and_architecture_freeze.py`
- `python -m pytest tests/tooling/test_check_m264_b001_compatibility_strictness_and_claim_semantics_contract_and_architecture_freeze.py -q`
- `python scripts/run_m264_b001_lane_b_readiness.py`

Evidence:
- `tmp/reports/m264/M264-B001/compatibility_strictness_and_claim_semantics_summary.json`
