# External Validation Source Surface

This directory is the checked-in source-of-truth root for `M303` intake and
independent replay contracts.

It does not hold a second executable corpus. External credibility work must
normalize incoming evidence onto the existing checked-in fixture and conformance
surfaces:

- `tests/tooling/fixtures/objc3c/`
- `tests/conformance/`
- `scripts/check_objc3c_*_replay_proof.ps1`

Machine-owned replay outputs belong under `tmp/`, not here.
