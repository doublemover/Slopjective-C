<!-- markdownlint-disable-file MD041 -->

## Semantic Surface (Current)

The live frontend currently enforces:

- deterministic parser and semantic diagnostics
- lexical scope and symbol resolution
- scalar type compatibility across the admitted surface
- control-flow legality for loops, switches, and returns
- fail-closed handling for unsupported or incomplete language slices

## Lowering Surface (Current)

The live lowering path currently covers:

- scalar values and control flow
- function calls and admitted message-send lowering
- manifest generation
- LLVM IR emission
- object emission through the configured backend

## Runtime Boundary (Current)

The live compiler/runtime boundary is centered on emitted metadata plus the native runtime library under `native/objc3c/src/runtime` and `artifacts/lib/objc3_runtime.lib`.
