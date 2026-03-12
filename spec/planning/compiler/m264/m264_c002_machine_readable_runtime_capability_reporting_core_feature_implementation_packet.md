Packet: `M264-C002`
Issue: `#7239`
Milestone: `M264`
Wave: `W57`
Lane: `C`

Summary:
Implement machine-readable runtime capability and public conformance report emission that reflects the actual native execution surface.

Dependencies:
- `M264-C001`
- `M264-B003`

Artifacts:
- semantic surface `frontend.pipeline.semantic_surface.objc_runtime_capability_report`
- nested sidecar payload `runtime_capability_report`
- nested sidecar payload `public_conformance_report`
- IR anchor `runtime_capability_reporting = ...`

Acceptance:
- capability/public payloads are emitted as real compiler artifacts rather than placeholders
- payloads stay a truthful projection of the lowered conformance sidecar
- profile and optional-feature status remain fail-closed for unimplemented runtime-backed features
- deterministic checker, pytest, and lane-C readiness evidence land under `tmp/reports/m264/M264-C002/`
