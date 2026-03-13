# M267-B001 Expectations

Issue: `M267-B001`
Contract ID: `objc3c-part6-error-semantic-model/m267-b001-v1`
Surface path: `frontend.pipeline.semantic_surface.objc_part6_error_semantic_model`

## Goal

Freeze one truthful Part 6 lane-B semantic packet for the currently implemented error surface.

## Required truths

- `throws` declaration legality is live as a deterministic semantic packet.
- result-like carrier profiles are carried through sema as deterministic semantic state.
- `NSError` bridging profiles are carried through sema as deterministic semantic state.
- canonical bridge-marker clauses remain admitted and carried through the semantic packet.
- `try`, `throw`, and `do/catch` remain parser-owned fail-closed boundaries.
- postfix propagation, status-to-error execution, bridge temporaries, and native thrown-error ABI remain deferred.
- any inherited throws/unwind shard summaries must be described as carried placeholders, not executable propagation semantics.
- the packet must remain deterministic and replayable.

## Code anchors

- `native/objc3c/src/sema/objc3_sema_contract.h`
- `native/objc3c/src/sema/objc3_semantic_passes.h`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_pipeline.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`

## Spec/doc anchors

- `docs/objc3c-native.md`
- `docs/objc3c-native/src/30-semantics.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`
- `spec/PART_6_ERRORS_RESULTS_THROWS.md`
- `package.json`

## Required validation

- issue-local checker proves the contract/doc/code anchors exist
- positive semantic probe over `tests/tooling/fixtures/native/m267_part6_error_source_closure_positive.objc3`
- positive semantic probe over `tests/tooling/fixtures/native/m267_error_bridge_marker_surface_positive.objc3`
- negative probes prove `try`, `throw`, and `do/catch` remain fail-closed
- negative probe proves malformed `objc_status_code(...)` mapping remains rejected
- issue-local pytest passes
- lane-B readiness runner passes
