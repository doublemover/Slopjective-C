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
| `capability_schema_examples.md` | Examples and anti-examples for matrix and evidence rows. |
| `hard_cutover_capability_truth.md` | Human-readable hard-cutover support boundary for docs, site, stdlib, and runbook prose. |

Issue closeout payloads are downstream evidence consumers, not new support
truth. They must point back to committed branch surfaces in the files above:

- `docs/issues/hard_cutover_8132_8150_evidence.md`
- `docs/issues/hard_cutover_8132_8150_evidence.json`
- `docs/issues/hard_cutover_8132_8150_closeout/payload_index.json`
- `docs/issues/hard_cutover_8132_8150_closeout/payloads.md`

The canonical capability schema IDs are registered through
`scripts/objc3c_shared/schema_registry.py`:

- `objc3c-capability-matrix-v1`
- `objc3c-capability-evidence-map-v1`
- backing schema files under `schemas/`
- `schemas/README.md`
- `scripts/objc3c_shared/schema_registry.py`

There is no support-directory schema mirror. Capability matrix consumers load
the matrix and evidence-map schemas through the shared registry so schema
ownership cannot drift between local copies and checked-in registry entries.

`capability_matrix.json` also carries `projection_policy`. That object names
the authoritative data files, schema sources, and human projections so consumers
can distinguish source truth from reader-facing summaries.

`capability_matrix.json` carries `claim_contract` as the state-to-claim rule.
Only `implemented` rows with `support_claims` in `objc3c.behavior.*` can become
public Objective-C 3.0 behavior claims. `rejected`, `reserved`, and `internal`
rows remain negative, unavailable, schema, workflow, report, or owner truth and
must not be promoted by aliases, compatibility/fallback wording, direct helper
commands, registry facades, or generated reports.

`evidence_map.json` carries `projection_contract`. That contract makes the
evidence map a flattened projection of
`docs/support/capability_matrix.json#/capabilities/*/evidence`, owned by
`scripts/capability_docs_validator/evidence_map.py`. The stable row key is
`capability_id`, `support_claim`, `evidence_kind`, `path`, and `command`; the
validator rejects duplicate, missing, or extra evidence-map keys.

## Change Rules

- Add or change a support claim in `capability_matrix.json` first.
- Add matching evidence rows in `evidence_map.json`; the row key must match
  the matrix evidence projection exactly.
- Keep capability schema shape changes in `schemas/`; support-directory JSON
  files consume those schemas but do not own duplicate schema fragments.
- Use `owner_modules` for internal implementation boundaries that support a
  claim without becoming public command surface.
- Update the markdown projections in this directory when the machine-readable
  truth changes.
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
- Closeout payloads may use only committed branch evidence and must keep
  validation, push, and remote issue actions explicitly deferred unless those
  operations actually ran.

## Validation Owner Modules

`scripts/validate_capability_docs.py` remains the stable public validator
entrypoint. Its implementation is split under
`scripts/capability_docs_validator/` so matrix shape, support-claim manifest
truth, evidence-map projection keys, docs references, and CLI behavior have
separate owners.
