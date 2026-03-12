Packet: `M264-D001`
Issue: `#7240`
Milestone: `M264`
Wave: `W57`
Lane: `D`

Summary:
Freeze the driver/publication contract for conformance profile selection and report publication.

Dependencies:
- `M264-C002`

Artifacts:
- CLI flag `--objc3-conformance-profile <core|strict|strict-concurrency|strict-system>`
- emitted artifact `module.objc3-conformance-publication.json`
- fail-closed rejection for non-core profile selection in the native CLI path
- frontend C API runner publication artifact for the default `core` profile surface

Acceptance:
- native CLI and frontend C API runner both emit the publication artifact
- artifact records selected profile, supported/rejected profile ids, publication surface kind, and the linked lowered/runtime/public contract ids
- unsupported profile selection fails closed with a deterministic diagnostic
- deterministic checker, pytest, and lane-D readiness evidence land under `tmp/reports/m264/M264-D001/`
