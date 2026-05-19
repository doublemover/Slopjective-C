# Capability Claim Responsibility

This page defines which checked-in surfaces may own Objective-C 3.0 capability
claims. It is the reader-facing split for responsibility rules that would
otherwise be repeated across issue closeout notes, schema docs, site pages, and
spec prose.

## Owner Surfaces

| Responsibility                             | Owner surface                                                                                                            | What it may do                                                                                                                                 |
| ------------------------------------------ | ------------------------------------------------------------------------------------------------------------------------ | ---------------------------------------------------------------------------------------------------------------------------------------------- |
| Capability state and public behavior claim | `docs/support/capability_matrix.json` with `schemas/objc3c-capability-matrix-v1.schema.json`                             | Define whether a row is `implemented`, `rejected`, `reserved`, or `internal`; define the exact `objc3c.behavior.*` claim for implemented rows. |
| Support-directory matrix schema entrypoint | `docs/support/capability_matrix.schema.json`                                                                             | Delegate docs/editor schema discovery to the canonical matrix schema without defining a second schema ID or copying schema shape.               |
| Capability evidence projection             | `docs/support/evidence_map.json` with `schemas/objc3c-capability-evidence-map-v1.schema.json`                            | Flatten matrix evidence into stable rows keyed by capability, claim, evidence kind, path, and command.                                         |
| Reader projections                         | `docs/support/capability_matrix.md`, `docs/support/evidence_map.md`, and `docs/support/hard_cutover_capability_truth.md` | Explain matrix and evidence-map data without creating new support claims.                                                                      |
| Issue closeout evidence                    | `docs/issues/hard_cutover_8132_8150_evidence.md` and `docs/issues/hard_cutover_8132_8150_closeout/*`                     | Preserve checked-in closeout evidence, implementation commit inventories, deferred-operation notes, and rejection or absence evidence.         |
| Schema registry descriptions               | `schemas/README.md` and schema files under `schemas/`                                                                    | Define schema IDs, data shapes, and registry-backed schema ownership.                                                                          |
| Site/spec summaries                        | `site/`, `spec/`, `README.md`, and `CONTRIBUTING.md`                                                                     | Route readers to matrix and evidence-map owners; do not restate support as independent prose.                                                  |

## Claim Rules

- Public Objective-C 3.0 behavior claims come only from implemented matrix rows
  with `support_claims` in the `objc3c.behavior.*` namespace.
- Rejected, reserved, and internal rows are negative, unavailable, schema,
  workflow, owner-boundary, or evidence-boundary rows. They do not create public
  language/runtime behavior.
- The evidence map is a projection of matrix evidence. It cannot add a row that
  is absent from `docs/support/capability_matrix.json`.
- Markdown projections, site pages, spec chapters, runbooks, and issue closeout
  payloads must link to the matrix/evidence-map owners when they describe
  support.
- Generated evidence outputs, archived evidence, closeout payloads, and issue
  evidence tables may preserve provenance and absence/rejection evidence. They
  cannot promote support unless the matrix and evidence map already carry the
  implemented row and evidence.
- Implementation commit inventories in closeout docs are checked-in branch
  evidence boundaries. They do not prove validation, push state, GitHub issue
  edits, remote closure, release readiness, or compatibility support.

## Command Responsibility

Public replay commands are limited to the npm bridge:

```text
npm run objc3c -- <action>
```

Direct helper invocations, registry facades, retired package-script aliases, and
generated-output paths may appear as implementation or provenance evidence, but
they are not public workflow actions and must not be used as support claim
sources.

## Update Order

1. Change `docs/support/capability_matrix.json` first.
2. Keep `docs/support/evidence_map.json` in exact projection agreement with the
   matrix evidence rows.
3. Update `schemas/` when the matrix or evidence-map shape changes.
4. Update the markdown projections and public docs after the machine-readable
   owners are correct.
5. Keep issue closeout docs as evidence consumers that point back to the owner
   surfaces above.
