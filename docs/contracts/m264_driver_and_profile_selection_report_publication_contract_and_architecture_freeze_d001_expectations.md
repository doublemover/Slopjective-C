# M264-D001 Expectations

Issue: `#7240`
Packet: `M264-D001`
Milestone: `M264`
Lane: `D`

The driver publication contract must now be explicit and machine-readable.

Canonical identifiers:

- contract id: `objc3c-driver-conformance-report-publication/m264-d001-v1`
- schema id: `objc3c-driver-conformance-publication-v1`

Required behavior:

- native CLI accepts `--objc3-conformance-profile <core|strict|strict-concurrency|strict-system>`
- current live profile is `core`
- selecting `strict`, `strict-concurrency`, or `strict-system` fails closed with a deterministic diagnostic
- native CLI writes `module.objc3-conformance-publication.json`
- frontend C API runner also writes `module.objc3-conformance-publication.json`
- publication artifact records the lowered conformance/runtime capability contract ids and the public schema id `objc3-conformance-report/v1`
