# M256 Category Attachment and Protocol Conformance Runtime Checks Core Feature Implementation Expectations (D003)

Contract ID: `objc3c-runtime-category-attachment-protocol-conformance/m256-d003-v1`

## Scope
- Runtime must retain preferred category attachments on realized class nodes after image registration.
- Runtime protocol conformance queries must walk direct class protocol refs, attached category protocol refs, and inherited protocol closures.
- Attached category method dispatch and protocol-aware fallback behavior must be proven against the live runtime path rather than manifests alone.
- Validation evidence must land at `tmp/reports/m256/M256-D003/category_attachment_protocol_conformance_runtime_checks_summary.json`.

## Required models
- `realized-class-nodes-own-preferred-category-attachments-after-registration`
- `runtime-protocol-conformance-queries-walk-class-category-and-inherited-protocol-closures`
- `invalid-attachment-owner-identities-or-broken-protocol-refs-disable-runtime-attachment-queries`

## Required fixture and probe
- Fixture: `tests/tooling/fixtures/native/m256_d003_category_attachment_protocol_runtime_library.objc3`
- Probe: `tests/tooling/runtime/m256_d003_category_attachment_protocol_runtime_probe.cpp`

## Dynamic proof requirements
- `objc3_runtime_dispatch_i32(1042, "tracedValue", ...)` must resolve through the attached category and return `13`.
- `objc3_runtime_dispatch_i32(1043, "classValue", ...)` must resolve the class method and return `11`.
- `objc3_runtime_dispatch_i32(1042, "ignoredValue", ...)` must fail closed to compatibility fallback and equal the probe-computed fallback value.
- The realized graph must publish one attached category and two protocol-conformance edges.
- Realized class `Widget` must publish one direct protocol and one attached-category protocol.
- Runtime query `Widget -> Worker` must conform without an attachment owner match.
- Runtime query `Widget -> Tracer` must conform through attachment owner `category:Widget(Tracing)`.
- Runtime query `Base -> Worker` must fail closed without attachment or protocol matches.
