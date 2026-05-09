# Fixture Boundary Index

`tests/fixtures` separates hand-authored behavior truth from generated replay
artifacts.

- `canonical/`: manifest entries for hand-authored `tests/native` fixtures.
  These are behavior contracts and may include positive, rejection, and
  strict-error cases.
- `generated/`: replay or generator output used as provenance evidence. These
  artifacts do not define positive native behavior.

Hard-cutover retired-surface residues must not be accepted as positives in
either tree. Removed mode flags, retired adapter toggles, alternate runtime
acceptance routes, and legacy literal spellings belong in canonical rejection or
strict-error metadata.

The issue-closeout indexes for `#8132`-`#8150` intentionally reference the
canonical fixture tree rather than generated artifacts when documenting
behavior support. Retired-source lanes and retired adapter surfaces are either
listed in `tests/conformance/hard_cutover_retired_surface_absence.json` as
rejection or strict-error evidence, cross-checked in
`tests/conformance/hard_cutover_retired_surface_fixture_contracts.json` against
their fixture sidecars and diagnostic owners, tied together by
`tests/conformance/hard_cutover_fixture_boundary_contracts.json`, or marked
absent from public support.
