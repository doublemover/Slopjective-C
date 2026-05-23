# objc3c-native Source Ownership

This file defines source ownership for generated `docs/objc3c-native.md`.

## Scope

- Source directory: `docs/objc3c-native/src/`
- Generated output: `docs/objc3c-native.md`
- Generator action: `npm run objc3c -- build-native-docs`

## Fragment Ownership Matrix

| Fragment                     | Responsibility                                          | Primary owner        | Backup owner       |
| ---------------------------- | ------------------------------------------------------- | -------------------- | ------------------ |
| `10-cli.md`                  | CLI usage, flags, defaults, invocation examples         | compiler/tooling     | compiler/docs      |
| `20-grammar.md`              | Lexer/parser grammar and token surface                  | compiler/parser      | compiler/docs      |
| `30-semantics.md`            | Semantic contracts and lowering behavior                | compiler/semantics   | compiler/docs      |
| `35-runtime-architecture.md` | Runtime execution, owner surfaces, and claim boundaries | runtime/compiler     | compiler/docs      |
| `40-diagnostics.md`          | Diagnostic codes and failure semantics                  | compiler/diagnostics | compiler/semantics |
| `50-artifacts.md`            | Artifact paths, runtime output contracts, exit surfaces | compiler/release     | compiler/tooling   |
| `60-tests.md`                | Replay proof and validation command contracts           | compiler/qa          | compiler/release   |
| `library-api.md`             | Embedding ABI, header ownership, and version gates      | compiler/api         | compiler/tooling   |
| `README.md`                  | Fragment taxonomy, stitch contract, author instructions | compiler/docs        | compiler/tooling   |

## Runtime Architecture Section Ownership

`35-runtime-architecture.md` is the single stitched runtime chapter. Keep new
runtime-source claims inside these owner sections unless the stitcher contract
is deliberately changed.

| Section group               | Headings                                                                  | Primary owner       |
| --------------------------- | ------------------------------------------------------------------------- | ------------------- |
| Pipeline and bootstrap      | working boundary, execution flow, state publication, bootstrap, startup   | runtime/compiler    |
| Metaprogramming             | metaprogramming source, package/provenance, semantics, lowering, cache    | compiler/semantics  |
| Concurrency                 | unified concurrency source, normalization, lowering, metadata             | runtime/concurrency |
| Error handling              | error source, catch/finalization, propagation, unwind diagnostics, ABI    | runtime/errors      |
| Blocks and ARC              | block/ARC source, ownership transfer, lowering, helper ABI, preservation  | runtime/memory      |
| Object model and reflection | property/ivar/accessor, realization, dispatch tables, reflection, lookup  | runtime/classes     |
| Installation and validation | loader lifecycle, acceptance suite, shared harness, evidence, claim rules | compiler/qa         |

## Update Workflow

1. Edit only source fragments in `docs/objc3c-native/src/`.
1. Run `npm run objc3c -- build-native-docs` to regenerate output.
1. Run `npm run objc3c -- check-native-docs` and require pass.
1. Run `npm run objc3c -- check-public-command-surface` when public command
   references change.
1. Run `npm run objc3c -- check-public-command-budget` when public command
   appendix or package bridge ownership changes.
1. Commit fragment edits and generated output together when output changed.

## Review Policy

- Direct changes to `docs/objc3c-native.md` must match source-section updates
  and regeneration evidence.
- Changes must be reviewed by the primary owner for each touched fragment.
- Cross-fragment changes require at least one reviewer from `compiler/docs`.
- PR description must include regeneration command and check output status.

## Public Doc Style And Accessibility Rules

When fragments affect `README.md`, `site/index.md`, or `docs/objc3c-native.md`,
review against these rules:

- state current support and current gaps plainly,
- prefer short lead paragraphs and structured lists over long dense narrative,
- explain implementation jargon in the same section that introduces it,
- keep machine-only report inventories in generated runbooks or `tmp/`, not in reader-facing lead sections,
- give readers the next exact file or command when sending them deeper into the repo.

Reject changes that make the reader infer:

- whether a feature is real or aspirational,
- whether a path is human-facing or machine-facing,
- or whether a section is current implementation truth versus archived transition material.
