# Native Behavior Fixtures

`tests/native` is the hand-authored behavior source of truth for the
hard-cutover compiler and runtime surface. Fixtures are organized by owner
phase first, then by behavior family:

- `parser`: canonical syntax positives, parser rejections, and diagnostics
  snapshots.
- `sema`: type, ownership, Objective-C surface, control-flow, error,
  concurrency, and negative semantic behavior.
- `lowering`: expression, statement, Objective-C runtime, ownership, and
  lowering-error behavior.
- `ir`: module, function, metadata, and runtime-call IR contracts.
- `runtime`: dispatch, object model, storage, ARC, blocks, errors, and
  concurrency behavior.
- `e2e`: smoke, feature-matrix, and negative-execution behavior.

Positive fixtures prove canonical Objective-C 3 behavior only. Retired mode
flags, retired adapters, retired-source lanes, alternate dispatch acceptance
routes, unsupported-feature claims, and legacy literal aliases must live as
rejection, strict-error, or absent-support metadata with stable diagnostic ownership. The
retired-surface contract index under
`tests/conformance/hard_cutover_retired_surface_fixture_contracts.json` records
the fixture family, outcome owner, diagnostic owner, sidecar, and positive
absence for each retired surface.

Runtime dispatch fixtures are typed boundary evidence. `objc3_runtime_dispatch_i32`
and related message-send projections either materialize through the canonical
runtime owner path or remain strict link/run errors; they are never retired route,
gate, or compatibility acceptance lanes.
