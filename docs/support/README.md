# Support Boundary Contract

This directory owns the public Objective-C 3.0 support boundary. Other docs,
spec chapters, site pages, runbooks, and generated-output rows may
summarize support, but they must not widen it beyond these files.

## Canonical Files

| File                                 | Purpose                                                                                                                |
| ------------------------------------ | ---------------------------------------------------------------------------------------------------------------------- |
| `capability_matrix.json`             | Machine-readable capability states, support claims, evidence pointers, command-surface policy, and hard-cutover rules. |
| `capability_matrix.schema.json`      | Ref-only support-directory schema entrypoint for the canonical capability matrix schema under `schemas/`.              |
| `capability_matrix.md`               | Human-readable projection of the capability matrix.                                                                    |
| `evidence_map.json`                  | Machine-readable flattened capability-to-evidence rows.                                                                |
| `evidence_map.md`                    | Human-readable evidence table.                                                                                         |
| `umbrella_readiness.json`            | Machine-readable readiness gates for broad reserved umbrella capability rows.                                          |
| `umbrella_readiness.md`              | Human-readable projection of umbrella promotion blockers and final criteria.                                           |
| `capability_schema_examples.md`      | Examples and anti-examples for matrix and evidence rows.                                                               |
| `capability_claim_responsibility.md` | Responsibility split for capability claims, checked-in evidence, issue closeout payloads, and generated-output rows.   |
| `hard_cutover_capability_truth.md`   | Human-readable hard-cutover support boundary for docs, site, stdlib, and runbook prose.                                |

Issue closeout payloads are downstream evidence consumers, not support owners.
They must point back to committed branch surfaces in the files above and must
describe implementation commit lists as checked-in branch evidence boundaries. A
payload or issue evidence table is not validation evidence, pushed-state
evidence, GitHub issue action, remote closure, or compatibility support.

- `docs/issues/hard_cutover_8132_8150_evidence.md`
- `docs/issues/hard_cutover_8132_8150_evidence.json`
- `docs/issues/hard_cutover_8132_8150_closeout/payload_index.json`
- `docs/issues/hard_cutover_8132_8150_closeout/payloads.md`

The canonical capability schema IDs are registered through the shared schema
registry and native artifact publication records:

- `objc3c-capability-matrix-v1`
- `objc3c-capability-evidence-map-v1`
- `objc3c-umbrella-readiness-v1`
- backing schema files under `schemas/`
- `schemas/README.md`
- `scripts/objc3c_shared/schema_registry.py`
- `native/objc3c/src/artifacts/json/capability_support_schema_records.cpp`

There is no support-directory schema mirror. `capability_matrix.schema.json` is
a ref-only entrypoint that delegates to
`schemas/objc3c-capability-matrix-v1.schema.json` for docs and editor tooling;
it carries no independent schema ID.
Capability matrix consumers load the canonical matrix and evidence-map schemas
through the shared registry and artifact schema contract records so schema
ownership cannot drift between local copies and checked-in registry entries.

`capability_matrix.json` also carries `projection_policy`. That object names
the authoritative data files, schema sources, and human projections so consumers
can distinguish owner data from reader-facing summaries.

`capability_matrix.json` carries `claim_contract` as the state-to-claim rule.
Only `implemented` rows with `support_claims` in `objc3c.behavior.*` can become
public Objective-C 3.0 behavior claims. `rejected`, `reserved`, and `internal`
rows remain negative, unavailable, schema, workflow, owner-boundary, or
evidence-boundary rows and must not be promoted by aliases,
compatibility/retired-route wording, direct helper commands, registry facades,
or generated-output rows.

`evidence_map.json` carries `projection_contract`. That contract makes the
evidence map a flattened projection of
`docs/support/capability_matrix.json#/capabilities/*/evidence`, owned by
`scripts/capability_docs_validator/evidence_map.py`. The stable row key is
`capability_id`, `support_claim`, `evidence_kind`, `path`, and `command`; the
validator rejects duplicate, missing, or extra evidence-map keys.

`umbrella_readiness.json` carries schema-backed promotion gates for broad
reserved rows. It can explain missing prerequisites and blockers, but it cannot
promote an umbrella row or create a public support claim unless the matrix row
itself changes to `implemented` with evidence.

## Change Rules

- Add or change a support claim in `capability_matrix.json` first.
- Add matching evidence rows in `evidence_map.json`; the row key must match
  the matrix evidence projection exactly.
- Keep capability schema shape changes in `schemas/`; support-directory JSON
  files consume those schemas but do not own duplicate schema fragments.
- Use `owner_modules` for internal implementation boundaries that support a
  claim without becoming public command surface.
- Update the markdown projections in this directory when the machine-readable
  owner data changes.
- Keep site/spec/runbook summaries subordinate to this directory.
- Treat markdown projections as summaries of `capability_matrix.json` and
  `evidence_map.json`; they cannot introduce support claims on their own.
- Keep `claim_contract` aligned with capability states so only implemented
  rows can carry public behavior claims.
- Use only `implemented`, `rejected`, `reserved`, and `internal` as capability
  states.
- Public replay commands must use `npm run objc3c -- <action>`.
- Evidence-map rows without commands are ownership or boundary rows only; they
  must not be treated as public workflow actions.
- Keep retired alternate-surface categories in `retired_surface_terms`; active
  rows must use current canonical capability names.
- Retired alternate surfaces are diagnostics, history, or anti-examples; they
  are not support modes.
- Evidence schema section identifiers are not public capability labels. If a
  checked-in artifact carries older section names, support prose must describe
  the row through current adoption, replay, upgrade, support, or rejection
  terminology and point back to the matrix/evidence map.
- Runtime, object-model, stdlib, or workflow prose must not upgrade an
  `internal` or `reserved` row into public behavior. Link the matrix row and
  evidence instead.
- Closeout payloads may use only committed branch evidence, must label
  implementation commit inventories as checked-in branch evidence, and must keep
  validation, push, and remote issue actions explicitly deferred unless those
  operations actually ran.

## Validation Owner Modules

`scripts/validate_capability_docs.py` remains the stable public validator
entrypoint. Its implementation is split under
`scripts/capability_docs_validator/` so matrix shape, support-claim manifest
state rules, evidence-map projection keys, docs references, and CLI behavior have
separate owners.

`scripts/build_capability_support_docs.py` owns the generated markdown
projections for `capability_matrix.md`, `evidence_map.md`, and
`umbrella_readiness.md`. The validator checks those generated files for drift,
so public support prose fails closed if it claims a behavior above the support
matrix, evidence map, umbrella readiness gates, canonical fixture manifest, or
conformance phase-owner contract.
