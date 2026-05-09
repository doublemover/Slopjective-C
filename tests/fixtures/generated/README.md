# Generated Fixture Provenance

`manifest.json` tracks generated artifacts used for deterministic replay and
contract provenance. Generated entries are intentionally not native behavior
expectations and must stay disjoint from `tests/native`.

Generated artifacts can prove that a generator or replay surface is stable, but
they cannot make an old-mode, shim, fallback, or compatibility path acceptable.
Those cases must be promoted into `tests/native` as canonical rejection or
strict-error fixtures before they can satisfy hard-cutover behavior coverage.
