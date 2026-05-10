# Canonical Native Behavior Manifest

`manifest.json` is the hand-authored behavior index for `tests/native`.
Entries are grouped by behavior owner phase in this order:

1. parser
2. sema
3. lowering
4. ir
5. runtime
6. e2e

The same phase grouping is extracted into `phase_splits/*.json` for
phase-local ownership checks. Those files do not replace `manifest.json`; they
make the parser, sema, lowering, IR, runtime, and e2e portions of the canonical
manifest citable without treating the full manifest as one mixed fixture bucket.

The behavior outcome grouping is extracted into `behavior_splits/*.json` for
positive, diagnostic-negative, canonical-rejection, and strict-error ownership
checks. Those indexes are outcome-first: a fixture with compatibility, gate,
retired-route, unsupported-feature, old-mode, or runtime-dispatch residue is owned by
rejection or strict-error metadata even when its path sits beside ordinary
negative diagnostics.

The manifest mirrors each fixture sidecar instead of replacing it. A canonical
positive must have no diagnostic code or retired-surface tag. A canonical
rejection or strict-error fixture must declare the stable diagnostic code and,
when it covers a retired surface, the retired tag used by the behavior harness.
The phase owner decides the fixture boundary before path convenience does:
parser owns syntax rejections, semantic owns typed diagnostics, lowering ABI and
IR own strict lowering/link failures, runtime owns dispatch/status failures, and
e2e owns execution-boundary confirmation. No canonical entry should describe a
retired surface as retired route, gate, compatibility, or migration support.
Mixed fixture directories in `tests/tooling/fixtures/native` are split by the
same owner phases before they can be cited as canonical coverage. Legacy-looking
positive residues are canonical-rejection candidates first; they become positive
behavior only when this manifest gives them a parser, sema, lowering, runtime,
or e2e owner with an empty diagnostic code and no retired-surface tag.
`tests/conformance/hard_cutover_retired_surface_fixture_contracts.json` is the
fixture-contract index for those retired surfaces: it names the owning behavior
outcome, diagnostic owner, sidecar file, and non-positive disposition for each
retired mode, retired adapter, alternate acceptance path, retired-source lane,
unsupported-feature claim, or runtime-dispatch case. `tests/conformance/hard_cutover_fixture_boundary_contracts.json`
is the cross-boundary index that keeps this canonical manifest distinct from
generated provenance, conformance reference anchors, and lexical residue audit
entries.
