# M267-B003 Packet: Bridging Legality And Diagnostic Completion

Milestone: `M267`
Lane: `B`
Issue: `M267-B003`

## Goal

Close the remaining NSError/status bridging legality gaps in sema so bridge markers are not merely counted; they become deterministic legality surfaces that gate whether a callable may participate in `try` as a bridged call surface.

## Required implementation

1. Publish one semantic packet at `frontend.pipeline.semantic_surface.objc_part6_error_bridge_legality`.
2. Enforce bridge-marker legality for functions and Objective-C methods.
3. Tighten `try` bridge qualification so invalid bridge markers do not count as bridged call surfaces.
4. Only semantically valid bridge call surfaces qualify for `try`.
5. Packet literal: only semantically valid bridge call surfaces qualify for `try`.
6. Keep native lowering/runtime behavior fail-closed for this surface.

## Minimum legality rules

- `objc_nserror` requires an NSError out parameter.
- `objc_nserror` currently requires a BOOL-like success return.
- NSError/status bridge markers cannot currently be combined with `throws`.
- `objc_nserror` and `objc_status_code` cannot appear on the same callable.
- `objc_status_code` requires an NSError out parameter.
- `objc_status_code` currently requires `error_type: NSError`.
- `objc_status_code` requires a BOOL-like or integer status return.
- `objc_status_code` mapping symbol must resolve to a declared function.
- `objc_status_code` mapping function must accept one matching status parameter and return `NSError`.

## Code anchors

- `native/objc3c/src/sema/objc3_sema_contract.h`
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

## Evidence

Write summary evidence to `tmp/reports/m267/M267-B003/bridging_legality_summary.json`.
