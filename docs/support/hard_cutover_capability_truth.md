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
| Object-model interface method table and private class/category/protocol query evidence                                                                | `implemented` where the matrix has behavior rows | Claim the narrow rows directly, and cite the full object-model row only when the combined runtime/debugger proof is required.                                                                                                                                                                                         |
| Full object-model runtime realization                                                                                                                 | `implemented`                                    | Claim the Objective-C 3 runtime identity graph only: class/metaclass/category/protocol/property/ivar/selector/method metadata, registration replay, public reflection, debug anchors, value inspection, production source maps/native line tables, emitted native debug info, typed-keypath debugger metadata, and integrated statement stepping. Do not imply Objective-C 2 compatibility, Swift/C++ mirroring, dynamic forwarding, or broad full-source-map publication for every artifact path. |
| Direct `@import` module syntax                                                                                                                        | `implemented` where the matrix has behavior rows | Claim only parser-admitted deterministic module identity plus locked package provenance; do not imply live hosted lookup or fallback package resolution.                                                                                                                                                              |
| Standalone textual interface payload schema, import, and roundtrip                                                                                    | `implemented` where the matrix has behavior rows | Claim only the schema-registered payload artifact, fail-closed importer, package-lock identity check, source-count drift checks, reserved metadata boundary, and `validate-standalone-textual-interface-payload` evidence. Do not claim broad hosted module distribution or implementation-body export.                |
| Package registry, network, release-channel, and security hardening fixtures                                                                           | `implemented` where the matrix has behavior rows | Claim only source-owned offline fixtures, deterministic local trust, and fail-closed package security contracts; do not imply live hosted service behavior.                                                                                                                                                           |
| Full source-map publication and public hosted package registry services                                                                               | `reserved`                                       | Describe as unavailable for broad production/integrated-program claims even when bounded source-map, source-graph, local registry, hosted-fixture, or debug-anchor evidence exists.                                                                                                                                    |
| Bounded debugger source maps, statement stepping, LLDB replay, inline-frame source maps, and typed-keypath debugger metadata                          | `implemented` where the matrix has behavior rows | Claim the checked rows backed by `validate-debug-source-maps`, `validate-debugger-integration`, inline-frame negative cases, typed-keypath descriptor metadata, and the object-model debugger proof. Do not promote them to broad full-source-map publication.                                                           |
| Blocks, ARC automation, `throws`, async/await, actors, tasks, macros, property behaviors, and broad interop closure                                   | `reserved` unless separately implemented         | Describe as unavailable or reserved spec surface, not runnable support.                                                                                                                                                                                                                                               |
| `throws(E)` typed throws and `Optional<T>` value optionals                                                                                            | typed throws single-payload error-out ABI plus catch/bridge policy; value optionals bounded packed i32/bool/id-handle ABI plus wide Optional<i64> direct ABI | For typed throws, claim only single-payload source/interface preservation, exact sema effect identity, hidden error-out ABI lowering without erasure to bare `throws`, exact typed catch acceptance, policy-backed `id<Error>` bridge catches, incompatible-catch rejection, and unsupported foreign-carrier fail-closed records; the public capability row remains reserved until the public validation gate proves full support. Malformed and multi-payload forms remain rejected. For value optionals, cite only semantic type-signature carrier, stable packed `has_value`/`payload` ABI identity for supported `i32`, `bool`, and `id` handle payload forms, Optional<i64> language call/return ABI through the wide `{has_value,i64}` carrier, direct functions, and direct dispatch path, explicit absent/present construction contracts, checked unwrap/binding diagnostics, and contract fixtures. Object-pointer/nullability bridges, nested optionals, generic payload lowering, property/ivar storage, nil-to-scalar, implicit nil, unchecked unwrap, throws/result conversions, lowercase `optional<T>` aliases, and broad dynamic runtime dispatch remain fail-closed. |
| Generic callable reification                                                                                                                          | bounded source-owned metadata policy             | Claim only Objective-C 3 generic functions and Objective-C generic methods with erased-default or declaration-scoped explicit-reified metadata policy, stable replay keys, and selector-stable method identity. Do not claim selector-local method clauses, C/Objective-C style generic functions, overload-by-generic-signature, variadics, module-wide reification, body cloning, or runtime-specialized metadata. |
| Statement-form guarded match patterns and bounded expression-form `match`                                                                             | source-level only, not a strict profile claim    | Admit statement `case pattern where bool_condition: { ... }` and bounded expression `case pattern where bool_condition => expression;` with scoped bindings, bool guards, exhaustiveness, and result convergence. Do not claim type-test patterns, statement fat-arrow arms, Result payload ABI extraction, aliases, or strict-profile promotion. |
| Built-in strict, strict-concurrency, and strict-system conformance profiles                                                                           | `strict` / `strict-concurrency` implemented; `strict-system` fail-closed | Claim `core`, `strict`, and `strict-concurrency` only through native profile validation, strict diagnostics, strict-concurrency enforcement, public conformance publication, and release-candidate replay evidence; keep `strict-system` target-only and rejected until separate system evidence exists.                                                                                 |
| #8207 language-evolution umbrella                                                                                                                     | `reserved` readiness/truth row                   | Treat as alignment over #8233 through #8237; it is not a behavior claim until every prerequisite row is implemented or explicitly scoped out by a future umbrella decision.                                                                                                                                           |
| #8206 platform expansion umbrella                                                                                                                     | `internal` readiness/truth row                   | Treat as source-owned closure over Windows x64 support, fail-closed Linux/macOS rows, reserved ASan/UBSan rows with expected detection records and release-runtime isolation, and the #8232 coherent `llc --filetype=obj` native-object boundary; do not claim Linux, macOS, sanitizer package, clang-fallback object emission, mixed LLVM roots, mismatched LLVM versions, unsupported LLVM versions, or unresolved LLVM tool identity as support. |
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
