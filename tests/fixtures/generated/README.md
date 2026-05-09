# Generated Fixture Provenance

`manifest.json` tracks generated artifacts used for deterministic replay and
contract provenance. Generated entries are intentionally not native behavior
expectations and must stay disjoint from `tests/native`.

Generated artifacts can prove that a generator or replay surface is stable, but
they cannot make a retired adapter, alternate acceptance path, retired-source
lane, unsupported-feature claim, or runtime-dispatch path acceptable.
Those cases must be promoted into `tests/native` as canonical rejection or
strict-error fixtures before they can satisfy hard-cutover behavior coverage.
Generated metadata also cannot own parser, semantic, lowering ABI, runtime, or
e2e behavior claims. It can only reference canonical fixture families as
provenance, while the phase owner indexes decide whether the referenced behavior
is supported, rejected, strict-error, or absent.
`tests/conformance/hard_cutover_fixture_boundary_contracts.json` records this
generated-provenance-only contract beside the canonical manifest boundary.
