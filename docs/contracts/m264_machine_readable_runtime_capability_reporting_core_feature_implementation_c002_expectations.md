# M264-C002 Expectations

Issue: `#7239`
Packet: `M264-C002`
Milestone: `M264`
Lane: `C`

The compiler must extend `module.objc3-conformance-report.json` with truthful nested capability payloads:

- `runtime_capability_report`
- `public_conformance_report`

Required truth:

- contract id `objc3c-runtime-capability-reporting/m264-c002-v1`
- semantic surface `frontend.pipeline.semantic_surface.objc_runtime_capability_report`
- claimed profile `core`
- not-claimed profiles `strict`, `strict-concurrency`, `strict-system`
- mode `strictness=permissive`
- mode `concurrency=off`
- optional features `throws`, `async-await`, `actors`, `blocks`, and `arc` remain `not-claimed`
- public report schema `objc3-conformance-report/v1`
- replay timestamp `1970-01-01T00:00:00Z`

Dynamic proof must validate both:

- native hello path with IR/object emission
- frontend-runner manifest-only path
