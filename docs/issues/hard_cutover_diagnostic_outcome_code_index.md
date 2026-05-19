# Hard-Cutover Diagnostic Outcome Code Index

This is the human-readable companion to
`tests/conformance/hard_cutover_diagnostic_outcome_code_index.json`. It is local
checked-in evidence only; no validation, GitHub edits, push, build, generator,
formatter, lint, npm, cmake, or test command was run while preparing it.

| Code                                    | Outcome              | Retired / Unsupported Surface                   | Issues                             |
| --------------------------------------- | -------------------- | ----------------------------------------------- | ---------------------------------- |
| `O3C002`                                | parser rejection     | old-mode literal aliases                        | `#8134`, `#8144`, `#8150`          |
| `OBJC3-E-REMOVED-COMPATIBILITY-MODE`    | parser rejection     | removed compatibility mode flag                 | `#8134`, `#8144`, `#8150`          |
| `OBJC3-E-REMOVED-RETIRED_ROUTE-FLAG`    | parser rejection     | removed parser retired route flag               | `#8134`, `#8144`, `#8150`          |
| `OBJC3-E-REMOVED-COMPATIBILITY-GATE`    | semantic rejection   | removed retired adapter gate                    | `#8135`, `#8145`, `#8147`          |
| `O3S214`                                | semantic rejection   | unsupported or unknown assignment target        | `#8135`, `#8146`, `#8147`          |
| `O3S221`                                | semantic rejection   | unsupported feature claim                       | `#8135`, `#8145`, `#8147`          |
| `OBJC3-E-REMOVED-RUNTIME-RETIRED_ROUTE` | strict error         | removed runtime dispatch retired route flag     | `#8136`, `#8137`, `#8147`          |
| `link.unresolved_symbol`                | strict error         | non-nil runtime dispatch retired route linkage  | `#8136`, `#8137`, `#8143`, `#8147` |
| `O3RT002`                               | runtime strict error | unknown receiver runtime dispatch retired route | `#8133`, `#8143`, `#8144`, `#8150` |
| `absent-support`                        | absent support       | retired-source lane as behavior support         | `#8145`, `#8149`, `#8150`          |

The entries are evidence-routing keys. They identify which owner rejects a
retired or unsupported surface, but they are not compatibility support claims.
The `absent-support` row exists because retired-source lane support has no diagnostic
surface to execute: it is absent from public support and positive fixture
indexes.

Latest diagnostic owner evidence is folded into the checked-in hard-cutover
commit-refresh note, including diagnostic
code, severity, core render/record, parse, removed-mode classifier, and catalog
owner splits. Those commits refresh diagnostic ownership only; they do not add
new retired-surface diagnostics or positive retired-surface behavior.
