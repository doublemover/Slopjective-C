# Parser Bucket

Minimum scope:

- keyword reservation/contextual keyword parsing,
- `?.`, postfix propagation `?`, `??`, `try`/`await` precedence/disambiguation,
- actor/executor syntax and capture-list grammar edge cases.

## Wave 0 fixture set (E.3.1 mode selection)

These fixtures cover the language-mode selection requirements tracked by
issues `#48` and `#49`.

- `TUV-01.json`: accepts TU-wide CLI mode selection (`-fobjc-version=3`).
- `LMV-48-NEG-01.json`: rejects unsupported CLI language-version values.
- `TUV-02.json`: accepts file-scope `#pragma objc_language_version(3)`.
- `TUV-03.json`: rejects region-scoped `push/pop` language-version forms.
- `TUV-04.json`: rejects late/post-declaration language-version pragmas.
- `TUV-05.json`: rejects conflicting CLI/source language-version selections.

See `tests/conformance/parser/manifest.json` for machine-readable indexing.

## Feature-test macro fixture set (issue #50)

These fixtures cover mode and feature macro conformance:

- `FTM-50-01.json`: required core feature-test macro presence checks.
- `FTM-50-02.json`: strictness/concurrency mode-selection macro consistency.
- `FTM-50-03.json`: recognized-but-disabled feature macro deterministic value.

## Async/await parser fixture set (issue #80)

These fixtures cover parser acceptance/rejection for async/await grammar:

- `ASY-01.json`: accepted async declaration and await-expression forms.
- `ASY-02.json`: rejected invalid await placement and malformed async forms.

## D-011 await parser fixture set (issue #87)

These fixtures cover await grammar for all potentially-suspending categories:

- `AWT-01.json`: accepted await operands spanning async calls, executor hops,
  and actor-isolated sends.
- `AWT-02.json`: rejected malformed await expression without an operand.

## Part 8 canonical spelling parser fixture set (issue #88)

These fixtures cover canonical system-programming spellings:

- `SYS-ATTR-01.json`..`SYS-ATTR-04.json`: canonical parsing and sugar
  normalization checks for `objc_resource`, `objc_returns_borrowed`, `borrowed`,
  and capture-list contextual keywords.

## Capture-list parser fixture set (issue #93)

These fixtures cover capture-list grammar parsing:

- `CAP-01.json`, `CAP-02.json`: valid capture-list forms and malformed-form
  rejection for Part 8 capture-list syntax.

## Performance-control parser fixture set (issue #95)

These fixtures cover canonical performance/dynamism attributes:

- `PERF-ATTR-01.json`, `PERF-ATTR-02.json`: canonical parsing for
  `objc_direct`, `objc_final`, and `objc_sealed`.

## Objective-C 3.0 container layout fixture set (issue #8010)

These fixtures cover parser-owned container, property, and ivar layout closure:

- `tests/tooling/fixtures/native/recovery/dispatch/parser_container_inherited_ivar_layout.objc3`:
  accepts superclass/subclass interfaces and checks that the public workflow can
  emit stable runtime metadata for inherited property-backed ivar slots.
- `tests/tooling/fixtures/native/recovery/negative/negative_parser_container_ivar_layout_cycle.objc3`:
  rejects cyclic interface inheritance with `O3P150` before layout metadata is
  treated as claimable.

## Objective-C 3.0 draft syntax surface fixture set (issue #8011)

These fixtures cover parser-owned block/error/async/actor/macro/property
behavior/interop syntax admission and deterministic replay accounting:

- `tests/tooling/fixtures/native/recovery/dispatch/parser_draft_syntax_surfaces.objc3`:
  accepts one aggregate public-workflow fixture spanning a bound block literal,
  `throws`/`try`/`throw`/`do catch`, async/await executor annotations,
  contextual `actor class`, macro package/provenance/cache/sandbox attributes,
  `behavior=...` properties, property accessor/synthesis/reflection metadata,
  foreign/header/Swift/C++ interop annotations, bridged error payload metadata,
  and nested cleanup markers under a do/catch surface.
- `tests/tooling/fixtures/native/recovery/negative/negative_parser_draft_syntax_macro_payload.objc3`:
  rejects malformed macro payload syntax with `O3P341` so macro admission stays
  fail-closed and source-range stable.
- `tests/tooling/fixtures/native/recovery/negative/negative_parser_draft_syntax_macro_cache_key_non_string.objc3`
  and `tests/tooling/fixtures/native/recovery/negative/negative_parser_draft_syntax_macro_sandbox_payload.objc3`:
  reject malformed cache-key and sandbox policy payloads before later macro host
  semantics can claim runtime behavior.
- `tests/tooling/fixtures/native/recovery/negative/negative_parser_draft_syntax_nested_cleanup_defer_body.objc3`:
  rejects malformed nested cleanup syntax with `O3P110` before unwind lowering
  can claim the cleanup marker.

## Objective-C 3.0 draft syntax conformance matrix (issue #8012)

The durable conformance map is
`tests/conformance/parser/draft_syntax_surface_conformance.json`. It maps each
parser-owned draft syntax surface to:

- positive source tokens in
  `tests/tooling/fixtures/native/recovery/dispatch/parser_draft_syntax_surfaces.objc3`;
- the emitted `draft_syntax_surface_handoff_key` field that must report a
  non-zero replay count for that surface;
- a negative fixture with the expected diagnostic code, line, and column.

The negative fixture set covers malformed or unsupported variants for block
literals, `try`, `throw`, `do catch`, `throws`, `async`, `await`, `actor class`,
macro marker/package/provenance payloads, property behavior payloads, and
interop bridge/import attributes. The implementation anchor for the executable
evidence builder is `scripts/build_objc3c_parser_draft_syntax_conformance.py`;
public validation remains routed through `npm run objc3c -- validate-conformance-corpus`.
