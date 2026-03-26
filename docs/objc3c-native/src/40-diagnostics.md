<!-- markdownlint-disable-file MD041 -->

## Diagnostics

The live frontend writes deterministic diagnostics in two forms:

- `<prefix>.diagnostics.txt`
- `<prefix>.diagnostics.json`

Diagnostics are always emitted, including on failure.
The current native path is intentionally fail closed when it encounters unsupported constructs outside the admitted runnable surface.
