# Hard-Cutover Diagnostic Outcome Code Index

This is the human-readable companion to
`tests/conformance/hard_cutover_diagnostic_outcome_code_index.json`. It is local
evidence only; no validation, GitHub edits, push, build, generator, formatter,
lint, npm, cmake, or test command was run while preparing it.

| Code | Outcome | Retired / Unsupported Surface | Issues |
| --- | --- | --- | --- |
| `O3C002` | parser rejection | old-mode literal aliases | `#8134`, `#8144`, `#8150` |
| `OBJC3-E-REMOVED-COMPATIBILITY-MODE` | parser rejection | removed compatibility mode flag | `#8134`, `#8144`, `#8150` |
| `OBJC3-E-REMOVED-FALLBACK-FLAG` | parser rejection | removed parser fallback flag | `#8134`, `#8144`, `#8150` |
| `OBJC3-E-REMOVED-COMPATIBILITY-SHIM` | semantic rejection | removed retired adapter gate | `#8135`, `#8145`, `#8147` |
| `O3S214` | semantic rejection | unsupported or unknown assignment target | `#8135`, `#8146`, `#8147` |
| `O3S221` | semantic rejection | unsupported feature claim | `#8135`, `#8145`, `#8147` |
| `OBJC3-E-REMOVED-RUNTIME-FALLBACK` | strict error | removed runtime dispatch fallback flag | `#8136`, `#8137`, `#8147` |
| `link.unresolved_symbol` | strict error | non-nil runtime dispatch fallback linkage | `#8136`, `#8137`, `#8143`, `#8147` |
| `O3RT002` | runtime strict error | unknown receiver runtime dispatch fallback | `#8133`, `#8143`, `#8144`, `#8150` |
| `absent-support` | absent support | retired-source lane as behavior support | `#8145`, `#8149`, `#8150` |

The entries are evidence-routing keys. They identify which owner rejects a
retired or unsupported surface, but they are not compatibility support claims.
The `absent-support` row exists because retired-source lane support has no diagnostic
surface to execute: it is absent from public support and positive fixture
indexes.

Latest diagnostic owner evidence is folded into
`docs/issues/hard_cutover_latest_local_commit_refresh.md`, including diagnostic
code, severity, core render/record, parse, removed-mode classifier, and catalog
owner splits. Those commits refresh diagnostic ownership only; they do not add
new compatibility diagnostics or positive retired-surface behavior.
