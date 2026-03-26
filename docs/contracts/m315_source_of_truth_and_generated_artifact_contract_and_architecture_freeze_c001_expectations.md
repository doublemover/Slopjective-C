# M315-C001 Expectations

Contract ID: `objc3c-cleanup-source-of-truth-generated-artifact-contract/m315-c001-v1`

`M315-C001` freezes the stable source-of-truth and generated-artifact contract for the
remaining milestone-coded residue still tolerated after `M315-B005`.

The contract must:
- distinguish archival planning references from durable product and generated
  source-of-truth surfaces;
- define stable replacement field names for emitted or persisted references that still
  use `_issue`, `next_issue`, or legacy `m248_*` identifiers;
- freeze the rule that planning-only follow-on references must be removed from durable
  generated truth unless the surface genuinely requires a durable forward reference;
- inventory the remaining legacy follow-on refs, dependency refs, issue-key fields,
  and transitional source-model literals so `M315-C002` through `M315-C004` can rewrite
  them mechanically instead of inventing names mid-flight.

The frozen contract must specifically cover these residual classes from
`M315-B005`:
- `legacy_m248_surface_identifier`
- `dependency_issue_array`
- `next_issue_schema_field`
- `issue_key_schema_field`
- `transitional_source_model`

Stable replacements must not contain milestone, lane, or issue-era tokens. Durable
surface identifiers must remain consistent with the `M315-B001` grammar.

This issue must hand off authenticity-envelope details to `M315-C002`, live replay
regeneration to `M315-C003`, and synthetic fixture relocation to `M315-C004`.
