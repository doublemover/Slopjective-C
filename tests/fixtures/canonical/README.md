# Canonical Native Behavior Manifest

`manifest.json` is the hand-authored behavior index for `tests/native`.
Entries are grouped by behavior owner phase in this order:

1. parser
2. sema
3. lowering
4. ir
5. runtime
6. e2e

The manifest mirrors each fixture sidecar instead of replacing it. A canonical
positive must have no diagnostic code or retired-surface tag. A canonical
rejection or strict-error fixture must declare the stable diagnostic code and,
when it covers a retired surface, the retired tag used by the behavior harness.
`tests/conformance/hard_cutover_retired_surface_fixture_contracts.json` is the
fixture-contract index for those retired surfaces: it names the owning behavior
outcome, diagnostic owner, sidecar file, and non-positive disposition for each
old-mode, shim, fallback, compatibility, migration-lane, unsupported-feature,
or runtime-dispatch case.
