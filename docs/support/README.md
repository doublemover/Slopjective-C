# Support Truth Contract

This directory owns the public Objective-C 3.0 support truth. Other docs, spec
chapters, site pages, runbooks, and generated reports may summarize support,
but they must not widen it beyond these files.

## Canonical Files

| File | Purpose |
| ---- | ------- |
| `capability_matrix.json` | Machine-readable capability states, support claims, evidence pointers, command-surface policy, and hard-cutover rules. |
| `capability_matrix.md` | Human-readable projection of the capability matrix. |
| `evidence_map.json` | Machine-readable flattened capability-to-evidence rows. |
| `evidence_map.md` | Human-readable evidence table. |
| `capability_matrix.schema.json` | Local mirror for consumers that historically read the schema from this directory. |
| `capability_schema_examples.md` | Examples and anti-examples for matrix and evidence rows. |

The canonical schema registry entries live under `schemas/`:

- `schemas/objc3c-capability-matrix-v1.schema.json`
- `schemas/objc3c-capability-evidence-map-v1.schema.json`
- `schemas/README.md`

## Change Rules

- Add or change a support claim in `capability_matrix.json` first.
- Add matching evidence rows in `evidence_map.json`.
- Use `owner_modules` for internal implementation boundaries that support a
  claim without becoming public command surface.
- Update the markdown projections in this directory when the machine-readable
  truth changes.
- Keep site/spec/runbook summaries subordinate to this directory.
- Use only `implemented`, `rejected`, `reserved`, and `internal` as capability
  states.
- Public replay commands must use `npm run objc3c -- <action>`.
- Retired alternate surfaces are diagnostics, history, or anti-examples; they
  are not support modes.
