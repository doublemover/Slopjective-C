# Hard-Cutover Capability Boundary

This file is the reader-facing boundary for hard-cutover support claims. It
does not replace the machine-readable matrix; it explains how other docs must
read it.

Authoritative inputs:

- `docs/support/capability_matrix.json`
- `docs/support/capability_matrix.schema.json`
- `docs/support/evidence_map.json`
- `docs/support/umbrella_readiness.json`
- `docs/support/capability_matrix.md`
- `docs/support/evidence_map.md`
- `docs/support/umbrella_readiness.md`
- `docs/support/capability_claim_responsibility.md`
- `scripts/objc3c_shared/schema_registry.py`
- `native/objc3c/src/artifacts/json/capability_support_schema_records.cpp`

Schema ownership is not mirrored under `docs/support`. The support schema
entrypoint delegates to the canonical matrix schema under `schemas/`; consumers
validate the matrix and evidence map through registry-owned schema IDs and
artifact-owned publication records: `objc3c-capability-matrix-v1` and
`objc3c-capability-evidence-map-v1`. Broad umbrella readiness is checked by
`objc3c-umbrella-readiness-v1`; it records blockers and final promotion
criteria, not public behavior support.

Projection rule: markdown files, site pages, and runbooks are projections of
the JSON matrix and evidence map. They may clarify reader expectations, but they
must not introduce a public support claim, command surface, or completion state
that is absent from the authoritative data.

Claim rule: only `implemented` rows with `support_claims` in
`objc3c.behavior.*` are public Objective-C 3.0 behavior claims. Rejected,
reserved, and internal rows are negative, unavailable, schema, workflow, report,
or owner/evidence boundaries only; compatibility/retired-route wording and
aliases cannot fill in a missing support claim.

Issue closeout payloads are support-boundary evidence only when they point back
to committed branch surfaces listed by the capability matrix, evidence map, or
hard-cutover issue evidence files. Implementation commit lists in those files
are checked-in branch evidence boundaries. They are not validation reports,
pushed-state evidence, remote issue edits, release claims, or remote closure,
and they do not close the gap left by deferred validation, push, or tracker
operations. Source-head labels in those issue maps identify the committed
implementation evidence covered by the docs; they do not upgrade capability
state or imply validation, GitHub issue edits, push state, or remote closeout.

## Support States

| State         | Meaning for public docs                                                      | What docs must not infer                                                               |
| ------------- | ---------------------------------------------------------------------------- | -------------------------------------------------------------------------------------- |
| `implemented` | The named behavior has an evidence-backed support claim.                     | Broader language/runtime support outside the row.                                      |
| `rejected`    | The source form or behavior is a diagnostic/strict-error case.               | Alternate acceptance by flag, retired adapter, alternate path, or old source spelling. |
| `reserved`    | The syntax, feature family, or runtime closure remains unavailable.          | A roadmap promise, preview mode, or partial runtime claim.                             |
| `internal`    | The row names implementation, schema, report, workflow, or owner boundaries. | Public Objective-C 3.0 language behavior.                                              |

## Current Claim Boundary

| Area                                                                                                                                                  | Capability state                                 | Public wording                                                                                                                                                                                                                                                                                                        |
| ----------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------ | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Parser, typed sema, strict runtime-dispatch lowering, IR module emission, strict dispatch diagnostics, and runnable smoke                             | `implemented` where the matrix has behavior rows | Claim only the named support claim and its linked evidence.                                                                                                                                                                                                                                                           |
| Native module decomposition, public C runtime API shape, workflow bridge, and JSON/schema helpers                                                     | `internal`                                       | Treat as owner/evidence surfaces, not language features.                                                                                                                                                                                                                                                              |
| Object-model interface method table and private class/category/protocol query evidence                                                                | `implemented` where the matrix has behavior rows | Claim only the narrow matrix row; do not promote it to broader runtime behavior.                                                                                                                                                                                                                                      |
| Full object-model runtime realization                                                                                                                 | `reserved` until its own matrix row changes      | Describe as unclaimed; link evidence owners instead of promising runtime behavior.                                                                                                                                                                                                                                    |
| Direct `@import` module syntax                                                                                                                        | `implemented` where the matrix has behavior rows | Claim only parser-admitted deterministic module identity plus locked package provenance; do not imply live hosted lookup or fallback package resolution.                                                                                                                                                              |
| Standalone textual interface payload schema, import, and roundtrip                                                                                    | `implemented` where the matrix has behavior rows | Claim only the schema-registered payload artifact, fail-closed importer, package-lock identity check, source-count drift checks, reserved metadata boundary, and `validate-standalone-textual-interface-payload` evidence. Do not claim broad hosted module distribution or implementation-body export.                |
| Package registry, network, release-channel, and security hardening fixtures                                                                           | `implemented` where the matrix has behavior rows | Claim only source-owned offline fixtures, deterministic local trust, and fail-closed package security contracts; do not imply live hosted service behavior.                                                                                                                                                           |
| Full source-map publication and public hosted package registry services                                                                               | `reserved`                                       | Describe as unavailable for broad production/integrated-program claims even when bounded source-map, source-graph, local registry, hosted-fixture, or debug-anchor evidence exists.                                                                                                                                    |
| Bounded debugger source maps, statement stepping, LLDB replay, inline-frame source maps, and typed-keypath debugger metadata                          | `implemented` where the matrix has behavior rows | Claim only the checked rows backed by `validate-debug-source-maps`, `validate-debugger-integration`, inline-frame negative cases, and typed-keypath descriptor metadata. Do not promote these rows to full object-model realization or full source-map publication.                                                     |
| Blocks, ARC automation, `throws`, async/await, actors, tasks, macros, property behaviors, and broad interop closure                                   | `reserved` unless separately implemented         | Describe as unavailable or reserved spec surface, not runnable support.                                                                                                                                                                                                                                               |
| `throws(E)` typed throws, `Optional<T>` value optionals, and expression-form `match`                                                                  | `reserved` / strict-error rejection              | Cite only the parser diagnostics, reserved capability rows, and checked negative/contract fixtures; typed throws are not erased to bare `throws`, lowercase `optional<T>` is not an alias, and none of these are preview, partial, or accepted syntax.                                                                |
| Generic callable reification                                                                                                                          | `reserved` beyond erased metadata                | Claim only generic class receiver substitution and Objective-C 3 `fn name<T>(...)` free-function metadata where implemented; do not claim `@reify_generics`, Objective-C method type-parameter clauses, C/Objective-C style generic functions, overload-by-generic-signature, variadics, or runtime reified metadata. |
| Statement-form guarded match patterns                                                                                                                 | source-level only, not a strict profile claim    | Admit only `case pattern where bool_condition: { ... }` inside statement `match`; do not claim expression-match, type-test patterns, aliases, or strict-profile promotion.                                                                                                                                            |
| Built-in strict, strict-concurrency, and strict-system conformance profiles                                                                           | targeted, not claimed                            | Say only `core` is claimed; strict profiles are release-evidence targets and fail-closed selection surfaces.                                                                                                                                                                                                          |
| #8207 language-evolution umbrella                                                                                                                     | `reserved` readiness/truth row                   | Treat as alignment over #8233 through #8237; it is not a behavior claim until every prerequisite row is implemented or explicitly scoped out by a future umbrella decision.                                                                                                                                           |
| #8206 platform expansion umbrella                                                                                                                     | `internal` readiness/truth row                   | Treat as source-owned closure over Windows x64 support, fail-closed Linux/macOS rows, reserved ASan/UBSan rows, and the #8232 `llc --filetype=obj` native-object boundary; do not claim Linux, macOS, sanitizer package, or clang-fallback object-emission support.                                                   |
| Old modes, retired mode labels, alias adapters, alternate acceptance paths, retired-source lanes, direct helper commands, and evidence-log completion | unsupported/retired wording                      | Mention only as negative evidence, source-hygiene data, or rejection inventory.                                                                                                                                                                                                                                       |

Advanced runtime closure is split into explicit implemented rows and reserved
boundary rows. Implemented rows may be named only through their support claims in
`docs/support/capability_matrix.json`; the following boundaries remain
non-claiming unless a future matrix row changes state:

- `runtime.blocks.full-language-closure`
- `runtime.arc.full-automation`
- `runtime.errors.generalized-foreign-exception-abi`
- `runtime.concurrency.broad-async-actor-closure`
- `runtime.metaprogramming.arbitrary-macro-ecosystem`
- `runtime.interop.broad-runtime-closure`

The #8200 cross-lane evidence is still row-scoped. Text/package source-graph
and declaration-debug-anchor proof does not promote full source-map publication;
bounded statement stepping and LLDB replay are claimable only through the
implemented `runtime.debug-trace.statement-stepping` and
`runtime.debug-trace.lldb-plugin` rows. Direct `@import` support is claimed only
through `modules.direct-import-syntax` and its package provenance validator.
Optimization runtime equivalence now claims only the bounded method-inlining
subset with exact callee identity, checked before/after IR proof, inline-frame
source maps, side-effect replay, and runtime invalidation replay; all
missing-proof paths remain fail-closed. Distribution lifecycle evidence proves
local clean install, release-operation rollback safety, and tampered installed
package rejection; package-specific rows now own offline hosted-fixture
resolution, offline network dependency resolution, source-owned release-channel
publication metadata, and security hardening without promoting a live hosted
registry service, network install, background updater, or public production
release channel. Advanced-runtime
native artifact evidence is likewise bounded: it proves the combined fixture
compiles through the real direct-native path, links and runs with a checked
source-owned provider module, and publishes object, LLVM IR, manifest, runtime
registration, runtime metadata, error replay, and executable artifacts from a
fresh generated-attempt directory. The implemented umbrella claim stops at that
integrated Objective-C 3 runtime envelope; broad scheduler fairness, Swift/C++
runtime mirroring, distributed actor networking, and arbitrary macro-host
execution remain separate reserved or fail-closed rows.

## Documentation Rule

Docs, specs, site pages, stdlib notes, and runbooks may summarize support only
by linking back to the capability matrix and evidence map. A prose statement is
not a support claim unless an `implemented` capability row carries the matching
evidence.

Source-hygiene hard-cutover report schemas describe registry-owned
negative-evidence surfaces:

- `schemas/source-hygiene-hard-cutover-report-v1.schema.json`
- registry owner: `scripts/objc3c_shared/schema_registry.py`

They classify retired-surface residue and generated-output row inventory;
they do not define alternate old-surface support or create evidence-log claims.

Capability matrix and evidence-map schema publication is narrower than the
general schema registry: `capability_support_schema_records.cpp` owns the
artifact-facing records for the support data files and their markdown
projections, so docs do not restate those schema identities independently.

When a feature is partially present in parser, metadata, emitted artifacts, or
runtime owner modules, docs must name the owner surface and matrix state. They
must not round that into full runtime behavior, alternate old-surface support, or an
unsupported completion claim.
