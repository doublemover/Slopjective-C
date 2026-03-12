# M264-C001 Versioned Conformance Report Lowering Contract And Architecture Freeze Packet

Packet: `M264-C001`  
Milestone: `M264`  
Lane: `C`  
Issue: `#7238`  
Contract ID: `objc3c-versioned-conformance-report-lowering/m264-c001-v1`

Summary:

Freeze the current lowering boundary that publishes one machine-readable
versioned conformance-report sidecar from the truthful frontend claim and
semantic packets without overstating unsupported Objective-C 3 surfaces.

Dependencies:

- `M264-A001`
- `M264-A002`
- `M264-B001`
- `M264-B002`
- `M264-B003`

Code anchors:

- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/io/objc3_manifest_artifacts.cpp`
- `native/objc3c/src/driver/objc3_objc3_path.cpp`
- `native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`

Spec anchors:

- `docs/objc3c-native.md`
- `spec/CONFORMANCE_PROFILE_CHECKLIST.md`
- `spec/DECISIONS_LOG.md`

Required outputs:

- `frontend.pipeline.semantic_surface.objc_versioned_conformance_report_lowering_contract`
- `module.objc3-conformance-report.json`
- `tmp/reports/m264/M264-C001/versioned_conformance_report_lowering_summary.json`

Truth constraints:

- the lowered report must be sourced from the already-truthful frontend
  inventory/truth/semantic packets rather than inventing a second authority
- the current separate-compilation boundary remains explicit via
  `canonical_interface_mode = no-standalone-interface-payload-yet`
- source-only recognized claims remain downgraded
- strictness, strict concurrency, and feature-macro publication remain
  unsupported
- native executable and manifest-only runner probes must both publish the same
  contract ids and truthful claim surface
