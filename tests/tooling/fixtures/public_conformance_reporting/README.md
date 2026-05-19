# Public Conformance Reporting Contracts

This directory holds the checked-in `M304` source-surface contracts for public
conformance reporting.

It does not hold a second executable corpus. Public scorecards and summaries
must resolve back to the existing conformance corpus, external-validation, and
release-evidence surfaces.

The workflow surface is split into source, schema, stability-policy, and action
payload owners under `scripts/objc3c_workflow/actions/release_governance_public_conformance_*`.
Fixtures name real checked-in paths only; command strings belong to action
payload fragments, not source-path inventories.
