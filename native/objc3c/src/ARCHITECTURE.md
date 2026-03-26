# objc3c Native Frontend Architecture

## Layer Model

The native frontend is split into five primary layers plus adapter modules:

1. `lex/*`
2. `parse/*`
3. `sema/*`
4. `lower/*`
5. `ir/*`

Adapter modules:

- `pipeline/*`: stage orchestration and result passing
- `libobjc3c_frontend/*`: embedding API over the pipeline
- `driver/*`: CLI parsing and top-level command routing
- `io/*`: filesystem, manifest, artifact, and tool adapters

Allowed dependencies:

- `driver` -> `libobjc3c_frontend`, `io`, `lower`, `pipeline`
- `libobjc3c_frontend` -> `pipeline`, `io`
- `pipeline` -> `lex`, `parse`, `sema`, `lower`, `ir`, `io`
- `lower` -> `sema`
- `ir` -> `lower`, `parse`
- `io` -> `lower`, `pipeline`
- `parse` -> `lex`, `pipeline`
- `sema` -> `parse`, `pipeline`
- `lex` -> none

Forbidden dependencies:

- code outside the declared map must not bypass module boundaries
- archived planning and closeout material must stay outside the live product tree
- milestone-specific issue bookkeeping must not be embedded in live product-source prose or identifiers
