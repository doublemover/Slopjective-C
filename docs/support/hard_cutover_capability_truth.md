# Hard-Cutover Capability Truth

This file is the reader-facing boundary for hard-cutover support claims. It
does not replace the machine-readable matrix; it explains how other docs must
read it.

Authoritative inputs:

- `docs/support/capability_matrix.json`
- `docs/support/evidence_map.json`
- `docs/support/capability_matrix.md`
- `docs/support/evidence_map.md`
- `schemas/objc3c-capability-matrix-v1.schema.json`
- `schemas/objc3c-capability-evidence-map-v1.schema.json`

Projection rule: markdown files, site pages, and runbooks are projections of
the JSON matrix and evidence map. They may clarify reader expectations, but they
must not introduce a public support claim, command surface, or completion state
that is absent from the authoritative data.

Issue closeout payloads are support-boundary evidence only when they point back
to committed branch surfaces listed by the capability matrix, evidence map, or
hard-cutover issue evidence files. They are not validation reports, remote issue
edits, or release claims, and they do not close the gap left by deferred
validation, push, or tracker operations.

## Support States

| State | Meaning for public docs | What docs must not infer |
| --- | --- | --- |
| `implemented` | The named behavior has an evidence-backed support claim. | Broader language/runtime support outside the row. |
| `rejected` | The source form or behavior is a diagnostic/strict-error case. | Alternate acceptance by flag, shim, fallback, or old source spelling. |
| `reserved` | The syntax, feature family, or runtime closure remains unavailable. | A roadmap promise, preview mode, or partial runtime claim. |
| `internal` | The row names implementation, schema, report, workflow, or owner boundaries. | Public Objective-C 3.0 language behavior. |

## Current Claim Boundary

| Area | Capability state | Public wording |
| --- | --- | --- |
| Parser, typed sema, strict runtime-dispatch lowering, IR module emission, strict dispatch diagnostics, and runnable smoke | `implemented` where the matrix has behavior rows | Claim only the named support claim and its linked evidence. |
| Native module decomposition, public C runtime API shape, workflow bridge, and JSON/schema helpers | `internal` | Treat as owner/evidence surfaces, not language features. |
| Full object-model runtime realization | `reserved` until a matrix row changes | Describe as unclaimed; link evidence owners instead of promising runtime behavior. |
| Blocks, ARC automation, `throws`, async/await, actors, tasks, macros, property behaviors, and broad interop closure | `reserved` unless separately implemented | Describe as unavailable or reserved spec surface, not runnable support. |
| Old modes, compatibility modes, aliases, fallback paths, shims, migration lanes, direct helper commands, and report-only completion | unsupported/retired wording | Mention only as negative evidence, source-hygiene data, or rejection inventory. |

## Documentation Rule

Docs, specs, site pages, stdlib notes, and runbooks may summarize support only
by linking back to the capability matrix and evidence map. A prose statement is
not a support claim unless an `implemented` capability row carries the matching
evidence.

When a feature is partially present in parser, metadata, emitted artifacts, or
runtime owner modules, docs must name the owner surface and matrix state. They
must not round that into full runtime behavior, compatibility support, or an
unsupported completion claim.
