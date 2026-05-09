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

The checked-in workflow surface lives in
`tests/tooling/fixtures/external_validation/workflow_surface.json`; it owns the
public action inventory, integrated child ordering, and child report contracts.

`owner_contracts.json` is the machine-readable trust boundary for the public
actions. Accepted intake entries are the only capability-truth source. Candidate,
quarantined, rejected, local-only, and report-only evidence cannot publish a
capability claim, and there is no fallback trust route outside the intake,
quarantine, artifact, and workflow owners.
