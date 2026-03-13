# M267-B001 Packet

Milestone: `M267`
Lane: `B`
Issue: `M267-B001`

## Summary

Freeze the truthful Part 6 semantic boundary as implemented today: sema owns deterministic throws-declaration, result-like, `NSError`, and bridge-marker carriage, while runnable propagation and catch execution remain deferred.

## Dependencies

- `M267-A002`
- `M259-E002`
- `M263-E002`
- `M265-E002`

## Deliverables

- Part 6 semantic-model contract constants and packet schema in `objc3_sema_contract.h`
- semantic-model builder in `objc3_semantic_passes.cpp`
- frontend pipeline/artifact handoff into `frontend.pipeline.semantic_surface.objc_part6_error_semantic_model`
- issue-local expectations, packet, checker, pytest, and readiness assets
- explicit doc/spec/package anchors explaining the truthful live-vs-deferred split

## Positive proof surface

Use these existing fixtures:

- `tests/tooling/fixtures/native/m267_part6_error_source_closure_positive.objc3`
- `tests/tooling/fixtures/native/m267_error_bridge_marker_surface_positive.objc3`

Required positive truths:

- throws declarations are counted and carried semantically
- result-like profiles remain normalized through sema
- `NSError` bridging profiles remain normalized through sema
- bridge-marker attributes and clause counts remain carried semantically
- the new packet explicitly marks runtime propagation and native error ABI as deferred

## Negative proof surface

- `tests/tooling/fixtures/native/m267_try_expression_fail_closed_negative.objc3`
- `tests/tooling/fixtures/native/m267_throw_statement_fail_closed_negative.objc3`
- `tests/tooling/fixtures/native/m267_do_catch_fail_closed_negative.objc3`
- `tests/tooling/fixtures/native/m267_status_code_attribute_missing_mapping_negative.objc3`

## Exit condition

One deterministic semantic packet is emitted at `frontend.pipeline.semantic_surface.objc_part6_error_semantic_model`, all issue-local validations pass, and the packet remains truthful about live declaration/profile semantics versus deferred runnable propagation behavior.
