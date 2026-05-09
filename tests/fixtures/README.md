# Fixture Boundary Index

`tests/fixtures` separates hand-authored behavior truth from generated replay
artifacts.

- `canonical/`: manifest entries for hand-authored `tests/native` fixtures.
  These are behavior contracts and may include positive, rejection, and
  strict-error cases.
- `generated/`: replay or generator output used as provenance evidence. These
  artifacts do not define positive native behavior.

Hard-cutover compatibility residues must not be accepted as positives in either
tree. Removed old-mode flags, compatibility shim toggles, fallback runtime
dispatch routes, and legacy literal spellings belong in canonical rejection or
strict-error metadata.
