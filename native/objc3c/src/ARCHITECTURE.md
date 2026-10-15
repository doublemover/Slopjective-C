# objc3c Native Frontend Architecture

## Layer Model

The native frontend is split into five primary layers plus adapter modules:

1. `lex/*`
2. `parse/*`
3. `sema/*`
4. `lower/*`
5. `ir/*`

`ast/*` owns shared declarations, type identity, and effect facts consumed by
these layers. Shared AST headers contain data and pure fact queries; they do
not invoke semantic passes, lowering, emission, or artifact publication.

Adapter modules:

- `artifacts/*`: cross-phase manifest, metadata, and publication projections
- `pipeline/*`: stage orchestration and result passing
- `libobjc3c_frontend/*`: embedding API over the pipeline
- `driver/*`: CLI parsing and top-level command routing
- `io/*`: filesystem, manifest, artifact, and tool adapters

Artifact names used by lowering belong to `lower/contracts/*`. The artifact
adapter consumes those names when publishing products and host identity; IR
must not call the publication adapter to obtain a default filename.

Allowed dependencies:

- `artifacts` -> `parse`, `sema`, `lower`, `ir`, `pipeline`, `io`
- `driver` -> `artifacts`, `libobjc3c_frontend`, `io`, `lower`, `pipeline`
- `libobjc3c_frontend` -> `artifacts`, `pipeline`, `io`
- `pipeline` -> `lex`, `parse`, `sema`, `lower`, `ir`, `io`
- `lower` -> `sema`
- `ir` -> `lower`, `parse`
- `io` -> `artifacts`, `lower`, `pipeline`
- `parse` -> `lex`, `pipeline`
- `sema` -> `parse`, `pipeline`
- `lex` -> none

Forbidden dependencies:

- code outside the declared map must not bypass module boundaries
- archived planning and closeout material must stay outside the live product tree
- milestone-specific issue bookkeeping must not be embedded in live product-source prose or identifiers
