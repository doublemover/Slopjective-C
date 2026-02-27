# Conformance Examples Corpus

This directory contains executable test specs for punctuation-heavy edge cases.

Each `*.json` file is one conformance artifact with:

- `id`: stable example identifier used by spec tables and issue tracking.
- `feature`: feature bucket for coverage accounting.
- `bucket`: conformance suite bucket (`parser`, `semantic`, `diagnostics`).
- `profile`: minimum profile that must enforce the expectation.
- `source`: complete ObjC 3.0 snippet to compile.
- `expect`: expected parse outcome plus portable diagnostics.

Portable diagnostics in `expect.diagnostics[*]` use this shape:

- `code`: stable diagnostic code.
- `severity`: `error`, `warning`, or `note`.
- `span`: `{ "start": { "line", "column" }, "end": { "line", "column" } }`.
- `message`: stable prose fragment for resilient matching.
- `fixits`: machine-applicable edits when the spec requires a mechanical rewrite.

`manifest.json` indexes all artifacts and can be used by a harness to enumerate
and execute this corpus.
