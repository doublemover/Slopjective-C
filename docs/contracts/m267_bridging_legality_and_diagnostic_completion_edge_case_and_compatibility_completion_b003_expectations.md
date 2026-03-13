# M267 Bridging Legality And Diagnostic Completion Expectations (B003)

Contract ID: `objc3c-part6-error-bridge-legality/m267-b003-v1`

This issue lands one truthful source-only semantic packet at `frontend.pipeline.semantic_surface.objc_part6_error_bridge_legality`.

Required behavior:
- canonical `objc_nserror` and `objc_status_code(...)` bridge markers undergo deterministic semantic legality checking
- only semantically valid bridge call surfaces qualify as NSError-bridged call surfaces for `try`
- unsupported bridge combinations fail closed in native mode while runnable lowering/runtime work remains deferred

Required live legality rules:
- `objc_nserror` requires an NSError out parameter
- `objc_nserror` currently requires a BOOL-like success return
- NSError/status bridge markers cannot currently be combined with `throws`
- `objc_nserror` and `objc_status_code` cannot appear on the same callable
- `objc_status_code` requires an NSError out parameter
- `objc_status_code` currently requires `error_type: NSError`
- `objc_status_code` requires a BOOL-like or integer status return
- `objc_status_code` mapping symbol must resolve to a declared function
- `objc_status_code` mapping function must accept one matching status parameter and return `NSError`
- exact mapping rule surface: objc_status_code mapping function must accept one matching status parameter and return `NSError`

Required positive proof:
- source-only probe over `tests/tooling/fixtures/native/m267_bridge_legality_positive.objc3`
- positive payload must report:
  - `bridge_callable_sites = 2`
  - `objc_nserror_callable_sites = 1`
  - `objc_status_code_callable_sites = 1`
  - `semantically_valid_bridge_callable_sites = 2`
  - `try_eligible_bridge_callable_sites = 2`
  - `contract_violation_sites = 0`
  - `bridge_legality_landed = true`
  - `try_bridge_filter_landed = true`
  - `unsupported_combinations_fail_closed = true`
  - `native_emit_remains_fail_closed = true`
  - `deterministic = true`
  - `ready_for_lowering_and_runtime = false`
- the same manifest must still show `objc_part6_try_do_catch_semantics.bridged_callable_try_sites = 1`

Required negative proof:
- `m267_bridge_legality_nserror_missing_out_negative.objc3` -> `objc_nserror requires an NSError out parameter [O3S275]`
- `m267_bridge_legality_nserror_bad_return_negative.objc3` -> `objc_nserror currently requires a BOOL-like success return [O3S276]`
- `m267_bridge_legality_throws_conflict_negative.objc3` -> `NSError/status bridge markers cannot currently be combined with throws [O3S277]`
- `m267_bridge_legality_marker_conflict_negative.objc3` -> `objc_nserror and objc_status_code cannot appear on the same callable [O3S278]`
- `m267_bridge_legality_bad_error_type_negative.objc3` -> `objc_status_code currently requires error_type: NSError [O3S280]`
- `m267_bridge_legality_missing_mapping_negative.objc3` -> `objc_status_code mapping symbol must resolve to a declared function [O3S282]`
- `m267_bridge_legality_bad_mapping_signature_negative.objc3` -> `objc_status_code mapping function must accept one matching status parameter and return NSError [O3S283]`
- `m267_bridge_legality_bad_status_return_negative.objc3` -> `objc_status_code requires a BOOL-like or integer status return [O3S281]`
- native probe over `m267_bridge_legality_native_fail_closed.objc3` must fail closed with `unsupported feature claim: NSError/status bridge legality is not yet runnable in Objective-C 3 native mode [O3S285]`

Validation commands:
- `python scripts/check_m267_b003_bridging_legality_and_diagnostic_completion_edge_case_and_compatibility_completion.py`
- `python -m pytest tests/tooling/test_check_m267_b003_bridging_legality_and_diagnostic_completion_edge_case_and_compatibility_completion.py -q`
- `python scripts/run_m267_b003_lane_b_readiness.py`
