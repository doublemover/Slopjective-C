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
